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

#ifndef __IPLATFORM_RENDER_INTERFACE_H__
#define __IPLATFORM_RENDER_INTERFACE_H__

#undef LoadBitmap

#include "Core/Media/FontStyle.h"

#include "Interfaces/IBitmap.h"
#include "Interfaces/IGeometry.h"
#include "Interfaces/IWindowImpl.h"

#include <initializer_list>
#include <boost/uuid/uuid.hpp>

#undef LoadBitmap
#undef CreateWindow

namespace XUI::Core::Media
{
    struct Typeface;
}

namespace XUI::UI
{
    enum class WindowStyle : uint8;
}

namespace XUI::Interfaces
{
    struct IFormattedText;
    struct FormattedTextStyleSpan;

    /// <summary>
    /// Defines the main platform-specific interface for the rendering subsystem.
    /// </summary>
    struct IPlatformRenderInterface
    {
        //// {65FD7EAC-9B49-4C5B-95D8-38A08C1E4003}
        static constexpr boost::uuids::uuid __uuid = { 0x65, 0xfd, 0x7e, 0xac, 0x9b, 0x49, 0x4c, 0x5b, 0x95, 0xd8, 0x38, 0xa0, 0x8c, 0x1e, 0x40, 0x3 };

        virtual ~IPlatformRenderInterface() {}

        /// <summary>
        /// Creates a formatted text implementation.
        /// </summary>
        /// <param name="text">The text.</param>
        /// <param name="typeface">The base typeface.</param>
        /// <param name="textAlignment">The text alignment.</param>
        /// <param name="wrapping">The text wrapping mode.</param>
        /// <param name="constraint">The text layout constraints.</param>
        /// <param name="spans">The style spans.</param>
        /// <returns>An <see cref="IFormattedTextImpl"/>.</returns>
        virtual IFormattedText* CreateFormattedText(
            std::wstring const& text,
            Core::Media::Typeface const& typeface,
            Core::Media::TextAlignment textAlignment,
            Core::Media::TextWrapping wrapping,
            Core::Media::Size const& constraint,
            std::initializer_list<FormattedTextStyleSpan> spans) = 0;

        /// <summary>
        /// Creates a stream geometry implementation.
        /// </summary>
        /// <returns>An <see cref="IStreamGeometryImpl"/>.</returns>
        virtual std::shared_ptr<IStreamGeometry> CreateStreamGeometry() = 0;

        /// <summary>
        /// Creates a renderer.
        /// </summary>
        /// <param name="surfaces">
        /// The list of native platform surfaces that can be used for output.
        /// </param>
        /// <returns>An <see cref="IRenderTarget"/>.</returns>
        virtual std::shared_ptr<IRenderTarget> CreateRenderTarget(std::vector<void*> surfaces) = 0;

        /// <summary>
        /// Creates a render target bitmap implementation.
        /// </summary>
        /// <param name="width">The width of the bitmap.</param>
        /// <param name="height">The height of the bitmap.</param>
        /// <param name="dpiX">The horizontal DPI of the bitmap.</param>
        /// <param name="dpiY">The vertical DPI of the bitmap.</param>
        /// <returns>An <see cref="IRenderTargetBitmapImpl"/>.</returns>
        virtual std::shared_ptr<IRenderTarget> CreateRenderTargetBitmap(
            int width,
            int height,
            double dpiX,
            double dpiY) = 0;

        /// <summary>
        /// Creates a writable bitmap implementation.
        /// </summary>
        /// <param name="width">The width of the bitmap.</param>
        /// <param name="height">The height of the bitmap.</param>
        /// <param name="format">Pixel format (optional).</param>
        /// <returns>An <see cref="IWritableBitmapImpl"/>.</returns>
        virtual std::shared_ptr<IBitmapImpl> CreateWritableBitmap(int width, int height, System::Imaging::PixelFormat format) = 0;

        virtual std::shared_ptr<IWindowImpl> CreateWindow(XUI::UI::WindowStyle state) = 0;
        virtual std::shared_ptr<IWindowImpl> CreateEmbeddableWindow(XUI::UI::WindowStyle state, XUI::Interfaces::IWindowImpl* parent) = 0;
        virtual std::shared_ptr<IWindowImpl> CreatePopup(XUI::UI::WindowStyle state, XUI::Interfaces::IWindowImpl* parent) = 0;
    };
}
#endif // !__IPLATFORM_RENDER_INTERFACE_H__