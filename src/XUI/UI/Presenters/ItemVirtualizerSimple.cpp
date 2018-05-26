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

#include "ItemVirtualizerSimple.h"
#include "UI/VirtualizingStackPanel.h"
#include "UI/Presenters/ItemsPresenter.h"
#include "UI/Generators/ItemContainerGenerator.h"
#include "Platform/UIRenderer.h"
#include "Platform/UIService.h"

using namespace XUI;
using namespace XUI::Interfaces;
using namespace XUI::UI::Presenters;

ItemVirtualizerSimple::ItemVirtualizerSimple(ItemsPresenter *owner) : ItemVirtualizer(owner)
{
    // Don't need to add children here as UpdateControls should be called by the panel
    // measure/arrange.
}

bool ItemVirtualizerSimple::GetIsLogicalScrollEnabled() const
{
    return true;
}

double ItemVirtualizerSimple::GetExtentValue() const
{
    return ItemCount;
}

double ItemVirtualizerSimple::GetOffsetValue() const
{
    double offset = VirtualizingPanel->PixelOffset > 0 ? 1 : 0;
    return FirstIndex + offset;
}

void ItemVirtualizerSimple::SetOffsetValue(const double &value)
{
    auto panel = VirtualizingPanel;
    double offset = VirtualizingPanel->PixelOffset > 0 ? 1 : 0;
    double delta = (int)(value - (FirstIndex + offset));

    if (delta != 0)
    {
        int newLastIndex = (NextIndex - 1) + delta;

        if (newLastIndex < ItemCount)
        {
            if (panel->PixelOffset > 0)
            {
                panel->PixelOffset = 0;
                delta += 1;
            }

            if (delta != 0)
            {
                RecycleContainersForMove(delta);
            }
        }
        else
        {
            // We're moving to a partially obscured item at the end of the list so
            // offset the panel by the height of the first item.
            double firstIndex = ItemCount - panel->Children.size();
            RecycleContainersForMove(firstIndex - FirstIndex);

            panel->PixelOffset = VirtualizingPanel->ScrollDirection == UI::Orientation::Vertical ?
                panel->Children[0]->Bounds.Height :
                panel->Children[0]->Bounds.Width;
        }
    }
}

double ItemVirtualizerSimple::GetViewportValue() const
{
    // If we can't fit the last item in the panel fully, subtract 1 from the viewport.
    double overflow = VirtualizingPanel->PixelOverflow > 0 ? 1 : 0;
    return VirtualizingPanel->Children.size() - overflow;
}

void ItemVirtualizerSimple::ItemsChanged(System::Collection::Generic::IContainer *items, System::Collection::NotifyCollectionChangedEventArgs& e)
{
    ItemVirtualizer::ItemsChanged(items, e);

    Generators::IItemContainerGenerator* generator = Owner->ItemContainerGenerator.get();
    auto panel = VirtualizingPanel;

    if (Items != nullptr)
    {
        switch (e.Action)
        {
            case System::Collection::NotifyCollectionChangedAction::Add:

                CreateAndRemoveContainers();

                if (e.NewStartingIndex < NextIndex)
                    RecycleContainers();

                panel->ForceInvalidateMeasure();
                break;

            case System::Collection::NotifyCollectionChangedAction::Remove:
                if (e.OldStartingIndex >= FirstIndex &&
                    e.OldStartingIndex < NextIndex)
                {
                    RecycleContainersOnRemove();
                }

                panel->ForceInvalidateMeasure();
                break;

            case System::Collection::NotifyCollectionChangedAction::Replace:
            {
                RecycleContainers();
                break;

            }
            case System::Collection::NotifyCollectionChangedAction::Move:
                // TODO: Handle move in a more efficient manner. At the moment we just
                // drop through to Reset to recreate all the containers.

            case System::Collection::NotifyCollectionChangedAction::Reset:
                RecycleContainersOnRemove();
                CreateAndRemoveContainers();
                panel->ForceInvalidateMeasure();
                break;
        }
    }
    else
    {
        Owner->ItemContainerGenerator->Clear();
        VirtualizingPanel->Children.Clear();
        FirstIndex = NextIndex = 0;
    }

    // If we are scrolled to view a partially visible last item but controls were added
    // then we need to return to a non-offset scroll position.
    if (panel->PixelOffset != 0 && FirstIndex + panel->Children.size() < ItemCount)
    {
        panel->PixelOffset = 0;
        RecycleContainersForMove(1);
    }

    InvalidateScroll();
}

void ItemVirtualizerSimple::ScrollIntoView(System::Reflection::Variant const& item)
{
    if (Items != nullptr)
    {
        auto index = Items->IndexOf(item);
    
        if (index != -1)
        {
            ScrollIntoView(index);
        }
    }
}

Core::Media::Size XUI::UI::Presenters::ItemVirtualizerSimple::MeasureOverride(Core::Media::Size const & availableSize)
{
    ItemsPresenter* scrollable = Owner.get();
    UI::Controls::Visual* visualRoot = Owner->GetVisualRoot()->ToVisual();
    Core::Media::Size maxAvailableSize = visualRoot ? visualRoot->TransformedBounds.GetSize() : Core::Media::Size::Empty;
    Core::Media::Size _availableSize = availableSize;

    // If infinity is passed as the available size and we're virtualized then we need to
    // fill the available space, but to do that we *don't* want to materialize all our
    // items! Take a look at the root of the tree for a MaxClientSize and use that as
    // the available size.
    if (VirtualizingPanel->ScrollDirection == UI::Orientation::Vertical)
    {
        if (_availableSize.Height == std::numeric_limits<double>::max())
        {
            if (visualRoot)
            {
                _availableSize = _availableSize.WithHeight(maxAvailableSize.Height);
            }
        }

        if (scrollable->CanHorizontallyScroll)
        {
            _availableSize = _availableSize.WithWidth(std::numeric_limits<double>::max());
        }
    }
    else
    {
        if (_availableSize.Width == std::numeric_limits<double>::max())
        {
            if (visualRoot)
            {
                _availableSize = _availableSize.WithWidth(maxAvailableSize.Width);
            }
        }

        if (scrollable->CanVerticallyScroll)
        {
            _availableSize = _availableSize.WithHeight(std::numeric_limits<double>::max());
        }
    }

    Owner->Panel->Measure(availableSize);
    return Owner->Panel->DesiredSize;
}

Core::Media::Size XUI::UI::Presenters::ItemVirtualizerSimple::ArrangeOverride(Core::Media::Size const & finalSize)
{
    return ItemVirtualizer::ArrangeOverride(finalSize);
}

XUI::UI::Controls::Control * XUI::UI::Presenters::ItemVirtualizerSimple::GetControlInDirection(NavigationDirection direction, Controls::Control * from)
{
    auto generator = Owner->ItemContainerGenerator;
    auto panel = VirtualizingPanel;
    auto itemIndex = generator->IndexFromContainer(from);
    auto vertical = VirtualizingPanel->ScrollDirection == UI::Orientation::Vertical;

    if (itemIndex == -1)
    {
        return nullptr;
    }

    int newItemIndex = -1;

    switch (direction)
    {
        case NavigationDirection::First:
            newItemIndex = 0;
            break;

        case NavigationDirection::Last:
            newItemIndex = ItemCount - 1;
            break;

        case NavigationDirection::Up:
            if (vertical)
            {
                newItemIndex = itemIndex - 1;
            }

            break;
        case NavigationDirection::Down:
            if (vertical)
            {
                newItemIndex = itemIndex + 1;
            }

            break;

        case NavigationDirection::Left:
            if (!vertical)
            {
                newItemIndex = itemIndex - 1;
            }
            break;

        case NavigationDirection::Right:
            if (!vertical)
            {
                newItemIndex = itemIndex + 1;
            }
            break;

        case NavigationDirection::PageUp:
            newItemIndex = std::max(0, itemIndex - (int)ViewportValue);
            break;

        case NavigationDirection::PageDown:
            newItemIndex = std::min(ItemCount - 1, itemIndex + (int)ViewportValue);
            break;
    }

    return ScrollIntoView(newItemIndex);
}

std::vector<XUI::Interfaces::ItemContainerInfo> ItemVirtualizerSimple::AddContainers(int index, System::Collection::Generic::IContainer *items)
{
    auto generator = Owner->ItemContainerGenerator.get();
    std::vector<ItemContainerInfo> result;

    for (auto enumerator = items->GetGenericEnumerator(); !enumerator->IsFinished(); enumerator->MoveNext())
    {
        auto i = AddItem(index++, enumerator->Current());
        result.push_back(i);
    }

    return result;
}

XUI::Interfaces::ItemContainerInfo ItemVirtualizerSimple::AddItem(int index, System::Reflection::Variant const& item)
{
    auto panel = Owner->Panel;
    auto generator = Owner->ItemContainerGenerator.get();
    ItemContainerInfo i = generator->Materialize(index, item, Owner->MemberSelector);

    if (i.ContainerControl != nullptr)
    {
        // Workaround for popup and cross toplevel control template
        if (i.ContainerControl->Parent && i.ContainerControl->Parent->VisualRoot != panel->VisualRoot)
        {
            i.ContainerControl->Parent->RemoveLogicalChild(i.ContainerControl.get());
        }

        if (i.Index < panel->Children.size())
        {
            // TODO: This will insert at the wrong place when there are null items.
            panel->Children.Insert(i.Index, i.ContainerControl);
        }
        else
        {
            panel->Children.Add(i.ContainerControl);
        }
    }

    return i;
}

void ItemVirtualizerSimple::RemoveContainers(std::vector<XUI::Interfaces::ItemContainerInfo> const&items)
{
    auto panel = Owner->Panel;

    for (auto const& i : items)
    {
        if (i.ContainerControl)
        {
            panel->Children.Remove(i.ContainerControl);
        }
    }
}

void MoveRange(XUI::UI::Controls::Control::LogicalChildrens& _inner, int oldIndex, int count, int newIndex)
{
    std::list<std::shared_ptr<XUI::UI::Controls::Control>> items;

    auto remove_itr = std::next(_inner.GetNativeEnumerator().begin(), oldIndex); //(oldIndex, count);
    int modifiedNewIndex = newIndex;
    for (int i = 0; i != count; ++i)
    {
        auto itr = remove_itr;
        ++remove_itr;
        items.push_back(*itr);
        _inner.GetNativeEnumerator().erase(itr);
    }
    //_inner.RemoveRange(oldIndex, count);
    
    if (newIndex > oldIndex)
    {
        modifiedNewIndex -= count;
    }
    
    int oldid = oldIndex;
    int newid = newIndex;

    auto insert_itr = std::next(_inner.GetNativeEnumerator().begin(), modifiedNewIndex);
    for (auto const& item : items)
    {
        insert_itr = _inner.GetNativeEnumerator().insert(insert_itr, item);

        if (_inner.CollectionChanged)
        {
            System::Collection::NotifyCollectionChangedEventArgs e;
            e.Action = System::Collection::NotifyCollectionChangedAction::Move;
            e.OldStartingIndex = oldid;
            e.NewStartingIndex = newid;
            e.NewValue = item;
            _inner.CollectionChanged(&_inner, e);
        }

        ++oldid;
        ++newid;
    }
}

void XUI::UI::Presenters::ItemVirtualizerSimple::RecycleContainersForMove(int delta)
{
    auto panel = VirtualizingPanel;
    auto generator = Owner->ItemContainerGenerator;
    auto selector = Owner->MemberSelector;
    auto sign = delta < 0 ? -1 : 1;
    int count = std::min(abs(delta), int(panel->Children.size()));
    int move = count < panel->Children.size();
    auto first = delta < 0 && move ? panel->Children.size() + delta : 0;

    for (int i = 0; i < count; ++i)
    {
        auto oldItemIndex = FirstIndex + first + i;
        auto newItemIndex = oldItemIndex + delta + ((panel->Children.size() - count) * sign);

        auto item = Items->ElementAt(newItemIndex);

        if (!generator->TryRecycle(oldItemIndex, newItemIndex, item, selector))
        {
            throw System::NotImplementedException();
        }
    }

    if (move)
    {
        if (delta > 0)
        {
            MoveRange(panel->Children, first, count, panel->Children.size());
            //panel->Children.MoveRange(first, count, panel->Children.size());
        }
        else
        {
            MoveRange(panel->Children, first, count, 0);
            //panel->Children.MoveRange(first, count, 0);
        }
    }

    FirstIndex += delta;
    NextIndex += delta;
}

void XUI::UI::Presenters::ItemVirtualizerSimple::RecycleContainersOnRemove()
{
    auto panel = VirtualizingPanel;

    if (NextIndex <= ItemCount)
    {
        // Items have been removed but FirstIndex..NextIndex is still a valid range in the
        // items, so just recycle the containers to adapt to the new state.
        RecycleContainers();
    }
    else
    {
        // Items have been removed and now the range FirstIndex..NextIndex goes out of 
        // the item bounds. Remove any excess containers, try to scroll up and then recycle
        // the containers to make sure they point to the correct item.
        auto newFirstIndex = std::max(0, FirstIndex - (NextIndex - ItemCount));
        auto delta = newFirstIndex - FirstIndex;
        auto newNextIndex = NextIndex + delta;

        if (newNextIndex > ItemCount)
        {
            RemoveContainers(newNextIndex - ItemCount);
        }

        if (delta != 0)
        {
            RecycleContainersForMove(delta);
        }

        RecycleContainers();
    }
}

void XUI::UI::Presenters::ItemVirtualizerSimple::CreateAndRemoveContainers()
{
    auto generator = Owner->ItemContainerGenerator;
    auto panel = VirtualizingPanel;

    if (!panel->IsFull && Items != nullptr && panel->IsAttachedToVisualTree)
    {
        auto memberSelector = Owner->MemberSelector;
        int index = NextIndex;
        int step = 1;

        while (!panel->IsFull && index >= 0)
        {
            if (index >= ItemCount)
            {
                // We can fit more containers in the panel, but we're at the end of the
                // items. If we're scrolled to the top (FirstIndex == 0), then there are
                // no more items to create. Otherwise, go backwards adding containers to
                // the beginning of the panel.
                if (FirstIndex == 0)
                {
                    break;
                }
                else
                {
                    index = FirstIndex - 1;
                    step = -1;
                }
            }

            auto materialized = generator->Materialize(index, Items->ElementAt(index), memberSelector);

            if (step == 1)
            {
                panel->Children.Add(materialized.ContainerControl);
            }
            else
            {
                panel->Children.Insert(0, materialized.ContainerControl);
            }

            index += step;
        }

        if (step == 1)
        {
            NextIndex = index;
        }
        else
        {
            NextIndex = ItemCount;
            FirstIndex = index + 1;
        }
    }

    if (panel->OverflowCount > 0)
    {
        RemoveContainers(panel->OverflowCount);
    }
}

void XUI::UI::Presenters::ItemVirtualizerSimple::RecycleContainers()
{
    auto panel = VirtualizingPanel;
    auto generator = Owner->ItemContainerGenerator;
    auto selector = Owner->MemberSelector;
    auto const& containers = generator->GetContainers();
    auto itemIndex = FirstIndex;

    for(auto container : containers)
    {
        auto item = Items->ElementAt(itemIndex);

        if (!container.second.Item.IsFastEqual(item))
        {
            if (!generator->TryRecycle(itemIndex, itemIndex, item, selector))
            {
                throw System::NotImplementedException();
            }
        }

        ++itemIndex;
    }
}

void XUI::UI::Presenters::ItemVirtualizerSimple::RemoveContainers(int count)
{
    auto index = VirtualizingPanel->Children.size() - count;

    for (int i = index; i < count; ++i)
        VirtualizingPanel->Children.RemoveAt(i);
    Owner->ItemContainerGenerator->Dematerialize(FirstIndex + index, count);
    NextIndex -= count;
}

XUI::UI::Controls::Control * XUI::UI::Presenters::ItemVirtualizerSimple::ScrollIntoView(int index)
{
    auto panel = VirtualizingPanel;
    auto generator = Owner->ItemContainerGenerator;
    auto newOffset = -1.0;

    if (index >= 0 && index < ItemCount)
    {
        if (index < FirstIndex)
        {
            newOffset = index;
        }
        else if (index >= NextIndex)
        {
            newOffset = index - ceil(ViewportValue - 1);
        }
        else if (OffsetValue + ViewportValue >= ItemCount)
        {
            newOffset = OffsetValue - 1;
        }

        if (newOffset != -1)
        {
            OffsetValue = newOffset;
        }

        auto container = generator->ContainerFromIndex(index);
        auto layoutManager = XUI::Platform::UIService::Instance()->Get<XUI::Platform::UIRender>();

        // We need to do a layout here because it's possible that the container we moved to
        // is only partially visible due to differing item sizes. If the container is only 
        // partially visible, scroll again. Don't do this if there's no layout manager:
        // it means we're running a unit test.
        if (container && layoutManager)
        {
            layoutManager->ExecuteLayoutPass();

            if (panel->ScrollDirection == UI::Orientation::Vertical)
            {
                if (container->Bounds.Y < panel->Bounds.Y || container->Bounds.Bottom > panel->Bounds.Bottom)
                {
                    OffsetValue += 1;
                }
            }
            else
            {
                if (container->Bounds.X < panel->Bounds.X || container->Bounds.Right > panel->Bounds.Right)
                {
                    OffsetValue += 1;
                }
            }
        }

        return container;
    }

    return nullptr;
}
