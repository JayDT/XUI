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

#ifndef __XUI_ITEMCONTAINERGENERATOR_GENERIC_H__
#define __XUI_ITEMCONTAINERGENERATOR_GENERIC_H__

#include "UI/Generators/IItemContainerGenerator.h"

namespace XUI::UI::Generators
{
    /// <summary>
    /// Creates containers for items and maintains a list of created containers.
    /// </summary>
    class ItemContainerGeneratorGeneric : public IItemContainerGenerator
    {
    private:
        std::map<int, Interfaces::ItemContainerInfo> _containers;
        std::map<UI::Controls::Control*, Interfaces::ItemContainerInfo*> _containerIndexCache;
        std::shared_ptr<DataTemplates::DataTemplate> _itemTemplate;

    public:
        /// <summary>
        /// Initializes a new instance of the <see cref="ItemContainerGenerator"/> class.
        /// </summary>
        /// <param name="owner">The owner control.</param>
        ItemContainerGeneratorGeneric(Controls::Control *owner);

        virtual ~ItemContainerGeneratorGeneric();

        /// <inheritdoc/>
        //PROPERTY_(IDENTITY_VA(std::map<int, Interfaces::ItemContainerInfo> const&), Containers);
        std::map<int, Interfaces::ItemContainerInfo> const& GetContainers() const override;

        /// <summary>
        /// Gets or sets the data template used to display the items in the control.
        /// </summary>
        virtual std::shared_ptr<DataTemplates::DataTemplate> GetItemTemplate() const { return _itemTemplate; }
        virtual void SetItemTemplate(std::shared_ptr<DataTemplates::DataTemplate> const& value)
        {
            _itemTemplate = value;
        }

        /// <summary>
        /// Gets the owner control.
        /// </summary>
        std::shared_ptr<Controls::Control> Owner;

        /// <inheritdoc/>
        //PROPERTY_(const System::Reflection::Type * , ContainerType);
        virtual const System::Reflection::Type *GetContainerType() const override;

        /// <inheritdoc/>
        Interfaces::ItemContainerInfo Materialize(int index, System::Reflection::Variant const& item, System::Delegate<System::Reflection::Variant(System::Reflection::Variant const&)> const& selector) override;

        /// <inheritdoc/>
        virtual std::vector<Interfaces::ItemContainerInfo> Dematerialize(int startingIndex, int count) override;

        /// <inheritdoc/>
        virtual void InsertSpace(int index, int count);

        /// <inheritdoc/>
        virtual std::vector<Interfaces::ItemContainerInfo> RemoveRange(int startingIndex, int count);

        /// <inheritdoc/>
        virtual bool TryRecycle(int oldIndex, int newIndex, System::Reflection::Variant item, System::Delegate<System::Reflection::Variant(System::Reflection::Variant const&)> const& selector);

        /// <inheritdoc/>
        virtual std::vector<Interfaces::ItemContainerInfo> Clear();

        /// <inheritdoc/>
        Controls::Control *ContainerFromIndex(int index);

        /// <inheritdoc/>
        int IndexFromContainer(Controls::Control *container) override;
        int IndexFromItem(System::Reflection::Variant const& item) override;

    protected:
        /// <summary>
        /// Creates the container for an item.
        /// </summary>
        /// <param name="item">The item.</param>
        /// <returns>The created container control.</returns>
        virtual std::shared_ptr<Controls::Control> CreateContainer(System::Reflection::Variant const& item);

        /// <summary>
        /// Moves a container.
        /// </summary>
        /// <param name="oldIndex">The old index.</param>
        /// <param name="newIndex">The new index.</param>
        /// <param name="item">The new item.</param>
        /// <returns>The container info.</returns>
        Interfaces::ItemContainerInfo const& MoveContainer(int oldIndex, int newIndex, System::Reflection::Variant const& item);

        /// <summary>
        /// Gets all containers with an index that fall within a range.
        /// </summary>
        /// <param name="index">The first index.</param>
        /// <param name="count">The number of elements in the range.</param>
        /// <returns>The containers.</returns>
        std::vector<Interfaces::ItemContainerInfo> GetContainerRange(int index, int count);

        /// <summary>
        /// Raises the <see cref="Recycled"/> event.
        /// </summary>
        /// <param name="e">The event args.</param>
        void RaiseRecycled(Interfaces::ItemContainerEventArgs& e);
    };
}


#endif	//#ifndef __XUI_ITEMCONTAINERGENERATOR_GENERIC_H__
