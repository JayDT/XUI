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

#ifndef __ICURSOR_XUI_H__
#define __ICURSOR_XUI_H__

MetaAssemblyBegin
Meta(modul: ReflInclude("Core/Input/Cursors.h"))
MetaAssemblyEnd

namespace XUI::Core::Input
{
    enum class StandardCursorType : uint8;
}

namespace XUI::Interfaces
{
    struct PlatformHandle;

    /// <summary>
    /// Interface for objects with a <see cref="Description"/>.
    /// </summary>
    struct ICursor : System::Reflection::Object
    {
        virtual void SetCursorType(Core::Input::StandardCursorType) = 0;
        virtual XUI::Interfaces::PlatformHandle& GetPlatformCursor() = 0;
    };
}

#endif // !__ICURSOR_XUI_H__
