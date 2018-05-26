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

#ifndef __BIND_VALUE_ENTRY_H__
#define __BIND_VALUE_ENTRY_H__

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
    class BindValueLevel;

    class BindValueEntry
    {
    private:
        BindValueLevel* _owner = nullptr;
        rxcpp::composite_subscription _subscription;

        XamlCPP::Core::Dependency::GenericObservable* _observable = nullptr;
        std::shared_ptr<XamlCPP::Core::Dependency::BindingSubject> _subject;

        std::string _description;
        int _index = -1;
        System::Reflection::meta::Variant _value;
    public:

        BindValueEntry(BindValueLevel* owner, int index);
        ~BindValueEntry();

        /// <summary>
        /// Gets the observable associated with the entry.
        /// </summary>
        AUTO_PROPERTY(_observable, Observable, );

        /// <summary>
        /// Gets a description of the binding.
        /// </summary>
        AUTO_PROPERTY(_description, Description, );

        /// <summary>
        /// Gets the binding entry index. Later bindings will have higher indexes.
        /// </summary>
        PROPERTY_GET_ONLY(int, Index);
        int GetIndex() const { return _index; }

        /// <summary>
        /// The current value of the binding.
        /// </summary>
        PROPERTY_GET_ONLY(System::Reflection::meta::Variant const&, Value);
        System::Reflection::meta::Variant const& GetValue() const { return _value; }

        /// <summary>
        /// Starts listening to the binding.
        /// </summary>
        /// <param name="binding">The binding.</param>
        void Start(XamlCPP::Core::Dependency::GenericObservable* binding);
        void Start(XamlCPP::Core::Dependency::BindingSubject* binding);

        void ValueChanged(System::Reflection::meta::Variant const& value);

        void Completed();
    };
}


#endif // !__BIND_VALUE_ENTRY_H__
