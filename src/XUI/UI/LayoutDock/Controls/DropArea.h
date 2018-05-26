/*
* Copyright (C) 2017-2018 Tauri JayD <https://www.>
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

#ifndef __XUI_UI_LAYOUT_DOCK_LAYOUT_DROP_AREA_H__
#define __XUI_UI_LAYOUT_DOCK_LAYOUT_DROP_AREA_H__

#include "UI/LayoutDock/DockingDefines.h"

namespace XUI::UI
{
    template<class TElement>
    class DropArea : public UI::IDropArea
    {
    private:
        Core::Media::Rect _detectionRect;

    public:
        DropArea(std::shared_ptr<TElement> const& areaElement, DropAreaType type)
        {
            AreaElement = areaElement;
            _detectionRect = areaElement->GetTransformedBounds();
            Type = type;
        }

        Core::Media::Rect GetDetectionRect() override
        {
            return _detectionRect;
        }

        std::shared_ptr<TElement> AreaElement;
    };
}

#endif //!__XUI_UI_LAYOUT_DOCK_LAYOUT_DROP_AREA_H__