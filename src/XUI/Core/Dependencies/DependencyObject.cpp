/*
 * Copyright (C) 2017-2018 Tauri JayD <https://www.>
 * Copyright (c) 2014 Steven Kirk <The Avalonia Project>
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

#include "rxcpp/rx.hpp"
#include "cpplinq/linq.hpp"

#include "Core/Dependencies/BindValue.h"
#include "Core/Dependencies/DependencyPropertyMgr.h"
#include "Core/Dependencies/DependencyObservable.h"
#include "Core/Dependencies/DependencyObject.h"
#include "XamlCPP/DataBind/BindingNotification.h"
#include "XamlCPP/Core/Dependencies/IPropertyObject.h"
#include "Core/Dependencies/Models/DataProperty.h"
#include "Core/Dependencies/Models/DirectProperty.h"
#include "XamlCPP/DataBind/Binding.h"
#include "XamlCPP/Core/TypeExtensions/TypeConverter.h"
#include "XamlCPP/Core/LogInterface.h"
#include "Reflection/Runtime/CXXReflection.h"
#include "Core/Observer/IObserver.h"

#include "Platform/UIService.h"
#include "UI/Controls/Control.h"

#include <standard/collection.h>

using namespace XamlCPP;
using namespace XUI::Core;
using namespace XUI::Core::Dependency;

DependencyObject::DependencyObject()
{
    //_values.set_empty_key(nullptr); //google::dense_hash_map
}

DependencyObject::~DependencyObject()
{
    Cleanup();
}

void XUI::Core::Dependency::DependencyObject::Cleanup()
{
    for (auto& bindSet : _directBindingExpressions)
    {
        while (!bindSet.second.empty())
        {
            if ((*bindSet.second.begin())->UnregisterSubcription.is_subscribed())
                (*bindSet.second.begin())->UnregisterSubcription.unsubscribe();
            else
                bindSet.second.erase(bindSet.second.begin());
        }
    }

    for (auto& bindValue : _values)
    {
        auto& binds = (std::set<std::shared_ptr<XamlCPP::DataBind::InstancedBinding>>&)bindValue.second->GetBindingExpressions();
        while (!binds.empty())
        {
            if ((*binds.begin())->UnregisterSubcription.is_subscribed())
                (*binds.begin())->UnregisterSubcription.unsubscribe();
            else
                binds.erase(binds.begin());
        }
    }

    for (auto& sub : _directBindings)
        if (sub->is_subscribed())
            sub->unsubscribe();

    _directBindings.clear();
}

System::Reflection::Type const * XUI::Core::Dependency::DependencyObject::GetType(void) const
{
    if (_typeCache && *_typeCache == typeid(*this))
        return _typeCache;
    return _typeCache = System::Reflection::Object::GetType();
}

System::Reflection::meta::Variant const& XUI::Core::Dependency::DependencyObject::GetDefaultValue(XamlCPP::Core::Dependency::IPropertyObject * _property) const
{
    if (_property->Inherits && _inheritanceParent != nullptr)
    {
        return _inheritanceParent->GetValueInternal(_property);
    }
    else
    {
        return _property->ToIBindPropertyAccessor()->GetDefaultValue(GetType());
    }
}

void XUI::Core::Dependency::DependencyObject::RaisePropertyChanged(XamlCPP::Core::Dependency::IPropertyObject * _property, System::Reflection::meta::Variant const & oldValue, System::Reflection::meta::Variant const & newValue, BindingLevel level /*= BindingLevel::LocalValue*/)
{
    System::ThrowIfFailed<System::ArgumentNullException>(_property != nullptr);

    XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs e(
        static_cast<XamlCPP::Core::Dependency::IDependencyObject*>(this),
        _property,
        oldValue,
        newValue);

    e.Level = level;

    try
    {
        if (_property->Notifying)
            _property->Notifying(this, e);
        if (_property->Changed.has_observers())
            _property->Changed.get_observer().on_next(e);
        if (DependencyPropertyChanged)
            DependencyPropertyChanged(this, e);
        NotifyPropertyChanged::RaisePropertyChanged(_property->Name.c_str());
    }
    catch(...)
    {
    }
}

/// <summary>
/// Gets a <see cref="XamlCPP::Core::Dependency::IPropertyObject"/> value
/// without check for registered as this can slow getting the value
/// this method is intended for internal usage in dependencyObject only
/// it's called only after check the _property is registered
/// </summary>
/// <param name="_property">The _property.</param>
/// <returns>The value.</returns>

System::Reflection::meta::Variant const& XUI::Core::Dependency::DependencyObject::GetValueInternal(XamlCPP::Core::Dependency::IPropertyObject * _property) const
{
    System::Reflection::meta::Variant const* result = nullptr;

    auto iValue = _values.find(_property);
    if (iValue != _values.end())
        result = &iValue->second->Value;
    
    if (result && *result->GetType() == typeid(XUI::Core::Dependency::Unset*)) //*result == DependencyPropertyMgr::UnsetValue)
        result = &GetDefaultValue(_property);
    else if (iValue == _values.end())
        result = &GetDefaultValue(_property);

    EXCEPT(result);

    return *result;
}

BindValue* XUI::Core::Dependency::DependencyObject::GetValueSource(XamlCPP::Core::Dependency::IPropertyObject * _property) const
{
    auto iValue = _values.find(_property);
    if (iValue != _values.end())
        return iValue->second.get();

    return nullptr;
}

/// <summary>
/// Sets the value of a direct _property.
/// </summary>
/// <param name="_property">The _property.</param>
/// <param name="value">The value.</param>

void XUI::Core::Dependency::DependencyObject::SetDirectValue(XamlCPP::Core::Dependency::IPropertyObject * _property, System::Reflection::meta::Variant const & value)
{
    static auto const& controlType = typeof(DataBind::BindingNotification);

    System::Reflection::meta::Variant _value = value;
    DataBind::BindingNotification* notification = nullptr;
    
    if (controlType.IsAssignableFrom(*_value.GetType()))
    {
        notification = static_cast<DataBind::BindingNotification*>(_value.ToObject());

        if (notification->ErrorType == BindingErrorType::Error)
        {
            XamlCPP::Platform::ILog::Error(
                DependencyPropertyMgr::LogNamedCategories::Binding,
                nullptr,
                "Error in binding to {%s}.{%s}: {%s}",
                Name.c_str(),
                _property->Name.c_str(),
                notification->Error ? notification->Error->what() : "");
        }
    
        if (notification->HasValue())
        {
            _value = notification->Value;
        }
    }
    
    if (notification == nullptr || notification->HasValue())
    {
        auto finalValue = _value.GetType() == DependencyPropertyMgr::UnsetValue.GetType() ? _property->ToIDirectPropertyAccessor()->GetDefaultValue(GetType()) : _value;
    
        XamlCPP::Platform::ILog::Verbose(
            DependencyPropertyMgr::LogNamedCategories::Property,
            nullptr,
            "Set {%s} to {%s} with priority {Priority}",
            _property->Name.c_str(),
            value.GetType() ? value.GetType()->GetFullName() : "");
    
        _property->SetValue(this, finalValue);
    }
    
    if (notification != nullptr)
    {
        UpdateDataValidation(_property, notification);
    }
}

std::shared_ptr<BindValue> XUI::Core::Dependency::DependencyObject::CreateBindValue(XamlCPP::Core::Dependency::IPropertyObject * _property)
{
    XamlCPP::Core::Dependency::IBindPropertyMetadata::ValidateFunction const& validate = _property->ToIBindPropertyAccessor()->GetValidationFunc(GetType());
    BindValue::ValidateFunction validate_bind = nullptr;
    
    if (validate != nullptr)
    {
        validate_bind = [this, validate](System::Reflection::meta::Variant const& v)
        {
            return validate(this, v); 
        };
    }

    return std::make_shared<BindValue>(
        this,
        _property,
        _property->PropertyType,
        &validate_bind);
}

/// <summary>
/// Sets the value of a styled _property.
/// </summary>
/// <param name="_property">The _property.</param>
/// <param name="value">The value.</param>
/// <param name="priority">The priority of the value.</param>

void XUI::Core::Dependency::DependencyObject::SetInternalValue(XamlCPP::Core::Dependency::IPropertyObject * _property, System::Reflection::meta::Variant const & value, BindingLevel level)
{
    static auto const& controlType = typeof(DataBind::BindingNotification);

    System::Reflection::meta::Variant _value = value;

    // We currently accept BindingNotifications for non-direct properties but we just
    // strip them to their underlying value.
    if (controlType.IsAssignableFrom(*_value.GetType()))
    {
        auto notification = static_cast<DataBind::BindingNotification*>(_value.ToObject());

        if (!notification->HasValue())
            return;
        _value = notification->Value;
    }

    //System::ThrowIfFailed<System::ArgumentException>(DependencyPropertyMgr::Instance()->IsRegistered(this, _property), []() { throw System::ArgumentException("Property '{_property.Name} not registered on '{this.GetType()}"); });

    auto iBind = _values.find(_property);
    if (iBind == _values.end())
    {
        if (value == DependencyPropertyMgr::UnsetValue)
            return;

        _values[_property] = CreateBindValue(_property);
        iBind = _values.find(_property);
    }

    XamlLogVerbose(
        DependencyPropertyMgr::LogNamedCategories::Property,
        nullptr,
        "Set {%s} to {%s} with priority {Priority}",
        _property->Name.c_str(),
        value.GetType() ? value.GetType()->GetFullName() : "");
    
    iBind->second->SetValue(value, level);
}

void XUI::Core::Dependency::DependencyObject::StorePropertyData(XamlCPP::Core::Dependency::IPropertyObject * _property, System::Reflection::meta::Variant const & value, BindingLevel level)
{
    auto iBind = _values.find(_property);
    if (iBind == _values.end())
    {
        if (value == DependencyPropertyMgr::UnsetValue)
            return;

        _values[_property] = CreateBindValue(_property);
        iBind = _values.find(_property);
    }

    XamlCPP::Platform::ILog::Verbose(
        DependencyPropertyMgr::LogNamedCategories::Property,
        nullptr,
        "Set {%s} to {%s} with priority {Priority}",
        _property->Name.c_str(),
        value.GetType() ? value.GetType()->GetFullName() : "");

    iBind->second->SetValue(value, level);
}

System::Reflection::meta::Variant XUI::Core::Dependency::DependencyObject::GetPropertyData(XamlCPP::Core::Dependency::IPropertyObject* _property) const
{
    return GetValue(_property);
}

bool XUI::Core::Dependency::DependencyObject::IsSet(XamlCPP::Core::Dependency::IPropertyObject * _property)
{
    System::ThrowIfFailed<System::ArgumentNullException>(_property != nullptr);

    auto iBind = _values.find(_property);
    if (iBind != _values.end())
    {
        return iBind->second->Value != DependencyPropertyMgr::UnsetValue;
    }

    return false;
}

void XUI::Core::Dependency::DependencyObject::ReValidation(XamlCPP::Core::Dependency::IPropertyObject* _property)
{
    auto iBind = _values.find(_property);
    if (iBind != _values.end())
        iBind->second->Revalidate();
}

void XUI::Core::Dependency::DependencyObject::UpdateDataValidation(XamlCPP::Core::Dependency::IPropertyObject * _property, DataBind::BindingNotification * status)
{
}

XamlCPP::Core::Dependency::IPropertyObject* XUI::Core::Dependency::DependencyObject::GetRegistered(XamlCPP::Core::Dependency::IPropertyObject* _property) const
{
    auto result = DependencyPropertyMgr::Instance()->FindRegistered(GetType(), _property);
    System::ThrowIfFailed<System::ArgumentNullException>(result != nullptr);
    return result;
}

System::Reflection::meta::Variant XUI::Core::Dependency::DependencyObject::GetValue(XamlCPP::Core::Dependency::IPropertyObject* _property) const
{
    System::ThrowIfFailed<System::ArgumentNullException>(_property != nullptr);

    if (_property->IsDirect)
    {
        return std::move(_property->GetValue(const_cast<DependencyObject*>(this))); // GetRegistered(_property)
    }
    else
    {
        return GetValueInternal(_property);
    }
    return System::Reflection::meta::Variant::Empty;
}

void XUI::Core::Dependency::DependencyObject::SetValue(XamlCPP::Core::Dependency::IPropertyObject* _property, System::Reflection::meta::Variant const& value, BindingLevel level /*= BindingLevel::LocalValue*/)
{
    System::ThrowIfFailed<System::ArgumentNullException>(_property != nullptr);

    if (_property->IsDirect)
    {
        SetDirectValue(_property, value);
    }
    else
    {
        SetInternalValue(_property, value, level);
    }
}

std::string XUI::Core::Dependency::DependencyObject::GetDescription(XamlCPP::Core::Dependency::GenericObservable* o)
{
    //auto description = o as IDescription;
    return ""; // description ? .Description ? ? o.ToString();
}


/// <summary>
/// Tries to cast a value to a type, taking into account that the value may be a
/// <see cref="BindingNotification"/>.
/// </summary>
/// <param name="value">The value.</param>
/// <param name="type">The type.</param>
/// <returns>The cast value, or a <see cref="BindingNotification"/>.</returns>

System::Reflection::meta::Variant XUI::Core::Dependency::DependencyObject::CastOrDefault(System::Reflection::meta::Variant const & value, System::Reflection::meta::Type const * type)
{
    static auto const& controlType = typeof(DataBind::BindingNotification);

    if (controlType.IsAssignableFrom(*value.GetType()))
    {
        auto notification = static_cast<DataBind::BindingNotification*>(value.ToObject());
        if (notification->HasValue())
        {
            notification->SetValue(XamlCPP::Core::TypeExtensions::TypeConverter::CastOrDefault(notification->Value, type));
        }
        
        return notification;
    }
    else
    {
        return XamlCPP::Core::TypeExtensions::TypeConverter::CastOrDefault(value, type);
    }
}

rxcpp::subscription XUI::Core::Dependency::DependencyObject::Bind(
    XamlCPP::Core::Dependency::IPropertyObject * _property,
    XamlCPP::Core::Dependency::GenericObservable* _source,
    BindingLevel level,
    XamlCPP::Core::Dependency::BindingSubject * _subject)
{
    System::ThrowIfFailed<System::ArgumentNullException>(_property != nullptr);
    System::ThrowIfFailed<System::ArgumentNullException>(_source != nullptr || _subject != nullptr);

    auto description = GetDescription(_source);
    
    if (_property->IsDirect)
    {
        if (_property->IsReadOnly)
        {
            throw System::ArgumentException("The _property {_property.Name} is readonly.");
        }

        System::Reflection::meta::Variant logSource(this);
        XamlCPP::Platform::ILog::Verbose(
            DependencyPropertyMgr::LogNamedCategories::Property,
            &logSource,
            "Bound %s to %s with priority LocalValue",
            _property->Name.c_str(),
            description.c_str());


        std::shared_ptr<rxcpp::composite_subscription> subscription = std::make_shared<rxcpp::composite_subscription>();


        auto propertyPtr = _property->shared_from_this();

        // ToDo: hack
        if (_subject)
        {
            auto subject_ptr = _subject->shared_from_this();
            subscription->add(_subject->Subject.get_observable().subscribe([this, subject_ptr, propertyPtr](System::Reflection::meta::Variant const& value) // OnNext
            {
                SetDirectValue(propertyPtr.get(), value);
            }, [this, subscription]() // OnCompleted
            {
                _directBindings.remove(subscription);
            }));
        }
        else
        {
            subscription->add(_source->subscribe([this, propertyPtr](System::Reflection::meta::Variant const& value) // OnNext
            {
                SetDirectValue(propertyPtr.get(), value);
            }, [this, subscription]() // OnCompleted
            {
                _directBindings.remove(subscription);
            }));
        }

        _directBindings.push_back(subscription);

        std::weak_ptr<rxcpp::composite_subscription> weakSubscription = subscription;
        return rxcpp::make_subscription([this, subscripable = weakSubscription]()
        {
            if (subscripable.expired())
                return;

            auto ptr = subscripable.lock();
            ptr->unsubscribe();
            _directBindings.remove(ptr);
        });
    }
    else
    {
        System::ThrowIfFailed<System::ArgumentNullException>(DependencyPropertyMgr::Instance()->IsRegistered(this, _property));
    
        auto iBind = _values.find(_property);
        if (iBind == _values.end())
        {
            _values[_property] = CreateBindValue(_property); //std::make_shared<BindValue>(this, _property, _property->PropertyType);
            iBind = _values.find(_property);
        }

        System::Reflection::meta::Variant logSource(this);
        XamlCPP::Platform::ILog::Verbose(
            DependencyPropertyMgr::LogNamedCategories::Property,
            &logSource,
            "Bound %s to %s with priority {Priority}",
            _property->Name.c_str(),
            description.c_str());
    
        return iBind->second->Add(_source, level, _subject);
    }
}

rxcpp::subscription Meta(Disable) XUI::Core::Dependency::DependencyObject::AddDirectBind(
    XamlCPP::Core::Dependency::IPropertyObject* _property,
    std::function<void(System::Reflection::meta::Variant const& value)> _func)
{
    std::shared_ptr<rxcpp::composite_subscription> subscription = std::make_shared<rxcpp::composite_subscription>();

    auto observer = GetUnsafeObservable(_property);
    auto propertyPtr = _property->shared_from_this();
    subscription->add(observer->subscribe([_func, observer](System::Reflection::meta::Variant const& value) // OnNext
    {
        _func(value);
    }, [this, subscription]() // OnCompleted
    {
        _directBindings.remove(subscription);
    }));

    _directBindings.push_back(subscription);

    std::weak_ptr<rxcpp::composite_subscription> weakSubscription = subscription;
    return rxcpp::make_subscription([this, subscripable = weakSubscription]()
    {
        if (subscripable.expired())
            return;

        auto ptr = subscripable.lock();
        ptr->unsubscribe();
        _directBindings.remove(ptr);
    });
}

/// <summary>
/// Binds a property on an <see cref="DependencyObject"/> to an <see cref="IBinding"/>.
/// </summary>
/// <param name="target">The object.</param>
/// <param name="property">The property to bind.</param>
/// <param name="binding">The binding.</param>
/// <param name="anchor">
/// An optional anchor from which to locate required context. When binding to objects that
/// are not in the logical tree, certain types of binding need an anchor into the tree in 
/// order to locate named controls or resources. The <paramref name="anchor"/> parameter 
/// can be used to provice this context.
/// </param>
/// <returns>An <see cref="IDisposable"/> which can be used to cancel the binding.</returns>

rxcpp::subscription XUI::Core::Dependency::DependencyObject::Bind(DependencyObject * target, DependencyProperty * property, XamlCPP::Interfaces::IBinding * binding, System::Reflection::meta::Object * anchor)
{
    System::ThrowIfFailed<System::ArgumentNullException >(target != nullptr);
    System::ThrowIfFailed<System::ArgumentNullException >(property != nullptr);
    System::ThrowIfFailed<System::ArgumentNullException >(binding != nullptr);

    auto metadata = property->GetMetadata(target->GetType())->shared_from_base_dynamic<XamlCPP::Core::Dependency::IDirectPropertyMetadata>();

    auto namedScope = target->GetSharedNamedScope();
    auto instanceBinding = binding->Initiate(
        target,
        property,
        anchor,
        metadata ? metadata->GetEnableDataValidation() : false,
        &namedScope);

    if (instanceBinding != nullptr)
    {
        instanceBinding->UnregisterSubcription.add(DataBind::Binding::Apply(target, property, instanceBinding.get(), anchor));
        target->AddBindingExpression(property, instanceBinding.get());
        instanceBinding->UnregisterSubcription.add([owner = target->weak_from_base_static<DependencyObject>(), bind = instanceBinding.get(), property]()
        {
            if (!owner.expired())
                owner.lock()->RemoveBindingExpression(property, bind);
        });
        return instanceBinding->UnregisterSubcription;
    }

    return rxcpp::subscription();
}

XamlCPP::Core::Dependency::IPropertyObject * XUI::Core::Dependency::DependencyObject::GetDataContextProperty() const
{
    return XUI::UI::Controls::Control::DataContextProperty.get();
}

XamlCPP::Core::Dependency::IPropertyObject * XUI::Core::Dependency::DependencyObject::FindRegisteredGlob(System::Reflection::meta::Type const * type, std::string const & name) const
{
    return DependencyPropertyMgr::Instance()->FindRegistered(type, name);
}

void XUI::Core::Dependency::DependencyObject::LogPropertySet(XamlCPP::Core::Dependency::IPropertyObject * _property, System::Reflection::meta::Variant const & value)
{
    System::Reflection::meta::Variant source(this);
    XamlLogVerbose(
        DependencyPropertyMgr::LogNamedCategories::Property,
        &source,
        "Set %s to %s with priority %d",
        _property->Name,
        value.ToString(),
        0);
}

void XUI::Core::Dependency::DependencyObject::Changed(BindValue* sender, System::Reflection::meta::Variant const& oldValue, System::Reflection::meta::Variant const& newValue)
{
    auto _property = sender->GetProperty();
    
    auto const& _oldValue = (oldValue.GetType() == DependencyPropertyMgr::UnsetValue.GetType()) ?
        GetDefaultValue(_property) :
        oldValue;
    auto const& _newValue = (newValue.GetType() == DependencyPropertyMgr::UnsetValue.GetType()) ?
        GetDefaultValue(_property) :
        newValue;

    if ((!_oldValue.GetBase() && _newValue.GetBase()) || !_oldValue.IsFastEqual(_newValue))
    {
        RaisePropertyChanged(_property, _oldValue, _newValue, (XUI::Core::Dependency::BindingLevel)sender->ValueLevel);
    
        XamlLogVerbose(
            DependencyPropertyMgr::LogNamedCategories::Property,
            nullptr,
            "%s changed from {$Old} to {$Value} with priority {Priority}",
            _property->Name.c_str(),
            _oldValue,
            _newValue,
            sender->ValueLevel);
    }
}

void XUI::Core::Dependency::DependencyObject::BindingNotificationReceived(BindValue* sender, DataBind::BindingNotification* notification)
{
    UpdateDataValidation(sender->Property, notification);
}

std::shared_ptr<XamlCPP::Core::Dependency::GenericObservable> XUI::Core::Dependency::DependencyObject::GetObservable(XamlCPP::Core::Dependency::IPropertyObject * _property)
{
    //std::weak_ptr<XamlCPP::Core::Dependency::IPropertyObject> propertyPtr = _property->shared_from_this();
    std::weak_ptr<XUI::Core::Dependency::DependencyObject> instance = ToSharedPtr();
    System::ThrowIfFailed<System::NotSupportedException>(!instance.expired());
    System::ThrowIfFailed<System::NotSupportedException>(_property);
    //System::ThrowIfFailed<System::NotSupportedException>(GetRegistered(_property));

    return std::make_shared<XamlCPP::Core::Dependency::GenericObservable>(rxcpp::dynamic_observable<System::Reflection::meta::Variant>(
        [this, instance, property = _property](rxcpp::subscriber<System::Reflection::meta::Variant> const& observer)
    {
        //auto propertyObject = property; //property.lock().get();
        auto value = GetValue(property);
        observer.on_next(value);

        System::Events::IEventHook* handler = DependencyPropertyChanged += [lproperty = property, lobserver = observer](void* sender, XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const& e)
        {
            if (e.Property == lproperty) //lproperty.lock().get())
            {
                lobserver.on_next(const_cast<System::Reflection::meta::Variant&>(e.NewValue));
            }
        };

        observer.add(rxcpp::make_subscription([lhandler = handler, ldependencyObject = instance]()
        {
            if (!ldependencyObject.expired())
                ldependencyObject.lock()->DependencyPropertyChanged -= lhandler;
        }));
    }));
}

std::shared_ptr<XamlCPP::Core::Dependency::GenericObservable> XUI::Core::Dependency::DependencyObject::GetUnsafeObservable(XamlCPP::Core::Dependency::IPropertyObject* _property)
{
    System::ThrowIfFailed<System::NotSupportedException>(_property);
    //System::ThrowIfFailed<System::NotSupportedException>(GetRegistered(_property));

    return std::make_shared<XamlCPP::Core::Dependency::GenericObservable>(rxcpp::dynamic_observable<System::Reflection::meta::Variant>(
        [this, instance = this, property = _property](rxcpp::subscriber<System::Reflection::meta::Variant> const& observer)
    {
        auto value = GetValue(property);
        observer.on_next(value);

        System::Events::IEventHook* handler = DependencyPropertyChanged += [lproperty = property, lobserver = observer](void* sender, XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const& e)
        {
            if (e.Property == lproperty)
            {
                lobserver.on_next(const_cast<System::Reflection::meta::Variant&>(e.NewValue));
            }
        };

        observer.add(rxcpp::make_subscription([lhandler = handler, ldependencyObject = instance]()
        {
            ldependencyObject->DependencyPropertyChanged -= lhandler;
        }));
    }));
}

std::shared_ptr<XamlCPP::Core::Dependency::GenericObservable> XUI::Core::Dependency::DependencyObject::GetRelativeObservable(XamlCPP::Markup::RelativeSource * _source)
{
	return std::make_shared<XamlCPP::Core::Dependency::GenericObservable>(rxcpp::dynamic_observable<System::Reflection::meta::Variant>(
		[this, instance = this, relativeSource = _source](rxcpp::subscriber<System::Reflection::meta::Variant> const& observer)
	{
		DependencyObject* source = nullptr;
		if (relativeSource->Mode == RelativeSourceMode::FindAncestor)
		{
            auto _event = [this, lrelativeSource = *relativeSource, lobserver = observer](void* sender, System::Events::EventArg& e)
            {
                if (lrelativeSource.Mode == RelativeSourceMode::FindAncestor)
                {
                    DependencyObject* source = FindInheritanceAncestor(lrelativeSource.AncestorType);
                    lobserver.on_next(source);
                }
            };

            System::Events::IEventHook* handler =
                Is<UI::Controls::Control>() ? 
                static_cast<UI::Controls::Control*>(this)->ChangedLogicalTree += _event :
                InheritanceParentChanged += _event;
			
			observer.add(rxcpp::make_subscription([lhandler = handler, ldependencyObject = instance]()
			{
				ldependencyObject->InheritanceParentChanged -= lhandler;
			}));

			source = FindInheritanceAncestor(relativeSource->AncestorType);
		}

		observer.on_next(source);
	}));
	return std::shared_ptr<XamlCPP::Core::Dependency::GenericObservable>();
}

std::shared_ptr<XamlCPP::Core::Dependency::GenericObservable> XUI::Core::Dependency::DependencyObject::GetObservable(std::function<System::Events::IEventHook*(rxcpp::subscriber<System::Reflection::meta::Variant> const&)> const& addEvent, std::function<void(System::Events::IEventHook*)> const& removeEvent)
{
    std::weak_ptr<XUI::Core::Dependency::DependencyObject> instance = ToSharedPtr();
    System::ThrowIfFailed<System::NotSupportedException>(!instance.expired());
    System::ThrowIfFailed<System::NotSupportedException>(addEvent != nullptr);
    System::ThrowIfFailed<System::NotSupportedException>(removeEvent != nullptr);

    return std::make_shared<XamlCPP::Core::Dependency::GenericObservable>(rxcpp::dynamic_observable<System::Reflection::meta::Variant>(
        [this, instance, event_add = addEvent, event_remove = removeEvent](rxcpp::subscriber<System::Reflection::meta::Variant> const& observer)
    {
        System::Events::IEventHook* handler = event_add(observer);
        System::ThrowIfFailed<System::NotSupportedException>(handler);

        observer.add(rxcpp::make_subscription([lhandler = handler, levent_remove = event_remove]()
        {
            levent_remove(lhandler);
        }));
    }
    ));
}

/// <summary>
/// Clears a <see cref="DependencyProperty"/>'s local value.
/// </summary>
/// <param name="property">The property.</param>
void XUI::Core::Dependency::DependencyObject::ClearValue(DependencyProperty * _property)
{
    System::ThrowIfFailed<System::ArgumentNullException>(_property != nullptr);
    //VerifyAccess();

    SetValue(_property, DependencyPropertyMgr::UnsetValue);
}

void XUI::Core::Dependency::DependencyObject::CoerceValue(DependencyProperty * _property)
{
    if (_property->IsDirect)
    {
        auto baseValue = _property->GetValue(this);
        auto value = _property->GetMetadata(GetType())->CoerceValue(this, baseValue);
        if (!baseValue.IsFastEqual(value))
            SetValue(_property, value);
    }
    else
    {
        auto baseValue = GetValueInternal(_property);
        auto value = _property->GetMetadata(GetType())->CoerceValue(this, baseValue);
        if (!baseValue.IsFastEqual(value))
            SetValue(_property, value);
    }
}

bool XUI::Core::Dependency::DependencyObject::IsDefaultValue(DependencyProperty * _property)
{
    if (_property->IsDirect)
        return _property->GetValue(this) == _property->ToIDirectPropertyAccessor()->GetDefaultValue(GetType());

    auto iValue = _values.find(_property);
    if (iValue != _values.end())
        return false;

    if (_property->Inherits && _inheritanceParent != nullptr)
        return _inheritanceParent->IsDefaultValue(_property);

    return true;
}

void XUI::Core::Dependency::DependencyObject::SetInheritanceParent(XUI::Core::Dependency::DependencyObject * parent)
{
    if (_inheritanceParent.get() != parent)
    {
        if (_inheritanceParent)
            _inheritanceParent->DependencyPropertyChanged -= System::make_eventhandler(&DependencyObject::ParentPropertyChanged, shared_from_base_static<XUI::Core::Dependency::DependencyObject>());

        struct PropertyValueCache
        {
            PropertyValueCache(Core::Dependency::DependencyProperty* property, System::Reflection::Variant const& value)
                : Property(property)
                , value(std::move(value))
            { }

            Core::Dependency::DependencyProperty* Property;
            System::Reflection::Variant value;
        };

        std::vector<PropertyValueCache> inherited;
        inherited.reserve(32);
        auto type = GetType();

        auto const& registeredProperties = Dependency::DependencyPropertyMgr::Instance()->GetInheritRegistered(type);
        for (auto const& p : registeredProperties)
        {
            if (p.second->Inherits)
                inherited.emplace_back(static_cast<Core::Dependency::DependencyProperty*>(p.second), GetValue(p.second));
        }

        auto TemplateParentProperty = Dependency::DependencyPropertyMgr::Instance()->FindRegistered(type, XUI::UI::Controls::UIElement::TemplatedParentProperty.get());
        if (TemplateParentProperty)
            inherited.emplace_back(static_cast<Core::Dependency::DependencyProperty*>(TemplateParentProperty), GetValue(TemplateParentProperty));

        _inheritanceParent = parent ? parent->shared_from_base_dynamic<XUI::Core::Dependency::DependencyObject>() : nullptr;
        OnInheritanceParentChanged();

        for (auto const& p : inherited)
        {
            System::Reflection::Variant newValue = GetValue(p.Property);

            if (!p.value.IsFastEqual(newValue))
                RaisePropertyChanged(p.Property, p.value, newValue);
        }

        if (_inheritanceParent)
            _inheritanceParent->DependencyPropertyChanged += System::make_eventhandler(&DependencyObject::ParentPropertyChanged, shared_from_base_static<XUI::Core::Dependency::DependencyObject>());
    }
}

void XUI::Core::Dependency::DependencyObject::AddBindingExpression(XamlCPP::Core::Dependency::IPropertyObject * dp, XamlCPP::DataBind::InstancedBinding * bind)
{
    if (dp->IsDirect)
    {
        _directBindingExpressions[dp].insert(bind->shared_from_base_static<XamlCPP::DataBind::InstancedBinding>());
    }
    else
    {
        GetValueSource(dp)->AddBindingExpression(bind);
    }
}

void XUI::Core::Dependency::DependencyObject::RemoveBindingExpression(XamlCPP::Core::Dependency::IPropertyObject * dp, XamlCPP::DataBind::InstancedBinding * bind)
{
    if (dp->IsDirect)
    {
        auto itr = _directBindingExpressions.find(dp);
        if (itr != _directBindingExpressions.end())
            itr->second.erase(bind->shared_from_base_static<XamlCPP::DataBind::InstancedBinding>());
    }
    else
    {
        GetValueSource(dp)->AddBindingExpression(bind);
    }
}

XamlCPP::DataBind::InstancedBinding * XUI::Core::Dependency::DependencyObject::GetBindingExpression(XamlCPP::Core::Dependency::IPropertyObject * dp, System::Reflection::Variant const & element) const
{
    return nullptr;
}

std::set<std::shared_ptr<XamlCPP::DataBind::InstancedBinding>> const * XUI::Core::Dependency::DependencyObject::GetBindingExpressions(XamlCPP::Core::Dependency::IPropertyObject * dp) const
{
    if (dp->IsDirect)
    {
        auto itr = _directBindingExpressions.find(dp);
        if (itr != _directBindingExpressions.end())
            return &itr->second;
    }
    else
    {
        return &GetValueSource(dp)->GetBindingExpressions();
    }

    return nullptr;
}

void XUI::Core::Dependency::DependencyObject::ParentPropertyChanged(void* sender, XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
    if (e.Property && e.Property->Inherits && !IsSet(e.Property))
    {
        RaisePropertyChanged(e.Property, e.OldValue, e.NewValue, BindingLevel::LocalValue);
    }
}