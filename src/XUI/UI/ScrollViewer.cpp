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

#include "ScrollViewer.h"
#include "ScrollContentPresenter.h"

#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;
using namespace XUI::UI;

STATIC_CLASS_INITIALIZER(ScrollViewer, ScrollViewer::StaticClassInitializer)

Core::Dependency::RefDependencyProperty ScrollViewer::CanScrollHorizontallyProperty;
Core::Dependency::RefDependencyProperty ScrollViewer::ExtentProperty;
Core::Dependency::RefDependencyProperty ScrollViewer::OffsetProperty;
Core::Dependency::RefDependencyProperty ScrollViewer::ViewportProperty;
Core::Dependency::RefDependencyProperty ScrollViewer::HorizontalScrollBarMaximumProperty;
Core::Dependency::RefDependencyProperty ScrollViewer::HorizontalScrollBarValueProperty;
Core::Dependency::RefDependencyProperty ScrollViewer::HorizontalScrollBarViewportSizeProperty;
Core::Dependency::RefDependencyProperty ScrollViewer::HorizontalScrollBarVisibilityProperty;
Core::Dependency::RefDependencyProperty ScrollViewer::VerticalScrollBarMaximumProperty;
Core::Dependency::RefDependencyProperty ScrollViewer::VerticalScrollBarValueProperty;
Core::Dependency::RefDependencyProperty ScrollViewer::VerticalScrollBarViewportSizeProperty;
Core::Dependency::RefDependencyProperty ScrollViewer::VerticalScrollBarVisibilityProperty;

void XUI::UI::ScrollViewer::StaticClassInitializer()
{
    CanScrollHorizontallyProperty = (Core::Dependency::BindProperty<bool>::Register<ScrollViewer>(nameof(CanScrollHorizontally), true));
    ExtentProperty = (Core::Dependency::DirectProperty<ScrollViewer, Core::Media::Size>::RegisterDirect(nameof(Extent),
        [](ScrollViewer* x) { return x->Extent; },
        [](ScrollViewer* x, Core::Media::Size const& v) { x->Extent = v; })
        );
    OffsetProperty = (Core::Dependency::DirectProperty<ScrollViewer, Core::Media::Vector>::RegisterDirect(nameof(Offset),
        [](ScrollViewer* x) { return x->Offset; },
        [](ScrollViewer* x, Core::Media::Vector const& v) { x->Offset = v; })
        );
    ViewportProperty = (Core::Dependency::DirectProperty<ScrollViewer, Core::Media::Size>::RegisterDirect(nameof(Viewport),
        [](ScrollViewer* x) { return x->Viewport; },
        [](ScrollViewer* x, Core::Media::Size const& v) { x->Viewport = v; })
        );
    HorizontalScrollBarMaximumProperty = (Core::Dependency::DirectProperty<ScrollViewer, double>::RegisterDirect(nameof(HorizontalScrollBarMaximum),
        [](ScrollViewer* x) { return x->HorizontalScrollBarMaximum; }, nullptr)
        );
    HorizontalScrollBarValueProperty = (Core::Dependency::DirectProperty<ScrollViewer, double>::RegisterDirect(nameof(HorizontalScrollBarValue),
        [](ScrollViewer* x) { return x->HorizontalScrollBarValue; },
        [](ScrollViewer* x, double v) { x->HorizontalScrollBarValue = v; })
        );
    HorizontalScrollBarViewportSizeProperty = (Core::Dependency::DirectProperty<ScrollViewer, double>::RegisterDirect(nameof(HorizontalScrollBarViewportSize),
        [](ScrollViewer* x) { return x->HorizontalScrollBarViewportSize; }, nullptr)
        );
    HorizontalScrollBarVisibilityProperty = (Core::Dependency::BindProperty<ScrollBarVisibility>::RegisterAttached<XUI::UI::ScrollViewer, XUI::UI::Controls::Control>(nameof(HorizontalScrollBarVisibility), ScrollBarVisibility::Auto));
    VerticalScrollBarMaximumProperty = (Core::Dependency::DirectProperty<ScrollViewer, double>::RegisterDirect(nameof(VerticalScrollBarMaximum),
        [](ScrollViewer* x) { return x->VerticalScrollBarMaximum; }, nullptr)
        );
    VerticalScrollBarValueProperty = (Core::Dependency::DirectProperty<ScrollViewer, double>::RegisterDirect(nameof(VerticalScrollBarValue),
        [](ScrollViewer* x) { return x->VerticalScrollBarValue; },
        [](ScrollViewer* x, double v) { x->HorizontalScrollBarValue = v; })
        );
    VerticalScrollBarViewportSizeProperty = (Core::Dependency::DirectProperty<ScrollViewer, double>::RegisterDirect(nameof(VerticalScrollBarViewportSize),
        [](ScrollViewer* x) { return x->VerticalScrollBarViewportSize; }, nullptr)
        );
    VerticalScrollBarVisibilityProperty = (Core::Dependency::BindProperty<ScrollBarVisibility>::RegisterAttached<XUI::UI::ScrollViewer, XUI::UI::Controls::Control>(nameof(VerticalScrollBarVisibility), ScrollBarVisibility::Auto));

    ExtentProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const& e)
    {
        static_cast<Core::Dependency::DependencyObject*>(e.Sender)->ReValidation(ExtentProperty.get());
        static_cast<Core::Dependency::DependencyObject*>(e.Sender)->ReValidation(OffsetProperty.get());
    });
    OffsetProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const& e)
    {
        static_cast<Core::Dependency::DependencyObject*>(e.Sender)->ReValidation(ExtentProperty.get());
        static_cast<Core::Dependency::DependencyObject*>(e.Sender)->ReValidation(ViewportProperty.get());
        static_cast<Core::Dependency::DependencyObject*>(e.Sender)->ReValidation(OffsetProperty.get());
    });
    ViewportProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const& e)
    {
        static_cast<Core::Dependency::DependencyObject*>(e.Sender)->ReValidation(ViewportProperty.get());
        static_cast<Core::Dependency::DependencyObject*>(e.Sender)->ReValidation(OffsetProperty.get());
    });
}

ScrollViewer::ScrollViewer()
{
    AddHandler(RequestBringIntoViewEvent.get(), System::make_eventhandler([this](void *sender, Controls::RequestBringIntoViewEventArgs& e)
    {
        if (_presenter)
            _presenter->BringIntoViewRequested(sender, e);
    }));
}

Core::Media::Size const& ScrollViewer::GetExtent() const
{
    return _extent;
}

void ScrollViewer::SetExtent(Core::Media::Size const& value)
{
    if (SetAndRaise(ExtentProperty, _extent, value))
    {
        CalculatedPropertiesChanged();
    }
}

Core::Media::Vector const& ScrollViewer::GetOffset() const
{
    return _offset;
}

void ScrollViewer::SetOffset(Core::Media::Vector const& value)
{
    auto _value = ValidateOffset(this, value);

    if (SetAndRaise(OffsetProperty, _offset, value))
    {
        CalculatedPropertiesChanged();
    }
}

Core::Media::Size const& ScrollViewer::GetViewport() const
{
    return _viewport;
}

void ScrollViewer::SetViewport(Core::Media::Size const& value)
{
    if (SetAndRaise(ViewportProperty, _viewport, value))
    {
        CalculatedPropertiesChanged();
    }
}

bool ScrollViewer::GetCanScrollHorizontally() const
{
    return GetValue<bool>(CanScrollHorizontallyProperty);
}

void ScrollViewer::SetCanScrollHorizontally(const bool &value)
{
    SetValue(CanScrollHorizontallyProperty, value);
}

ScrollBarVisibility ScrollViewer::GetHorizontalScrollBarVisibility() const
{
    return GetValue<ScrollBarVisibility>(HorizontalScrollBarVisibilityProperty);
}

void ScrollViewer::SetHorizontalScrollBarVisibility(ScrollBarVisibility value)
{
    SetValue(HorizontalScrollBarVisibilityProperty, value);
}

ScrollBarVisibility ScrollViewer::GetVerticalScrollBarVisibility() const
{
    return GetValue<ScrollBarVisibility>(VerticalScrollBarVisibilityProperty);
}

void ScrollViewer::SetVerticalScrollBarVisibility(ScrollBarVisibility value)
{
    SetValue(VerticalScrollBarVisibilityProperty, value);
}

double ScrollViewer::GetHorizontalScrollBarMaximum() const
{
    return Max(_extent.Width - _viewport.Width, 0);
}

double ScrollViewer::GetHorizontalScrollBarValue() const
{
    return _offset.x;
}

void ScrollViewer::SetHorizontalScrollBarValue(const double &value)
{
    if (_offset.x != value)
    {
        auto old = Offset.x;
        Offset = Core::Media::Vector(value, Offset.y);
        RaisePropertyChanged(HorizontalScrollBarValueProperty.get(), old, value);
    }
}

double ScrollViewer::GetHorizontalScrollBarViewportSize() const
{
    return _viewport.Width;
}

double ScrollViewer::GetVerticalScrollBarMaximum() const
{
    return Max(_extent.Height - _viewport.Height, 0);
}

double ScrollViewer::GetVerticalScrollBarValue() const
{
    return _offset.y;
}

void ScrollViewer::SetVerticalScrollBarValue(const double &value)
{
    if (_offset.y != value)
    {
        auto old = Offset.y;
        Offset = Core::Media::Vector(Offset.x, value);
        RaisePropertyChanged(VerticalScrollBarValueProperty.get(), old, value);
    }
}

double ScrollViewer::GetVerticalScrollBarViewportSize() const
{
    return _viewport.Height;
}

ScrollBarVisibility ScrollViewer::GetHorizontalScrollBarVisibility(Control *control)
{
    return control->GetValue<ScrollBarVisibility>(HorizontalScrollBarVisibilityProperty);
}

void ScrollViewer::SetHorizontalScrollBarVisibility(Control *control, ScrollBarVisibility value)
{
    control->SetValue(HorizontalScrollBarVisibilityProperty, value);
}

ScrollBarVisibility ScrollViewer::GetVerticalScrollBarVisibility(Control *control)
{
    return control->GetValue<ScrollBarVisibility>(VerticalScrollBarVisibilityProperty);
}

void ScrollViewer::SetVerticalScrollBarVisibility(Control *control, ScrollBarVisibility value)
{
    control->SetValue(VerticalScrollBarVisibilityProperty, value);
}

Core::Media::Vector ScrollViewer::CoerceOffset(Core::Media::Size const& extent, Core::Media::Size const& viewport, Core::Media::Vector const& offset)
{
    auto maxX = std::max(extent.Width - viewport.Width, 0.0);
    auto maxY = std::max(extent.Height - viewport.Height, 0.0);
    return Core::Media::Vector(std::clamp(offset.x, 0.0f, (float)maxX), std::clamp(offset.y, 0.0f, (float)maxY));
}

double ScrollViewer::Clamp(double value, double min, double max)
{
    return (value < min) ? min : (value > max) ? max : value;
}

double ScrollViewer::Max(double x, double y)
{
    auto result = std::max(x, y);
    return std::isnan(result) ? 0 : result;
}

Core::Media::Vector ScrollViewer::ValidateOffset(Core::Dependency::DependencyObject *o, Core::Media::Vector const& value)
{
    ScrollViewer *scrollViewer = o->Static_As<ScrollViewer>();

    if (scrollViewer != nullptr)
    {
        auto const& extent = scrollViewer->Extent;
        auto viewport = scrollViewer->Viewport;
        return CoerceOffset(extent, viewport, value);
    }
    else
    {
        return value;
    }
}

void ScrollViewer::CalculatedPropertiesChanged()
{
    // Pass old values of 0 here because we don't have the old values at this point,
    // and it shouldn't matter as only the template uses these properies.
    RaisePropertyChanged(HorizontalScrollBarMaximumProperty.get(), 0, HorizontalScrollBarMaximum);
    RaisePropertyChanged(HorizontalScrollBarValueProperty.get(), 0, HorizontalScrollBarValue);
    RaisePropertyChanged(HorizontalScrollBarViewportSizeProperty.get(), 0, HorizontalScrollBarViewportSize);
    RaisePropertyChanged(VerticalScrollBarMaximumProperty.get(), 0, VerticalScrollBarMaximum);
    RaisePropertyChanged(VerticalScrollBarValueProperty.get(), 0, VerticalScrollBarValue);
    RaisePropertyChanged(VerticalScrollBarViewportSizeProperty.get(), 0, VerticalScrollBarViewportSize);
}

void ScrollViewer::OnTemplateApplied(Controls::TemplateAppliedEventArgs const& e)
{
    auto object = Template->FindName("PART_ContentPresenter", this);
    if (!object)
        throw System::KeyNotFoundException("Not found presenter");

    auto pPresenter = object->Static_As<Presenters::ScrollContentPresenter>();
    if (!pPresenter)
        throw System::KeyNotFoundException("Not found presenter");

    _presenter = pPresenter->shared_from_base_static<Presenters::ScrollContentPresenter>();
}