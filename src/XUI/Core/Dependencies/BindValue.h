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

#ifndef __BIND_VALUE_H__
#define __BIND_VALUE_H__

#include <System/System.h>
#include "standard/BasicPrimities.h"
#include "standard/Errors.h"
#include "Reflection/Runtime/Variant.h"
#include "Core/Dependencies/DependencyDefines.h"
#include "Core/Dependencies/BindValueLevel.h"
#include "XamlCPP/Core/Dependencies/IDependencyObject.h"

#include "rxcpp/rx.hpp"
#include <standard/Preprocessor/pp_platform.h>
#include <map>

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
        class InstancedBinding;
    }
}

namespace XUI::Core::Dependency
{
    struct BindValue;
    class DependencyObject;
    struct IBindValueOwner;
    class BindValueEntry;
    class BindValueLevel;

    struct BindValue : public System::EnableSharedFromThisEx<BindValue>
    {
    public:
        typedef std::function<System::Reflection::meta::Variant(System::Reflection::meta::Variant const&)> ValidateFunction;
    private:
        typedef std::array<BindValueLevel*, size_t(BindingLevel::Max)> LevelMap;

        IBindValueOwner* _owner = nullptr;
        System::Reflection::meta::Type const* _valueType = nullptr;
        System::Reflection::meta::Variant _value;
        ValidateFunction _validate;
        BindingLevel _activeLevel = BindingLevel::Unset;
        XamlCPP::Core::Dependency::IPropertyObject* _property = nullptr;
        LevelMap _levels;
        std::set<std::shared_ptr<XamlCPP::DataBind::InstancedBinding>> _bindingExpressions;

    public:

        /// <summary>
        /// Gets the property that the value represents.
        /// </summary>
        PROPERTY_G(_property, Property);

        /// <summary>
        /// Gets the current value type.
        /// </summary>
        PROPERTY_G(_valueType, ValueType);

        /// <summary>
        /// Gets the current value.
        /// </summary>
        PROPERTY_TG(System::Reflection::meta::Variant const&, Value);
        System::Reflection::meta::Variant const& GetValue() const { return _value; }

        /// <summary>
        /// Gets the priority of the binding that is currently active.
        /// </summary>
        PROPERTY_G(_activeLevel, ValueLevel);

        PROPERTY_TG(LevelMap const&, Level);
        LevelMap const& GetLevel() const { return _levels; }

        BindValue(
            DependencyObject* owner,
            XamlCPP::Core::Dependency::IPropertyObject* property,
            System::Reflection::meta::Type const* valueType,
            ValidateFunction* validate = nullptr);

        virtual ~BindValue();

        rxcpp::subscription Add(XamlCPP::Core::Dependency::GenericObservable* binding, BindingLevel level, XamlCPP::Core::Dependency::BindingSubject * subject = nullptr);

        void UpdateValue(System::Reflection::meta::Variant const& value, BindingLevel level);

        void SetValue(System::Reflection::meta::Variant const& value, BindingLevel level);

        /// <summary>
        /// Gets the currently active bindings on this object.
        /// </summary>
        /// <returns>An enumerable collection of bindings.</returns>
        std::list<BindValueEntry*> GetBindings();

        /// <summary>
        /// Called when the value for a priority level changes.
        /// </summary>
        /// <param name="level">The priority level of the changed entry.</param>
        void LevelValueChanged(BindValueLevel& level);

        /// <summary>
        /// Causes a revalidation of the value.
        /// </summary>
        void Revalidate();

        void AddBindingExpression(XamlCPP::DataBind::InstancedBinding* bind);
        void RemoveBindingExpression(XamlCPP::DataBind::InstancedBinding* bind);
        XamlCPP::DataBind::InstancedBinding* GetBindingExpression(System::Reflection::Variant const& element) const;
        std::set<std::shared_ptr<XamlCPP::DataBind::InstancedBinding>> const& GetBindingExpressions() const { return _bindingExpressions; }

    private:
        /// <summary>
        /// Gets the <see cref="PriorityLevel"/> with the specified priority, creating it if it
        /// doesn't already exist.
        /// </summary>
        /// <param name="priority">The priority.</param>
        /// <returns>The priority level.</returns>
        BindValueLevel* GetLevel(BindingLevel level);
    };
}


#endif // !__BIND_VALUE_H__
