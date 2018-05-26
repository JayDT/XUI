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

#ifndef _XUI_IFRAMEBUFFER_H__
#define _XUI_IFRAMEBUFFER_H__

#include "Core/Media/Size_t.h"
#include "System/Image/ImageSource.h"

#include <vector>
#include <string>

namespace XUI::Interfaces
{
    /// <summary>
    /// Defines the platform-specific interface for a <see cref="Avalonia.Media.Imaging.Bitmap"/>.
    /// </summary>
    struct Meta(Enable) IFramebuffer
    {
        /// <summary>
        /// Address of the first pixel
        /// </summary>
        virtual uint8_t* Address() = 0;

        /// <summary>
        /// Framebuffer width
        /// </summary>
        virtual int Width() = 0;
        
        /// <summary>
        /// Framebuffer height
        /// </summary>
        virtual int Height() = 0;
        
        /// <summary>
        /// Number of bytes per row
        /// </summary>
        virtual int RowBytes() = 0;
        
        /// <summary>
        /// DPI of underling screen
        /// </summary>
        virtual XUI::Core::Media::Size Dpi() = 0;
        
        /// <summary>
        /// Pixel format
        /// </summary>
        virtual System::Imaging::PixelFormat Format() = 0;
    };
}

#endif // !_XUI_IFRAMEBUFFER_H__
