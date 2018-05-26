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

#ifndef __XUI_SETTER_H__
#define __XUI_SETTER_H__

#include "XamlCPP/Interfaces/IStyle.h"
#include "XamlCPP/Interfaces/ISetter.h"
#include "XamlCPP/Interfaces/IStyleable.h"
#include "XamlCPP/Core/ReflEx/TypeConversion/BuildInConverters.h"
#include "Core/Dependencies/Models/PropertyPrototype.h"

#undef FindResource

MetaAssemblyBegin
Meta(modul: ReflInclude("UI/Controls/UIElement.h"))
MetaAssemblyEnd

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
        Setter : public XamlCPP::Interfaces::ISetter
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="Setter"/> class.
        /// </summary>
        Setter()
        {
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="Setter"/> class.
        /// </summary>
        /// <param name="property">The property to set.</param>
        /// <param name="value">The property value.</param>
        Setter(XamlCPP::Core::Dependency::IPropertyObject *_property, void *value)
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
        Meta(XamlCPP::ContentAttribute())
        Meta(XamlCPP::DependsOnAttribute("Property"))
        Meta(XamlCPP::DependsOnAttribute("TargetName"))
		Meta(XamlCPP::AssignBindingAttribute())
        Meta(XamlCPP::TypeConverterAttribute(typeid(XamlCPP::Core::ReflEx::TypeConversion::SetterTriggerConditionalValueConverter)))
        PROPERTY_(System::Reflection::meta::Variant&, Value);
        System::Reflection::meta::Variant const& GetValue() const { return _value; }
        void SetValue(System::Reflection::meta::Variant const& value) { _value = value; }

        PROPERTY_AUTO_QT(std::string, TargetName)

        uint32 _gid = 0;
        PROPERTY_T(uint32, GID)
        uint32 GetGID() const { return _gid; }
        void SetGID(uint32 value) { _gid = value; }

        /// <summary>
        /// Applies the setter to a control.
        /// </summary>
        /// <param name="style">The style that is being applied.</param>
        /// <param name="control">The control.</param>
        /// <param name="activator">An optional activator.</param>
        rxcpp::subscription Apply(XamlCPP::Interfaces::IStyle *style, XamlCPP::Interfaces::IStyleable *control, std::shared_ptr<rxcpp::subjects::subject<bool>> const& activator) override;

    private:

        XamlCPP::Core::Dependency::IPropertyObject * _property = nullptr;
        System::Reflection::Variant _value;
        XamlCPP::Core::Dependency::GenericSubject _setterSubject;
        std::shared_ptr<XamlCPP::Core::Dependency::GenericObservable> _setterObserver;

        std::shared_ptr<XamlCPP::DataBind::InstancedBinding> SetterClone(XamlCPP::DataBind::InstancedBinding *sourceInstance, XamlCPP::Interfaces::IStyle *style, std::shared_ptr<rxcpp::subjects::subject<bool>> const& activator);
    };
}


#endif	//#ifndef __XUI_SETTER_H__
