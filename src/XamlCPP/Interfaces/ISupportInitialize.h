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

#ifndef __XUI_ISUPPORTINITIALIZE_H__
#define __XUI_ISUPPORTINITIALIZE_H__

#include <System/System.h>
#include <standard/collection.h>
#include <XamlCPP/Interfaces/IStyleHost.h>

#include <rxcpp/rx.hpp>

#undef FindResource

namespace XamlCPP::Interfaces
{
    /// <summary>
    /// Specifies that this object supports a simple, transacted notification for batch 
    /// initialization.
    /// </summary>
    struct Meta(Enable) ISupportInitialize
    {
        /// <summary>
        /// Signals the object that initialization is starting.
        /// </summary>
        virtual void BeginInit() = 0;

        /// <summary>
        /// Signals the object that initialization is complete.
        /// </summary>
        virtual void EndInit() = 0;
    };
}


#endif	//#ifndef __XUI_ISUPPORTINITIALIZE_H__
