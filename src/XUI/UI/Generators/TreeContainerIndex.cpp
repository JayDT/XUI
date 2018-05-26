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

#include "TreeContainerIndex.h"
#include "cpplinq/linq.hpp"

using namespace XUI;
using namespace XUI::UI;
using namespace XUI::UI::Generators;

std::vector<Controls::Control*> TreeContainerIndex::GetItems() const
{
    return cpplinq::from(_containerToItem).select([](std::pair<std::shared_ptr<Controls::Control>, System::Reflection::RefObject> const& x) { return x.first.get(); }).to_vector();
}

void TreeContainerIndex::Add(System::Reflection::RefObject const& item, Controls::Control *container)
{
    _itemToContainer[item] = container->shared_from_base_static<Controls::Control>();
    _containerToItem[container->shared_from_base_static<Controls::Control>()] = item;

    if (Materialized)
    {
        Interfaces::ItemContainerInfo containerInfo(container->shared_from_base_static<Controls::Control>(), item, 0);
        System::Collection::Vector<Interfaces::ItemContainerInfo> result;
        result.Add(containerInfo);
        Interfaces::ItemContainerEventArgs e(containerInfo.Index, &result);
        Materialized(this, e);
    }
}

void TreeContainerIndex::Remove(Controls::Control *container)
{
    auto item = _containerToItem[container->shared_from_base_static<Controls::Control>()];
    _containerToItem.erase(container->shared_from_base_static<Controls::Control>());
    _itemToContainer.erase(item);

    if (Dematerialized)
    {
        Interfaces::ItemContainerInfo containerInfo(container->shared_from_base_static<Controls::Control>(), item, 0);
        System::Collection::Vector<Interfaces::ItemContainerInfo> result;
        result.Add(containerInfo);
        Interfaces::ItemContainerEventArgs e(containerInfo.Index, &result);
        Dematerialized(this, e);
    }
}

void TreeContainerIndex::Remove(int startingIndex, std::vector<Interfaces::ItemContainerInfo> &containers)
{
    for (auto const& container : containers)
    {
        auto item = _containerToItem[container.ContainerControl];
        _containerToItem.erase(container.ContainerControl);
        _itemToContainer.erase(item);
    }

    if (Dematerialized)
    {
        System::Collection::Vector<Interfaces::ItemContainerInfo> result;
        result.GetNativeEnumerator() = containers;
        Interfaces::ItemContainerEventArgs e(startingIndex, &result);
        Dematerialized(this, e);
    }
}

XUI::UI::Controls::Control *TreeContainerIndex::ContainerFromItem(System::Reflection::RefObject const& item)
{
    auto _itemToContainer_iterator = _itemToContainer.find(item);
    if (_itemToContainer_iterator != _itemToContainer.end())
        return  _itemToContainer_iterator->second.get();
    return nullptr;
}

System::Reflection::RefObject TreeContainerIndex::ItemFromContainer(Controls::Control *container)
{
    auto _containerToItem_iterator = _containerToItem.find(container->shared_from_base_static<Controls::Control>());
    if (_containerToItem_iterator != _containerToItem.end())
        return _containerToItem_iterator->second;
    return nullptr;
}
