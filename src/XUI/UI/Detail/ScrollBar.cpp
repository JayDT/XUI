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

#include "ScrollBar.h"
#include "UI/Panel.h"
#include "UI/ScrollViewer.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;
using namespace XUI::UI;

STATIC_CLASS_INITIALIZER(ScrollBar, ScrollBar::StaticClassInitializer)

Core::Dependency::RefDependencyProperty ScrollBar::ViewportSizeProperty;
Core::Dependency::RefDependencyProperty ScrollBar::VisibilityProperty;
Core::Dependency::RefDependencyProperty ScrollBar::OrientationProperty;

void XUI::UI::ScrollBar::StaticClassInitializer()
{
    ViewportSizeProperty = (Core::Dependency::BindProperty<double>::Register<ScrollBar>(nameof(ViewportSize), -1.0));
    VisibilityProperty = (Core::Dependency::BindProperty<ScrollBarVisibility>::Register<ScrollBar>(nameof(Visibility)));
    OrientationProperty = (Core::Dependency::BindProperty<UI::Orientation>::Register<ScrollBar>(nameof(Orientation), UI::Orientation::Vertical));
}

ScrollBar::ScrollBar()
{
    AddDirectBind(VisibilityProperty.get(), [this](System::Reflection::Variant const& variant)
    {
        IsVisible = CalculateIsVisible();
    });

    AddDirectBind(ViewportSizeProperty.get(), [this](System::Reflection::Variant const& variant)
    {
        IsVisible = CalculateIsVisible();
    });

    AddDirectBind(MaximumProperty.get(), [this](System::Reflection::Variant const& variant)
    {
        IsVisible = CalculateIsVisible();
    });


    //auto isVisible = Observable::Merge(this->GetObservable(MinimumProperty)->Select([&](void *_)
    //{
    //    Unit::Default;
    //}), this->GetObservable(MaximumProperty)->Select(_ = > Unit::Default), this->GetObservable(ViewportSizeProperty)->Select(_ = > Unit::Default), this->GetObservable(VisibilityProperty)->Select(_ = > Unit::Default))->Select(_ = > CalculateIsVisible());
    //Bind(IsVisibleProperty, isVisible, BindingPriority::Style);
}

double ScrollBar::GetViewportSize() const
{
    return GetValue<double>(ViewportSizeProperty);
}

void ScrollBar::SetViewportSize(const double &value)
{
    SetValue(ViewportSizeProperty, value);
}

ScrollBarVisibility ScrollBar::GetVisibility() const
{
    return GetValue<ScrollBarVisibility>(VisibilityProperty);
}

void ScrollBar::SetVisibility(ScrollBarVisibility value)
{
    SetValue(VisibilityProperty, value);
}

UI::Orientation ScrollBar::GetOrientation() const
{
    return GetValue<UI::Orientation>(OrientationProperty);
}

void ScrollBar::SetOrientation(UI::Orientation value)
{
    SetValue(OrientationProperty, value);
}

bool ScrollBar::CalculateIsVisible()
{
    switch (Visibility)
    {
        case ScrollBarVisibility::Visible:
            return true;

        case ScrollBarVisibility::Hidden:
            return false;

        case ScrollBarVisibility::Auto:
            return ViewportSize == -1 || Maximum > 0;

        default:
            throw System::InvalidOperationException("Invalid value for ScrollBar.Visibility.");
    }
}

void ScrollBar::OnTemplateApplied(Controls::TemplateAppliedEventArgs const& e)
{
    RangeBase::OnTemplateApplied(e);

    if (_lineUpButton != nullptr)
        _lineUpButton->Click -= System::make_eventhandler(&ScrollBar::LineUpClick, this);

    if (_lineDownButton != nullptr)
        _lineDownButton->Click -= System::make_eventhandler(&ScrollBar::LineDownClick, this);

    if (_pageUpButton != nullptr)
        _pageUpButton->Click -= System::make_eventhandler(&ScrollBar::PageUpClick, this);

    if (_pageDownButton != nullptr)
        _pageDownButton->Click -= System::make_eventhandler(&ScrollBar::PageDownClick, this);

    auto object = Template->FindName("PART_LineUpButton", this);
    if (object)
        _lineUpButton = object->Dynamic_As<Button>()->shared_from_base_static<Button>();
    object = Template->FindName("PART_LineDownButton", this);
    if (object)
        _lineDownButton = object->Dynamic_As<Button>()->shared_from_base_static<Button>();
    object = Template->FindName("PART_PageUpButton", this);
    if (object)
        _pageUpButton = object->Dynamic_As<Button>()->shared_from_base_static<Button>();
    object = Template->FindName("PART_PageDownButton", this);
    if (object)
        _pageDownButton = object->Dynamic_As<Button>()->shared_from_base_static<Button>();

    if (_lineUpButton != nullptr)
        _lineUpButton->Click += System::make_eventhandler(&ScrollBar::LineUpClick, this);

    if (_lineDownButton != nullptr)
        _lineDownButton->Click += System::make_eventhandler(&ScrollBar::LineDownClick, this);

    if (_pageUpButton != nullptr)
        _pageUpButton->Click += System::make_eventhandler(&ScrollBar::PageUpClick, this);

    if (_pageDownButton != nullptr)
        _pageDownButton->Click += System::make_eventhandler(&ScrollBar::PageDownClick, this);
}

void ScrollBar::LineUpClick(void *sender, Core::Observer::RoutedEventArgs& e)
{
    SmallDecrement();
}

void ScrollBar::LineDownClick(void *sender, Core::Observer::RoutedEventArgs& e)
{
    SmallIncrement();
}

void ScrollBar::PageUpClick(void *sender, Core::Observer::RoutedEventArgs& e)
{
    LargeDecrement();
}

void ScrollBar::PageDownClick(void *sender, Core::Observer::RoutedEventArgs& e)
{
    LargeIncrement();
}

void ScrollBar::SmallDecrement()
{
    Value = std::max(Value - SmallChange * ViewportSize, Minimum);
}

void ScrollBar::SmallIncrement()
{
    Value = std::min(Value + SmallChange * ViewportSize, Maximum);
}

void ScrollBar::LargeDecrement()
{
    Value = std::max(Value - LargeChange * ViewportSize, Minimum);
}

void ScrollBar::LargeIncrement()
{
    Value = std::min(Value + LargeChange * ViewportSize, Maximum);
}

