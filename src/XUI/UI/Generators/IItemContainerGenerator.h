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

#ifndef __XUI_IITEMCONTAINERGENERATOR_H__
#define __XUI_IITEMCONTAINERGENERATOR_H__

#include "Interfaces/InputEventArgs.h"
#include <map>

namespace XUI::UI
{
    class VirtualizingStackPanel;

    namespace Controls
    {
        class Control;
        class Visual;
    }

    namespace DataTemplates
    {
        class DataTemplate;
    }
}

namespace XUI::UI::Generators
{
    /// <summary>
    /// Creates containers for items and maintains a list of created containers.
    /// </summary>
    class IItemContainerGenerator
    {
    public:
        /// <summary>
        /// Gets the currently realized containers.
        /// </summary>
//#ifndef __REFLECTION_PARSER__
        //PROPERTY_(IDENTIFY_VA(std::map<int, Interfaces::ItemContainerInfo> const&), Containers);
//#endif
        virtual std::map<int, Interfaces::ItemContainerInfo> const& GetContainers() const = 0;

        /// <summary>
        /// Gets or sets the data template used to display the items in the control.
        /// </summary>
        PROPERTY_(std::shared_ptr<DataTemplates::DataTemplate>, ItemTemplate);
        virtual std::shared_ptr<DataTemplates::DataTemplate> GetItemTemplate() const = 0;
        virtual void SetItemTemplate(std::shared_ptr<DataTemplates::DataTemplate> const& value) = 0;

        /// <summary>
        /// Gets the ContainerType, or null if its an untyped ContainerGenerator.
        /// </summary>
        PROPERTY_(const System::Reflection::Type * , ContainerType);
        virtual const System::Reflection::Type *GetContainerType() const = 0;

        /// <summary>
        /// Signalled whenever new containers are materialized.
        /// </summary>
        System::Events::EventHandler<Interfaces::ItemContainerEventArgs> Materialized;

        /// <summary>
        /// Event raised whenever containers are dematerialized.
        /// </summary>
        System::Events::EventHandler<Interfaces::ItemContainerEventArgs> Dematerialized;

        /// <summary>
        /// Event raised whenever containers are recycled.
        /// </summary>
        System::Events::EventHandler<Interfaces::ItemContainerEventArgs> Recycled;

        /// <summary>
        /// Creates a container control for an item.
        /// </summary>
        /// <param name="index">
        /// The index of the item of data in the control's items.
        /// </param>
        /// <param name="item">The item.</param>
        /// <param name="selector">An optional member selector.</param>
        /// <returns>The created controls.</returns>
        virtual Interfaces::ItemContainerInfo Materialize(int index, System::Reflection::Variant const& item, System::Delegate<System::Reflection::Variant(System::Reflection::Variant const&)> const& selector) = 0;

        /// <summary>
        /// Removes a set of created containers.
        /// </summary>
        /// <param name="startingIndex">
        /// The index of the first item in the control's items.
        /// </param>
        /// <param name="count">The the number of items to remove.</param>
        /// <returns>The removed containers.</returns>
        virtual std::vector<Interfaces::ItemContainerInfo> Dematerialize(int startingIndex, int count) = 0;

        /// <summary>
        /// Inserts space for newly inserted containers in the index.
        /// </summary>
        /// <param name="index">The index at which space should be inserted.</param>
        /// <param name="count">The number of blank spaces to create.</param>
        virtual void InsertSpace(int index, int count) = 0;

        /// <summary>
        /// Removes a set of created containers and updates the index of later containers to fill
        /// the gap.
        /// </summary>
        /// <param name="startingIndex">
        /// The index of the first item in the control's items.
        /// </param>
        /// <param name="count">The the number of items to remove.</param>
        /// <returns>The removed containers.</returns>
        virtual std::vector<Interfaces::ItemContainerInfo> RemoveRange(int startingIndex, int count) = 0;

        virtual bool TryRecycle(int oldIndex, int newIndex, System::Reflection::Variant item, System::Delegate<System::Reflection::Variant(System::Reflection::Variant const&)> const& selector) = 0;

        /// <summary>
        /// Clears all created containers and returns the removed controls.
        /// </summary>
        /// <returns>The removed controls.</returns>
        virtual std::vector<Interfaces::ItemContainerInfo> Clear() = 0;

        /// <summary>
        /// Gets the container control representing the item with the specified index.
        /// </summary>
        /// <param name="index">The index.</param>
        /// <returns>The container, or null if no container created.</returns>
        virtual Controls::Control *ContainerFromIndex(int index) = 0;

        /// <summary>
        /// Gets the index of the specified container control.
        /// </summary>
        /// <param name="container">The container.</param>
        /// <returns>The index of the container, or -1 if not found.</returns>
        virtual int IndexFromContainer(Controls::Control *container) = 0;
        virtual int IndexFromItem(System::Reflection::Variant const& item) = 0;
    };
}


#endif	//#ifndef __XUI_IITEMCONTAINERGENERATOR_H__
