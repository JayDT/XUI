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

#ifndef __XUI_TREEITEMCONTAINERGENERATOR_H__
#define __XUI_TREEITEMCONTAINERGENERATOR_H__

#include "ItemContainerGenerator.h"
#include "UI/Generators/TreeContainerIndex.h"
#include "UI/DataTemplates/DataTemplate.h"

namespace XUI::UI::Generators
{
    /// <summary>
    /// Creates containers for tree items and maintains a list of created containers.
    /// </summary>
    /// <typeparam name="T">The type of the container.</typeparam>
    template<typename T>
    class TreeItemContainerGenerator : public ItemContainerGenerator<T>//, public ITreeItemContainerGenerator
    {
        static_assert(std::is_class<T>::value && std::is_base_of<Controls::Control, T>::value && std::is_default_constructible<T>::value, "T must be a class, must inherit from IControl, and requires default-constructible elements");

    public:
        /// <summary>
        /// Initializes a new instance of the <see cref="TreeItemContainerGenerator{T}"/> class.
        /// </summary>
        /// <param name="owner">The owner control.</param>
        /// <param name="contentProperty">The container's Content property.</param>
        /// <param name="contentTemplateProperty">The container's ContentTemplate property.</param>
        /// <param name="itemsProperty">The container's Items property.</param>
        /// <param name="isExpandedProperty">The container's IsExpanded property.</param>
        /// <param name="index">The container index for the tree</param>
        TreeItemContainerGenerator(Controls::Control *owner, Core::Dependency::DependencyProperty *contentProperty, Core::Dependency::DependencyProperty *contentTemplateProperty, Core::Dependency::DependencyProperty *itemsProperty, Core::Dependency::DependencyProperty *isExpandedProperty, std::shared_ptr<TreeContainerIndex> const& index)
            : ItemContainerGenerator<T>(owner, contentProperty, contentTemplateProperty)
        {
            System::ThrowIfFailed<System::ArgumentException>(owner != nullptr);
            System::ThrowIfFailed<System::ArgumentException>(contentProperty != nullptr);
            System::ThrowIfFailed<System::ArgumentException>(itemsProperty != nullptr);
            System::ThrowIfFailed<System::ArgumentException>(isExpandedProperty != nullptr);
            System::ThrowIfFailed<System::ArgumentException>(index != nullptr);

            ItemsProperty = itemsProperty;
            IsExpandedProperty = isExpandedProperty;
            Index = index;
        }

        /// <summary>
        /// Gets the container index for the tree.
        /// </summary>
        std::shared_ptr<TreeContainerIndex> Index;

    protected:
        /// <summary>
        /// Gets the item container's Items property.
        /// </summary>
        Core::Dependency::DependencyProperty *ItemsProperty = nullptr;

        /// <summary>
        /// Gets the item container's IsExpanded property.
        /// </summary>
        Core::Dependency::DependencyProperty *IsExpandedProperty = nullptr;

        /// <inheritdoc/>
        std::shared_ptr<UI::Controls::Control> CreateContainer(System::Reflection::Variant const& _item) override
        {
            if (!_item.GetBase())
                return nullptr;

            System::Reflection::RefObject item = _item.ToSharedObject();

            if (!item)
                return nullptr;

            std::shared_ptr<DataTemplates::DataTemplate> m_template = GetTreeDataTemplate(item, ItemTemplate);

            std::shared_ptr<T> result;
            if (item->Is<T>())
            {
                result = std::static_pointer_cast<T>(item);

                result->SetValue(ContentTemplateProperty, m_template, Core::Dependency::BindingLevel::Style);
                //result->SetValue(ContentProperty, m_template->Build(), Core::Dependency::BindingLevel::Style);
                Index->Add(item, item->Static_As<Controls::Control>());
                return result;
            }

            result = std::make_shared<T>();

            result->SetValue(ContentTemplateProperty, m_template, Core::Dependency::BindingLevel::Style);
            result->SetValue(ContentProperty, item, Core::Dependency::BindingLevel::Style);
            
            //auto itemsSelector = m_template->ItemsSelector(item);
            //
            //if (itemsSelector != nullptr)
            //{
            //    BindingOperations::Apply(result, ItemsProperty, itemsSelector, nullptr);
            //}
            
            if (!item->GetType()->IsAssignableFrom<Controls::Control>())
                result->DataContext = item;
            
            //NameScope tempVar();
            //NameScope::SetNameScope(result.get(), &tempVar);
            Index->Add(item, result->Static_As<Controls::Control>());

            return result;
        }

    public:
        std::vector<Interfaces::ItemContainerInfo> Clear() override
        {
            std::vector<Interfaces::ItemContainerInfo> items = ItemContainerGenerator<T>::Clear();
            Index->Remove(0, items);
            return items;
        }

        std::vector<Interfaces::ItemContainerInfo> Dematerialize(int startingIndex, int count) override
        {
            std::vector<Interfaces::ItemContainerInfo> range = GetContainerRange(startingIndex, count);
            Index->Remove(startingIndex, range);
            return ItemContainerGenerator<T>::Dematerialize(startingIndex, count);
        }

        std::vector<Interfaces::ItemContainerInfo> RemoveRange(int startingIndex, int count) override
        {
            std::vector<Interfaces::ItemContainerInfo> range = GetContainerRange(startingIndex, count);
            Index->Remove(startingIndex, range);
            return ItemContainerGenerator<T>::RemoveRange(startingIndex, count);
        }

        bool TryRecycle(int oldIndex, int newIndex, System::Reflection::Variant item, System::Delegate<System::Reflection::Variant(System::Reflection::Variant const&)> const& selector) override
        {
            return false;
        }

    private:
        std::shared_ptr<DataTemplates::DataTemplate> GetTreeDataTemplate(System::Reflection::RefObject const& item, std::shared_ptr<DataTemplates::DataTemplate> const& primary)
        {
            if (primary && primary->DataType->IsAssignableFrom(*item->GetType()))
                return primary;
            auto datatemplate = static_cast<XUI::UI::Controls::Control*>(Owner.get())->FindDataTemplate(item->GetType());
            return datatemplate;
        }
    };
}


#endif	//#ifndef __XUI_TREEITEMCONTAINERGENERATOR_H__
