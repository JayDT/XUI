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

#ifndef __XUI_IMAGE_H__
#define __XUI_IMAGE_H__

#include "UI/Controls/Control.h"
#include "Core/Media/TileBrush.h"
#include "Interfaces/IBitmap.h"

namespace XUI::UI
{
    /// <summary>
    /// Displays a <see cref="Bitmap"/> image.
    /// </summary>
    class TC_XUI_API Image : public Controls::Control
    {
    public:
        /// <summary>
        /// Defines the <see cref="Source"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty SourceProperty;

        /// <summary>
        /// Defines the <see cref="Stretch"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty StretchProperty;

        static void StaticConstructor();

        Image() {}
        virtual ~Image() {}

        /// <summary>
        /// Gets or sets the bitmap image that will be displayed.
        /// </summary>
        PROPERTY_(std::shared_ptr<Interfaces::IBitmapImpl>, Source);
        std::shared_ptr<Interfaces::IBitmapImpl> GetSource() const;
        void SetSource(std::shared_ptr<Interfaces::IBitmapImpl> const& value);

        /// <summary>
        /// Gets or sets a value controlling how the image will be stretched.
        /// </summary>
        PROPERTY_(Core::Media::Stretch, Stretch);
        Core::Media::Stretch GetStretch() const;
        void SetStretch(Core::Media::Stretch value);

        /// <summary>
        /// Renders the control.
        /// </summary>
        /// <param name="context">The drawing context.</param>
        void Render(Interfaces::IDrawingContextImpl * context) override;

    protected:
        /// <summary>
        /// Measures the control.
        /// </summary>
        /// <param name="availableSize">The available size.</param>
        /// <returns>The desired size of the control.</returns>
        Core::Media::Size MeasureOverride(Core::Media::Size const& availableSize) override;
    };
}


#endif	//#ifndef __XUI_IMAGE_H__
