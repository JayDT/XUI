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

#ifndef __XUI_TREECONTAINERINDEX_H__
#define __XUI_TREECONTAINERINDEX_H__

#include "UI/Controls/Control.h"
#include "Interfaces/InputEventArgs.h"

namespace XUI::UI::Generators
{
    /// <summary>
    /// Maintains an index of all item containers currently materialized by a <see cref="TreeView"/>.
    /// </summary>
    /// <remarks>
    /// Each <see cref="TreeViewItem"/> has its own <see cref="TreeItemContainerGenerator{T}"/> 
    /// that maintains the list of its direct children, but they also share an instance of this
    /// class in their <see cref="TreeItemContainerGenerator{T}.Index"/> property which tracks 
    /// the containers materialized for the entire tree.
    /// </remarks>
    class TreeContainerIndex
    {
    private:
        std::unordered_map<System::Reflection::RefObject, std::shared_ptr<Controls::Control>> _itemToContainer;
        std::unordered_map<std::shared_ptr<Controls::Control>, System::Reflection::RefObject> _containerToItem;

        /// <summary>
        /// Signalled whenever new containers are materialized.
        /// </summary>
    public:
        System::Events::EventHandler<Interfaces::ItemContainerEventArgs> Materialized;

        /// <summary>
        /// Event raised whenever containers are dematerialized.
        /// </summary>
        System::Events::EventHandler<Interfaces::ItemContainerEventArgs> Dematerialized;

        /// <summary>
        /// Gets the currently materialized containers.
        /// </summary>
        PROPERTY_(std::vector<Controls::Control*> , Items);
        std::vector<Controls::Control*> GetItems() const;

        /// <summary>
        /// Adds an entry to the index.
        /// </summary>
        /// <param name="item">The item.</param>
        /// <param name="container">The item container.</param>
        void Add(System::Reflection::RefObject const& item, Controls::Control *container);

        /// <summary>
        /// Removes a container from the index.
        /// </summary>
        /// <param name="container">The item container.</param>
        void Remove(Controls::Control *container);

        /// <summary>
        /// Removes a set of containers from the index.
        /// </summary>
        /// <param name="startingIndex">The index of the first item.</param>
        /// <param name="containers">The item containers.</param>
        void Remove(int startingIndex, std::vector<Interfaces::ItemContainerInfo> &containers);

        /// <summary>
        /// Gets the container for an item.
        /// </summary>
        /// <param name="item">The item.</param>
        /// <returns>The container, or null of not found.</returns>
        Controls::Control *ContainerFromItem(System::Reflection::RefObject const& item);

        /// <summary>
        /// Gets the item for a container.
        /// </summary>
        /// <param name="container">The container.</param>
        /// <returns>The item, or null of not found.</returns>
        System::Reflection::RefObject ItemFromContainer(Controls::Control *container);
    };
}


#endif	//#ifndef __XUI_TREECONTAINERINDEX_H__
