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

#include "Canvas.h"

#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;
using namespace XUI::UI;

STATIC_CLASS_INITIALIZER(Canvas, Canvas::StaticClassInitializer)

Core::Dependency::RefDependencyProperty Canvas::LeftProperty;
Core::Dependency::RefDependencyProperty Canvas::TopProperty;
Core::Dependency::RefDependencyProperty Canvas::RightProperty;
Core::Dependency::RefDependencyProperty Canvas::BottomProperty;

void XUI::UI::Canvas::StaticClassInitializer()
{
    LeftProperty = (Core::Dependency::BindProperty<double>::RegisterAttached<XUI::UI::Canvas, XUI::UI::Controls::Control>(nameof(Left), -1.0));
    TopProperty = (Core::Dependency::BindProperty<double>::RegisterAttached<XUI::UI::Canvas, XUI::UI::Controls::Control>(nameof(Top), -1.0));
    RightProperty = (Core::Dependency::BindProperty<double>::RegisterAttached<XUI::UI::Canvas, XUI::UI::Controls::Control>(nameof(Right), -1.0));
    BottomProperty = (Core::Dependency::BindProperty<double>::RegisterAttached<XUI::UI::Canvas, XUI::UI::Controls::Control>(nameof(Bottom), -1.0));

    ClipToBoundsProperty->OverrideDefaultValue<Canvas>(false);
    AffectsCanvasArrange({ LeftProperty.get(), TopProperty.get(), RightProperty.get(), BottomProperty.get() });
}

double Canvas::GetLeft(Core::Dependency::DependencyObject *element)
{
    return element->GetValue<double>(LeftProperty);
}

void Canvas::SetLeft(Core::Dependency::DependencyObject *element, double value)
{
    element->SetValue(LeftProperty, value);
}

double Canvas::GetTop(Core::Dependency::DependencyObject *element)
{
    return element->GetValue<double>(TopProperty);
}

void Canvas::SetTop(Core::Dependency::DependencyObject *element, double value)
{
    element->SetValue(TopProperty, value);
}

double Canvas::GetRight(Core::Dependency::DependencyObject *element)
{
    return element->GetValue<double>(RightProperty);
}

void Canvas::SetRight(Core::Dependency::DependencyObject *element, double value)
{
    element->SetValue(RightProperty, value);
}

double Canvas::GetBottom(Core::Dependency::DependencyObject *element)
{
    return element->GetValue<double>(BottomProperty);
}

void Canvas::SetBottom(Core::Dependency::DependencyObject *element, double value)
{
    element->SetValue(BottomProperty, value);
}

Controls::InputElement *Canvas::GetControl(Presenters::NavigationDirection direction, Controls::InputElement *from)
{
    // TODO: Implement this
    return nullptr;
}

Core::Media::Size Canvas::MeasureOverride(Core::Media::Size const& availableSize)
{
    Core::Media::Size _availableSize = Core::Media::Size(std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity());

    for (auto const& child : Children.GetNativeEnumerator())
    {
        child->Measure(_availableSize);
    }

    return availableSize;
}

Core::Media::Size Canvas::ArrangeOverride(Core::Media::Size const& finalSize)
{
    for (auto const& child : Children.GetNativeEnumerator())
    {
        double x = 0.0;
        double y = 0.0;
        double elementLeft = GetLeft(child.get());

        if (elementLeft != -1.0)
        {
            x = elementLeft;
        }
        else
        {
            // Arrange with right.
            double elementRight = GetRight(child.get());
            if (elementRight != -1.0)
            {
                x = finalSize.Width - child->DesiredSize.Width - elementRight;
            }
        }

        double elementTop = GetTop(child.get());
        if (elementTop != -1.0)
        {
            y = elementTop;
        }
        else
        {
            double elementBottom = GetBottom(child.get());
            if (elementBottom != -1.0)
            {
                y = finalSize.Height - child->DesiredSize.Height - elementBottom;
            }
        }

        child->Arrange(Core::Media::Rect(Core::Media::Vector(x, y), Core::Media::Vector(child->DesiredSize.Width, child->DesiredSize.Height)));// child->DesiredSize.ToVector()));
    }

    return finalSize;
}

void Canvas::AffectsCanvasArrange(std::vector<Core::Dependency::DependencyProperty*> properties)
{
    for (auto _property : properties)
    {
        _property->Changed.get_observable().subscribe(AffectsCanvasArrangeInvalidate);
    }
}

void Canvas::AffectsCanvasArrangeInvalidate(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
    auto control = e.Sender->Static_As<Controls::Control>();
    if (control)
    {
        auto canvas = control->VisualParent;
        if (canvas)
            canvas->InvalidateArrange();
    }
}

