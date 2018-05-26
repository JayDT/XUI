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

#include "RadioButton.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;
using namespace XUI::UI;

STATIC_CLASS_INITIALIZER(RadioButton, XUI::UI::RadioButton::StaticClassInitializer)

Core::Dependency::RefDependencyProperty RadioButton::GroupNameProperty;

void RadioButton::StaticClassInitializer()
{
    GroupNameProperty = (Core::Dependency::BindProperty<std::string>::Register<RadioButton>(nameof(GroupName)));
}

void XUI::UI::RadioButton::Toggle()
{
    if (IsChecked == ToggleButtonState::TBS_NONE)
        IsChecked = ToggleButtonState::TBS_CHECKED;
}

XUI::UI::RadioButton::RadioButton()
{
    AddDirectBind(IsCheckedProperty.get(), [this](System::Reflection::Variant const& variant)
    {
        auto state = variant.GetValue<ToggleButtonState>();
        IsCheckedChanged(state);
    });
}

XUI::UI::RadioButton::~RadioButton()
{
}

std::string XUI::UI::RadioButton::GetGroupName() const
{
    return GetValue<std::string>(GroupNameProperty);
}

void XUI::UI::RadioButton::SetGroupName(std::string const & value)
{
    SetValue(GroupNameProperty, value);
}

void XUI::UI::RadioButton::IsCheckedChanged(ToggleButtonState state)
{
    auto root = GetVisualRoot();
    if (!root)
        return;

    auto visualRoot = root->ToVisual();
    if (!visualRoot)
        return;

    // ToDo: Scope guard 
    _CheckedStateOnChanging = true;

    std::string groupname = GroupName;
    visualRoot->CallVisualDescendents([&](XUI::UI::Controls::Visual* visual)
    {
        if (visual && visual != this && visual->Is<XUI::UI::RadioButton>())
        {
            XUI::UI::RadioButton* button = static_cast<XUI::UI::RadioButton*>(visual);
            if (button->IsChecked == ToggleButtonState::TBS_NONE || button->_CheckedStateOnChanging)
                return;

            if ((groupname.empty() && (button->GetVisualParent() == GetVisualParent())) || (!groupname.empty() && groupname == button->GroupName))
            {
                button->IsChecked = ToggleButtonState::TBS_NONE;
            }
        }
    });

    _CheckedStateOnChanging = false;
}
