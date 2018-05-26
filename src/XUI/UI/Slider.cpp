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

#include "Slider.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;
using namespace XUI::UI;

STATIC_CLASS_INITIALIZER(Slider, XUI::UI::Slider::StaticClassInitializer);

XUI::Core::Dependency::RefDependencyProperty Slider::OrientationProperty;
XUI::Core::Dependency::RefDependencyProperty Slider::IsSnapToTickEnabledProperty;
XUI::Core::Dependency::RefDependencyProperty Slider::TickFrequencyProperty;

void XUI::UI::Slider::StaticClassInitializer()
{
    OrientationProperty = (Core::Dependency::BindProperty<UI::Orientation>::Register<Slider>(nameof(Orientation), UI::Orientation::Horizontal));
    IsSnapToTickEnabledProperty = (Core::Dependency::BindProperty<bool>::Register<Slider>(nameof(IsSnapToTickEnabled), false));
    TickFrequencyProperty = (Core::Dependency::BindProperty<double>::Register<Slider>(nameof(TickFrequency), 0.0));

    Thumb::DragStartedEvent->AddClassSpecHandler<Interfaces::VectorEventArgs, Slider>(System::make_eventhandler([](void* x, Interfaces::VectorEventArgs& e)
    {
        static_cast<Slider*>((System::Reflection::Object*)x)->OnThumbDragStarted(e);
    }), Core::Observer::RoutingStrategies::Bubble);
    Thumb::DragDeltaEvent->AddClassSpecHandler<Interfaces::VectorEventArgs, Slider>(System::make_eventhandler([](void* x, Interfaces::VectorEventArgs& e)
    {
        static_cast<Slider*>((System::Reflection::Object*)x)->OnThumbDragDelta(e);
    }), Core::Observer::RoutingStrategies::Bubble);
    Thumb::DragCompletedEvent->AddClassSpecHandler<Interfaces::VectorEventArgs, Slider>(System::make_eventhandler([](void* x, Interfaces::VectorEventArgs& e)
    {
        static_cast<Slider*>((System::Reflection::Object*)x)->OnThumbDragCompleted(e);
    }), Core::Observer::RoutingStrategies::Bubble);

    SmallChangeProperty->OverrideDefaultValue<Slider>(1.0);
    LargeChangeProperty->OverrideDefaultValue<Slider>(10.0);
}

Slider::Slider()
{
}

UI::Orientation Slider::GetOrientation() const
{
    return GetValue<UI::Orientation>(OrientationProperty);
}

void Slider::SetOrientation(UI::Orientation value)
{
    SetValue(OrientationProperty, value);
}

bool Slider::GetIsSnapToTickEnabled() const
{
    return GetValue<bool>(IsSnapToTickEnabledProperty);
}

void Slider::SetIsSnapToTickEnabled(const bool &value)
{
    SetValue(IsSnapToTickEnabledProperty, value);
}

double Slider::GetTickFrequency() const
{
    return GetValue<double>(TickFrequencyProperty);
}

void Slider::SetTickFrequency(const double &value)
{
    SetValue(TickFrequencyProperty, value);
}

void Slider::OnTemplateApplied(Controls::TemplateAppliedEventArgs const& e)
{
    if (_decreaseButton != nullptr)
    {
        _decreaseButton->Click -= System::make_eventhandler(&Slider::DecreaseClick, this);
    }

    if (_increaseButton != nullptr)
    {
        _increaseButton->Click -= System::make_eventhandler(&Slider::IncreaseClick, this);
    }

    _decreaseButton = Template->FindNameByManagedType<Button>("PART_DecreaseButton", this);
    _track = Template->FindNameByManagedType<Track>("PART_Track", this);
    _increaseButton = Template->FindNameByManagedType<Button>("PART_IncreaseButton", this);

    if (_decreaseButton != nullptr)
    {
        _decreaseButton->Click += System::make_eventhandler(&Slider::DecreaseClick, this);
    }

    if (_increaseButton != nullptr)
    {
        _increaseButton->Click += System::make_eventhandler(&Slider::IncreaseClick, this);
    }
}

void Slider::DecreaseClick(void* sender, Core::Observer::RoutedEventArgs& e)
{
    ChangeValueBy(-LargeChange);
}

void Slider::IncreaseClick(void* sender, Core::Observer::RoutedEventArgs& e)
{
    ChangeValueBy(LargeChange);
}

void Slider::ChangeValueBy(double by)
{
    if (IsSnapToTickEnabled)
    {
        by = by < 0 ? std::min(-TickFrequency, by) : std::max(TickFrequency, by);
    }

    auto value = Value;
    auto next = SnapToTick(std::max(std::min(value + by, Maximum), Minimum));
    if (next != value)
    {
        Value = next;
    }
}

void Slider::OnThumbDragStarted(Interfaces::VectorEventArgs& e)
{
}

void Slider::OnThumbDragDelta(Interfaces::VectorEventArgs& e)
{
    Thumb *thumb = e.Source->Static_As<Thumb>();
    if (thumb && _track && _track->Thumb.get() == thumb)
        MoveToNextTick(_track->Value);
}

void Slider::OnThumbDragCompleted(Interfaces::VectorEventArgs& e)
{
}

void Slider::MoveToNextTick(double value)
{
    double next = SnapToTick(std::max(Minimum, std::min(Maximum, value)));
    if (next != value)
        Value = next;
}

double Slider::SnapToTick(double value)
{
    if (IsSnapToTickEnabled && TickFrequency > 0.0)
    {
        double previous = Minimum + (std::round(((value - Minimum) / TickFrequency)) * TickFrequency);
        double next = std::min(Maximum, previous + TickFrequency);
        value = value > (previous + next) * 0.5 ? next : previous;
    }

    return value;
}
