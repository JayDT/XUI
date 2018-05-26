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

#ifndef __XUI_ITEMSCONTROL_H__
#define __XUI_ITEMSCONTROL_H__

#include "UI/Controls/Control.h"
#include "UI/Panel.h"
#include "UI/Presenters/ItemsPresenter.h"
#include "UI/Generators/ItemContainerGenerator.h"

namespace XUI::UI::Controls
{
    /// <summary>
    /// Displays a collection of items.
    /// </summary>
    class TC_XUI_API ItemsControl : public Controls::Control
    {
    protected:
        int m_ItemCount = 0;

        std::shared_ptr<System::Collection::Generic::IContainer> _items;
        std::shared_ptr<Generators::IItemContainerGenerator> _itemContainerGenerator;

    public:
        /// <summary>
        /// Defines the <see cref="Items"/> property.
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty ItemsProperty;

        /// <summary>
        /// Defines the <see cref="ItemsPanel"/> property.
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty ItemsPanelProperty;

        /// <summary>
        /// Defines the <see cref="ItemTemplate"/> property.
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty ItemTemplateProperty;

        /// <summary>
        /// Defines the <see cref="MemberSelector"/> property.
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty MemberSelectorProperty;

        static XUI::Core::Dependency::RefDependencyProperty AlternationCountProperty;
        static XUI::Core::Dependency::RefDependencyProperty AlternationIndexProperty;

        static XUI::UI::DataTemplates::FuncTemplate<std::shared_ptr<XUI::UI::Panel>>* __0; // this hack removed after implemented correct template handle in reflection

        /// <summary>
        /// Initializes static members of the <see cref="ItemsControl"/> class.
        /// </summary>
        static void StaticClassInitializer();

        /// <summary>
        /// Initializes a new instance of the <see cref="ItemsControl"/> class.
        /// </summary>
        ItemsControl();

        DEPENDENCY_PROPERTY_(int, AlternationCount)
        DEPENDENCY_PROPERTY_(int, AlternationIndex)

        /// <summary>
        /// Gets or sets the items to display.
        /// </summary>
        Meta(XamlCPP::ContentAttribute())
        PROPERTY_(std::shared_ptr<System::Collection::Generic::IContainer> , Items);
        std::shared_ptr<System::Collection::Generic::IContainer> const& GetItems() const;
        void SetItems(std::shared_ptr<System::Collection::Generic::IContainer> const& value);

        PROPERTY_(int , ItemCount);
        int GetItemCount();
        void SetItemCount(int value);

        /// <summary>
        /// Gets or sets the panel used to display the items.
        /// </summary>
        PROPERTY_(std::shared_ptr<XamlCPP::Interfaces::ITemplate>, ItemsPanel);
        std::shared_ptr<XamlCPP::Interfaces::ITemplate> GetItemsPanel();
        void SetItemsPanel(std::shared_ptr<XamlCPP::Interfaces::ITemplate> const& value);

        /// <summary>
        /// Selects a member from <see cref="Items"/> to use as the list item.
        /// </summary>
        PROPERTY_(System::Delegate<System::Reflection::Variant(System::Reflection::Variant const&)>, MemberSelector);
        System::Delegate<System::Reflection::Variant(System::Reflection::Variant const&)> GetMemberSelector() const;
        void SetMemberSelector(System::Delegate<System::Reflection::Variant(System::Reflection::Variant const&)> const& value);

        /// <summary>
        /// Gets or sets the data template used to display the items in the control.
        /// </summary>
        PROPERTY_(std::shared_ptr<UI::DataTemplates::DataTemplate> , ItemTemplate);
        std::shared_ptr<UI::DataTemplates::DataTemplate>  GetItemTemplate();
        void SetItemTemplate(std::shared_ptr<UI::DataTemplates::DataTemplate> const& value);

        /// <summary>
        /// Gets the items presenter control.
        /// </summary>
        std::shared_ptr<Presenters::ItemsPresenterBase> Presenter;

        /// <inheritdoc/>
        void RegisterItemsPresenter(Presenters::ItemsPresenterBase *presenter);

        std::shared_ptr<Generators::IItemContainerGenerator> GetItemContainerGenerator();

    protected:

        /// <summary>
        /// Creates the <see cref="ItemContainerGenerator"/> for the control.
        /// </summary>
        /// <returns>
        /// An <see cref="IItemContainerGenerator"/> or null.
        /// </returns>
        /// <remarks>
        /// Certain controls such as <see cref="TabControl"/> don't actually create item 
        /// containers; however they want it to be ItemsControls so that they have an Items 
        /// property etc. In this case, a derived class can override this method to return null
        /// in order to disable the creation of item containers.
        /// </remarks>
        virtual std::shared_ptr<Generators::IItemContainerGenerator> CreateItemContainerGenerator();

        /// <summary>
        /// Gets the item at the specified index in a collection.
        /// </summary>
        /// <param name="items">The collection.</param>
        /// <param name="index">The index.</param>
        /// <returns>The index of the item or -1 if the item was not found.</returns>
        static System::Reflection::Variant ElementAt(System::Collection::Generic::IContainer *items, int index);

        /// <summary>
        /// Gets the index of an item in a collection.
        /// </summary>
        /// <param name="items">The collection.</param>
        /// <param name="item">The item.</param>
        /// <returns>The index of the item or -1 if the item was not found.</returns>
        static int IndexOf(System::Collection::Generic::IContainer *items, System::Reflection::Variant const& item);

        /// <summary>
        /// Called when new containers are materialized for the <see cref="ItemsControl"/> by its
        /// <see cref="ItemContainerGenerator"/>.
        /// </summary>
        /// <param name="e">The details of the containers.</param>
        virtual void OnContainersMaterialized(Interfaces::ItemContainerEventArgs& e);

        /// <summary>
        /// Called when containers are dematerialized for the <see cref="ItemsControl"/> by its
        /// <see cref="ItemContainerGenerator"/>.
        /// </summary>
        /// <param name="e">The details of the containers.</param>
        virtual void OnContainersDematerialized(Interfaces::ItemContainerEventArgs& e);

        /// <summary>
        /// Called when containers are recycled for the <see cref="ItemsControl"/> by its
        /// <see cref="ItemContainerGenerator"/>.
        /// </summary>
        /// <param name="e">The details of the containers.</param>
        virtual void OnContainersRecycled(Interfaces::ItemContainerEventArgs& e);

        /// <summary>
        /// Caled when the <see cref="Items"/> property changes.
        /// </summary>
        /// <param name="e">The event args.</param>
        virtual void ItemsChanged(Core::Dependency::DependencyPropertyChangedEventArgs& e);

        /// <summary>
        /// Called when the <see cref="INotifyCollectionChanged.CollectionChanged"/> event is
        /// raised on <see cref="Items"/>.
        /// </summary>
        /// <param name="sender">The event sender.</param>
        /// <param name="e">The event args.</param>
        virtual void ItemsCollectionChanged(void *sender, System::Collection::NotifyCollectionChangedEventArgs& e);

    private:
        /// <summary>
        /// Given a collection of items, adds those that are controls to the logical children.
        /// </summary>
        /// <param name="items">The items.</param>
        void AddControlItemsToLogicalChildren(System::Collection::Generic::IContainer *items);

        /// <summary>
        /// Given a collection of items, removes those that are controls to from logical children.
        /// </summary>
        /// <param name="items">The items.</param>
        void RemoveControlItemsFromLogicalChildren(System::Collection::Generic::IContainer *items);

        /// <summary>
        /// Subscribes to an <see cref="Items"/> collection.
        /// </summary>
        /// <param name="items">The items collection.</param>
        void SubscribeToItems(System::Collection::Generic::IContainer *items);

        /// <summary>
        /// Called when the <see cref="ItemTemplate"/> changes.
        /// </summary>
        /// <param name="e">The event args.</param>
        void ItemTemplateChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e);
    };
}


#endif	//#ifndef __XUI_ITEMSCONTROL_H__
