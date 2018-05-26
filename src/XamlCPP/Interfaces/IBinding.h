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

#ifndef __XAMLCPP_IBINDING_H__
#define __XAMLCPP_IBINDING_H__

#include "standard/misc.h"
#include "XamlCPP/Interfaces/IMarkupExtension.h"

namespace XamlCPP
{
    namespace DataBind
    {
        class InstancedBinding;
        class SourceObserver;
    }

    namespace Core::Dependency
    {
        struct IPropertyObject;
        struct IDependencyObject;
    }
}

namespace XamlCPP::Interfaces
{
    struct INameScope;

    /// <summary>
    /// Holds a binding that can be applied to a property on an object.
    /// </summary>
    struct IBinding : System::Reflection::Object, System::EnableSharedFromThisEx<IBinding>
    {
		virtual ~IBinding() {}

		virtual void SetBindingLevel(XamlCPP::BindingLevel level) = 0;

        /// <summary>
        /// Initiates the binding on a target object.
        /// </summary>
        /// <param name="target">The target instance.</param>
        /// <param name="targetProperty">The target property. May be null.</param>
        /// <param name="anchor">
        /// An optional anchor from which to locate required context. When binding to objects that
        /// are not in the logical tree, certain types of binding need an anchor into the tree in 
        /// order to locate named controls or resources. The <paramref name="anchor"/> parameter 
        /// can be used to provice this context.
        /// </param>
        /// <param name="enableDataValidation">Whether data validation should be enabled.</param>
        /// <returns>
        /// A <see cref="InstancedBinding"/> or null if the binding could not be resolved.
        /// </returns>
        virtual std::shared_ptr<XamlCPP::DataBind::InstancedBinding> Initiate(
            XamlCPP::Core::Dependency::IDependencyObject* target,
            XamlCPP::Core::Dependency::IPropertyObject* targetProperty,
            System::Reflection::meta::Object* anchor = nullptr,
            bool enableDataValidation = false,
            std::shared_ptr<XamlCPP::Interfaces::INameScope>* namedScope = nullptr) = 0;
    };
}

#endif // !__XAMLCPP_IBINDING_H__
