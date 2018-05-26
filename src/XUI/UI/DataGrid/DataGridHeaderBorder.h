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

#ifndef __XUI_UI_DATAGRID_HEADER_BORDER_H__
#define __XUI_UI_DATAGRID_HEADER_BORDER_H__

#include "UI/Border.h"
#include "UI/DataGrid/DataGridDefines.h"

namespace XUI::UI
{
    class DataGridHeaderBorder : public Border
    {
    public:

        static Core::Dependency::RefDependencyProperty IsHoveredProperty;
        static Core::Dependency::RefDependencyProperty IsPressedProperty;
        static Core::Dependency::RefDependencyProperty IsClickableProperty;
        static Core::Dependency::RefDependencyProperty SortDirectionProperty;
        static Core::Dependency::RefDependencyProperty IsSelectedProperty;
        static Core::Dependency::RefDependencyProperty OrientationProperty;
        static Core::Dependency::RefDependencyProperty SeparatorBrushProperty;
        static Core::Dependency::RefDependencyProperty SeparatorVisibilityProperty;
        static Core::Dependency::RefDependencyProperty ControlBrushProperty;

        static void StaticClassInitializer();

        DataGridHeaderBorder();
        virtual ~DataGridHeaderBorder();

        DEPENDENCY_PROPERTY_(bool, IsHovered)
        DEPENDENCY_PROPERTY_(bool, IsPressed)
        DEPENDENCY_PROPERTY_(bool, IsClickable)
        DEPENDENCY_PROPERTY_(UI::ListSortDirection, SortDirection)
        DEPENDENCY_PROPERTY_(bool, IsSelected)
        DEPENDENCY_PROPERTY_(UI::Orientation, Orientation)
        DEPENDENCY_PROPERTY_(std::shared_ptr<Interfaces::IBrush>, SeparatorBrush)
        DEPENDENCY_PROPERTY_(UI::Visibility, SeparatorVisibility)
        DEPENDENCY_PROPERTY_(std::shared_ptr<Interfaces::IBrush>, ControlBrush);


        void Render(Interfaces::IDrawingContextImpl* context) override;

        bool UsingBorderImplementation() const
        {
            return (Background != nullptr) || (BorderBrush != nullptr);
        }

    private:

        void RenderClassic(Interfaces::IDrawingContextImpl* context);

    };
}

#endif //!__XUI_UI_DATAGRID_HEADER_BORDER_H__