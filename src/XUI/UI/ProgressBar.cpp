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

#include "ProgressBar.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;
using namespace XUI::UI;

STATIC_CLASS_INITIALIZER(ProgressBar, XUI::UI::ProgressBar::StaticClassInitializer);

XUI::Core::Dependency::RefDependencyProperty ProgressBar::IsIndeterminateProperty;
XUI::Core::Dependency::RefDependencyProperty ProgressBar::OrientationProperty;

void XUI::UI::ProgressBar::StaticClassInitializer()
{
    IsIndeterminateProperty = (Core::Dependency::BindProperty<bool>::Register<ProgressBar>(nameof(IsIndeterminate)));
    OrientationProperty = (Core::Dependency::BindProperty<UI::Orientation>::Register<ProgressBar>(nameof(Orientation), UI::Orientation::Horizontal));

    ValueProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const & e)
    {
        ProgressBar* control = e.Sender->Static_As<ProgressBar>();
        if (control)
            control->ValueChanged((XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs&)e);
    });

    IsIndeterminateProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const & e)
    {
        ProgressBar* control = e.Sender->Static_As<ProgressBar>();
        if (control)
            control->UpdateIsIndeterminate(control->IsIndeterminate);
    });

    ValueProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const & e)
    {
        ProgressBar* control = e.Sender->Static_As<ProgressBar>();
        if (control)
            control->UpdateOrientation(control->Orientation);
    });
}

XUI::UI::ProgressBar::ProgressBar()
    : _indeterminateAnimation(this)
{
}

void XUI::UI::ProgressBar::BeginInit()
{
    RangeBase::BeginInit();
}

bool ProgressBar::GetIsIndeterminate() const
{
    return GetValue<bool>(IsIndeterminateProperty);
}

void ProgressBar::SetIsIndeterminate(const bool &value)
{
    SetValue(IsIndeterminateProperty, value);
}

UI::Orientation ProgressBar::GetOrientation() const
{
    return GetValue<UI::Orientation>(OrientationProperty);
}

void ProgressBar::SetOrientation(UI::Orientation value)
{
    SetValue(OrientationProperty, value);
}

Core::Media::Size ProgressBar::ArrangeOverride(Core::Media::Size const&finalSize)
{
    UpdateIndicator(finalSize);
    return RangeBase::ArrangeOverride(finalSize);
}

void ProgressBar::OnTemplateApplied(Controls::TemplateAppliedEventArgs const& e)
{
    auto indicatorObj = Template->FindName("PART_Indicator", this);
    if (indicatorObj && indicatorObj->Is<Border>())
        _indicator = indicatorObj->Static_As<Border>()->shared_from_base_static<Border>();

    UpdateIndicator(Bounds.GetSize());
    UpdateOrientation(Orientation);
    UpdateIsIndeterminate(IsIndeterminate);
}

void ProgressBar::UpdateIndicator(Core::Media::Size const& bounds)
{
    if (!_indicator)
        return;

    if (IsIndeterminate)
    {
        if (Orientation == UI::Orientation::Horizontal)
        {
            _indicator->Width = bounds.Width / 5.0;
        }
        else
        {
            _indicator->Height = bounds.Height / 5.0;
        }
    }
    else
    {
        double percent = Maximum == Minimum ? 1.0 : (Value - Minimum) / (Maximum - Minimum);

        if (Orientation == UI::Orientation::Horizontal)
        {
            _indicator->Width = std::max(1.0, bounds.Width * percent);
        }
        else
        {
            _indicator->Height = std::max(1.0, bounds.Height * percent);
        }
    }
}

void ProgressBar::UpdateOrientation(UI::Orientation orientation)
{
    if (!_indicator)
        return;

    if (orientation == UI::Orientation::Horizontal)
    {
        MinHeight = 14;
        MinWidth = 200;

        _indicator->HorizontalAlignment = Controls::HorizontalAlignment::Left;
        _indicator->VerticalAlignment = Controls::VerticalAlignment::Stretch;
    }
    else
    {
        MinHeight = 200;
        MinWidth = 14;

        _indicator->HorizontalAlignment = Controls::HorizontalAlignment::Stretch;
        _indicator->VerticalAlignment = Controls::VerticalAlignment::Bottom;
    }
}

void ProgressBar::UpdateIsIndeterminate(bool isIndeterminate)
{
    if (!_indicator)
        return;

    if (isIndeterminate)
    {
        if (_indeterminateAnimation.Disposed)
        {
            _indeterminateAnimation.StartAnimation(this);
        }
    }
    else
    {
        _indeterminateAnimation.Stop();
    }
}

void ProgressBar::ValueChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
    UpdateIndicator(Bounds.GetSize());
}

bool ProgressBar::IndeterminateAnimation::GetDisposed() const
{
    return _disposed;
}

ProgressBar::IndeterminateAnimation::IndeterminateAnimation(ProgressBar *progressBar)
    : _progressBar(progressBar)
{
}

void ProgressBar::IndeterminateAnimation::InitializeComponent()
{

}

void ProgressBar::IndeterminateAnimation::StartAnimation(ProgressBar *progressBar)
{
    _startTime = std::chrono::system_clock::now();

    // ToDo: Animate manager
    //_indeterminateBindSubscription = Animate::Timer::TakeWhile([&](System::Reflection::Variant x)
    //{
    //    return (x - _startTime).TotalSeconds <= 4.0;
    //})->Select(GetAnimationRect)->Finally(() = > _startTime = Animate::Stopwatch::Elapsed).Repeat()->Subscribe(AnimationTick);
}

void XUI::UI::ProgressBar::IndeterminateAnimation::Stop()
{
    if (_indeterminateBindSubscription.is_subscribed())
        _indeterminateBindSubscription.unsubscribe();
    _disposed = true;
}

Core::Media::Rect ProgressBar::IndeterminateAnimation::GetAnimationRect(std::chrono::time_point<std::chrono::system_clock> const& time)
{
    if (_progressBar)
    {
        if (_progressBar->Orientation == UI::Orientation::Horizontal)
        {
            return Core::Media::Rect(-_progressBar->_indicator->Width - 5 + std::chrono::duration_cast<std::chrono::seconds>(time - _startTime).count() / 4.0 * (_progressBar->Bounds.Width + _progressBar->_indicator->Width + 10), 0, _progressBar->_indicator->Bounds.Width, _progressBar->_indicator->Bounds.Height);
        }
        else
        {
            return Core::Media::Rect(0, _progressBar->Bounds.Height + 5 - std::chrono::duration_cast<std::chrono::seconds>(time - _startTime).count() / 4.0 * (_progressBar->Bounds.Height + _progressBar->_indicator->Height + 10), _progressBar->_indicator->Bounds.Width, _progressBar->_indicator->Bounds.Height);
        }
    }
    else
    {
        if (_indeterminateBindSubscription.is_subscribed())
            _indeterminateBindSubscription.unsubscribe();
        return Core::Media::Rect::Empty;
    }

    return Core::Media::Rect::Empty;
}

void ProgressBar::IndeterminateAnimation::AnimationTick(Core::Media::Rect const& rect)
{
    if (_progressBar)
    {
        _progressBar->_indicator->Arrange(rect);
    }
    else
    {
        if (_indeterminateBindSubscription.is_subscribed())
            _indeterminateBindSubscription.unsubscribe();
    }
}

ProgressBar::IndeterminateAnimation::~IndeterminateAnimation()
{
    Stop();
}
