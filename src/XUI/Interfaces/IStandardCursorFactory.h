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

#ifndef __XUI_ISTANDARDCURSORFACTORY_H__
#define __XUI_ISTANDARDCURSORFACTORY_H__

#include <boost/uuid/uuid.hpp>

MetaAssemblyBegin
Meta(modul: ReflInclude("Core/Input/Cursors.h"))
MetaAssemblyEnd

namespace XUI::Core::Input
{
    enum class StandardCursorType : uint8;
}

namespace XUI::Interfaces
{
    struct PlatformHandle
    {
        PlatformHandle() {}
        PlatformHandle(std::intptr_t handle, std::string descriptor)
        {
            Handle = handle;
            HandleDescriptor = descriptor;
        }

        std::intptr_t Handle = std::intptr_t();
        std::string HandleDescriptor;

        bool operator==(PlatformHandle const& rhs) const
        {
            return Handle == rhs.Handle;
        }
    };

    struct IStandardCursorFactory
    {
        //// {1BC08524-9BB4-44F5-ACB8-EE550A87E831}
        static constexpr boost::uuids::uuid __uuid = { 0x1b, 0xc0, 0x85, 0x24, 0x9b, 0xb4, 0x44, 0xf5, 0xac, 0xb8, 0xee, 0x55, 0xa, 0x87, 0xe8, 0x31 };

        virtual PlatformHandle GetCursor(XUI::Core::Input::StandardCursorType cursorType) = 0;
    };
}

#endif	//#ifndef __XUI_ISTANDARDCURSORFACTORY_H__
