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

#ifndef __XUI_VISUALBRUSH_H__
#define __XUI_VISUALBRUSH_H__

#include "Core/Media/TileBrush.h"
#include "Core/Media/RelativePoint_t.h"

MetaAssemblyBegin
Meta(modul: ReflInclude("UI/Controls/Visual.h"))
MetaAssemblyEnd

namespace XUI::UI::Controls
{
    class InputElement;
    class Visual;
}

namespace XUI::Core::Media
{
    /// <summary>
    /// Paints an area with an <see cref="IVisual"/>.
    /// </summary>
    class TC_XUI_API VisualBrush : public TileBrush
    {
    public:
        /// <summary>
        /// Defines the <see cref="Visual"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty VisualProperty;

        /// <summary>
        /// Initializes a new instance of the <see cref="VisualBrush"/> class.
        /// </summary>
        VisualBrush()
        {
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="VisualBrush"/> class.
        /// </summary>
        /// <param name="visual">The visual to draw.</param>
        VisualBrush(std::shared_ptr<UI::Controls::Visual> const& visual)
        {
            Visual = visual;
        }

        /// <summary>
        /// Gets or sets the visual to draw.
        /// </summary>
        PROPERTY_(std::shared_ptr<UI::Controls::Visual>, Visual);
        std::shared_ptr<UI::Controls::Visual> GetVisual() const;
        void SetVisual(std::shared_ptr<UI::Controls::Visual> const& value);

        virtual Interfaces::BurshTypes GetTypeId() const override { return Interfaces::BurshTypes::VISUAL_BRUSH; }
        virtual void Render(Interfaces::IDrawingContextImpl* context);
    };
}


#endif	//#ifndef __XUI_VISUALBRUSH_H__
