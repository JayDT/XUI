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

#ifndef __XAMLCPP_STYLERESOURCEBINDING_H__
#define __XAMLCPP_STYLERESOURCEBINDING_H__

#include <System/System.h>
#include <standard/misc.h>
#include "Interfaces/IBinding.h"
#include "Interfaces/IMarkupExtension.h"
#include "Interfaces/IValueConverter.h"
#include "XamlCPP/DataBind/InstancedBinding.h"

namespace XamlCPP::Markup
{
    class StyleResourceBinding : public XamlCPP::Interfaces::IBinding
    {
	private:

		XamlCPP::BindingLevel _bindinglevel = BindingLevel::LocalValue;

    public:
        /// <summary>
        /// Initializes a new instance of the <see cref="StyleResourceBinding"/> class.
        /// </summary>
        /// <param name="name">The resource name.</param>
        StyleResourceBinding(const std::string &name)
        {
            Name = name;
        }

        /// <inheritdoc/>
        BindingMode Mode = BindingMode::OneTime;

        /// <summary>
        /// Gets the resource name.
        /// </summary>
        std::string Name;

		/// <summary>
		/// Gets or sets the binding priority.
		/// </summary>
		PROPERTY_(XamlCPP::BindingLevel, BindingLevel);
		XamlCPP::BindingLevel GetBindingLevel() const { return _bindinglevel; }
		void SetBindingLevel(XamlCPP::BindingLevel level) override { _bindinglevel = level; }

        /// <inheritdoc/>
        std::shared_ptr<DataBind::InstancedBinding> Initiate(Core::Dependency::IDependencyObject *target, Core::Dependency::IPropertyObject *targetProperty, System::Reflection::Object *anchor = nullptr, bool enableDataValidation = false, std::shared_ptr<XamlCPP::Interfaces::INameScope>* namedScope = nullptr);
    };
}


#endif	//#ifndef __XAMLCPP_STYLERESOURCEBINDING_H__
