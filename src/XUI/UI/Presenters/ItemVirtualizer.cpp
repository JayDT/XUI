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

#include "ItemVirtualizer.h"
#include "ItemsPresenter.h"
#include "ItemVirtualizerNone.h"
#include "ItemVirtualizerSimple.h"
#include "UI/VirtualizingStackPanel.h"
#include "UI/Panel.h"

using namespace XUI;
using namespace XUI::UI::Presenters;

ItemVirtualizer::ItemVirtualizer(ItemsPresenter *owner)
{
    Owner = owner->shared_from_base_static<ItemsPresenter>();
    Items = owner->Items;
    ItemCount = owner->Items->size();

    auto panel = VirtualizingPanel;
    
    //if (panel != nullptr)
    //{
    //    _subscriptions = panel->GetObservable(Panel::BoundsProperty).Skip(1).Subscribe([&](void *_)
    //    {
    //        InvalidateScroll();
    //    });
    //}
}

UI::VirtualizingStackPanel* ItemVirtualizer::GetVirtualizingPanel() const
{
    return Owner->Panel->Dynamic_As<UI::VirtualizingStackPanel>();
}

System::Collection::Generic::IContainer *ItemVirtualizer::GetItems() const
{
    return m_Items;
}

void ItemVirtualizer::SetItems(System::Collection::Generic::IContainer *value)
{
    m_Items = value;
}

int ItemVirtualizer::GetItemCount() const
{
    return m_ItemCount;
}

void ItemVirtualizer::SetItemCount(const int &value)
{
    m_ItemCount = value;
}

bool ItemVirtualizer::GetVertical() const
{
    return VirtualizingPanel ? VirtualizingPanel->ScrollDirection == Orientation::Vertical : false;
}

Core::Media::Size ItemVirtualizer::GetExtent() const
{
    return Vertical ? Core::Media::Size(Owner->Panel->DesiredSize.Width, ExtentValue) : Core::Media::Size(ExtentValue, Owner->Panel->DesiredSize.Height);
}

Core::Media::Size ItemVirtualizer::GetViewport() const
{
    return Vertical ? Core::Media::Size(Owner->Panel->Bounds.width(), ViewportValue) : Core::Media::Size(ViewportValue, Owner->Panel->Bounds.height());
}

Core::Media::Vector ItemVirtualizer::GetOffset() const
{
    return Vertical ? Core::Media::Vector(_crossAxisOffset, OffsetValue) : Core::Media::Vector(OffsetValue, _crossAxisOffset);
}

void ItemVirtualizer::SetOffset(Core::Media::Vector const& value)
{
    auto oldCrossAxisOffset = _crossAxisOffset;

    if (Vertical)
    {
        OffsetValue = value.y;
        _crossAxisOffset = value.x;
    }
    else
    {
        OffsetValue = value.x;
        _crossAxisOffset = value.y;
    }

    if (_crossAxisOffset != oldCrossAxisOffset)
    {
        Owner->InvalidateArrange();
    }
}

std::shared_ptr<ItemVirtualizer> ItemVirtualizer::Create(ItemsPresenter *owner)
{
    auto virtualizingPanel = owner->Panel->Dynamic_As<UI::VirtualizingStackPanel>();
    auto scrollable = owner;
    std::shared_ptr<ItemVirtualizer> result = nullptr;

    if (virtualizingPanel != nullptr && scrollable->InvalidateScroll != nullptr)
    {
        switch (owner->VirtualizationMode)
        {
            case ItemVirtualizationMode::Simple:
                result = std::make_shared<ItemVirtualizerSimple>(owner);
                break;
        }
    }

    if (result == nullptr)
    {
        result = std::make_shared<ItemVirtualizerNone>(owner);
    }

    if (virtualizingPanel != nullptr)
    {
        virtualizingPanel->Controller = result;
    }

    return result;
}

Core::Media::Size ItemVirtualizer::MeasureOverride(Core::Media::Size const& availableSize)
{
    Owner->Panel->Measure(availableSize);
    return Owner->Panel->DesiredSize;
}

Core::Media::Size ItemVirtualizer::ArrangeOverride(Core::Media::Size const& finalSize)
{
    if (VirtualizingPanel != nullptr)
    {
        VirtualizingPanel->CrossAxisOffset = _crossAxisOffset;
        Owner->Panel->Arrange(Core::Media::Rect::xywh(0, 0, finalSize.Width, finalSize.Height));
    }
    else
    {
        auto origin = Vertical ? Core::Media::Point(-_crossAxisOffset, 0) : Core::Media::Point(0, _crossAxisOffset);
        Owner->Panel->Arrange(Core::Media::Rect(origin, finalSize.ToVector()));
    }

    return finalSize;
}

void ItemVirtualizer::UpdateControls()
{
}

UI::Controls::Control *ItemVirtualizer::GetControlInDirection(NavigationDirection direction, UI::Controls::Control *from)
{
    return nullptr;
}

void ItemVirtualizer::ItemsChanged(System::Collection::Generic::IContainer *items, System::Collection::NotifyCollectionChangedEventArgs& e)
{
    Items = items;
    ItemCount = items->size();
}

void ItemVirtualizer::ScrollIntoView(System::Reflection::Variant const& item)
{
}

ItemVirtualizer::~ItemVirtualizer()
{
    if (_subscriptions.is_subscribed())
        _subscriptions.unsubscribe();

    if (VirtualizingPanel != nullptr)
    {
        VirtualizingPanel->Controller = nullptr;
        VirtualizingPanel->Children.Clear();
    }
    
    //Owner->ItemContainerGenerator->Clear();
}

void ItemVirtualizer::InvalidateScroll()
{
    Owner->Static_As<ItemsPresenter>()->InvalidateScroll();
}
