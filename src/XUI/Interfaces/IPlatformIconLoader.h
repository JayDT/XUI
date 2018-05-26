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

#ifndef __IPLATFORM_WINDOW_ICON_INTERFACE_H__
#define __IPLATFORM_WINDOW_ICON_INTERFACE_H__

#undef LoadBitmap
#undef LoadIcon

#include "Core/Media/FontStyle.h"

#include "Interfaces/IBitmap.h"
#include "Interfaces/IGeometry.h"

#include <initializer_list>
#include <boost/uuid/uuid.hpp>

namespace XUI::Interfaces
{
    struct IWindowIconImpl;

    /// <summary>
    /// Defines the main platform-specific interface for the rendering subsystem.
    /// </summary>
    struct IPlatformIconLoader
    {
        //// {30F619BA-7039-44D5-9C9F-FDB051FB4427}
        static constexpr boost::uuids::uuid __uuid = { 0x30, 0xf6, 0x19, 0xba, 0x70, 0x39, 0x44, 0xd5, 0x9c, 0x9f, 0xfd, 0xb0, 0x51, 0xfb, 0x44, 0x27 };

        virtual ~IPlatformIconLoader() {}

        virtual std::shared_ptr<IWindowIconImpl> LoadIcon(const std::string &fileName) = 0;
        virtual std::shared_ptr<IWindowIconImpl> LoadIcon(const std::vector<byte> &stream) = 0;
        virtual std::shared_ptr<IWindowIconImpl> LoadIcon(std::shared_ptr<IBitmapImpl> bitmap) = 0;
    };
}
#endif // !__IPLATFORM_WINDOW_ICON_INTERFACE_H__