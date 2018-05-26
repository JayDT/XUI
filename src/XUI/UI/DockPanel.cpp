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

#include "DockPanel.h"
#include "UI/Controls/Control.h"

#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;

STATIC_CLASS_INITIALIZER(DockPanel, XUI::UI::DockPanel::StaticConstructor)

Core::Dependency::RefDependencyProperty XUI::UI::DockPanel::DockProperty;
Core::Dependency::RefDependencyProperty XUI::UI::DockPanel::LastChildFillProperty;

void XUI::UI::DockPanel::StaticConstructor()
{
    DockProperty = (Core::Dependency::BindProperty<XUI::UI::Dock>::RegisterAttached<XUI::UI::DockPanel, XUI::UI::Controls::Control>(nameof(Dock)));
    LastChildFillProperty = (Core::Dependency::BindProperty<bool>::Register<XUI::UI::DockPanel>(nameof(LastChildFill)));
}

XUI::UI::Dock XUI::UI::DockPanel::GetDock(Controls::Visual* control)
{
    return control->GetValue<XUI::UI::Dock>(DockProperty);
}

void XUI::UI::DockPanel::SetDock(Controls::Visual* control, Dock value)
{
    control->SetValue(DockProperty, value);
}

bool XUI::UI::DockPanel::GetLastChildFill() const
{
    return GetValue<bool>(LastChildFillProperty);
}

void XUI::UI::DockPanel::SetLastChildFill(const bool &value)
{
    SetValue(LastChildFillProperty, value);
}

Core::Media::Size XUI::UI::DockPanel::MeasureOverride(Core::Media::Size const& availableSize)
{
    double usedWidth = 0.0;
    double usedHeight = 0.0;
    double maximumWidth = 0.0;
    double maximumHeight = 0.0;

    // Measure each of the Children
    for (auto element : m_visualChildrens.GetNativeEnumerator())
    {
        // Get the child's desired size
        Core::Media::Size remainingSize(std::max(0.0, availableSize.Width - usedWidth), std::max(0.0, availableSize.Height - usedHeight));
        element->Measure(remainingSize);
        Core::Media::Size desiredSize = element->DesiredSize;

        // Decrease the remaining space for the rest of the children
        switch (GetDock(element.get()))
        {
            case Dock::Left:
            case Dock::Right:
                maximumHeight = std::max(maximumHeight, usedHeight + desiredSize.Height);
                usedWidth += desiredSize.Width;
                break;
            case Dock::Top:
            case Dock::Bottom:
                maximumWidth = std::max(maximumWidth, usedWidth + desiredSize.Width);
                usedHeight += desiredSize.Height;
                break;
        }
    }

    maximumWidth = std::max(maximumWidth, usedWidth);
    maximumHeight = std::max(maximumHeight, usedHeight);
    return Core::Media::Size(maximumWidth, maximumHeight);
}

Core::Media::Size XUI::UI::DockPanel::ArrangeOverride(Core::Media::Size const& arrangeSize)
{
    double left = 0.0;
    double top = 0.0;
    double right = 0.0;
    double bottom = 0.0;

    // Arrange each of the Children
    auto children = Children;
    int dockedCount = children.size() - (LastChildFill ? 1 : 0);
    int index = 0;

    for (auto element : children)
    {
        // Determine the remaining space left to arrange the element
        Core::Media::Rect remainingRect = Core::Media::Rect::xywh(left, top, std::max(0.0, arrangeSize.Width - left - right), std::max(0.0, arrangeSize.Height - top - bottom));

        // Trim the remaining Rect to the docked size of the element
        // (unless the element should fill the remaining space because
        // of LastChildFill)
        if (index < dockedCount)
        {
            Core::Media::Size desiredSize = element->DesiredSize;
            switch (GetDock(element.get()))
            {
                case Dock::Left:
                    left += desiredSize.Width;
                    remainingRect = Core::Media::Rect::xywh(remainingRect.x0(), remainingRect.y0(), desiredSize.Width, remainingRect.height());
                    break;
                case Dock::Top:
                    top += desiredSize.Height;
                    remainingRect = Core::Media::Rect::xywh(remainingRect.x0(), remainingRect.y0(), remainingRect.width(), desiredSize.Height);
                    break;
                case Dock::Right:
                    right += desiredSize.Width;
                    remainingRect = Core::Media::Rect::xywh(std::max(0.0, arrangeSize.Width - right), remainingRect.y0(), desiredSize.Width, remainingRect.height());
                    break;
                case Dock::Bottom:
                    bottom += desiredSize.Height;
                    remainingRect = Core::Media::Rect::xywh(remainingRect.x0(), std::max(0.0, arrangeSize.Height - bottom), remainingRect.width(), desiredSize.Height);
                    break;
            }
        }

        element->Arrange(remainingRect);
        index++;
    }

    return arrangeSize;
}
