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

#ifndef __XAMLCPP_ISTYLE_H__
#define __XAMLCPP_ISTYLE_H__

#include <System/System.h>
#include <standard/misc.h>

#undef FindResource

namespace XamlCPP::Interfaces
{
    struct IStyleable;
    struct IStyleHost;

    /// <summary>
    /// Defines the interface for styles.
    /// </summary>
    struct IStyle : public System::Reflection::Object, System::EnableSharedFromThisEx<IStyle>
    {
        /// <summary>
        /// Attaches the style to a control if the style's selector matches.
        /// </summary>
        /// <param name="control">The control to attach to.</param>
        /// <param name="container">
        /// The control that contains this style. May be null.
        /// </param>
        virtual void Attach(IStyleable *control, IStyleHost *container) = 0;
        virtual void Detach(IStyleable *control) = 0;

        /// <summary>
        /// Tries to find a named resource within the style.
        /// </summary>
        /// <param name="name">The resource name.</param>
        /// <returns>
        /// The resource if found, otherwise <see cref="AvaloniaProperty.UnsetValue"/>.
        /// </returns>
        virtual System::Reflection::Variant const& FindResource(const std::string &name) const = 0;
    };
}


#endif	//#ifndef __XAMLCPP_ISTYLE_H__
