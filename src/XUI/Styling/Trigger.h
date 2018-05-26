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

#ifndef __XUI_TRIGGER_H__
#define __XUI_TRIGGER_H__

#include "XamlCPP/Interfaces/ITriggerBase.h"
#include "XamlCPP/Interfaces/ISetter.h"
#include "XamlCPP/Interfaces/IStyleable.h"
#include "XamlCPP/Core/ReflEx/TypeConversion/BuildInConverters.h"
#include "Core/Dependencies/Models/PropertyPrototype.h"

#undef FindResource

MetaAssemblyBegin
Meta(modul: ReflInclude("UI/Controls/UIElement.h"))
MetaAssemblyEnd

namespace XUI::UI::Controls
{
    class Control;
}

namespace XamlCPP::DataBind
{
    class InstancedBinding;
}

namespace XUI::Styling
{
    /// <summary>
    /// A setter for a <see cref="Style"/>.
    /// </summary>
    /// <remarks>
    /// A <see cref="Setter"/> is used to set a <see cref="AvaloniaProperty"/> value on a
    /// <see cref="AvaloniaObject"/> depending on a condition.
    /// </remarks>
    struct
        TC_XUI_API
#ifdef __REFLECTION_PARSER__
        Meta(ReflInclude("XamlCPP/DataBind/InstancedBinding.h"))
#endif
        Trigger : public XamlCPP::Interfaces::ITriggerBase
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="Setter"/> class.
        /// </summary>
        Trigger()
        {
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="Setter"/> class.
        /// </summary>
        /// <param name="property">The property to set.</param>
        /// <param name="value">The property value.</param>
        Trigger(XamlCPP::Core::Dependency::IPropertyObject *_property, void *value)
        {
            Property = _property;
            Value = value;
        }

        /// <summary>
        /// Gets or sets the property to set.
        /// </summary>
        PROPERTY_T(XamlCPP::Core::Dependency::IPropertyObject *, Property);
        XamlCPP::Core::Dependency::IPropertyObject * GetProperty() const { return _property; }
        void SetProperty(XamlCPP::Core::Dependency::IPropertyObject * value) { _property = value; }

        /// <summary>
        /// Gets or sets the property value.
        /// </summary>
        Meta(XamlCPP::DependsOnAttribute("Property"))
        Meta(XamlCPP::DependsOnAttribute("SourceName"))
        Meta(XamlCPP::TypeConverterAttribute(typeid(XamlCPP::Core::ReflEx::TypeConversion::SetterTriggerConditionalValueConverter)))
        PROPERTY_(System::Reflection::meta::Variant&, Value);
        System::Reflection::meta::Variant const& GetValue() const { return _value; }
        void SetValue(System::Reflection::meta::Variant const& value) { _value = value; }

        PROPERTY_AUTO_QT(std::string, SourceName)

        /// <summary>
        /// Gets or sets style's setters.
        /// </summary>
        Meta(XamlCPP::ContentAttribute())
        PROPERTY_(System::Collection::Vector<std::shared_ptr<XamlCPP::Interfaces::ISetter>> const&, Setters);
        System::Collection::Vector<std::shared_ptr<XamlCPP::Interfaces::ISetter>> const& GetSetters()
        {
            return _setters;
        }
        void SetSetters(const System::Collection::Vector<std::shared_ptr<XamlCPP::Interfaces::ISetter>> &value)
        {
            for (auto const& v : value.GetNativeEnumerator())
                _setters.Add(v);
        }

        PROPERTY_(System::Collection::Vector<std::shared_ptr<XamlCPP::Interfaces::ITriggerAction>>&, EnterActions);
        System::Collection::Vector<std::shared_ptr<XamlCPP::Interfaces::ITriggerAction>>& GetEnterActions() override { return _enterActions; }
        void SetEnterActions(System::Collection::Vector<std::shared_ptr<XamlCPP::Interfaces::ITriggerAction>> const& value)
        {
            for (auto const& v : value.GetNativeEnumerator())
                _enterActions.Add(v);
        }

        PROPERTY_(System::Collection::Vector<std::shared_ptr<XamlCPP::Interfaces::ITriggerAction>>&, ExitActions);
        System::Collection::Vector<std::shared_ptr<XamlCPP::Interfaces::ITriggerAction>>& GetExitActions() override { return _exitActions; }
        void SetExitActions(System::Collection::Vector<std::shared_ptr<XamlCPP::Interfaces::ITriggerAction>> const& value)
        {
            for (auto const& v : value.GetNativeEnumerator())
                _exitActions.Add(v);
        }

        uint32 _gid = 0;
        PROPERTY_T(uint32, GID)
        uint32 GetGID() const { return _gid; }
        void SetGID(uint32 value) { _gid = value; }

        /// <summary>
        /// Applies the trigger to a control.
        /// </summary>
        /// <param name="style">The style that is being applied.</param>
        /// <param name="control">The control.</param>
        /// <param name="activator">An optional activator.</param>
        rxcpp::subscription Apply(XUI::UI::Controls::UIElement *control) override;

        struct BindElement
        {
            bool _isEntered = false;
            std::shared_ptr<XamlCPP::Core::Dependency::GenericObservable> _triggerObserver;
            std::vector<rxcpp::subscription> _triggerSetterDetach;

            BindElement()
            {
                _triggerSetterDetach.reserve(16);
            }
        };

    private:
        std::unordered_map<XUI::UI::Controls::UIElement *, BindElement> _applied;

        XamlCPP::Core::Dependency::IPropertyObject * _property = nullptr;
        System::Reflection::Variant _value;
        System::Collection::Vector<std::shared_ptr<XamlCPP::Interfaces::ISetter>> _setters;
        System::Collection::Vector<std::shared_ptr<XamlCPP::Interfaces::ITriggerAction>> _enterActions;
        System::Collection::Vector<std::shared_ptr<XamlCPP::Interfaces::ITriggerAction>> _exitActions;
        XamlCPP::Core::Dependency::GenericSubject _triggerSubject;
        std::shared_ptr<XamlCPP::Core::Dependency::GenericObservable> _triggerObserver;
    };
}


#endif	//#ifndef __XUI_TRIGGER_H__
