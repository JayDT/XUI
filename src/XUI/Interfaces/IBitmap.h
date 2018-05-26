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

#ifndef _XUI_IBITMAP_H__
#define _XUI_IBITMAP_H__

#include "Interfaces/IFramebuffer.h"

#include <vector>
#include <string>

namespace XUI::Interfaces
{
    struct IDrawingContextImpl;
    struct IVisualBrushRenderer; // not implement yet

    /// <summary>
    /// Defines the platform-specific interface for a <see cref="Avalonia.Media.Imaging.Bitmap"/>.
    /// </summary>
    struct IBitmapImpl : System::Reflection::Object
    {
        /// <summary>
        /// Gets the width of the bitmap, in pixels.
        /// </summary>
        virtual int PixelWidth() = 0;

        /// <summary>
        /// Gets the height of the bitmap, in pixels.
        /// </summary>
        virtual int PixelHeight() = 0;

        /// <summary>
        /// Saves the bitmap to a file.
        /// </summary>
        /// <param name="fileName">The filename.</param>
        virtual void Save(std::string const& fileName) = 0;

        /// <summary>
        /// Saves the bitmap to a stream in png format.
        /// </summary>
        /// <param name="stream">The stream.</param>
        virtual void Save(std::vector<uint8_t> const& stream) = 0;

        virtual IFramebuffer* Lock() = 0;
        virtual void Unlock() = 0;
    };

    struct Meta(Enable) IRenderTarget
    {
        /// <summary>
        /// Creates an <see cref="IDrawingContextImpl"/> for a rendering session.
        /// </summary>
        /// <param name="visualBrushRenderer">
        /// A render to be used to render visual brushes. May be null if no visual brushes are
        /// to be drawn.
        /// </param>
        virtual IDrawingContextImpl* CreateDrawingContext(IVisualBrushRenderer* visualBrushRenderer) = 0;

        virtual IBitmapImpl* ToBitmap() = 0;
	};
}

#endif // !_XUI_IBITMAP_H__
