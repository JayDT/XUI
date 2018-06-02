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

#ifndef __XUI_MENU_H__
#define __XUI_MENU_H__

#include "UI/Detail/SelectingItemsControl.h"

MetaAssemblyBegin
Meta(modul: ReflInclude("UI/MenuItem.h"))
MetaAssemblyEnd

namespace XUI::UI
{
    class MenuItem;

    /// <summary>
    /// A top-level menu control.
    /// </summary>
    class TC_XUI_API Menu : public SelectingItemsControl
    {
    private:
        /// <summary>
        /// Defines the default items panel used by a <see cref="Menu"/>.
        /// </summary>
        static std::shared_ptr<XamlCPP::Interfaces::ITemplate> DefaultPanel;

    public:
        /// <summary>
        /// Defines the <see cref="IsOpen"/> property.
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty IsOpenProperty;

    private:
        bool _isOpen = false;
        bool _stayOpen = false;

        /// <summary>
        /// Tracks event handlers added to the root of the visual tree.
        /// </summary>
        rxcpp::subscription _subscription;

    public:
        /// <summary>
        /// Initializes static members of the <see cref="Menu"/> class.
        /// </summary>
        static void StaticClassInitializer();

        Menu() {}
        virtual ~Menu() {}

        /// <summary>
        /// Gets a value indicating whether the menu is open.
        /// </summary>
        PROPERTY_(bool , IsOpen);
        bool GetIsOpen() const;
    private:
        void SetIsOpen(const bool &value);

        /// <summary>
        /// Gets the selected <see cref="MenuItem"/> container.
        /// </summary>
        PROPERTY_GET_ONLY(MenuItem * , SelectedMenuItem);
        MenuItem *GetSelectedMenuItem() const;

    public:
        /// <summary>
        /// Closes the menu.
        /// </summary>
        void Close();

        /// <summary>
        /// Opens the menu in response to the Alt/F10 key.
        /// </summary>
        void Open();

    protected:
        /// <inheritdoc/>
        void OnAttachedToVisualTree(Controls::VisualTreeAttachmentEventArgs const& e) override;

        /// <inheritdoc/>
        void OnDetachedFromVisualTree(Controls::VisualTreeAttachmentEventArgs const& e) override;

        /// <inheritdoc/>
        std::shared_ptr<Generators::IItemContainerGenerator> CreateItemContainerGenerator() override;

        /// <summary>
        /// Called when a key is pressed within the menu.
        /// </summary>
        /// <param name="e">The event args.</param>
        void OnKeyDown(Interfaces::KeyEventArgs& e) override;

        /// <summary>
        /// Called when the menu loses focus.
        /// </summary>
        /// <param name="e">The event args.</param>
        void OnLostFocus(Core::Observer::RoutedEventArgs& e) override;

        /// <summary>
        /// Called when a submenu opens somewhere in the menu.
        /// </summary>
        /// <param name="e">The event args.</param>
        virtual void OnSubmenuOpened(Core::Observer::RoutedEventArgs& e);

        void OnGotFocus(Interfaces::GotFocusEventArgs & e) override;
        void ItemsCollectionChanged(void *sender, System::Collection::NotifyCollectionChangedEventArgs& e);

    private:
        /// <summary>
        /// Called when the top-level window is deactivated.
        /// </summary>
        /// <param name="sender">The sender.</param>
        /// <param name="e">The event args.</param>
        void Deactivated(void* sender, System::Events::EventArg& e);

        /// <summary>
        /// Listens for non-client clicks and closes the menu when one is detected.
        /// </summary>
        /// <param name="e">The raw event.</param>
        void ListenForNonClientClick(Interfaces::RawInputEventArgs& e);

        /// <summary>
        /// Called when a submenu is clicked somewhere in the menu.
        /// </summary>
        /// <param name="e">The event args.</param>
        void OnMenuClick(Core::Observer::RoutedEventArgs& e);

        /// <summary>
        /// Called when the pointer is pressed anywhere on the window.
        /// </summary>
        /// <param name="sender">The sender.</param>
        /// <param name="e">The event args.</param>
        void TopLevelPreviewPointerPress(void* sender, Interfaces::PointerPressedEventArgs& e);
    };
}


#endif	//#ifndef __XUI_MENU_H__
