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

#include "ItemsPresenter.h"
#include "ItemVirtualizer.h"
#include "Core/GlobalInitializerScheduler.h"
#include "Core/Input/KeyboardDevice.h"

using namespace XUI;
using namespace XUI::UI;
using namespace XUI::UI::Presenters;


STATIC_CLASS_INITIALIZER(ItemsPresenter, XUI::UI::Presenters::ItemsPresenter::StaticClassInitializer)

Core::Dependency::RefDependencyProperty Presenters::ItemsPresenter::VirtualizationModeProperty;

void XUI::UI::Presenters::ItemsPresenter::StaticClassInitializer()
{
    VirtualizationModeProperty = (Core::Dependency::BindProperty<ItemVirtualizationMode>::Register<ItemsPresenter>(nameof(VirtualizationMode)));

    //KeyboardNavigation::TabNavigationProperty::OverrideDefaultValue(ItemsPresenter::typeid, KeyboardNavigationMode::Once);
}

ItemsPresenter::ItemsPresenter()
{

}

ItemVirtualizationMode ItemsPresenter::GetVirtualizationMode() const
{
    return GetValue<ItemVirtualizationMode>(VirtualizationModeProperty);
}

void ItemsPresenter::SetVirtualizationMode(ItemVirtualizationMode value)
{
    SetValue(VirtualizationModeProperty, value);
}

bool ItemsPresenter::GetIsLogicalScrollEnabled()
{
    return _virtualizer ? _virtualizer->IsLogicalScrollEnabled ? _virtualizer->IsLogicalScrollEnabled : false : false;
}

Core::Media::Size ItemsPresenter::GetExtent()
{
    return _virtualizer->Extent;
}

Core::Media::Vector ItemsPresenter::GetOffset()
{
    return _virtualizer->Offset;
}

void ItemsPresenter::SetOffset(Core::Media::Vector const& value)
{
    _virtualizer->Offset = CoerceOffset(value);
}

Core::Media::Size ItemsPresenter::GetViewport()
{
    return _virtualizer->Viewport;
}

std::function<void()> ItemsPresenter::GetInvalidateScroll()
{
    return _invalidateScroll;
}

void ItemsPresenter::SetInvalidateScroll(const std::function<void()> &value)
{
    _invalidateScroll = value;
}

Core::Media::Size ItemsPresenter::GetScrollSize()
{
    return Core::Media::Size(1, 1);
}

Core::Media::Size ItemsPresenter::GetPageScrollSize()
{
    return Core::Media::Size(0, 1);
}

bool ItemsPresenter::BringIntoView(Controls::Control *target, Core::Media::Rect const& targetRect)
{
    return false;
}

Controls::Control *ItemsPresenter::GetControlInDirection(NavigationDirection direction, Controls::Control *from)
{
    return _virtualizer ? _virtualizer->GetControlInDirection(direction, from) : nullptr;
}

void ItemsPresenter::ScrollIntoView(System::Reflection::Variant const& item)
{
    if (_virtualizer)
        _virtualizer->ScrollIntoView(item);
}

Core::Media::Size ItemsPresenter::MeasureOverride(Core::Media::Size const& availableSize)
{
    if (_virtualizer)
        return _virtualizer->MeasureOverride(availableSize);
    return Core::Media::Size::Empty;
}

Core::Media::Size ItemsPresenter::ArrangeOverride(Core::Media::Size const& finalSize)
{
    if (_virtualizer)
        return _virtualizer->ArrangeOverride(finalSize);
    return Core::Media::Size::Empty;
}

void ItemsPresenter::PanelCreated(UI::Panel *panel)
{
    _virtualizer = ItemVirtualizer::Create(this);
    if (InvalidateScroll)
        InvalidateScroll();

    if (!Panel->IsSet(Core::Input::KeyboardDevice::DirectionalNavigationProperty.get()))
    {
        Core::Input::KeyboardDevice::SetDirectionalNavigation(Panel.get(), Core::Input::KeyboardNavigationMode::Contained);
    }
    
    Core::Input::KeyboardDevice::SetTabNavigation(Panel.get(), Core::Input::KeyboardDevice::GetTabNavigation(this));
}

void ItemsPresenter::ItemsChanged(System::Collection::NotifyCollectionChangedEventArgs& e)
{
    if (_virtualizer)
        _virtualizer->ItemsChanged(Items, e);
}

Core::Media::Vector ItemsPresenter::CoerceOffset(Core::Media::Vector const& value)
{
    auto scrollable = this;
    auto maxX = std::max(scrollable->Extent.Width - scrollable->Viewport.Width, 0.0);
    auto maxY = std::max(scrollable->Extent.Height - scrollable->Viewport.Height, 0.0);
    return Core::Media::Vector(std::clamp(value.x, 0.0f, (float)maxX), std::clamp(value.y, 0.0f, (float)maxY));
}

void ItemsPresenter::VirtualizationModeChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
    if (_virtualizer)
        _virtualizer = nullptr;
    _virtualizer = ItemVirtualizer::Create(this);
    if (InvalidateScroll)
        InvalidateScroll();
}
