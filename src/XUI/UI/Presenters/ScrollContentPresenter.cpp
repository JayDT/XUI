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

#include "ScrollContentPresenter.h"
#include "UI/Presenters/ItemsPresenter.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;
using namespace XUI::UI;
using namespace XUI::UI::Presenters;

STATIC_CLASS_INITIALIZER(ScrollContentPresenter, XUI::UI::Presenters::ScrollContentPresenter::StaticClassInitializer)

Core::Dependency::RefDependencyProperty ScrollContentPresenter::ExtentProperty;
Core::Dependency::RefDependencyProperty ScrollContentPresenter::OffsetProperty;
Core::Dependency::RefDependencyProperty ScrollContentPresenter::ViewportProperty;
Core::Dependency::RefDependencyProperty ScrollContentPresenter::CanScrollHorizontallyProperty;
Core::Dependency::RefDependencyProperty ScrollContentPresenter::CanScrollVerticallyProperty;

void XUI::UI::Presenters::ScrollContentPresenter::StaticClassInitializer()
{
    ExtentProperty = (Core::Dependency::DirectProperty<ScrollContentPresenter, Core::Media::Size>::RegisterDirect(nameof(Extent),
        [](ScrollContentPresenter* x) { return x->Extent; },
        [](ScrollContentPresenter* x, Core::Media::Size const& v) { x->Extent = v; })
        );

    OffsetProperty = (Core::Dependency::DirectProperty<ScrollContentPresenter, Core::Media::Vector>::RegisterDirect(nameof(Offset),
        [](ScrollContentPresenter* x) { return x->Offset; },
        [](ScrollContentPresenter* x, Core::Media::Vector const& v) { x->Offset = v; })
        );

    ViewportProperty = (Core::Dependency::DirectProperty<ScrollContentPresenter, Core::Media::Size>::RegisterDirect(nameof(Viewport),
        [](ScrollContentPresenter* x) { return x->Viewport; },
        [](ScrollContentPresenter* x, Core::Media::Size const& v) { x->Viewport = v; })
        );

    CanScrollHorizontallyProperty = (Core::Dependency::BindProperty<bool>::Register<ScrollContentPresenter>(nameof(CanScrollHorizontally)));
    CanScrollVerticallyProperty = (Core::Dependency::BindProperty<bool>::Register<ScrollContentPresenter>(nameof(CanScrollVertically), true)); // ToDo: implement this

    ClipToBoundsProperty->OverrideDefaultValue<ScrollContentPresenter, bool>(true);

    OffsetProperty->Changed.get_observable().subscribe(Visual::AffectsArrangeInvalidate);
    ChildProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const& e)
    {
        ScrollContentPresenter* control = e.Sender->Static_As<ScrollContentPresenter>();
        if (control)
            control->ChildChanged(e);
    });
}

ScrollContentPresenter::ScrollContentPresenter()
{
    AddHandler(RequestBringIntoViewEvent.get(), System::make_eventhandler(&ScrollContentPresenter::BringIntoViewRequested, this));
    AddDirectBind(ChildProperty.get(), [this](System::Reflection::Variant const& variant)
    {
        auto control = variant.TryGetValue<std::shared_ptr<XUI::UI::Controls::Control>>();
        if (control)
            UpdateScrollableSubscription(control.get());
    });
}

ScrollContentPresenter::~ScrollContentPresenter()
{
}

Core::Media::Size const& ScrollContentPresenter::GetExtent() const
{
    return _extent;
}

void ScrollContentPresenter::SetExtent(Core::Media::Size const& value)
{
    SetAndRaise(ExtentProperty, _extent, value);
}

Core::Media::Vector const& ScrollContentPresenter::GetOffset() const
{
    return _offset;
}

void ScrollContentPresenter::SetOffset(Core::Media::Vector const& value)
{
    SetAndRaise(OffsetProperty, _offset, value);
}

Core::Media::Size const& ScrollContentPresenter::GetViewport() const
{
    return _viewport;
}

void ScrollContentPresenter::SetViewport(Core::Media::Size const& value)
{
    SetAndRaise(ViewportProperty, _viewport, value);
}

bool ScrollContentPresenter::GetCanScrollHorizontally() const
{
    return GetValue<bool>(CanScrollHorizontallyProperty);
}

bool XUI::UI::Presenters::ScrollContentPresenter::GetCanScrollVertically() const
{
    return GetValue<bool>(CanScrollVerticallyProperty);
}

bool ScrollContentPresenter::BringDescendantIntoView(Controls::Visual *target, Core::Media::Rect const& targetRect)
{
    if (Child == nullptr)
        return false;

    auto control = Child->Static_As<Presenters::ItemsPresenter>();

    if (control && control->IsLogicalScrollEnabled == true && control != nullptr)
    {
        return control->BringIntoView(control, targetRect);
    }

    auto transform = target->TransformToVisual(Child.get());

    auto rect = targetRect * transform;
    auto offset = Offset;
    auto result = false;

    if (rect.Bottom > offset.y + Viewport.Height)
    {
        offset = Core::Media::Vector(offset.x, (rect.Bottom - Viewport.Height) + Child->Margin.Top);
        result = true;
    }

    if (rect.y0() < offset.y)
    {
        offset = Core::Media::Vector(offset.x, rect.y0());
        result = true;
    }

    if (rect.Right > offset.x + Viewport.Width)
    {
        offset = Core::Media::Vector((rect.Right - Viewport.Width) + Child->Margin.Left, offset.y);
        result = true;
    }

    if (rect.x0() < offset.x)
    {
        offset = Core::Media::Vector(rect.x0(), offset.y);
        result = true;
    }

    if (result)
    {
        Offset = offset;
    }

    return result;
}

Core::Media::Size ScrollContentPresenter::MeasureOverride(Core::Media::Size const& availableSize)
{
    auto child = Child;

    if (child != nullptr)
    {
        auto measureSize = availableSize;

        if (!_logicalScrollSubscription.is_subscribed())
        {
            measureSize = Core::Media::Size(std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity());

            if (!CanScrollHorizontally)
                measureSize = measureSize.WithWidth(availableSize.Width);
        }

        child->Measure(measureSize);
        auto size = child->DesiredSize;
        _measuredExtent = size;
        return size.Constrain(availableSize);
    }
    else
    {
        Extent = Core::Media::Size::Empty;
        return Extent;
    }
}

Core::Media::Size ScrollContentPresenter::ArrangeOverride(Core::Media::Size const& finalSize)
{
    Controls::Visual* child = GetVisualChildren().empty() ? nullptr : (*GetVisualChildren().begin()).get();
    auto logicalScroll = _logicalScrollSubscription.is_subscribed();

    if (!logicalScroll)
    {
        Viewport = finalSize;
        Extent = _measuredExtent;

        if (child != nullptr)
        {
            auto size = Core::Media::Size(std::max(finalSize.Width, child->DesiredSize.Width), std::max(finalSize.Height, child->DesiredSize.Height));
            child->Arrange(Core::Media::Rect::xywh(-Offset.x, -Offset.y, size.Width, size.Height));
            return finalSize;
        }
    }
    else if (child != nullptr)
    {
        child->Arrange(Core::Media::Rect::xywh(0, 0, finalSize.Width, finalSize.Height));
        return finalSize;
    }

    return Core::Media::Size::Empty;
}

void ScrollContentPresenter::OnPointerWheelChanged(Interfaces::PointerWheelEventArgs& e)
{
    if (Extent.Height > Viewport.Height || Extent.Width > Viewport.Width)
    {
        auto scrollable = Child->Static_As<Presenters::ItemsPresenter>();
        bool isLogical = scrollable == nullptr ? nullptr : scrollable->IsLogicalScrollEnabled == true;

        double x = Offset.x;
        double y = Offset.y;

        if (Extent.Height > Viewport.Height)
        {
            double height = isLogical ? scrollable->ScrollSize.Height : 50;
            y += -e.Delta.y * height;
            y = std::max(y, 0.0);
            y = std::min(y, Extent.Height - Viewport.Height);
        }

        if (Extent.Width > Viewport.Width)
        {
            double width = isLogical ? scrollable->ScrollSize.Width : 50;
            x += -e.Delta.x * width;
            x = std::max(x, 0.0);
            x = std::min(x, Extent.Width - Viewport.Width);
        }

        Offset = Core::Media::Vector(x, y);
        e.Handled = true;
    }
}

void ScrollContentPresenter::BringIntoViewRequested(void *sender, Controls::RequestBringIntoViewEventArgs& e)
{
    e.Handled = BringDescendantIntoView(e.TargetObject, e.TargetRect);
}

void ScrollContentPresenter::ChildChanged(const XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
    UpdateScrollableSubscription(e.NewValue.TryGetValue<Controls::Control*>());

    if (e.OldValue != nullptr)
    {
        Offset = Core::Media::Vector::Zero();
    }
}

void ScrollContentPresenter::UpdateScrollableSubscription(Controls::Control *child)
{
    auto scrollable = child->Static_As<Presenters::ItemsPresenter>();

    if (_logicalScrollSubscription.is_subscribed())
        _logicalScrollSubscription.unsubscribe();

    if (scrollable != nullptr)
    {
        scrollable->InvalidateScroll = [this, scrollable] ()
        {
            UpdateFromScrollable(scrollable);
        };

        if (scrollable->IsLogicalScrollEnabled == true)
        {
            auto observer = GetObservable(OffsetProperty.get());
            observer->subscribe([scrollable](System::Reflection::meta::Variant& value)
            {
                scrollable->Offset = value.GetValue<Core::Media::Vector>();
            });

            observer = GetObservable(CanScrollHorizontallyProperty.get());
            observer->subscribe([scrollable](System::Reflection::meta::Variant& value)
            {
                scrollable->CanHorizontallyScroll = value.GetValue<bool>();
            });

            _logicalScrollSubscription = rxcpp::make_subscription([scrollable, observer]()
            {
                scrollable->InvalidateScroll = nullptr;
            });

            UpdateFromScrollable(scrollable);
        }
    }
}

void ScrollContentPresenter::UpdateFromScrollable(Controls::Control * control)
{
    auto scrollable = control->Static_As<Presenters::ItemsPresenter>();

    auto logicalScroll = _logicalScrollSubscription.is_subscribed();

    if (logicalScroll != scrollable->IsLogicalScrollEnabled)
    {
        UpdateScrollableSubscription(Child.get());
        Offset = Core::Media::Vector::Zero();
        InvalidateMeasure();
    }
    else if (scrollable->IsLogicalScrollEnabled)
    {
        Viewport = scrollable->Viewport;
        Extent = scrollable->Extent;
        Offset = scrollable->Offset;
    }
}

