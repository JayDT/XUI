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

#ifndef _XUI_IDRAWING_CONTEXT_H__
#define _XUI_IDRAWING_CONTEXT_H__

#include "Core/Media/MediaTypes.h"
#include "standard/BasicPrimities.h"

#include <vector>
#include <string>

#include <boost/uuid/uuid.hpp>

#undef DrawText
#undef LoadBitmap

namespace System::Imaging
{
    enum class PixelFormat : uint8;
}


namespace XUI::Core::Media
{
    class Pen;
}

namespace XUI::Interfaces
{
    struct IBrush;
    struct IFormattedText;
    struct IGeometry;
    struct IBitmapImpl;
    struct PlatformHandle;

    /// <summary>
    /// Defines the interface through which drawing occurs.
    /// </summary>
    struct IDrawingContextImpl : System::Reflection::Object
    {
        //// {157D7A0B-69AD-4E1D-83E9-8549D5767B12}
        static constexpr boost::uuids::uuid __uuid = { 0x15, 0x7d, 0x7a, 0x0b, 0x69, 0xad, 0x4e, 0x1d, 0x83, 0xe9, 0x85, 0x49, 0xd5, 0x76, 0x7b, 0x12 };

        virtual ~IDrawingContextImpl() {}

        virtual void BeginScene() = 0;
        virtual void EndScene() = 0;
        virtual void SetCursor(Interfaces::PlatformHandle const& cursor) = 0;

        /// <summary>
        /// Loads a bitmap implementation from a file..
        /// </summary>
        /// <param name="fileName">The filename of the bitmap.</param>
        /// <returns>An <see cref="IBitmapImpl"/>.</returns>
        virtual std::shared_ptr<XUI::Interfaces::IBitmapImpl> LoadBitmap(std::string const& fileName) = 0;

        /// <summary>
        /// Loads a bitmap implementation from a file..
        /// </summary>
        /// <param name="stream">The stream to read the bitmap from.</param>
        /// <returns>An <see cref="IBitmapImpl"/>.</returns>
        virtual std::shared_ptr<XUI::Interfaces::IBitmapImpl> LoadBitmap(std::vector<byte> const& stream) = 0;

        /// <summary>
        /// Loads a bitmap implementation from a pixels in memory..
        /// </summary>
        /// <param name="format">Pixel format</param>
        /// <param name="data">Pointer to source bytes</param>
        /// <param name="width">Bitmap width</param>
        /// <param name="height">Bitmap height</param>
        /// <param name="stride">Bytes per row</param>
        /// <returns>An <see cref="IBitmapImpl"/>.</returns>
        virtual std::shared_ptr<XUI::Interfaces::IBitmapImpl> LoadBitmap(System::Imaging::PixelFormat format, byte* data, int width, int height, int stride) = 0;

        /// <summary>
        /// Gets or sets the current transform of the drawing context.
        /// </summary>
        PROPERTY_(Core::Media::Matrix, Transform);
        virtual Core::Media::Matrix const& PROPERTY_GET(Transform)() const = 0;
        virtual void PROPERTY_SET(Transform)(Core::Media::Matrix const&) = 0;

        /// <summary>
        /// Clears the render target to the specified color.
        /// </summary>
        /// <param name="color">The color.</param>
        virtual void Clear(Core::Media::Color const& color) = 0;

        /// <summary>
        /// Draws a bitmap image.
        /// </summary>
        /// <param name="source">The bitmap image.</param>
        /// <param name="opacity">The opacity to draw with.</param>
        /// <param name="sourceRect">The rect in the image to draw.</param>
        /// <param name="destRect">The rect in the output to draw to.</param>
        virtual void DrawImage(IBitmapImpl* source, double opacity, Core::Media::Rect const& sourceRect, Core::Media::Rect const& destRect) = 0;

        /// <summary>
        /// Draws a line.
        /// </summary>
        /// <param name="pen">The stroke pen.</param>
        /// <param name="p1">The first point of the line.</param>
        /// <param name="p2">The second point of the line.</param>
        virtual void DrawLine(Core::Media::Pen const& pen, Core::Media::Point const& p1, Core::Media::Point const& p2) = 0;

        /// <summary>
        /// Draws a geometry.
        /// </summary>
        /// <param name="brush">The fill brush.</param>
        /// <param name="pen">The stroke pen.</param>
        /// <param name="geometry">The geometry.</param>
        virtual void DrawGeometry(IBrush* brush, Core::Media::Pen const& pen, IGeometry* geometry) = 0;

        /// <summary>
        /// Draws the outline of a rectangle.
        /// </summary>
        /// <param name="pen">The pen.</param>
        /// <param name="rect">The rectangle bounds.</param>
        /// <param name="cornerRadius">The corner radius.</param>
        virtual void DrawRectangle(Core::Media::Pen const& pen, Core::Media::Rect const& rect, float cornerRadius = 0.0f) = 0;

        /// <summary>
        /// Draws text.
        /// </summary>
        /// <param name="foreground">The foreground brush.</param>
        /// <param name="origin">The upper-left corner of the text.</param>
        /// <param name="text">The text.</param>
        virtual void DrawText(IBrush* foreground, Core::Media::Point const& origin, IFormattedText* text) = 0;

        /// <summary>
        /// Draws a filled rectangle.
        /// </summary>
        /// <param name="brush">The brush.</param>
        /// <param name="rect">The rectangle bounds.</param>
        /// <param name="cornerRadius">The corner radius.</param>
        virtual void FillRectangle(IBrush* brush, Core::Media::Rect const& rect, float cornerRadius = 0.0f) = 0;

        /// <summary>
        /// Pushes a clip rectange.
        /// </summary>
        /// <param name="clip">The clip rectangle.</param>
        virtual void PushClip(Core::Media::Rect const& clip) = 0;

        virtual void PopClip() = 0;

        /// <summary>
        /// Pushes an opacity value.
        /// </summary>
        /// <param name="opacity">The opacity.</param>
        virtual void PushOpacity(double opacity) = 0;
        virtual void PopOpacity() = 0;

        virtual void PushOpacityMask(IBrush* mask, Core::Media::Rect const& bounds) = 0;
        virtual void PopOpacityMask() = 0;

        virtual void PushTransform(Core::Media::Matrix const& mat, bool relative) = 0;
        virtual void PopTransform() = 0;

        /// <summary>
        /// Pushes a clip geometry.
        /// </summary>
        /// <param name="clip">The clip geometry.</param>
        virtual void PushGeometryClip(IGeometry* clip) = 0;

        virtual void PopGeometryClip() = 0;
    };
}

#endif //!_XUI_IDRAWING_CONTEXT_H__ 