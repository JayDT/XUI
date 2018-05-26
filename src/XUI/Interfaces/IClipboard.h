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

#ifndef __XUI_ICLIPBOARD_H__
#define __XUI_ICLIPBOARD_H__

namespace XUI::Interfaces
{
    struct IClipboard : System::Reflection::Object
    {
        //// {536E0804-782E-4075-85DF-C9C1FADAD58D}
        static constexpr boost::uuids::uuid __uuid = { 0x53, 0x6e, 0x08, 0x04, 0x78, 0x2e, 0x40, 0x75, 0x85, 0xdf, 0xc9, 0xc1, 0xfa, 0xda, 0xd5, 0x8d };

        virtual std::string GetText() = 0;
        virtual void SetText(const std::string &text) = 0;
        virtual void Clear() = 0;
    };
}

#endif	//#ifndef __XUI_ICLIPBOARD_H__
