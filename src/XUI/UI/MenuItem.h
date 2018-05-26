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

#ifndef __XUI_MENUITEM_H__
#define __XUI_MENUITEM_H__

#include "UI/Detail/HeaderedSelectingControl.h"
#include "UI/Detail/Popup.h"
#include "Interfaces/ICommand.h"
#include "Core/Input/KeyGesture.h"

namespace XUI::UI
{
    enum class MenuItemRole
    {
        TopLevelItem = 0,
        TopLevelHeader = 1,
        SubmenuItem = 2,
        SubmenuHeader = 3
    };

    /// <summary>
    /// A menu item control.
    /// </summary>
    class TC_XUI_API MenuItem : public HeaderedSelectingItemsControl, public Interfaces::UIControls::ISelectable
    {
    public:
        /// <summary>
        /// Defines the <see cref="Command"/> property.
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty CommandProperty;

        /// <summary>
        /// Defines the <see cref="HotKey"/> property.
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty HotKeyProperty;

        /// <summary>
        /// Defines the <see cref="CommandParameter"/> property.
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty CommandParameterProperty;

        /// <summary>
        /// Defines the <see cref="Icon"/> property.
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty IconProperty;

        /// <summary>
        /// Defines the <see cref="IsSelected"/> property.
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty IsSelectedProperty;

        /// <summary>
        /// Defines the <see cref="IsSubMenuOpen"/> property.
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty IsSubMenuOpenProperty;

        static XUI::Core::Dependency::RefDependencyProperty CheckedProperty;
        static XUI::Core::Dependency::RefDependencyProperty IsCheckableProperty;
        static XUI::Core::Dependency::RefDependencyProperty RoleProperty;

        /// <summary>
        /// Defines the <see cref="Click"/> event.
        /// </summary>
        static std::shared_ptr<Core::Observer::RoutedEvent> ClickEvent;

        /// <summary>
        /// Defines the <see cref="SubmenuOpened"/> event.
        /// </summary>
        static std::shared_ptr<Core::Observer::RoutedEvent> SubmenuOpenedEvent;

    private:
        /// <summary>
        /// The default value for the <see cref="ItemsControl.ItemsPanel"/> property.
        /// </summary>
        static std::shared_ptr<XamlCPP::Interfaces::ITemplate> DefaultPanel;

        /// <summary>
        /// The timer used to display submenus.
        /// </summary>
        rxcpp::subscription _submenuTimer;

        /// <summary>
        /// The submenu popup.
        /// </summary>
        std::shared_ptr<Popup> _popup;

    public:
        /// <summary>
        /// Initializes static members of the <see cref="MenuItem"/> class.
        /// </summary>
        static void StaticClassInitializer();


        MenuItem();

        /// <summary>
        /// Occurs when a <see cref="MenuItem"/> without a submenu is clicked.
        /// </summary>
        Meta(XamlCPP::EventAttribute())
        Meta(XamlCPP::TypeConverterAttribute(typeid(XamlCPP::Core::ReflEx::TypeConversion::EventHookValueConverter)))
        Core::Observer::SpecPropertyRoutedEventHandler<&ClickEvent> Click;

        /// <summary>
        /// Occurs when a <see cref="MenuItem"/>'s submenu is opened.
        /// </summary>
        Meta(XamlCPP::EventAttribute())
        Meta(XamlCPP::TypeConverterAttribute(typeid(XamlCPP::Core::ReflEx::TypeConversion::EventHookValueConverter)))
        Core::Observer::SpecPropertyRoutedEventHandler<&SubmenuOpenedEvent> SubmenuOpened;

        /// <summary>
        /// Gets or sets the command associated with the menu item.
        /// </summary>
        PROPERTY_(std::shared_ptr<Interfaces::ICommand>, Command);
        std::shared_ptr<Interfaces::ICommand> GetCommand() const;
        void SetCommand(std::shared_ptr<Interfaces::ICommand> const& value);

        /// <summary>
        /// Gets or sets an <see cref="KeyGesture"/> associated with this control
        /// </summary>
        PROPERTY_(std::shared_ptr<Core::Input::KeyGesture> , HotKey);
        std::shared_ptr<Core::Input::KeyGesture> GetHotKey() const;
        void SetHotKey(std::shared_ptr<Core::Input::KeyGesture> const& value);

        /// <summary>
        /// Gets or sets the parameter to pass to the <see cref="Command"/> property of a
        /// <see cref="MenuItem"/>.
        /// </summary>
        PROPERTY_(System::Reflection::Variant  , CommandParameter);
        System::Reflection::Variant GetCommandParameter() const;
        void SetCommandParameter(System::Reflection::Variant value);

        /// <summary>
        /// Gets or sets the icon that appears in a <see cref="MenuItem"/>.
        /// </summary>
        PROPERTY_(System::Reflection::RefObject, Icon);
        System::Reflection::RefObject GetIcon() const;
        void SetIcon(System::Reflection::RefObject const& value);

        /// <summary>
        /// Gets or sets a value indicating whether the <see cref="MenuItem"/> is currently selected.
        /// </summary>
        PROPERTY_(bool , IsSelected);
        bool GetIsSelected() const override;
        void SetIsSelected(const bool value) override;

        PROPERTY_(bool, Checked);
        bool GetChecked() const;
        void SetChecked(const bool &value);

        PROPERTY_(bool, IsCheckable);
        bool GetIsCheckable() const;
        void SetIsCheckable(const bool &value);

        PROPERTY_(MenuItemRole, Role);
        MenuItemRole GetRole() const;
        void SetRole(MenuItemRole const& value);


        /// <summary>
        /// Gets or sets a value that indicates whether the submenu of the <see cref="MenuItem"/> is
        /// open.
        /// </summary>
        PROPERTY_(bool , IsSubMenuOpen);
        bool GetIsSubMenuOpen() const;
        void SetIsSubMenuOpen(const bool &value);

        /// <summary>
        /// Gets or sets a value that indicates whether the <see cref="MenuItem"/> has a submenu.
        /// </summary>
        PROPERTY_GET_ONLY(bool , HasSubMenu);
        bool GetHasSubMenu() const;

        /// <summary>
        /// Gets a value that indicates whether the <see cref="MenuItem"/> is a top-level menu item.
        /// </summary>
        PROPERTY_GET_ONLY(bool , IsTopLevel);
        bool GetIsTopLevel() const;

        /// <summary>
        /// Called when the <see cref="MenuItem"/> is clicked.
        /// </summary>
        /// <param name="e">The click event args.</param>
    protected:
        virtual void OnClick(Core::Observer::RoutedEventArgs& e);

        /// <summary>
        /// Called when the <see cref="MenuItem"/> recieves focus.
        /// </summary>
        /// <param name="e">The event args.</param>
        void OnGotFocus(Interfaces::GotFocusEventArgs& e) override;

        /// <inheritdoc/>
        std::shared_ptr<Generators::IItemContainerGenerator> CreateItemContainerGenerator() override;

        /// <summary>
        /// Called when a key is pressed in the <see cref="MenuItem"/>.
        /// </summary>
        /// <param name="e">The event args.</param>
        void OnKeyDown(Interfaces::KeyEventArgs& e) override;

        /// <summary>
        /// Called when the pointer enters the <see cref="MenuItem"/>.
        /// </summary>
        /// <param name="e">The event args.</param>
        void OnPointerEnter(Interfaces::PointerEventArgs& e) override;

        /// <summary>
        /// Called when the pointer leaves the <see cref="MenuItem"/>.
        /// </summary>
        /// <param name="e">The event args.</param>
        void OnPointerLeave(Interfaces::PointerEventArgs& e) override;

        /// <summary>
        /// Called when the pointer is pressed over the <see cref="MenuItem"/>.
        /// </summary>
        /// <param name="e">The event args.</param>
        void OnPointerPressed(Interfaces::PointerPressedEventArgs& e) override;

        /// <summary>
        /// Called when a submenu is opened on this MenuItem or a child MenuItem.
        /// </summary>
        /// <param name="e">The event args.</param>
        virtual void OnSubmenuOpened(Core::Observer::RoutedEventArgs& e);

        /// <inheritdoc/>
        void OnTemplateApplied(Controls::TemplateAppliedEventArgs const& e) override;
        void ItemsCollectionChanged(void *sender, System::Collection::NotifyCollectionChangedEventArgs& e) override;

    private:
        /// <summary>
        /// Called when the menu item's access key is pressed.
        /// </summary>
        /// <param name="e">The event args.</param>
        void AccessKeyPressed(Core::Observer::RoutedEventArgs& e);

        /// <summary>
        /// Closes all submenus of the menu item.
        /// </summary>
        void CloseSubmenus();

        /// <summary>
        /// Called when the <see cref="Icon"/> property changes.
        /// </summary>
        /// <param name="e">The property change event.</param>
        void IconChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e);

        /// <summary>
        /// Called when the <see cref="IsSubMenuOpen"/> property changes.
        /// </summary>
        /// <param name="e">The property change event.</param>
        void SubMenuOpenChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e);

        /// <summary>
        /// Called when the submenu's <see cref="Popup"/> is opened.
        /// </summary>
        /// <param name="sender">The event sender.</param>
        /// <param name="e">The event args.</param>
        void PopupOpened(void* sender, System::Events::EventArg& e);

        /// <summary>
        /// Called when the submenu's <see cref="Popup"/> is closed.
        /// </summary>
        /// <param name="sender">The event sender.</param>
        /// <param name="e">The event args.</param>
        void PopupClosed(void* sender, System::Events::EventArg& e);
    };
}


#endif	//#ifndef __XUI_MENUITEM_H__
