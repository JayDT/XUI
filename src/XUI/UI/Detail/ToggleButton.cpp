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

#include "ToggleButton.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;
using namespace XUI::UI;

STATIC_CLASS_INITIALIZER(ToggleButton, XUI::UI::ToggleButton::StaticClassInitializer)

Core::Dependency::RefDependencyProperty ToggleButton::IsCheckedProperty;
Core::Dependency::RefDependencyProperty ToggleButton::IsThreeStateProperty;

void XUI::UI::ToggleButton::StaticClassInitializer()
{
    IsCheckedProperty = (Core::Dependency::DirectProperty<ToggleButton, ToggleButtonState>::RegisterDirect(nameof(IsChecked),
        [](ToggleButton* x) { return x->IsChecked; },
        [](ToggleButton* x, ToggleButtonState v) { x->IsChecked = v; })
        );
    IsThreeStateProperty = (Core::Dependency::BindProperty<bool>::Register<ToggleButton>(nameof(IsThreeState)));
}

ToggleButtonState ToggleButton::GetIsChecked() const
{
    return _isChecked;
}

void ToggleButton::SetIsChecked(ToggleButtonState value)
{
    SetAndRaise(IsCheckedProperty, _isChecked, value);
}

bool ToggleButton::GetIsThreeState() const
{
    return GetValue<bool>(IsThreeStateProperty);
}

void ToggleButton::SetIsThreeState(bool value)
{
    SetValue(IsThreeStateProperty, value);
}

void ToggleButton::OnClick(Core::Observer::RoutedEventArgs & e)
{
    Toggle();
    Button::OnClick(e);
}

void ToggleButton::Toggle()
{
    if (IsChecked == ToggleButtonState::TBS_CHECKED)
    {
        if (IsThreeState)
            IsChecked = ToggleButtonState::TBS_INTERMEDIATE;
        else
            IsChecked = ToggleButtonState::TBS_NONE;
    }
    else if (IsChecked == ToggleButtonState::TBS_INTERMEDIATE)
    {
        IsChecked = ToggleButtonState::TBS_NONE;
    }
    else
    {
        IsChecked = ToggleButtonState::TBS_CHECKED;
    }
}

