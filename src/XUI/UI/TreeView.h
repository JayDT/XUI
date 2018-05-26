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

#ifndef __XUI_TREEVIEW_H__
#define __XUI_TREEVIEW_H__

#include "UI/Controls/ItemsControl.h"

namespace XUI::UI
{
    /// <summary>
    /// Displays a hierachical tree of data.
    /// </summary>
    class TC_XUI_API TreeView : public Controls::ItemsControl //, public ICustomKeyboardNavigation
    {
    public:
        /// <summary>
        /// Defines the <see cref="AutoScrollToSelectedItem"/> property.
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty AutoScrollToSelectedItemProperty;

        /// <summary>
        /// Defines the <see cref="SelectedItem"/> property.
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty SelectedItemProperty;

        static std::shared_ptr<Core::Observer::RoutedEvent> SelectedItemChangedEvent;

        static Core::Observer::RoutedPropertyChangedEventArgs<System::Reflection::RefObject>* __0; // this hack removed after implemented correct template handle in reflection

    private:
        System::Reflection::RefObject _selectedItem;

    public:
        /// <summary>
        /// Initializes static members of the <see cref="TreeView"/> class.
        /// </summary>
        static void StaticClassInitializer();

        TreeView();
        virtual ~TreeView();

    public:

        Meta(XamlCPP::EventAttribute())
        Meta(XamlCPP::TypeConverterAttribute(typeid(XamlCPP::Core::ReflEx::TypeConversion::EventHookValueConverter)))
        Core::Observer::SpecPropertyRoutedEventHandler<&SelectedItemChangedEvent>              SelectedItemChanged;

        /// <summary>
        /// Gets or sets a value indicating whether to automatically scroll to newly selected items.
        /// </summary>
        PROPERTY_(bool , AutoScrollToSelectedItem);
        bool GetAutoScrollToSelectedItem() const;
        void SetAutoScrollToSelectedItem(const bool &value);

        /// <summary>
        /// Gets or sets the selected item.
        /// </summary>
        PROPERTY_(System::Reflection::RefObject const&, SelectedItem);
        System::Reflection::RefObject const& GetSelectedItem() const;

        void SetSelectedItem(System::Reflection::RefObject const& value);

//        (bool handled, IInputElement next) ICustomKeyboardNavigation.GetNext(IInputElement element, NavigationDirection direction)
//        {
//            if (direction == NavigationDirection.Next || direction == NavigationDirection.Previous)
//            {
//                if (!this.IsVisualAncestorOf(element))
//                {
//                    IControl result = _selectedItem != nullptr ? ItemContainerGenerator.Index.ContainerFromItem(_selectedItem) : ItemContainerGenerator.ContainerFromIndex(0);
//                    return (true, result);
//                }
//                else
//                {
//                    return (true, nullptr);
//                }
//            }
//
//            return (false, nullptr);
//        }

    protected:
        /// <inheritdoc/>
        std::shared_ptr<Generators::IItemContainerGenerator> CreateItemContainerGenerator() override;

        /// <inheritdoc/>
        void OnGotFocus(Interfaces::GotFocusEventArgs& e) override;

        /// <inheritdoc/>
        void OnPointerPressed(Interfaces::PointerPressedEventArgs& e) override;

        /// <summary>
        /// Updates the selection for an item based on user interaction.
        /// </summary>
        /// <param name="container">The container.</param>
        /// <param name="select">Whether the item should be selected or unselected.</param>
        /// <param name="rangeModifier">Whether the range modifier is enabled (i.e. shift key).</param>
        /// <param name="toggleModifier">Whether the toggle modifier is enabled (i.e. ctrl key).</param>
        void UpdateSelectionFromContainer(Controls::Control *container, bool select = true, bool rangeModifier = false, bool toggleModifier = false);

        /// <summary>
        /// Updates the selection based on an event that may have originated in a container that 
        /// belongs to the control.
        /// </summary>
        /// <param name="eventSource">The control that raised the event.</param>
        /// <param name="select">Whether the container should be selected or unselected.</param>
        /// <param name="rangeModifier">Whether the range modifier is enabled (i.e. shift key).</param>
        /// <param name="toggleModifier">Whether the toggle modifier is enabled (i.e. ctrl key).</param>
        /// <returns>
        /// True if the event originated from a container that belongs to the control; otherwise
        /// false.
        /// </returns>
        bool UpdateSelectionFromEventSource(Controls::InputElement *eventSource, bool select = true, bool rangeModifier = false, bool toggleModifier = false);

        /// <summary>
        /// Tries to get the container that was the source of an event.
        /// </summary>
        /// <param name="eventSource">The control that raised the event.</param>
        /// <returns>The container or null if the event did not originate in a container.</returns>
        Controls::Control *GetContainerFromEventSource(Controls::InputElement *eventSource);

    private:
        /// <summary>
        /// Called when a new item container is materialized, to set its selected state.
        /// </summary>
        /// <param name="sender">The event sender.</param>
        /// <param name="e">The event args.</param>
        void ContainerMaterialized(void* sender, Interfaces::ItemContainerEventArgs& e);

        /// <summary>
        /// Sets a container's 'selected' class or <see cref="ISelectable.IsSelected"/>.
        /// </summary>
        /// <param name="container">The container.</param>
        /// <param name="selected">Whether the control is selected</param>
        void MarkContainerSelected(Controls::Control *container, bool selected);
    };
}


#endif	//#ifndef __XUI_TREEVIEW_H__
