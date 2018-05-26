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

#include "StackPanel.h"
#include "UI/Presenters/ItemsPresenterBase.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;
using namespace XUI::UI;

STATIC_CLASS_INITIALIZER(StackPanel, XUI::UI::StackPanel::StaticClassInitializer)

Core::Dependency::RefDependencyProperty StackPanel::GapProperty;
Core::Dependency::RefDependencyProperty StackPanel::OrientationProperty;

void StackPanel::StaticClassInitializer()
{
    GapProperty = (Core::Dependency::BindProperty<double>::Register<StackPanel>(nameof(Gap)));
    OrientationProperty = (Core::Dependency::BindProperty<UI::Orientation>::Register<StackPanel>(nameof(Orientation), UI::Orientation::Vertical));

    GapProperty->Changed.get_observable().subscribe(&Visual::AffectsMeasureInvalidate);
    OrientationProperty->Changed.get_observable().subscribe(&Visual::AffectsMeasureInvalidate);
}

double StackPanel::GetGap()
{
    return GetValue<double>(GapProperty);
}

void StackPanel::SetGap(double value)
{
    SetValue(GapProperty, value);
}

UI::Orientation StackPanel::GetOrientation()
{
    return GetValue<UI::Orientation>(OrientationProperty);
}

void StackPanel::SetOrientation(UI::Orientation value)
{
    SetValue(OrientationProperty, value);
}

Controls::InputElement *StackPanel::GetControl(Presenters::NavigationDirection direction, Controls::InputElement *from)
{
    auto fromControl = from->Static_As<Controls::Control>();
    return (fromControl != nullptr) ? GetControlInDirection(direction, fromControl) : nullptr;
}

Controls::InputElement *StackPanel::GetControlInDirection(Presenters::NavigationDirection direction, Controls::Control *from)
{
    auto horiz = Orientation == Orientation::Horizontal;
    int index = Children.IndexOf(from->shared_from_base_static<Controls::Control>());
    
    switch (direction)
    {
        case Presenters::NavigationDirection::First:
            index = 0;
            break;
        case Presenters::NavigationDirection::Last:
            index = Children.size() - 1;
            break;
        case Presenters::NavigationDirection::Next:
            ++index;
            break;
        case Presenters::NavigationDirection::Previous:
            --index;
            break;
        case Presenters::NavigationDirection::Left:
            index = horiz ? index - 1 : -1;
            break;
        case Presenters::NavigationDirection::Right:
            index = horiz ? index + 1 : -1;
            break;
        case Presenters::NavigationDirection::Up:
            index = horiz ? -1 : index - 1;
            break;
        case Presenters::NavigationDirection::Down:
            index = horiz ? -1 : index + 1;
            break;
        default:
            index = -1;
            break;
    }
    
    if (index >= 0 && index < Children.size())
    {
        return Children[index].get();
    }
    else
    {
        return nullptr;
    }
}

Core::Media::Size StackPanel::MeasureOverride(Core::Media::Size const& availableSize)
{
    double childAvailableWidth = std::numeric_limits<double>::infinity();
    double childAvailableHeight = std::numeric_limits<double>::infinity();

    if (Orientation == UI::Orientation::Vertical)
    {
        childAvailableWidth = availableSize.Width;

        if (Width != -1)
            childAvailableWidth = Width;

        childAvailableWidth = std::min(childAvailableWidth, MaxWidth);
        childAvailableWidth = std::max(childAvailableWidth, MinWidth);
    }
    else
    {
        childAvailableHeight = availableSize.Height;

        if (Height != -1)
            childAvailableHeight = Height;

        childAvailableHeight = std::min(childAvailableHeight, MaxHeight);
        childAvailableHeight = std::max(childAvailableHeight, MinHeight);
    }

    double measuredWidth = 0;
    double measuredHeight = 0;
    double gap = Gap;

    for (auto const& child : Children)
    {
        child->Measure(Core::Media::Size(childAvailableWidth, childAvailableHeight));
        Core::Media::Size size = child->DesiredSize;

        if (Orientation == UI::Orientation::Vertical)
        {
            measuredHeight += size.Height + gap;
            measuredWidth = std::max(measuredWidth, size.Width);
        }
        else
        {
            measuredWidth += size.Width + gap;
            measuredHeight = std::max(measuredHeight, size.Height);
        }
    }

    if (Orientation == UI::Orientation::Vertical)
    {
        measuredHeight -= gap;
    }
    else
    {
        measuredWidth -= gap;
    }

    return Core::Media::Size(measuredWidth, measuredHeight);
}

Core::Media::Size StackPanel::ArrangeOverride(Core::Media::Size const& finalSize)
{
    auto orientation = Orientation;
    double arrangedWidth = finalSize.Width;
    double arrangedHeight = finalSize.Height;
    double gap = Gap;

    if (Orientation == UI::Orientation::Vertical)
    {
        arrangedHeight = 0;
    }
    else
    {
        arrangedWidth = 0;
    }

    for (auto const& child : Children)
    {
        double childWidth = child->DesiredSize.Width;
        double childHeight = child->DesiredSize.Height;

        if (orientation == UI::Orientation::Vertical)
        {
            double width = std::max(childWidth, arrangedWidth);
            Core::Media::Rect childFinal = Core::Media::Rect::xywh(0, arrangedHeight, width, childHeight);
            ArrangeChild(child.get(), childFinal, finalSize, orientation);
            arrangedWidth = std::max(arrangedWidth, childWidth);
            arrangedHeight += childHeight + gap;
        }
        else
        {
            double height = std::max(childHeight, arrangedHeight);
            Core::Media::Rect childFinal = Core::Media::Rect::xywh(arrangedWidth, 0, childWidth, height);
            ArrangeChild(child.get(), childFinal, finalSize, orientation);
            arrangedWidth += childWidth + gap;
            arrangedHeight = std::max(arrangedHeight, childHeight);
        }
    }

    if (orientation == UI::Orientation::Vertical)
    {
        arrangedHeight = std::max(arrangedHeight - gap, finalSize.Height);
    }
    else
    {
        arrangedWidth = std::max(arrangedWidth - gap, finalSize.Width);
    }

    return Core::Media::Size(arrangedWidth, arrangedHeight);
}

void StackPanel::ArrangeChild(Controls::Control *child, Core::Media::Rect const& rect, Core::Media::Size const& panelSize, UI::Orientation orientation)
{
    child->Arrange(rect);
}
