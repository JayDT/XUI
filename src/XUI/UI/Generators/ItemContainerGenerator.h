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

#ifndef __XUI_ITEMCONTAINERGENERATOR_H__
#define __XUI_ITEMCONTAINERGENERATOR_H__

#include "UI/Generators/ItemContainerGeneratorGeneric.h"

namespace XUI::UI::Generators
{
    /// <summary>
    /// Creates containers for items and maintains a list of created containers.
    /// </summary>
    /// <typeparam name="T">The type of the container.</typeparam>
    template<typename T>
    class ItemContainerGenerator : public ItemContainerGeneratorGeneric
    {
#   ifndef __REFLECTION_PARSER__
        static_assert(std::is_class<T>::value && std::is_base_of<Controls::Control, T>::value && std::is_default_constructible<T>::value, "T must be a class, must inherit from Controls::Control, and requires default-constructible elements");
#   endif

    public:
        /// <summary>
        /// Initializes a new instance of the <see cref="ItemContainerGenerator{T}"/> class.
        /// </summary>
        /// <param name="owner">The owner control.</param>
        /// <param name="contentProperty">The container's Content property.</param>
        /// <param name="contentTemplateProperty">The container's ContentTemplate property.</param>
        ItemContainerGenerator(Controls::Control *owner, Core::Dependency::DependencyProperty *contentProperty, Core::Dependency::DependencyProperty *contentTemplateProperty)
            : ItemContainerGeneratorGeneric(owner)
        {
            System::ThrowIfFailed<System::ArgumentException>(owner != nullptr);
            System::ThrowIfFailed<System::ArgumentException>(contentProperty != nullptr);

            ContentProperty = contentProperty;
            ContentTemplateProperty = contentTemplateProperty;
        }

        /// <inheritdoc/>
        PROPERTY_(const System::Reflection::Type * , ContainerType);
        const System::Reflection::Type *GetContainerType() const override
        {
            return &typeof(T);
        }

    protected:
        /// <summary>
        /// Gets the container's Content property.
        /// </summary>
        Core::Dependency::DependencyProperty *ContentProperty = nullptr;

        /// <summary>
        /// Gets the container's ContentTemplate property.
        /// </summary>
        Core::Dependency::DependencyProperty *ContentTemplateProperty = nullptr;

        /// <inheritdoc/>
        std::shared_ptr<Controls::Control> CreateContainer(System::Reflection::Variant const& item) override
        {
            std::shared_ptr<T> container = item.TryGetValue<std::shared_ptr<T>>();

            if (!item.GetBase())
                return nullptr;

            if (container)
                return container;

            std::shared_ptr<T> result = std::make_shared<T>();

            if (ContentTemplateProperty && ItemTemplate)
                result->SetValue(ContentTemplateProperty, ItemTemplate, Core::Dependency::BindingLevel::Style);

            if (item.GetType() == ContentProperty->PropertyType)
                result->SetValue(ContentProperty, item, Core::Dependency::BindingLevel::Style);
            else if (item.GetBase() && item.GetBase()->IsSharedPointer())
                result->SetValue(ContentProperty, item.ToSharedObject(), Core::Dependency::BindingLevel::Style);
            else if (*ContentProperty->PropertyType == typeid(System::Reflection::Variant))
                result->SetValue(ContentProperty, item, Core::Dependency::BindingLevel::Style);
            else
                throw System::ArgumentException("Invalid Item conversion");

            if (!item.GetType()->IsAssignableFrom<Controls::Control>())
                result->DataContext = item.ToSharedObject();

            return result;
        }

    public:
        /// <inheritdoc/>
        bool TryRecycle(int oldIndex, int newIndex, System::Reflection::Variant item, System::Delegate<System::Reflection::Variant(System::Reflection::Variant const&)> const& selector) override
        {
            Controls::Control* container = ContainerFromIndex(oldIndex);

            if (!container)
                throw std::out_of_range("Could not recycle container: not materialized.");

            System::Reflection::Variant i = selector.GetDelegate() ? selector.GetDelegate()(item) : item;

            container->SetValue(ContentProperty, i);

            if (!item.GetType()->IsAssignableFrom<Controls::Control>())
                container->DataContext = i.ToSharedObject();

            Interfaces::ItemContainerInfo info = MoveContainer(oldIndex, newIndex, i);
            System::Collection::Vector<Interfaces::ItemContainerInfo> result;
            result.Add(info);
            Interfaces::ItemContainerEventArgs e(info.Index, &result);
            RaiseRecycled(e);
            return true;
        }
    };
}


#endif	//#ifndef __XUI_ITEMCONTAINERGENERATOR`1_H__
