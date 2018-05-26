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
#include "Reflection/Runtime/CXXReflection.h"
#include "BindValueEntry.h"
#include "BindValueLevel.h"

using namespace XUI::Core;

XUI::Core::Dependency::BindValueLevel::BindValueLevel(BindValue * owner, int level)
{
    _owner = owner->shared_from_this();
    _level = level;
}


/// <summary>
/// Gets or sets the direct value for this priority level.
/// </summary>

void XUI::Core::Dependency::BindValueLevel::SetDirectValue(System::Reflection::meta::Variant const & value)
{
    _value = _directValue = value;
    _owner->LevelValueChanged(*this);
}

/// <summary>
/// Activates the first binding that has a value.
/// </summary>
void XUI::Core::Dependency::BindValueLevel::ActivateFirstBinding()
{
    for (auto const& binding : Bindings)
    {
        if (binding->Value != DependencyPropertyMgr::UnsetValue)
        {
            _value = binding->Value;
            ActiveBindingIndex = binding->Index;
            _owner->LevelValueChanged(*this);
            return;
        }
    }

    _value = DirectValue;
    ActiveBindingIndex = -1;
    _owner->LevelValueChanged(*this);
}

rxcpp::subscription XUI::Core::Dependency::BindValueLevel::Add(XamlCPP::Core::Dependency::GenericObservable * binding, XamlCPP::Core::Dependency::BindingSubject * subject)
{
    System::ThrowIfFailed<System::ArgumentNullException>(binding != nullptr || subject != nullptr);

    auto entry = new BindValueEntry(this, _nextIndex++);
    Bindings.push_front(entry);

    if (subject)
        entry->Start(subject);
    else
        entry->Start(binding);

    return rxcpp::make_subscription([this, Index = _nextIndex - 1, entry]()
    {
        for (auto ibind = Bindings.begin(); ibind != Bindings.end(); ++ibind)
        {
            if ((*ibind) == entry)
            {
                delete *ibind;
                Bindings.erase(ibind);
                break;
            }
        }

        if (Index >= ActiveBindingIndex)
            ActivateFirstBinding();
    });
}

void XUI::Core::Dependency::BindValueLevel::Changed(BindValueEntry * entry)
{
    if (entry->Index >= ActiveBindingIndex)
    {
        if (entry->Value != DependencyPropertyMgr::UnsetValue)
        {
            _value = entry->Value;
            ActiveBindingIndex = entry->Index;
            _owner->LevelValueChanged(*this);
        }
        else
        {
            ActivateFirstBinding();
        }
    }
}


/// <summary>
/// Called when a priority level encounters an error.
/// </summary>
/// <param name="level">The priority level of the changed entry.</param>
/// <param name="error">The binding error.</param>

void XUI::Core::Dependency::BindValueLevel::LevelError(XamlCPP::DataBind::BindingNotification * error)
{
    System::Reflection::meta::Variant source(_owner, ReferenceVariantPolicy);
    XamlCPP::Platform::ILog::Error(
        DependencyPropertyMgr::LogNamedCategories::Binding,
        &source,
        "Error in binding to %s.%s : {Message}",
        typeid(_owner).name(),
        _owner->Property->Name.c_str(),
        error && error->Error ? error->Error->what() : "no message");
}

void XUI::Core::Dependency::BindValueLevel::Completed(BindValueEntry * entry)
{
    for (auto iEntry = Bindings.begin(); iEntry != Bindings.end(); ++iEntry)
    {
        if ((*iEntry) == entry)
        {
            delete *iEntry;
            Bindings.erase(iEntry);
            break;
        }
    }

    if (entry->Index >= ActiveBindingIndex)
    {
        ActivateFirstBinding();
    }
}
