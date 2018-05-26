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

#ifndef __XUI_TILEBRUSH_H__
#define __XUI_TILEBRUSH_H__

#include "Core/Media/Brush.h"
#include "Core/Media/MediaSharedDefines.h"

namespace XUI::Core::Media
{
    /// <summary>
    /// Base class for brushes which display repeating images.
    /// </summary>
    class TC_XUI_API TileBrush : public Brush
    {
    public:
        /// <summary>
        /// Defines the <see cref="AlignmentX"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty AlignmentXProperty;

        /// <summary>
        /// Defines the <see cref="AlignmentY"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty AlignmentYProperty;

        /// <summary>
        /// Defines the <see cref="DestinationRect"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty ViewportProperty;

        /// <summary>
        /// Defines the <see cref="SourceRect"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty ViewboxProperty;

        /// <summary>
        /// Defines the <see cref="Stretch"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty StretchProperty;

        /// <summary>
        /// Defines the <see cref="TileMode"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty TileModeProperty;

        /// <summary>
        /// Gets or sets the horizontal alignment of a tile in the destination.
        /// </summary>
        PROPERTY_(XUI::Core::Media::AlignmentX, AlignmentX);
        XUI::Core::Media::AlignmentX GetAlignmentX() const
        {
            return GetValue<XUI::Core::Media::AlignmentX>(AlignmentXProperty);
        }
        void SetAlignmentX(XUI::Core::Media::AlignmentX value)
        {
            SetValue(AlignmentXProperty, value);
        }

        /// <summary>
        /// Gets or sets the horizontal alignment of a tile in the destination.
        /// </summary>
        PROPERTY_(XUI::Core::Media::AlignmentY, AlignmentY);
        XUI::Core::Media::AlignmentY GetAlignmentY() const
        {
            return GetValue<XUI::Core::Media::AlignmentY>(AlignmentYProperty);
        }
        void SetAlignmentY(XUI::Core::Media::AlignmentY value)
        {
            SetValue(AlignmentYProperty, value);
        }

        /// <summary>
        /// Gets or sets the rectangle on the destination in which to paint a tile.
        /// </summary>
        PROPERTY_(Core::Media::Rect, ViewportRect);
        Core::Media::Rect GetViewportRect() const
        {
            return GetValue<Core::Media::Rect>(ViewportProperty);
        }
        void SetViewportRect(Core::Media::Rect const& value)
        {
            SetValue(ViewportProperty, value);
        }
        
        /// <summary>
        /// Gets or sets the rectangle of the source image that will be displayed.
        /// </summary>
        PROPERTY_(Core::Media::Rect, Viewbox);
        Core::Media::Rect GetViewbox() const
        {
            return GetValue<Core::Media::Rect>(ViewboxProperty);
        }
        void SetViewbox(Core::Media::Rect const& value)
        {
            SetValue(ViewboxProperty, value);
        }

        /// <summary>
        /// Gets or sets a value controlling how the source rectangle will be stretched to fill
        /// the destination rect.
        /// </summary>
        PROPERTY_(XUI::Core::Media::Stretch, Stretch);
        XUI::Core::Media::Stretch GetStretch() const
        {
            return GetValue<XUI::Core::Media::Stretch>(StretchProperty);
        }
        void SetStretch(XUI::Core::Media::Stretch value)
        {
            SetValue(StretchProperty, value);
        }

        /// <summary>
        /// Gets or sets the brush's tile mode.
        /// </summary>
        PROPERTY_(XUI::Core::Media::TileMode, TileMode);
        XUI::Core::Media::TileMode GetTileMode() const
        {
            return GetValue<XUI::Core::Media::TileMode>(TileModeProperty);
        }
        void SetTileMode(const XUI::Core::Media::TileMode &value)
        {
            SetValue(TileModeProperty, value);
        }

        virtual Interfaces::BurshTypes GetTypeId() const override { return Interfaces::BurshTypes::TILE_BRUSH; }
    };
}


#endif	//#ifndef __XUI_TILEBRUSH_H__
