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

#ifndef __BIND_VALUE_LEVEL_H__
#define __BIND_VALUE_LEVEL_H__

#include <System/System.h>
#include "standard/BasicPrimities.h"
#include "standard/Errors.h"
#include "Reflection/Runtime/Variant.h"
#include "Core/Dependencies/DependencyDefines.h"
#include "XamlCPP/Core/Dependencies/IDependencyObject.h"

#include "rxcpp/rx.hpp"

namespace System::Reflection::Assembly
{
    class AssemblyModulXUI;
}

namespace XamlCPP
{
    namespace Core::Dependency
    {
        struct IPropertyObject;
        struct DependencyPropertyChangedEventArgs;
    }

    namespace DataBind
    {
        class BindingNotification;
    }
}

namespace XUI::Core::Dependency
{
    struct BindValue;
    class DependencyObject;
    struct IBindValueOwner;
    class BindValueEntry;

    class BindValueLevel
    {
    private:
        std::shared_ptr<BindValue> _owner;
        //rxcpp::composite_subscription _subscription;

        std::string _description;
        int _level = 0;
        int _nextIndex = 0;
        System::Reflection::meta::Variant _value;
        System::Reflection::meta::Variant _directValue;

        std::list<BindValueEntry*> _bindings;

    public:

        BindValueLevel(BindValue* owner, int level);

        /// <summary>
        /// Gets the <see cref="PriorityBindingEntry.Index"/> value of the active binding, or -1
        /// if no binding is active.
        /// </summary>
        int ActiveBindingIndex = -1;

        /// <summary>
        /// Gets the binding entry index. Later bindings will have higher indexes.
        /// </summary>
        PROPERTY_TG(int, Level);
        int GetLevel() { return _level; }

        /// <summary>
        /// The current value of the binding.
        /// </summary>
        PROPERTY_TG(System::Reflection::meta::Variant&, Value);
        System::Reflection::meta::Variant& GetValue() { return _value; }


        /// <summary>
        /// Gets or sets the direct value for this priority level.
        /// </summary>
        PROPERTY_T(System::Reflection::meta::Variant const&, DirectValue);
        void SetDirectValue(System::Reflection::meta::Variant const& value);
        System::Reflection::meta::Variant const& GetDirectValue() const
        {
            return _directValue;
        }

        /// <summary>
        /// Gets the bindings for the priority level.
        /// </summary>
        PROPERTY_TG(std::list<BindValueEntry*>&, Bindings);
        std::list<BindValueEntry*>& GetBindings() { return _bindings; }

        /// <summary>
        /// Activates the first binding that has a value.
        /// </summary>
        void ActivateFirstBinding();

        rxcpp::subscription Add(XamlCPP::Core::Dependency::GenericObservable* binding, XamlCPP::Core::Dependency::BindingSubject * subject);

        void Changed(BindValueEntry * entry);

        /// <summary>
        /// Called when a priority level encounters an error.
        /// </summary>
        /// <param name="level">The priority level of the changed entry.</param>
        /// <param name="error">The binding error.</param>
        void LevelError(XamlCPP::DataBind::BindingNotification* error);

        void Completed(BindValueEntry* entry);
    };
}


#endif // !__BIND_VALUE_LEVEL_H__
