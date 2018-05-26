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

#include "TabStrip.h"
#include "TabStripItem.h"
#include "UI/WrapPanel.h"
#include "UI/Detail/HeaderedContentControl.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XUI::UI;

STATIC_CLASS_INITIALIZER(TabStrip, TabStrip::StaticClassInitializer)

std::shared_ptr<XamlCPP::Interfaces::ITemplate> TabStrip::DefaultPanel = std::make_shared<DataTemplates::FuncTemplate<std::shared_ptr<XUI::UI::Panel>>>([]()
{
    auto panel = std::make_shared<WrapPanel>();
    panel->Orientation = Orientation::Horizontal;
    return panel;
});

std::function<System::Reflection::Variant(System::Reflection::Variant const&)> TabStrip::MemberSelector = TabStrip::SelectHeader;

void TabStrip::StaticClassInitializer()
{
    MemberSelectorProperty->OverrideDefaultValue<TabStrip>(System::Delegate<System::Reflection::Variant(System::Reflection::Variant const&)>(MemberSelector));
    SelectionModeProperty->OverrideDefaultValue<TabStrip>(SelectionMode::AlwaysSelected);
    FocusableProperty->OverrideDefaultValue<TabStrip>(false);
    ItemsPanelProperty->OverrideDefaultValue<TabStrip>(DefaultPanel);
}

std::shared_ptr<Generators::IItemContainerGenerator> TabStrip::CreateItemContainerGenerator()
{
    return std::make_shared<Generators::ItemContainerGenerator<TabStripItem>>(this, Controls::ContentControl::ContentProperty.get(), Controls::ContentControl::ContentTemplateProperty.get());
}

void TabStrip::OnGotFocus(XUI::Interfaces::GotFocusEventArgs& e)
{
    SelectingItemsControl::OnGotFocus(e);

    if (e.NavigationMethod == XUI::Interfaces::NavigationMethod::Directional)
    {
        e.Handled = UpdateSelectionFromEventSource(e.Source);
    }
}

void TabStrip::OnPointerPressed(XUI::Interfaces::PointerPressedEventArgs& e)
{
    SelectingItemsControl::OnPointerPressed(e);

    if (e.MouseButton == XUI::Interfaces::MouseButton::Left)
    {
        e.Handled = UpdateSelectionFromEventSource(e.Source);
    }
}

void XUI::UI::TabStrip::ItemsChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs & e)
{
}

System::Reflection::Variant TabStrip::SelectHeader(System::Reflection::Variant o)
{
    if (o.GetType()->IsAssignableFrom<UI::HeaderedContentControl>())
    {
        auto headered = o.TryGetValue<UI::HeaderedContentControl*>();
        if (headered)
            return headered->Header;
    }

    return std::string("");
}
