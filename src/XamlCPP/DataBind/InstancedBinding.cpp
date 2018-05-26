/*
 * Copyright (C) 2017-2018 Tauri JayD <https://www.>
 * Copyright (c) 2016 OmniGUI Platform
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "StdAfx.h"

#include "InstancedBinding.h"
#include "SourceObserver.h"
#include "LogInterface.h"
#include "Core/TypeExtensions/TypeConverter.h"
#include "Core/Dependencies/DependencyPropertyMgr.h"
#include "Core/Dependencies/DependencyObject.h"
#include "DataBind/BindingNotification.h"
#include "Core/Dependencies/Models/PropertyPrototype.h"
#include "Log/Log.h"

/// <summary>
/// Initializes a new instance of the <see cref="InstancedBinding"/> class.
/// </summary>
/// <param name="value">
/// The value used for the <see cref="BindingMode.OneTime"/> binding.
/// </param>
/// <param name="priority">The binding priority.</param>
XamlCPP::DataBind::InstancedBinding::InstancedBinding(System::Reflection::meta::Variant const & value, BindingLevel priority)
{
    _mode = BindingMode::OneTime;
    _priority = priority;
    _value = value;
}

/// <summary>
/// Initializes a new instance of the <see cref="InstancedBinding"/> class.
/// </summary>
/// <param name="subject">The subject for a two-way binding.</param>
/// <param name="mode">The binding mode.</param>
/// <param name="priority">The binding priority.</param>
XamlCPP::DataBind::InstancedBinding::InstancedBinding(
    std::shared_ptr<SourceObserver> observable,
    System::Reflection::meta::Variant const& fallbackValue,
    BindingMode mode,
    BindingLevel priority)
{
    System::ThrowIfFailed<System::ArgumentNullException>(observable != nullptr);

    if (mode == BindingMode::TwoWay)
    {
        _innerSubject = new Core::Dependency::GenericSubject;
        _secondarySubject = new Core::Dependency::GenericSubject;
    }
    if (mode == BindingMode::OneWayToSource)
        _secondarySubject = new Core::Dependency::GenericSubject;
    if (mode == BindingMode::OneWay || mode == BindingMode::Default)
        _innerSubject = new Core::Dependency::GenericSubject;

    _mode = mode;
    _priority = priority;
    _inner = observable;
    _inner->SetOwner(this);
    _fallbackValue = fallbackValue;
}

XamlCPP::DataBind::InstancedBinding::InstancedBinding(
    std::shared_ptr<Core::Dependency::GenericObservable> const & subject,
    BindingMode mode,
    BindingLevel priority)
{
    if (mode == BindingMode::TwoWay)
    {
        _innerSubject = new Core::Dependency::GenericSubject;
        _secondarySubject = new Core::Dependency::GenericSubject;
    }
    if (mode == BindingMode::OneWayToSource)
        _secondarySubject = new Core::Dependency::GenericSubject;
    if (mode == BindingMode::OneWay || mode == BindingMode::Default)
        _innerSubject = new Core::Dependency::GenericSubject;

    _mode = mode;
    _priority = priority;
    Observer = subject;
}

XamlCPP::DataBind::InstancedBinding::~InstancedBinding()
{
    if (_inner && _inner->GetListenerSubject())
    {
        delete _inner->GetListenerSubject();
        _inner->SetListenerSubject(nullptr);
    }
    if (_errors)
        delete _errors;
    if (_innerSubject)
        delete _innerSubject;
    if (_secondarySubject)
        delete _secondarySubject;
}

void XamlCPP::DataBind::InstancedBinding::CreatePrimaryWay()
{
    if (_inner)
    {
        if (!_inner->GetListenerSubject() || (_inner->GetListenerSubject() && !_inner->GetListenerSubject()->get_subscription().is_subscribed()))
        {
            if (_innerListenerSubcribe.is_subscribed())
                _innerListenerSubcribe.unsubscribe();

            if (_inner->GetListenerSubject())
            {
                delete _inner->GetListenerSubject();
                _inner->SetListenerSubject(nullptr);
            }

            _inner->SetListenerSubject(new Core::Dependency::GenericSubject);
        }

        _innerListenerSubcribe = _inner->GetListenerSubject()->get_observable().subscribe(
            std::bind(&InstancedBinding::OnNext, this, std::placeholders::_1),
            std::bind(&InstancedBinding::OnError, this, std::placeholders::_1),
            std::bind(&InstancedBinding::OnCompleted, this));

        _inner->StartListening();
    }
    else if (Observer)
    {
        System::ThrowIfFailed<System::TargetInvocationException>(!_innerListenerSubcribe.is_subscribed());

        Observer->subscribe(std::move(_innerSubject->get_observer().get_subscriber()));
        _innerListenerSubcribe = _innerSubject->get_observable().subscribe(
            std::bind(&InstancedBinding::OnNext, this, std::placeholders::_1),
            std::bind(&InstancedBinding::OnError, this, std::placeholders::_1),
            std::bind(&InstancedBinding::OnCompleted, this));
    }
    else
        throw System::NotSupportedException();
}

rxcpp::subscription XamlCPP::DataBind::InstancedBinding::CreateSecondaryWay(XamlCPP::Core::Dependency::IDependencyObject * target, XamlCPP::Core::Dependency::IPropertyObject * property)
{
    System::ThrowIfFailed<System::TargetInvocationException>(!_secondaryListenerSubcribe.is_subscribed());

    // Get Secondary Listener and subscribe
    auto observable = target->GetObservable(property);
    auto subscription = observable->subscribe(std::move(_secondarySubject->get_observer().get_subscriber()));

    System::ThrowIfFailed<System::NotSupportedException>(_inner.get());

    _secondaryListenerSubcribe = _secondarySubject->get_observable().subscribe(
        std::bind(&InstancedBinding::OnSecondWayNext, this, std::placeholders::_1),
        std::bind(&InstancedBinding::OnSecondWayError, this, std::placeholders::_1),
        std::bind(&InstancedBinding::OnSecondWayCompleted, this));

    return subscription;
}

void XamlCPP::DataBind::InstancedBinding::GetValue(System::Reflection::meta::Variant const & value)
{
    _value = value;
}

XamlCPP::Core::Dependency::GenericSubject* XamlCPP::DataBind::InstancedBinding::GetSubject()
{
    return _innerSubject;
}

XamlCPP::Core::Dependency::GenericSubject * XamlCPP::DataBind::InstancedBinding::GetSecondarySubject()
{
    return _secondarySubject;
}

/// Listener input trigger write _resultSubject.get_subscriber().on_next();
void XamlCPP::DataBind::InstancedBinding::OnNext(System::Reflection::meta::Variant const& value)
{
    if (TargetProperty != nullptr && Converter != nullptr)
    {
        System::Reflection::meta::Variant newValue = value.GetType() == TargetProperty->PropertyType ? value : Converter->Convert(value, TargetProperty->PropertyType, ConverterParameter, 0);

        static auto const& bindingNotificationType = typeof(BindingNotification);

        if (bindingNotificationType.IsAssignableFrom(*newValue.GetType()))
        {
            auto notification = newValue.GetValue<std::shared_ptr<BindingNotification>>();
            
            if (notification->ErrorType == BindingErrorType::None)
            {
                throw System::ArgumentException(
                    "IValueConverter should not return non-errored BindingNotification.");
            }
            
            if (_errors)
                _errors->get_observer().on_next(notification.get());
            
            if (_inner)
                System::Console()->WriteErrorLine("[BindingNotification] not convert '%s' to '%s' Expression: %s.", value.GetType() ? value.GetType()->fullname : "<unknown>", TargetProperty->PropertyType ? TargetProperty->PropertyType->fullname : "<unset>", _inner->ToString());

            if (_fallbackValue.GetBase())
            {
                if (Core::TypeExtensions::TypeConverter::TryCast(
                    TargetProperty->PropertyType,
                    _fallbackValue,
                    newValue))
                {
                    if (_innerSubject)
                        _innerSubject->get_observer().on_next(newValue);
                }
                else
                {
                    Platform::ILog::Error(
                        Platform::LogArea::Binding,
                        nullptr,
                        "Could not convert FallbackValue {FallbackValue} to {Type}",
                        _fallbackValue,
                        TargetProperty->PropertyType);
                }
            }
        }
        else
        {
            _innerSubject->get_observer().on_next(newValue);
        }
    }
}

void XamlCPP::DataBind::InstancedBinding::OnSecondWayNext(System::Reflection::meta::Variant const & value)
{
    auto type = _inner->GetResultType();

    if (type != nullptr && type->IsValid() && Converter != nullptr)
    {
        System::Reflection::meta::Variant newValue = Converter->ConvertBack(value, type, ConverterParameter, 0);

        static auto const& bindingNotificationType = typeof(BindingNotification);

        if (bindingNotificationType.IsAssignableFrom(*newValue.GetType()))
        {
            auto notification = newValue.GetValue<std::shared_ptr<BindingNotification>>();

            if (notification->ErrorType == BindingErrorType::None)
            {
                throw System::ArgumentException(
                    "IValueConverter should not return non-errored BindingNotification.");
            }

            if (_errors)
                _errors->get_observer().on_next(notification.get());

            if (_inner)
                System::Console()->WriteErrorLine("[BindingNotification] not convert '%s' to '%s' Expression: %s.", value.GetType() ? value.GetType()->fullname : "<unknown>", type ? type->fullname : "<unset>", _inner->ToString());

            if (_fallbackValue.GetBase())
            {
                if (Core::TypeExtensions::TypeConverter::TryCast(
                    type,
                    _fallbackValue,
                    newValue))
                {
                    _inner->SetValue(newValue, _priority);
                }
                else
                {
                    Platform::ILog::Error(
                        Platform::LogArea::Binding,
                        nullptr,
                        "Could not convert FallbackValue {FallbackValue} to {Type}",
                        _fallbackValue,
                        type);
                }
            }
        }
        else
        {
            _inner->SetValue(newValue, _priority);
        }
    }
}

void XamlCPP::DataBind::InstancedBinding::UpdateSource(Core::Dependency::IDependencyObject* dp)
{
    if (TargetProperty && dp)
    {
        OnSecondWayNext(dp->GetValue(TargetProperty));
    }
}

void XamlCPP::DataBind::InstancedBinding::UpdateTarget()
{
    if (_inner && _inner->GetResultType() && _inner->GetResultType()->IsValid())
    {
        OnNext(_inner->GetValue());
    }
}
