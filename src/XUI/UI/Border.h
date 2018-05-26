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

#ifndef __XUI_UI_CONTROLS_BORDER_H__
#define __XUI_UI_CONTROLS_BORDER_H__

#include "UI/Controls/Decorator.h"

namespace XUI::UI
{
    class TC_XUI_API Border : public Controls::Decorator
    {
    public:

        /// <summary>
        /// Defines the <see cref="CornerRadius"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty CornerRadiusProperty;

        static void StaticClassInitializer();
    public:

        Border();
        virtual ~Border();

        /// <summary>
        /// Gets or sets the decorated control.
        /// </summary>
        PROPERTY_(float, CornerRadius);
        float GetCornerRadius() const;
        void SetCornerRadius(float value);

        void Render(Interfaces::IDrawingContextImpl* context) override;

    protected:

        /// <summary>
        /// Measures the control.
        /// </summary>
        /// <param name="availableSize">The available size.</param>
        /// <returns>The desired size of the control.</returns>
        Core::Media::Size MeasureOverride(Core::Media::Size const& availableSize) override;

        /// <summary>
        /// Arranges the control's child.
        /// </summary>
        /// <param name="finalSize">The size allocated to the control.</param>
        /// <returns>The space taken.</returns>
        Core::Media::Size ArrangeOverride(Core::Media::Size const& finalSize) override;
    };
}

#endif // !__XUI_UI_CONTROLS_BORDER_H__