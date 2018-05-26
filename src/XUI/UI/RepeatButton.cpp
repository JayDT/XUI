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

#include "RepeatButton.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;
using namespace XUI::Interfaces;
using namespace XUI::UI;

STATIC_CLASS_INITIALIZER(RepeatButton, RepeatButton::StaticClassInitializer)

Core::Dependency::RefDependencyProperty RepeatButton::DelayProperty;

void RepeatButton::StaticClassInitializer()
{
    DelayProperty = (Core::Dependency::BindProperty<int>::Register<RepeatButton>(nameof(Delay)));
}

int RepeatButton::GetDelay() const
{
    return GetValue<int>(DelayProperty);
}

void RepeatButton::SetDelay(const int &value)
{
    SetValue(DelayProperty, value);
}

void RepeatButton::StartTimer()
{
    //if (_repeatTimer == nullptr)
    //{
    //    _repeatTimer = new DispatcherTimer();
    //    _repeatTimer->Tick += [&](o, e)
    //    {
    //        OnClick();
    //    };
    //}
    //
    //if (_repeatTimer->IsEnabled)
    //{
    //    return;
    //}
    //
    //_repeatTimer->Interval = TimeSpan::FromMilliseconds(Delay);
    //_repeatTimer->Start();
}

void RepeatButton::StopTimer()
{
    //_repeatTimer == nullptr ? nullptr : _repeatTimer->Stop();
}

void RepeatButton::OnKeyDown(KeyEventArgs& e)
{
    Button::OnKeyDown(e);

    if (e.Key == Key::Space)
    {
        StartTimer();
    }
}

void RepeatButton::OnKeyUp(KeyEventArgs& e)
{
    Button::OnKeyUp(e);

    StopTimer();
}

void RepeatButton::OnPointerPressed(PointerPressedEventArgs& e)
{
    Button::OnPointerPressed(e);

    if (e.MouseButton == MouseButton::Left)
    {
        StartTimer();
    }
}

void RepeatButton::OnPointerReleased(PointerReleasedEventArgs& e)
{
    Button::OnPointerReleased(e);

    if (e.MouseButton == MouseButton::Left)
    {
        StopTimer();
    }
}
