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

#ifndef __XUI_ISETTER_H__
#define __XUI_ISETTER_H__

#include <System/System.h>
#include <standard/collection.h>

#include <rxcpp/rx.hpp>

#undef FindResource

namespace XamlCPP::Interfaces
{
    struct IStyle;
    struct IStyleable;

    /// <summary>
    /// Represents a setter for a <see cref="Style"/>.
    /// </summary>
    struct ISetter : public System::Reflection::Object
    {
        /// <summary>
        /// Applies the setter to a control.
        /// </summary>
        /// <param name="style">The style that is being applied.</param>
        /// <param name="control">The control.</param>
        /// <param name="activator">An optional activator.</param>
        virtual rxcpp::subscription Apply(IStyle *style, IStyleable *control, std::shared_ptr<rxcpp::subjects::subject<bool>> const& activator) = 0;
    };
}


#endif	//#ifndef __XUI_ISETTER_H__
