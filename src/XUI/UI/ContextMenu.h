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

#ifndef __XUI_CONTEXTMENU_H__
#define __XUI_CONTEXTMENU_H__

#include "UI/Detail/SelectingItemsControl.h"
#include "UI/Detail/Popup.h"

#undef OpenEvent

namespace XUI::UI
{
    class TC_XUI_API ContextMenu : public SelectingItemsControl
    {
    private:
        bool _isOpen = false;
        std::shared_ptr<Popup> _popup;

    public:
        /// <summary>
        /// Initializes static members of the <see cref="ContextMenu"/> class.
        /// </summary>
        static void StaticClassInitializer();

        ContextMenu();

        /// <summary>
        /// Called when the <see cref="Control.ContextMenu"/> property changes on a control.
        /// </summary>
        /// <param name="e">The event args.</param>
        static void ContextMenuChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e);

        static std::shared_ptr<Core::Observer::RoutedEvent> OpenedEvent;
        static std::shared_ptr<Core::Observer::RoutedEvent> ClosedEvent;

        /// <summary>
        /// Called when a submenu is clicked somewhere in the menu.
        /// </summary>
        /// <param name="e">The event args.</param>
        void OnContextMenuClick(Core::Observer::RoutedEventArgs& e);

        /// <summary>
        /// Closes the menu.
        /// </summary>
        void Hide();

        Meta(XamlCPP::EventAttribute())
        Meta(XamlCPP::TypeConverterAttribute(typeid(XamlCPP::Core::ReflEx::TypeConversion::EventHookValueConverter)))
        Core::Observer::SpecPropertyRoutedEventHandler<&OpenedEvent>            Opened;
        Meta(XamlCPP::EventAttribute())
        Meta(XamlCPP::TypeConverterAttribute(typeid(XamlCPP::Core::ReflEx::TypeConversion::EventHookValueConverter)))
        Core::Observer::SpecPropertyRoutedEventHandler<&ClosedEvent>            Closed;

    private:
        /// <summary>
        /// Shows a context menu for the specified control.
        /// </summary>
        /// <param name="control">The control.</param>
        void Show(Control *control);

        static void PopupClosed(void* sender, System::Events::EventArg& e);

        static void ControlPointerReleased(void* sender, Interfaces::PointerReleasedEventArgs& e);
    };
}


#endif	//#ifndef __XUI_CONTEXTMENU_H__
