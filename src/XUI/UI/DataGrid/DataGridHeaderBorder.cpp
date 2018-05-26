/*
*
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

#include "DataGridHeaderBorder.h"
#include "Core/Media/Brush.h"
#include "Core/Media/Colors.h"
#include "Core/Media/Pen.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;
using namespace XUI::Core::Media;
using namespace XUI::UI;

typedef std::vector<void*> ColumnCollection;

STATIC_CLASS_INITIALIZER(DataGridHeaderBorder, XUI::UI::DataGridHeaderBorder::StaticClassInitializer)

Core::Dependency::RefDependencyProperty DataGridHeaderBorder::IsHoveredProperty;
Core::Dependency::RefDependencyProperty DataGridHeaderBorder::IsPressedProperty;
Core::Dependency::RefDependencyProperty DataGridHeaderBorder::IsClickableProperty;
Core::Dependency::RefDependencyProperty DataGridHeaderBorder::SortDirectionProperty;
Core::Dependency::RefDependencyProperty DataGridHeaderBorder::IsSelectedProperty;
Core::Dependency::RefDependencyProperty DataGridHeaderBorder::OrientationProperty;
Core::Dependency::RefDependencyProperty DataGridHeaderBorder::SeparatorBrushProperty;
Core::Dependency::RefDependencyProperty DataGridHeaderBorder::SeparatorVisibilityProperty;
Core::Dependency::RefDependencyProperty DataGridHeaderBorder::ControlBrushProperty;

void XUI::UI::DataGridHeaderBorder::StaticClassInitializer()
{
    IsHoveredProperty = (Core::Dependency::BindProperty<bool>::Register<DataGridHeaderBorder>(nameof(IsHovered), false));
    IsPressedProperty = (Core::Dependency::BindProperty<bool>::Register<DataGridHeaderBorder>(nameof(IsPressed), false));
    IsClickableProperty = (Core::Dependency::BindProperty<bool>::Register<DataGridHeaderBorder>(nameof(IsClickable), false));
    SortDirectionProperty = (Core::Dependency::BindProperty<UI::ListSortDirection>::Register<DataGridHeaderBorder>(nameof(SortDirection)));
    IsSelectedProperty = (Core::Dependency::BindProperty<bool>::Register<DataGridHeaderBorder>(nameof(IsSelected), false));
    OrientationProperty = (Core::Dependency::BindProperty<UI::Orientation>::Register<DataGridHeaderBorder>(nameof(Orientation)));
    SeparatorBrushProperty = (Core::Dependency::BindProperty<std::shared_ptr<Interfaces::IBrush>>::Register<DataGridHeaderBorder>(nameof(SeparatorBrush)));
    SeparatorVisibilityProperty = (Core::Dependency::BindProperty<UI::Visibility>::Register<DataGridHeaderBorder>(nameof(SeparatorVisibility)));
    ControlBrushProperty = (Core::Dependency::BindProperty<std::shared_ptr<Interfaces::IBrush>>::Register<DataGridHeaderBorder>(nameof(ControlBrush)));
}

XUI::UI::DataGridHeaderBorder::DataGridHeaderBorder()
{
}

XUI::UI::DataGridHeaderBorder::~DataGridHeaderBorder()
{
}

void XUI::UI::DataGridHeaderBorder::Render(Interfaces::IDrawingContextImpl * context)
{
    if (UsingBorderImplementation())
    {
        return Border::Render(context);
    }
    else
    {
        RenderClassic(context);
    }
}

void XUI::UI::DataGridHeaderBorder::RenderClassic(Interfaces::IDrawingContextImpl * dc)
{
    static Core::Media::SolidColorBrush light(Core::Media::Colors::GetWhiteSmoke());
    static Core::Media::SolidColorBrush dark(Core::Media::Colors::GetDarkGray());

    Size size = _bounds.GetSize();
    bool isClickable = IsClickable && IsEnabled;
    bool isPressed = isClickable && IsPressed;
    UI::ListSortDirection sortDirection = SortDirection;
    bool isSorted = sortDirection != UI::ListSortDirection::UnSorted;
    bool horizontal = Orientation == UI::Orientation::Horizontal;
    auto background = ControlBrush.get();
    bool shouldDrawRight = true;
    bool shouldDrawBottom = true;
    bool usingSeparatorBrush = false;
    
    if (background == nullptr)
        background = &dark;
    
    Interfaces::IBrush* darkDarkRight = nullptr;
    if (!horizontal)
    {
        if (SeparatorVisibility == UI::Visibility::Visible && SeparatorBrush != nullptr)
        {
            darkDarkRight = SeparatorBrush.get();
            usingSeparatorBrush = true;
        }
        else
        {
            shouldDrawRight = false;
        }
    }
    else
    {
        darkDarkRight = &dark;
    }
    
    Interfaces::IBrush* darkDarkBottom = nullptr;
    if (horizontal)
    {
        if (SeparatorVisibility == UI::Visibility::Visible && SeparatorBrush != nullptr)
        {
            darkDarkRight = SeparatorBrush.get();
            usingSeparatorBrush = true;
        }
        else
        {
            shouldDrawBottom = false;
        }
    }
    else
    {
        darkDarkBottom = &dark;
    }
    
    //EnsureCache((int)ClassicFreezables.NumFreezables);
    
    // Draw the background
    dc->DrawRectangle(XUI::Core::Media::Pen(background), Rect(0.0, 0.0, size.Width, size.Height));
    
    if ((size.Width > 3.0) && (size.Height > 3.0))
    {
        // Draw the border
        if (isPressed)
        {
            dc->DrawRectangle(XUI::Core::Media::Pen(&dark), Rect(0.0, 0.0, size.Width, 1.0));
            dc->DrawRectangle(XUI::Core::Media::Pen(&dark), Rect(0.0, 0.0, 1.0, size.Height));
            dc->DrawRectangle(XUI::Core::Media::Pen(&dark), Rect(0.0, std::max(size.Height - 1.0, 0.0), size.Width, 1.0));
            dc->DrawRectangle(XUI::Core::Media::Pen(&dark), Rect(std::max(size.Width - 1.0, 0.0), 0.0, 1.0, size.Height));
        }
        else
        {
            dc->DrawRectangle(XUI::Core::Media::Pen(&light), Rect(0.0, 0.0, 1.0, std::max(size.Height - 1.0, 0.0)));
            dc->DrawRectangle(XUI::Core::Media::Pen(&light), Rect(0.0, 0.0, std::max(size.Width - 1.0, 0.0), 1.0));
            
            if (shouldDrawRight)
            {
                if (!usingSeparatorBrush)
                {
                    dc->DrawRectangle(XUI::Core::Media::Pen(&dark), Rect(std::max(size.Width - 2.0, 0.0), 1.0, 1.0, std::max(size.Height - 2.0, 0.0)));
                }
            
                dc->DrawRectangle(XUI::Core::Media::Pen(darkDarkRight), Rect(std::max(size.Width - 1.0, 0.0), 0.0, 1.0, size.Height));
            }
            
            if (shouldDrawBottom)
            {
                if (!usingSeparatorBrush)
                {
                    dc->DrawRectangle(XUI::Core::Media::Pen(&dark), Rect(1.0, std::max(size.Height - 2.0, 0.0), std::max(size.Width - 2.0, 0.0), 1.0));
                }
            
                dc->DrawRectangle(XUI::Core::Media::Pen(darkDarkBottom), Rect(0.0, std::max(size.Height - 1.0, 0.0), size.Width, 1.0));
            }
        }
    }
}
