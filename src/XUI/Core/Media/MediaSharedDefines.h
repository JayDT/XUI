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

#ifndef __XUI_MEDIA_SHARED_DEFINES_H__
#define __XUI_MEDIA_SHARED_DEFINES_H__

namespace XUI::Core::Media
{
    /// <summary>
    /// Describes how a <see cref="TileBrush"/> is tiled.
    /// </summary>
    enum class TileMode
    {
        /// <summary>
        /// A single repeat of the content will be displayed.
        /// </summary>
        None,

        /// <summary>
        /// The content will be repeated horizontally, with alternate tiles mirrored.
        /// </summary>
        FlipX,

        /// <summary>
        /// The content will be repeated vertically, with alternate tiles mirrored.
        /// </summary>
        FlipY,

        /// <summary>
        /// The content will be repeated horizontally and vertically, with alternate tiles mirrored.
        /// </summary>
        FlipXY,

        /// <summary>
        /// The content will be repeated.
        /// </summary>
        Tile
    };

    /// <summary>
    /// Describes how content is positioned horizontally in a container.
    /// </summary>
    enum class AlignmentX
    {
        /// <summary>
        /// The contents align themselves with the left of the container
        /// </summary>
        Left,

        /// <summary>
        /// The contents align themselves with the center of the container
        /// </summary>
        Center,

        /// <summary>
        /// The contents align themselves with the right of the container
        /// </summary>
        Right,
    };

    /// <summary>
    /// Describes how content is positioned vertically in a container.
    /// </summary>
    enum class AlignmentY
    {
        /// <summary>
        /// The contents align themselves with the top of the container
        /// </summary>
        Top,

        /// <summary>
        /// The contents align themselves with the center of the container
        /// </summary>
        Center,

        /// <summary>
        /// The contents align themselves with the bottom of the container
        /// </summary>
        Bottom,
    };

    /// <summary>
    /// Describes how content is resized to fill its allocated space.
    /// </summary>
    enum class Stretch : uint8
    {
        /// <summary>
        /// The content preserves its original size.
        /// </summary>
        None,

        /// <summary>
        /// The content is resized to fill the destination dimensions. The aspect ratio is not
        /// preserved.
        /// </summary>
        Fill,

        /// <summary>
        /// The content is resized to fit in the destination dimensions while preserving its
        /// native aspect ratio.
        /// </summary>
        Uniform,

        /// <summary>
        /// The content is resized to completely fill the destination rectangle while preserving
        /// its native aspect ratio. A portion of the content may not be visible if the aspect
        /// ratio of the content does not match the aspect ratio of the allocated space.
        /// </summary>
        UniformToFill,
    };
}

#endif //! __XUI_MEDIA_SHARED_DEFINES_H__
