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

#ifndef __XUI_IRENDER_ROOT_H__
#define __XUI_IRENDER_ROOT_H__

#include "Core/Media/MediaTypes.h"
#include "standard/BasicPrimities.h"
#include <memory>

namespace XUI::Core::Media
{
    struct Size;
}

namespace XUI::UI::Controls
{
    class Control;
    class Visual;
}

namespace XUI::Interfaces
{
    struct IDrawingContextImpl;

    /// <summary>
    /// Interface for objects with a <see cref="Description"/>.
    /// </summary>
    struct Meta(Enable) IRenderRoot
    {
        PURE_VIRTUAL_R_PROPERTY(Core::Media::Size const&, ClientSize);

        virtual void Render(Interfaces::IDrawingContextImpl* context) = 0;
        virtual void AddDirty(UI::Controls::Visual* context) = 0;
        virtual void Invalidate(Core::Media::Rect const& rect) = 0;
        virtual Core::Media::Point PointToClient(Core::Media::Point const& p) = 0;
        virtual Core::Media::Point PointToScreen(Core::Media::Point const& p) = 0;
        virtual UI::Controls::Visual* ToVisual() = 0;
        virtual bool IsRootMode() const { return true; }

        virtual UI::Controls::Control* HitTest(Core::Media::Point const& point, std::function<bool(UI::Controls::Control*)> const& filter) = 0;
        virtual UI::Controls::Control* HitTest(Core::Media::Point const& point, UI::Controls::Control* root, std::function<bool(UI::Controls::Control*)> const& filter) = 0;
        virtual UI::Controls::Visual* HitTestVisualTree(Core::Media::Point const& point, UI::Controls::Visual* root, std::function<bool(UI::Controls::Visual*)> const& filter) = 0;
        virtual void HitTestChain(std::vector<UI::Controls::Visual*>& result, Core::Media::Point const& point, UI::Controls::Visual* root, std::function<bool(UI::Controls::Visual*)> const& filter) = 0;
    };
}

#endif // !__XUI_IRENDER_ROOT_H__
