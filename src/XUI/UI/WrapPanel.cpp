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

#include "WrapPanel.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;
using namespace XUI::Interfaces;
using namespace XUI::UI;

STATIC_CLASS_INITIALIZER(WrapPanel, WrapPanel::StaticClassInitializer)

XUI::Core::Dependency::RefDependencyProperty WrapPanel::OrientationProperty;

void WrapPanel::StaticClassInitializer()
{
    OrientationProperty = (Core::Dependency::BindProperty<UI::Orientation>::Register<WrapPanel>(nameof(Orientation)));
    OrientationProperty->Changed.get_observable().subscribe(AffectsMeasureInvalidate);
}

UI::Orientation WrapPanel::GetOrientation() const
{
    return GetValue<UI::Orientation>(OrientationProperty);
}

void WrapPanel::SetOrientation(UI::Orientation value)
{
    SetValue(OrientationProperty, value);
}

Controls::InputElement *WrapPanel::GetControl(Presenters::NavigationDirection direction, Controls::InputElement* from)
{
    auto horiz = Orientation == UI::Orientation::Horizontal;
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

WrapPanel::UVSize WrapPanel::CreateUVSize(Core::Media::Size const& size)
{
    return UVSize(Orientation, size);
}

WrapPanel::UVSize WrapPanel::CreateUVSize()
{
    return UVSize(Orientation);
}

Core::Media::Size WrapPanel::MeasureOverride(Core::Media::Size const& availableSize)
{
    auto desiredSize = CreateUVSize();
    auto lineSize = CreateUVSize();
    auto uvAvailableSize = CreateUVSize(availableSize);

    for (auto child : Children)
    {
        child->Measure(availableSize);
        auto childSize = CreateUVSize(child->DesiredSize);
        if (lineSize.U + childSize.U <= uvAvailableSize.U) // same line
        {
            lineSize.U += childSize.U;
            lineSize.V = std::max(lineSize.V, childSize.V);
        }
        else // moving to next line
        {
            desiredSize.U = std::max(lineSize.U, uvAvailableSize.U);
            desiredSize.V += lineSize.V;
            lineSize = childSize;
        }
    }
    // last element
    desiredSize.U = std::max(lineSize.U, desiredSize.U);
    desiredSize.V += lineSize.V;

    return desiredSize.ToSize();
}

Core::Media::Size WrapPanel::ArrangeOverride(Core::Media::Size const& finalSize)
{
    double accumulatedV = 0;
    auto uvFinalSize = CreateUVSize(finalSize);
    auto lineSize = CreateUVSize();
    int firstChildInLineindex = 0;
    for (int index = 0; index < Children.size(); index++)
    {
        auto child = Children[index];
        auto childSize = CreateUVSize(child->DesiredSize);
        if (lineSize.U + childSize.U <= uvFinalSize.U) // same line
        {
            lineSize.U += childSize.U;
            lineSize.V = std::max(lineSize.V, childSize.V);
        }
        else // moving to next line
        {
            auto controlsInLine = GetControlsBetween(firstChildInLineindex, index);
            ArrangeLine(accumulatedV, lineSize.V, controlsInLine);
            accumulatedV += lineSize.V;
            lineSize = childSize;
            firstChildInLineindex = index;
        }
    }

    if (firstChildInLineindex < Children.size())
    {
        auto controlsInLine = GetControlsBetween(firstChildInLineindex, Children.size());
        ArrangeLine(accumulatedV, lineSize.V, controlsInLine);
    }
    return finalSize;
}

std::vector<Controls::Control*> WrapPanel::GetControlsBetween(int first, int last)
{
    std::vector<Controls::Control*> range;

    for (int i = first; i < Children.size() && i <= last; ++i)
    {
        range.push_back(Children[i].get());
    }

    return range; // Children->Skip(first).Take(last - first);
}

void WrapPanel::ArrangeLine(double v, double lineV, std::vector<Controls::Control*> &contols)
{
    double u = 0;
    bool isHorizontal = (Orientation == UI::Orientation::Horizontal);
    for (auto child : contols)
    {
        auto childSize = CreateUVSize(child->DesiredSize);
        auto x = isHorizontal ? u : v;
        auto y = isHorizontal ? v : u;
        auto width = isHorizontal ? childSize.U : lineV;
        auto height = isHorizontal ? lineV : childSize.U;
        child->Arrange(Core::Media::Rect::xywh(x, y, width, height));
        u += childSize.U;
    }
}

WrapPanel::UVSize::UVSize(UI::Orientation orientation, double width, double height) : _orientation(orientation)
{
    U = V = 0;
    Width = width;
    Height = height;
}

WrapPanel::UVSize::UVSize(UI::Orientation orientation, Core::Media::Size const& size) : UVSize(orientation, size.Width, size.Height)
{
}

WrapPanel::UVSize::UVSize(UI::Orientation orientation) : _orientation(orientation)
{
    U = V = 0;
}

double WrapPanel::UVSize::GetWidth() const
{
    return (_orientation == UI::Orientation::Horizontal ? U : V);
}

void WrapPanel::UVSize::SetWidth(const double &value)
{
    if (_orientation == UI::Orientation::Horizontal)
    {
        U = value;
    }
    else
    {
        V = value;
    }
}

double WrapPanel::UVSize::GetHeight() const
{
    return (_orientation == UI::Orientation::Horizontal ? V : U);
}

void WrapPanel::UVSize::SetHeight(const double &value)
{
    if (_orientation == UI::Orientation::Horizontal)
    {
        V = value;
    }
    else
    {
        U = value;
    }
}

Core::Media::Size WrapPanel::UVSize::ToSize()
{
    return Core::Media::Size(Width, Height);
}
