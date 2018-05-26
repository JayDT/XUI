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

#include "Track.h"
#include "UI/Panel.h"

#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;
using namespace XUI::Interfaces;
using namespace XUI::UI;

STATIC_CLASS_INITIALIZER(Track, Track::StaticClassInitializer)

Core::Dependency::RefDependencyProperty Track::MinimumProperty;
Core::Dependency::RefDependencyProperty Track::MaximumProperty;
Core::Dependency::RefDependencyProperty Track::ValueProperty;
Core::Dependency::RefDependencyProperty Track::ViewportSizeProperty;
Core::Dependency::RefDependencyProperty Track::OrientationProperty;
Core::Dependency::RefDependencyProperty Track::ThumbProperty;
Core::Dependency::RefDependencyProperty Track::IncreaseButtonProperty;
Core::Dependency::RefDependencyProperty Track::DecreaseButtonProperty;

void Track::StaticClassInitializer()
{
    MinimumProperty = (Core::Dependency::DirectProperty<Track, double>::RegisterDirect(nameof(Minimum),
        [](Track* x) { return x->Minimum; },
        [](Track* x, double v) { x->Minimum = v; })
        );
    MaximumProperty = (Core::Dependency::DirectProperty<Track, double>::RegisterDirect(nameof(Maximum),
        [](Track* x) { return x->Maximum; },
        [](Track* x, double v) { x->Maximum = v; })
        );
    ValueProperty = (Core::Dependency::DirectProperty<Track, double>::RegisterDirect(nameof(Value),
        [](Track* x) { return x->Value; },
        [](Track* x, double v) { x->Value = v; })
        );
    ViewportSizeProperty = (Core::Dependency::BindProperty<double>::Register<Track>(nameof(ViewportSize)));
    OrientationProperty = (Core::Dependency::BindProperty<UI::Orientation>::Register<Track>(nameof(Orientation)));
    ThumbProperty = (Core::Dependency::BindProperty<std::shared_ptr<UI::Thumb>>::Register<Track>(nameof(Thumb)));
    IncreaseButtonProperty = (Core::Dependency::BindProperty<std::shared_ptr<Button>>::Register<Track>(nameof(IncreaseButton)));
    DecreaseButtonProperty = (Core::Dependency::BindProperty<std::shared_ptr<Button>>::Register<Track>(nameof(DecreaseButton)));

    ThumbProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const & e)
    {
        Track* control = e.Sender->Static_As<Track>();
        if (control)
            control->ThumbChanged((XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs&)e);
    });
    IncreaseButtonProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const & e)
    {
        Track* control = e.Sender->Static_As<Track>();
        if (control)
            control->ButtonChanged((XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs&)e);
    });
    DecreaseButtonProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const & e)
    {
        Track* control = e.Sender->Static_As<Track>();
        if (control)
            control->ButtonChanged((XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs&)e);
    });

    OrientationProperty->Changed.get_observable().subscribe(AffectsArrangeInvalidate);
    MinimumProperty->Changed.get_observable().subscribe(AffectsArrangeInvalidate);
    MaximumProperty->Changed.get_observable().subscribe(AffectsArrangeInvalidate);
    ValueProperty->Changed.get_observable().subscribe(AffectsArrangeInvalidate);
}

Track::Track()
{
}

Track::~Track()
{

}

double Track::GetMinimum() const
{
    return _minimum;
}

void Track::SetMinimum(const double &value)
{
    SetAndRaise(MinimumProperty, _minimum, value);
}

double Track::GetMaximum() const
{
    return _maximum;
}

void Track::SetMaximum(const double &value)
{
    SetAndRaise(MaximumProperty, _maximum, value);
}

double Track::GetSCValue() const
{
    return _value;
}

void Track::SetSCValue(const double &value)
{
    SetAndRaise(ValueProperty, _value, value);
}

double Track::GetViewportSize() const
{
    return GetValue<double>(ViewportSizeProperty);
}

void Track::SetViewportSize(const double &value)
{
    SetValue(ViewportSizeProperty, value);
}

UI::Orientation Track::GetOrientation() const
{
    return GetValue<UI::Orientation>(OrientationProperty);
}

void Track::SetOrientation(UI::Orientation value)
{
    SetValue(OrientationProperty, value);
}

std::shared_ptr<UI::Thumb> Track::GetThumb() const
{
    return GetValue<std::shared_ptr<UI::Thumb>>(ThumbProperty);
}

void Track::SetThumb(std::shared_ptr<UI::Thumb> const& value)
{
    SetValue(ThumbProperty, value);
}

std::shared_ptr<Button> Track::GetIncreaseButton() const
{
    return GetValue<std::shared_ptr<Button>>(IncreaseButtonProperty);
}

void Track::SetIncreaseButton(std::shared_ptr<Button> const& value)
{
    SetValue(IncreaseButtonProperty, value);
}

std::shared_ptr<Button> Track::GetDecreaseButton() const
{
    return GetValue<std::shared_ptr<Button>>(DecreaseButtonProperty);
}

void Track::SetDecreaseButton(std::shared_ptr<Button> const& value)
{
    SetValue(DecreaseButtonProperty, value);
}

Core::Media::Size Track::MeasureOverride(Core::Media::Size const& availableSize)
{
    auto thumb = Thumb;

    if (thumb != nullptr)
    {
        thumb->Measure(availableSize);

        if (Orientation == UI::Orientation::Horizontal)
        {
            return Core::Media::Size(0, thumb->DesiredSize.Height);
        }
        else
        {
            return Core::Media::Size(thumb->DesiredSize.Width, 0);
        }
    }

    return Control::MeasureOverride(availableSize);
}

Core::Media::Size Track::ArrangeOverride(Core::Media::Size const& finalSize)
{
    auto thumb = Thumb.get();
    auto increaseButton = IncreaseButton;
    auto decreaseButton = DecreaseButton;

    auto range = Maximum - Minimum;
    auto offset = std::min(Value - Minimum, range);
    auto viewportSize = ViewportSize;
    auto extent = range + viewportSize;

    if (Orientation == UI::Orientation::Horizontal)
    {
        double thumbWidth = 0;

        if (viewportSize == -1.0)
        {
            thumbWidth = thumb ? thumb->DesiredSize.Width : 0;
        }
        else if (extent > 0)
        {
            thumbWidth = finalSize.Width * viewportSize / extent;
        }

        auto remaining = finalSize.Width - thumbWidth;
        auto firstWidth = range <= 0 ? 0 : remaining * offset / range;

        if (decreaseButton != nullptr)
        {
            decreaseButton->Arrange(Core::Media::Rect::xywh(0, 0, firstWidth, finalSize.Height));
        }

        if (thumb != nullptr)
        {
            thumb->Arrange(Core::Media::Rect::xywh(firstWidth, 0, thumbWidth, finalSize.Height));
        }

        if (increaseButton != nullptr)
        {
            increaseButton->Arrange(Core::Media::Rect::xywh(firstWidth + thumbWidth, 0, remaining - firstWidth, finalSize.Height));
        }
    }
    else
    {
        double thumbHeight = 0;

        if (viewportSize == -1.0)
        {
            thumbHeight = thumb ? thumb->DesiredSize.Height : 0;
        }
        else if (extent > 0)
        {
            thumbHeight = finalSize.Height * viewportSize / extent;
        }

        auto remaining = finalSize.Height - thumbHeight;
        auto firstHeight = range <= 0 ? 0 : remaining * offset / range;

        if (decreaseButton != nullptr)
        {
            decreaseButton->Arrange(Core::Media::Rect::xywh(0, 0, finalSize.Width, firstHeight));
        }

        if (thumb != nullptr)
        {
            thumb->Arrange(Core::Media::Rect::xywh(0, firstHeight, finalSize.Width, thumbHeight));
        }

        if (increaseButton != nullptr)
        {
            increaseButton->Arrange(Core::Media::Rect::xywh(0, firstHeight + thumbHeight, finalSize.Width, std::max(remaining - firstHeight, 0.0)));
        }
    }

    return finalSize;
}

void Track::ThumbChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
    auto oldThumb = (e.OldValue.TryGetValue<std::shared_ptr<UI::Thumb>>());
    auto newThumb = (e.NewValue.TryGetValue<std::shared_ptr<UI::Thumb>>());

    if (oldThumb != nullptr)
    {
        oldThumb->DragDelta -= System::make_eventhandler(&Track::ThumbDragged, this);

        RemoveLogicalChild(oldThumb.get());
        RemoveVisualChild(oldThumb.get());
    }

    if (newThumb != nullptr)
    {
        newThumb->DragDelta += System::make_eventhandler(&Track::ThumbDragged, shared_from_base_static<Track>());

        //LogicalChildren.Insert(0, newThumb);
        //m_visualChildrens.Insert(0, newThumb);
        AddLogicalChild(newThumb.get());
        AddVisualChild(newThumb.get());
    }
}

void Track::ButtonChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
    auto oldButton = (e.OldValue.TryGetValue<std::shared_ptr<UI::Button>>());
    auto newButton = (e.NewValue.TryGetValue<std::shared_ptr<UI::Button>>());

    if (oldButton != nullptr)
    {
        RemoveLogicalChild(oldButton.get());
        RemoveVisualChild(oldButton.get());
    }
    
    if (newButton != nullptr)
    {
        AddLogicalChild(newButton.get());
        AddVisualChild(newButton.get());
    }
}

void Track::ThumbDragged(void *sender, VectorEventArgs& e)
{
    double range = Maximum - Minimum;
    double value = Value;
    double offset;

    if (Orientation == UI::Orientation::Horizontal)
    {
        offset = e.Vector.x / ((Bounds.width() - Thumb->Bounds.width()) / range);
    }
    else
    {
        offset = e.Vector.y * (range / (Bounds.height() - Thumb->Bounds.height()));
    }

    if (!std::isnan(offset) && !std::isinf(offset))
    {
        value += offset;
        value = std::max(value, Minimum);
        value = std::min(value, Maximum);
        Value = value;
    }
}
