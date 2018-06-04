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

#include "RangeBase.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;
using namespace XUI::UI;

STATIC_CLASS_INITIALIZER(RangeBase, RangeBase::StaticClassInitializer)

Core::Dependency::RefDependencyProperty RangeBase::MinimumProperty;
Core::Dependency::RefDependencyProperty RangeBase::MaximumProperty;
Core::Dependency::RefDependencyProperty RangeBase::ValueProperty;
Core::Dependency::RefDependencyProperty RangeBase::SmallChangeProperty;
Core::Dependency::RefDependencyProperty RangeBase::LargeChangeProperty;
Core::Observer::RefRoutedEvent          RangeBase::ValueChangedEvent;

void XUI::UI::RangeBase::StaticClassInitializer()
{
    MinimumProperty = (Core::Dependency::DirectProperty<RangeBase, double>::RegisterDirect(nameof(Minimum),
        [](RangeBase* x) { return x->Minimum; },
        [](RangeBase* x, double v) { x->Minimum = v; })
        );

    MaximumProperty = (Core::Dependency::DirectProperty<RangeBase, double>::RegisterDirect(nameof(Maximum),
        [](RangeBase* x) { return x->Maximum; },
        [](RangeBase* x, double v) { x->Maximum = v; })
        );

    ValueProperty = (Core::Dependency::DirectProperty<RangeBase, double>::RegisterDirect(nameof(Value),
        [](RangeBase* x) { return x->Value; },
        [](RangeBase* x, double v) { x->Value = v; })
        );

    ValueChangedEvent = (Core::Observer::RoutedEvent::Register<XUI::UI::SelectingItemsControl, Core::Observer::RoutedEventArgs>(nameof(ValueChanged), Core::Observer::RoutingStrategies::Bubble));

    SmallChangeProperty = (Core::Dependency::BindProperty<double>::Register<RangeBase>(nameof(ItemsPanel), 0.1));
    LargeChangeProperty = (Core::Dependency::BindProperty<double>::Register<RangeBase>(nameof(ItemsPanel), 1.0));

    //ValueChangedEventArgs

    ValueProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const & e)
    {
        RangeBase* control = e.Sender->Dynamic_As<RangeBase>();
        if (control)
        {
            Interfaces::ValueChangedEventArgs _e(ValueChangedEvent.get());
            _e.Value = e.NewValue;
            control->RaiseEvent(_e);
        }
    });
}

RangeBase::RangeBase()
    : ValueChanged(this)
{
}

double RangeBase::GetMinimum() const
{
    return _minimum;
}

void RangeBase::SetMinimum(const double &value)
{
    auto _value = ValidateMinimum(value);
    Controls::Control::SetAndRaise(MinimumProperty, _minimum, _value);
    Maximum = ValidateMaximum(Maximum);
    Value = ValidateValue(Value);
}

double RangeBase::GetMaximum() const
{
    return _maximum;
}

void RangeBase::SetMaximum(const double &value)
{
    auto _value = ValidateMaximum(value);
    Controls::Control::SetAndRaise(MaximumProperty, _maximum, _value);
    Value = ValidateValue(Value);
}

double RangeBase::GetSCValue() const
{
    return _value;
}

void RangeBase::SetSCValue(const double &value)
{
    auto _valueNew = ValidateValue(value);
    SetAndRaise(ValueProperty, _value, _valueNew);
}

double RangeBase::GetSmallChange() const
{
    return Controls::Control::GetValue<double>(SmallChangeProperty);
}

void RangeBase::SetSmallChange(const double &value)
{
    Controls::Control::SetValue(SmallChangeProperty, value);
}

double RangeBase::GetLargeChange() const
{
    return Controls::Control::GetValue<double>(LargeChangeProperty);
}

void RangeBase::SetLargeChange(const double &value)
{
    Controls::Control::SetValue(LargeChangeProperty, value);
}

void RangeBase::ValidateDouble(double value, const std::string &property_Renamed)
{
    if (std::isinf(value) || std::isnan(value))
        throw System::ArgumentException("{value} is not a valid value for {property}.");
}

double RangeBase::ValidateMinimum(double value)
{
    ValidateDouble(value, "Minimum");
    return value;
}

double RangeBase::ValidateMaximum(double value)
{
    ValidateDouble(value, "Maximum");
    return std::max(value, Minimum);
}

double RangeBase::ValidateValue(double value)
{
    ValidateDouble(value, "Value");
    return std::clamp(value, Minimum, Maximum);
}
