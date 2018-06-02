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

#ifndef __XUI_MULTIBINDING_H__
#define __XUI_MULTIBINDING_H__

#include <System/System.h>
#include <standard/misc.h>
#include "Interfaces/IMarkupExtension.h"
#include "Interfaces/IValueConverter.h"
#include "XamlCPP/DataBind/RelativeSource.h"
#include "XamlCPP/Core/Dependencies/IDependencyObject.h"
#include "XamlCPP/Interfaces/IBinding.h"
#include "XamlCPP/DataBind/InstancedBinding.h"

namespace XamlCPP
{
    namespace DataBind
    {
        class InstancedBinding;
    }
}

namespace XamlCPP::Markup
{
    /// <summary>
    /// A XAML binding that calculates an aggregate value from multiple child <see cref="Bindings"/>.
    /// </summary>
    class TC_XAMLCPP_API MultiBinding : public Interfaces::IBinding
    {
	private:
		XamlCPP::BindingLevel _priority = XamlCPP::BindingLevel::LocalValue;

    public:
        /// <summary>
        /// Gets the collection of child bindings.
        /// </summary>
        std::vector<std::shared_ptr<Interfaces::IBinding>> Bindings;

        /// <summary>
        /// Gets or sets the <see cref="IValueConverter"/> to use.
        /// </summary>
       Interfaces::IMultiValueConverter *Converter = nullptr;

        /// <summary>
        /// Gets or sets the value to use when the binding is unable to produce a value.
        /// </summary>
        System::Reflection::Variant FallbackValue;

        /// <summary>
        /// Gets or sets the binding mode.
        /// </summary>
        BindingMode Mode = XamlCPP::BindingMode::Default;;

        /// <summary>
        /// Gets or sets the binding priority.
        /// </summary>
		PROPERTY_(XamlCPP::BindingLevel, BindingLevel);
		XamlCPP::BindingLevel GetBindingLevel() const { return _priority; }
		void SetBindingLevel(XamlCPP::BindingLevel level) override { _priority = level; }

        /// <summary>
        /// Gets or sets the relative source for the binding.
        /// </summary>
        RelativeSource RelativeSource;

        /// <inheritdoc/>
        std::shared_ptr<DataBind::InstancedBinding> Initiate(Core::Dependency::IDependencyObject *target, Core::Dependency::IPropertyObject *targetProperty, System::Reflection::Object *anchor = nullptr, bool enableDataValidation = false, std::shared_ptr<XamlCPP::Interfaces::INameScope> namedScope = nullptr);

        /// <summary>
        /// Applies a binding subject to a property on an instance.
        /// </summary>
        /// <param name="target">The target instance.</param>
        /// <param name="property">The target property.</param>
        /// <param name="subject">The binding subject.</param>
        void Bind(Core::Dependency::IDependencyObject *target, Core::Dependency::IPropertyObject *_property, Core::Dependency::BindingSubject* subject);

    private:
	    System::Reflection::Variant ConvertValue(std::vector<System::Reflection::Variant>& values,
	                                             const System::Reflection::Type* targetType);
    };
}


#endif	//#ifndef __XUI_MULTIBINDING_H__
