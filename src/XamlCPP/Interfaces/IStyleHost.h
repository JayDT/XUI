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

#ifndef __XUI_ISTYLEHOST_H__
#define __XUI_ISTYLEHOST_H__

#include <System/System.h>
#include <standard/collection.h>

#undef FindResource

namespace XamlCPP::Interfaces
{
    struct IStyle;

    typedef System::Collection::List<std::shared_ptr<IStyle>> StylesList;

    /// <summary>
    /// Defines an element that has a <see cref="Styles"/> collection.
    /// </summary>
    struct Meta(Enable) IStyleHost
    {
        /// <summary>
        /// Gets the styles for the element.
        /// </summary>
        virtual StylesList& GetStyles() = 0;

        /// <summary>
        /// Gets the parent style host element.
        /// </summary>
        virtual IStyleHost *GetStylingParent() = 0;

        System::Reflection::Variant FindStyleResource(std::string name);
    };
}


#endif	//#ifndef __XUI_ISTYLEHOST_H__
