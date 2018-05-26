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

#ifndef __XUI_DELAYEDBINDING_H__
#define __XUI_DELAYEDBINDING_H__

#include "XamlCPP/Core/Dependencies/IDependencyObject.h"
#include "standard/events.h"

namespace XamlCPP::Markup
{
    /// <summary>
    /// Provides delayed bindings for controls.
    /// </summary>
    /// <remarks>
    /// The XAML engine applies its bindings in a delayed manner where bindings are only applied
    /// when a control has finished initializing. This is done because applying bindings as soon
    /// as controls are created means that long-form bindings (i.e. bindings that don't use the
    /// `{Binding}` markup extension but instead use `&lt;Binding&gt;`) don't work, as the binding
    /// is applied to the property before the properties on the `Binding` object are set. Looking 
    /// at WPF it uses a similar mechanism for bindings that come from XAML.
    /// </remarks>
    class TC_XAMLCPP_API DelayedBinding final
    {
    private:
        struct Entry
        {
            virtual ~Entry() {}
            virtual void Apply(Core::Dependency::IDependencyObject*) = 0;
        };

        struct BindingEntry : Entry
        {
            BindingEntry(XamlCPP::Interfaces::IBinding *binding, Core::Dependency::IPropertyObject *_property);
            virtual ~BindingEntry() {}

            std::shared_ptr<XamlCPP::Interfaces::IBinding> Binding;
            Core::Dependency::IPropertyObject *Property;

            virtual void Apply(Core::Dependency::IDependencyObject*);
        };

        struct PropertyValueEntry : Entry
        {
            PropertyValueEntry(std::function<System::Reflection::Variant(Core::Dependency::IDependencyObject *)> const& _value, Core::Dependency::IPropertyObject *_property)
            {
                Value = _value;
                Property = _property;
            }
            virtual ~PropertyValueEntry() {}

            std::function<System::Reflection::Variant(Core::Dependency::IDependencyObject *)> Value;
            Core::Dependency::IPropertyObject *Property;

            virtual void Apply(Core::Dependency::IDependencyObject*);
        };

        static std::unordered_map<Core::Dependency::IDependencyObject*, std::vector<Entry*>> _entries;

    public:
        static void EventApplyBindings(void *sender, System::Events::EventArg& e);

        /// <summary>
        /// Adds a delayed binding to a control.
        /// </summary>
        /// <param name="target">The control.</param>
        /// <param name="property">The property on the control to bind to.</param>
        /// <param name="binding">The binding.</param>
        static void Add(std::shared_ptr<Core::Dependency::IDependencyObject> const& target, Core::Dependency::IPropertyObject *_property, XamlCPP::Interfaces::IBinding *binding);
        static void Add(std::shared_ptr<Core::Dependency::IDependencyObject> const& target, Core::Dependency::IPropertyObject *_property, std::function<System::Reflection::Variant(Core::Dependency::IDependencyObject *)> const& value);

        /// <summary>
        /// Applies any delayed bindings to a control.
        /// </summary>
        /// <param name="control">The control.</param>
        static void ApplyBindings(Core::Dependency::IDependencyObject *control);
    };
}


#endif	//#ifndef __XUI_DELAYEDBINDING_H__
