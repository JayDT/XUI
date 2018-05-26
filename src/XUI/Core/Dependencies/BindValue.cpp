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

#include "Interfaces/IDescription.h"
#include "Core/Dependencies/BindValue.h"
#include "Core/Dependencies/DependencyPropertyMgr.h"
#include "Core/Dependencies/DependencyObservable.h"
#include "Core/Dependencies/DependencyObject.h"
#include "XamlCPP/DataBind/BindingNotification.h"
#include "XamlCPP/Core/Dependencies/IPropertyObject.h"
#include "Core/Observer/IObserver.h"
#include "XamlCPP/Core/TypeExtensions/TypeConverter.h"
#include "XamlCPP/Core/LogInterface.h"
#include "XamlCPP/DataBind/InstancedBinding.h"
#include "Reflection/Runtime/CXXReflection.h"
#include "Core/Dependencies/BindValueEntry.h"
#include "Core/Dependencies/BindValueLevel.h"

#include "Log/Log.h"

using namespace XUI::Core;

XUI::Core::Dependency::BindValue::BindValue(
    DependencyObject * owner,
    XamlCPP::Core::Dependency::IPropertyObject * property,
    System::Reflection::meta::Type const * valueType,
    ValidateFunction* validate)
{
    memset(_levels.data(), 0, sizeof(BindValueLevel*) * _levels.size());

    _owner = owner;
    _property = property;
    _valueType = valueType;
    _value = DependencyPropertyMgr::UnsetValue;
    if (validate)
        _validate = *validate;
}

XUI::Core::Dependency::BindValue::~BindValue()
{
    for (auto& lvl : _levels)
    {
        if (lvl)
            delete lvl;
        lvl = nullptr;
    }
}

rxcpp::subscription XUI::Core::Dependency::BindValue::Add(XamlCPP::Core::Dependency::GenericObservable* binding, BindingLevel level, XamlCPP::Core::Dependency::BindingSubject * subject)
{
    return GetLevel(level)->Add(binding, subject);
}

void XUI::Core::Dependency::BindValue::UpdateValue(System::Reflection::meta::Variant const & value, BindingLevel level)
{
    static auto const& bindingNotificationType = typeof(XamlCPP::DataBind::BindingNotification);

    XamlCPP::DataBind::BindingNotification* notification = nullptr;
    if (bindingNotificationType.IsAssignableFrom(*value.GetType()))
        notification = value.GetValue<XamlCPP::DataBind::BindingNotification*>();

    System::Reflection::meta::Variant* castValue;

    if (notification)
    {
        if (notification->HasValue())
            castValue = &notification->Value;
        else
            castValue = nullptr;
    }
    else
        castValue = const_cast<System::Reflection::meta::Variant*>(&value);

    if (castValue && (XamlCPP::Core::TypeExtensions::TypeConverter::TryCast(_valueType, value, *castValue) || value == DependencyPropertyMgr::UnsetValue))
    {
        if (_validate && *castValue != DependencyPropertyMgr::UnsetValue)
            *castValue = _validate(*castValue);

        _activeLevel = level;
        System::Reflection::meta::Variant old(std::move(_value));
        _value = *castValue;

        if (notification && notification->HasValue())
            notification->SetValue(*castValue);

        if (_owner && (!notification || notification->HasValue()))
            _owner->Changed(this, old, _value);

        if (notification && _owner)
            _owner->BindingNotificationReceived(this, notification);
    }
    else
    {
        System::Console()->WriteErrorLine("Binding produced invalid value for %s (%s): {%s} (%s)",
                                          Property->Name.c_str(),
                                          _valueType->GetFullName(),
                                          value.ToString().c_str(),
                                          value.GetType()->GetFullName());

        System::Reflection::meta::Variant source(_owner, ReferenceVariantPolicy);
        XamlCPP::Platform::ILog::Error(
            DependencyPropertyMgr::LogNamedCategories::Binding,
            &source,
            "Binding produced invalid value for %s (%s): {%s} (%s)",
            Property->Name.c_str(),
            _valueType->GetFullName(),
            value.ToString(),
            value.GetType()->GetFullName());
    }
}

/// <summary>
/// Gets the currently active bindings on this object.
/// </summary>
/// <returns>An enumerable collection of bindings.</returns>

std::list<XUI::Core::Dependency::BindValueEntry*> XUI::Core::Dependency::BindValue::GetBindings()
{
    //for (auto const& level : _levels)
    //{
    //    return level.second.GetBindings();
    //}
    return {};
}

/// <summary>
/// Called when the value for a priority level changes.
/// </summary>
/// <param name="level">The priority level of the changed entry.</param>
void XUI::Core::Dependency::BindValue::LevelValueChanged(BindValueLevel& level)
{
    //if (level.Level <= int(_activeLevel))
    {
        //if (level.Value.GetBase() && level.Value != DependencyPropertyMgr::UnsetValue)
        //{
        //    UpdateValue(level.Value, BindingLevel(level.Level));
        //}
        //else
        //{
            for (auto& lvl : _levels)
            {
                if (lvl && lvl->Value.GetBase() && lvl->Value != DependencyPropertyMgr::UnsetValue)
                {
                    UpdateValue(lvl->Value, BindingLevel(lvl->Level));
                    return;
                }
            }

            UpdateValue(DependencyPropertyMgr::UnsetValue, BindingLevel::Unset);
        //}
    }
}

/// <summary>
/// Causes a revalidation of the value.
/// </summary>

void XUI::Core::Dependency::BindValue::Revalidate()
{
    if (_validate)
    {
        XUI::Core::Dependency::BindValueLevel *& iLevel = _levels[size_t(ValueLevel)];

        if (iLevel)
        {
            UpdateValue(iLevel->Value, BindingLevel(iLevel->Level));
        }
    }
}

void XUI::Core::Dependency::BindValue::AddBindingExpression(XamlCPP::DataBind::InstancedBinding * bind)
{
    _bindingExpressions.insert(bind->shared_from_base_static<XamlCPP::DataBind::InstancedBinding>());
}

void XUI::Core::Dependency::BindValue::RemoveBindingExpression(XamlCPP::DataBind::InstancedBinding * bind)
{
    _bindingExpressions.erase(bind->shared_from_base_static<XamlCPP::DataBind::InstancedBinding>());
}

XamlCPP::DataBind::InstancedBinding * XUI::Core::Dependency::BindValue::GetBindingExpression(System::Reflection::Variant const & element) const
{
    return nullptr;
}

/// <summary>
/// Gets the <see cref="PriorityLevel"/> with the specified priority, creating it if it
/// doesn't already exist.
/// </summary>
/// <param name="priority">The priority.</param>
/// <returns>The priority level.</returns>
XUI::Core::Dependency::BindValueLevel * XUI::Core::Dependency::BindValue::GetLevel(BindingLevel level)
{
    XUI::Core::Dependency::BindValueLevel *& iLevel = _levels[size_t(level)];

    if (!iLevel)
        return iLevel = new BindValueLevel(this, (int)level);

    return iLevel;
}

void XUI::Core::Dependency::BindValue::SetValue(System::Reflection::meta::Variant const& value, BindingLevel level)
{
    GetLevel(level)->DirectValue = value;
}
