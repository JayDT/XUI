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

#include "ItemVirtualizerNone.h"
#include "UI/Panel.h"
#include "UI/Presenters/ItemsPresenter.h"
#include "UI/Generators/ItemContainerGenerator.h"

using namespace XUI;
using namespace XUI::Interfaces;
using namespace XUI::UI::Presenters;

ItemVirtualizerNone::ItemVirtualizerNone(ItemsPresenter *owner) : ItemVirtualizer(owner)
{
    /// ItemsPresenterBase::CreatePanel will be initialized Items because call Reset notify
    //if (Items != nullptr && owner->Panel != nullptr)
    //{
    //    AddContainers(0, Items);
    //}
}

bool ItemVirtualizerNone::GetIsLogicalScrollEnabled() const
{
    return false;
}

double ItemVirtualizerNone::GetExtentValue() const
{
    throw System::NotSupportedException();
}

double ItemVirtualizerNone::GetOffsetValue() const
{
    throw System::NotSupportedException();
}

void ItemVirtualizerNone::SetOffsetValue(const double &value)
{
    throw System::NotSupportedException();
}

double ItemVirtualizerNone::GetViewportValue() const
{
    throw System::NotSupportedException();
}

void ItemVirtualizerNone::ItemsChanged(System::Collection::Generic::IContainer *items, System::Collection::NotifyCollectionChangedEventArgs& e)
{
    ItemVirtualizer::ItemsChanged(items, e);

    Generators::IItemContainerGenerator* generator = Owner->ItemContainerGenerator.get();

    switch (e.Action)
    {
        case System::Collection::NotifyCollectionChangedAction::Add:
            if (e.NewStartingIndex < Items->size()) // + e.NewItems->Count < Items->Count())
                generator->InsertSpace(e.NewStartingIndex, 1/*e.NewItems->Count*/);
            //AddContainers(e.NewStartingIndex, e->NewItems);
            AddItem(e.NewStartingIndex, e.NewValue/*items->ElementAt(e.NewStartingIndex)*/);
            break;
    
        case System::Collection::NotifyCollectionChangedAction::Remove:
            RemoveContainers(generator->RemoveRange(e.OldStartingIndex, 1/*e.OldItems->Count*/));
            break;
    
        case System::Collection::NotifyCollectionChangedAction::Replace:
        {
            auto panel = Owner->Panel.get();

            RemoveContainers(generator->Dematerialize(e.OldStartingIndex, 1/*e.OldItems->Count*/));
            //auto containers = AddContainers(e.NewStartingIndex, e->NewItems);
            //
            //auto i = e->NewStartingIndex;
            //
            //for (auto container : containers)
            //{
            //    panel->Children[i++] = container->ContainerControl;
            //}
            auto container = AddItem(e.NewStartingIndex, items->ElementAt(e.NewStartingIndex));
            panel->Children[e.NewStartingIndex] = container.ContainerControl;
            break;
    
        }
        case System::Collection::NotifyCollectionChangedAction::Move:
            // TODO: Handle move in a more efficient manner. At the moment we just
            // drop through to Reset to recreate all the containers.
    
        case System::Collection::NotifyCollectionChangedAction::Reset:
            RemoveContainers(generator->Clear());
            break;

        case System::Collection::NotifyCollectionChangedAction::Init:
            RemoveContainers(generator->Clear());

            if (Items != nullptr)
            {
                AddContainers(0, Items);
            }
    
            break;
    }

    Owner->InvalidateMeasure();
}

void ItemVirtualizerNone::ScrollIntoView(System::Reflection::Variant const& item)
{
    if (Items != nullptr)
    {
        auto index = Items->IndexOf(item);
    
        if (index != -1)
        {
            auto container = Owner->ItemContainerGenerator->ContainerFromIndex(index);
            if (container)
                container->BringIntoView(Core::Media::Rect::xywh(0, 0, container->Bounds.width(), container->Bounds.height()));
        }
    }
}

std::vector<XUI::Interfaces::ItemContainerInfo> ItemVirtualizerNone::AddContainers(int index, System::Collection::Generic::IContainer *items)
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

XUI::Interfaces::ItemContainerInfo ItemVirtualizerNone::AddItem(int index, System::Reflection::Variant const& item)
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

void ItemVirtualizerNone::RemoveContainers(std::vector<XUI::Interfaces::ItemContainerInfo> const&items)
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
