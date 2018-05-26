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

#include "ItemContainerGeneratorGeneric.h"
#include "UI/Controls/Control.h"
#include "UI/Presenters/ContentPresenter.h"
#include "cpplinq/linq.hpp"

using namespace XUI;

XUI::UI::Generators::ItemContainerGeneratorGeneric::ItemContainerGeneratorGeneric(Controls::Control * owner)
{
    System::ThrowIfFailed<System::ArgumentException>(owner != nullptr);

    Owner = owner->shared_from_base_static<Controls::Control>();
}

XUI::UI::Generators::ItemContainerGeneratorGeneric::~ItemContainerGeneratorGeneric() {}

std::map<int, Interfaces::ItemContainerInfo> const & XUI::UI::Generators::ItemContainerGeneratorGeneric::GetContainers() const
{
    return _containers;
}

const System::Reflection::Type * XUI::UI::Generators::ItemContainerGeneratorGeneric::GetContainerType() const
{
    return nullptr;
}

/// <inheritdoc/>

Interfaces::ItemContainerInfo XUI::UI::Generators::ItemContainerGeneratorGeneric::Materialize(int index, System::Reflection::Variant const & item, System::Delegate<System::Reflection::Variant(System::Reflection::Variant const&)> const & selector)
{
    auto i = selector.GetDelegate() ? selector.GetDelegate()(item) : item;

    auto control = CreateContainer(i);
    Interfaces::ItemContainerInfo container(control ? control->shared_from_base_static<Controls::Control>() :  std::shared_ptr<Controls::Control>(nullptr), item, index);

    auto& mirror = _containers[index] = container;
    _containerIndexCache.emplace(container.ContainerControl.get(), &mirror);

    //if (_containers.size() == container.Index)
    //{
    //    auto itr = _containers.end();
    //    auto insert_itr = _containers.insert(itr, container);
    //}
    //else
    //{
    //    auto itr = std::next(_containers.begin(), container.Index);
    //    auto insert_itr = _containers.insert(itr, container);
    //    _containerIndexCache.emplace(container.ContainerControl.get(), &(*insert_itr));
    //}

    if (Materialized)
    {
        System::Collection::Vector<Interfaces::ItemContainerInfo> result;
        result.Add(container);
        Interfaces::ItemContainerEventArgs e(0, &result);
        Materialized(this, e);
    }

    return std::move(container);
}

/// <inheritdoc/>

std::vector<Interfaces::ItemContainerInfo> XUI::UI::Generators::ItemContainerGeneratorGeneric::Dematerialize(int startingIndex, int count)
{
    System::Collection::Vector<Interfaces::ItemContainerInfo> result;

    for (int i = startingIndex + count - 1; i >= startingIndex; --i)
    {
        auto itr = _containers.find(i);
        if (itr != _containers.end())
        {
            result.GetNativeEnumerator().push_back(itr->second);
            _containerIndexCache.erase(itr->second.ContainerControl.get());
            _containers.erase(itr);
        }
    }

    if (Dematerialized)
    {
        Interfaces::ItemContainerEventArgs e(0, &result);
        Dematerialized(this, e);
    }

    return result.GetNativeEnumerator();
}

/// <inheritdoc/>

void XUI::UI::Generators::ItemContainerGeneratorGeneric::InsertSpace(int index, int count)
{
    if (count > 0)
    {
        for (auto& i : _containerIndexCache)
        {
            if (i.second->Index < index)
                continue;
            
            auto node = _containers.extract(i.second->Index);
            i.second->Index += count;
            node.key() = i.second->Index;
            _containers.insert(std::move(node));
        }
    }
}

/// <inheritdoc/>

std::vector<Interfaces::ItemContainerInfo> XUI::UI::Generators::ItemContainerGeneratorGeneric::RemoveRange(int startingIndex, int count)
{
    System::Collection::Vector<Interfaces::ItemContainerInfo> result;

    if (count > 0)
    {
        for (int i = startingIndex; i < startingIndex + count; ++i)
        {
            auto _containers_iterator = _containers.find(i);
            if (_containers_iterator != _containers.end())
            {
                result.GetNativeEnumerator().push_back(_containers_iterator->second);

                _containerIndexCache.erase(_containers_iterator->second.ContainerControl.get());
                _containers.erase(_containers_iterator);
            }
        }

        for (auto& i : _containerIndexCache)
        {
            if (i.second->Index < startingIndex)
                continue;

            auto node = _containers.extract(i.second->Index);
            i.second->Index -= count;
            node.key() = i.second->Index;
            _containers.insert(std::move(node));
        }
        
        if (Dematerialized)
        {
            Interfaces::ItemContainerEventArgs e(0, &result);
            Dematerialized(this, e);
        }
    }

    return result.GetNativeEnumerator();
}

/// <inheritdoc/>

bool XUI::UI::Generators::ItemContainerGeneratorGeneric::TryRecycle(int oldIndex, int newIndex, System::Reflection::Variant item, System::Delegate<System::Reflection::Variant(System::Reflection::Variant const&)> const & selector)
{
    return false;
}

/// <inheritdoc/>

std::vector<Interfaces::ItemContainerInfo> XUI::UI::Generators::ItemContainerGeneratorGeneric::Clear()
{
    System::Collection::Vector<Interfaces::ItemContainerInfo> result;
    for (auto const& i : _containers)
        result.Add(i.second);

    _containers.clear();

    if (result.size() > 0)
    {
        if (Dematerialized)
        {
            Interfaces::ItemContainerEventArgs e(0, &result);
            Dematerialized(this, e);
        }
    }

    return result.GetNativeEnumerator();
}

/// <inheritdoc/>

UI::Controls::Control * XUI::UI::Generators::ItemContainerGeneratorGeneric::ContainerFromIndex(int index)
{
    auto _containers_iterator = _containers.find(index);
    if (_containers_iterator != _containers.end())
        return _containers_iterator->second.ContainerControl.get();
    return nullptr;
}

/// <inheritdoc/>

int XUI::UI::Generators::ItemContainerGeneratorGeneric::IndexFromContainer(Controls::Control * container)
{
    auto itr = _containerIndexCache.find(container);
    if (itr != _containerIndexCache.end())
        return itr->second->Index;
    return -1;
}

int XUI::UI::Generators::ItemContainerGeneratorGeneric::IndexFromItem(System::Reflection::Variant const& item)
{
    auto object = item.ToObject();
    for (auto const& i : _containers)
    {
        if (object)
        {
            if (i.second.Item.ToObject() == object)
            {
                return i.second.Index;
            }
        }
        else
        {
            if (i.second.Item == item)
            {
                return i.second.Index;
            }
        }
    }

    return -1;
}

std::shared_ptr<UI::Controls::Control> XUI::UI::Generators::ItemContainerGeneratorGeneric::CreateContainer(System::Reflection::Variant const & item)
{
    std::shared_ptr<Controls::Control> result = item.TryGetValue<std::shared_ptr<Controls::Control>>();

    if (!result)
    {
        result = std::make_shared<UI::Presenters::ContentPresenter>();
        result->SetValue(UI::Presenters::ContentPresenter::ContentProperty.get(), item.ToSharedObject(), Core::Dependency::BindingLevel::Style);
        
        if (ItemTemplate)
        {
            result->SetValue(UI::Presenters::ContentPresenter::ContentTemplateProperty.get(), ItemTemplate, Core::Dependency::BindingLevel::TemplatedParent);
        }
    }

    return result;
}

/// <summary>
/// Moves a container.
/// </summary>
/// <param name="oldIndex">The old index.</param>
/// <param name="newIndex">The new index.</param>
/// <param name="item">The new item.</param>
/// <returns>The container info.</returns>

Interfaces::ItemContainerInfo const& XUI::UI::Generators::ItemContainerGeneratorGeneric::MoveContainer(int oldIndex, int newIndex, System::Reflection::Variant const & item)
{
    auto node = _containers.extract(oldIndex);
    node.mapped().Index = newIndex;
    node.mapped().Item = item;
    node.key() = newIndex;
    auto insert_itr = _containers.insert(std::move(node));

    //Interfaces::ItemContainerInfo container;
    //auto _containers_iterator = std::next(_containers.begin(), oldIndex);
    //if (_containers_iterator != _containers.end())
    //{
    //    container.ContainerControl = _containers_iterator->second.ContainerControl;
    //    _containerIndexCache.erase(_containers_iterator->second.ContainerControl.get());
    //    _containers.erase(_containers_iterator);
    //}
    //container.Index = newIndex;
    //container.Item = item;
    //auto itr = std::next(_containers.begin(), newIndex);
    //auto insert_itr = _containers[newIndex] = container;
    //_containerIndexCache.emplace(container.ContainerControl.get(), &insert_itr);
    return insert_itr.position->second;
}

/// <summary>
/// Gets all containers with an index that fall within a range.
/// </summary>
/// <param name="index">The first index.</param>
/// <param name="count">The number of elements in the range.</param>
/// <returns>The containers.</returns>

std::vector<Interfaces::ItemContainerInfo> XUI::UI::Generators::ItemContainerGeneratorGeneric::GetContainerRange(int index, int count)
{
    return cpplinq::from(_containers).where([&](std::pair<int, Interfaces::ItemContainerInfo> const& x)
    {
        return x.second.Index >= index && x.second.Index < index + count;
    }).select([&](std::pair<int, Interfaces::ItemContainerInfo> const& x)
    {
        return x.second;
    }).to_vector();
}

/// <summary>
/// Raises the <see cref="Recycled"/> event.
/// </summary>
/// <param name="e">The event args.</param>

void XUI::UI::Generators::ItemContainerGeneratorGeneric::RaiseRecycled(Interfaces::ItemContainerEventArgs & e)
{
    if (Recycled)
        Recycled(this, e);
}
