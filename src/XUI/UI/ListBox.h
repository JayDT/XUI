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

#ifndef __XUI_LISTBOX_H__
#define __XUI_LISTBOX_H__

#include "UI/Detail/SelectingItemsControl.h"
#include "UI/Presenters/ScrollContentPresenter.h"

namespace XUI::UI
{
    /// <summary>
    /// An <see cref="ItemsControl"/> in which individual items can be selected.
    /// </summary>
    class TC_XUI_API ListBox : public SelectingItemsControl
    {
    public:
        /// <summary>
        /// Defines the <see cref="Scroll"/> property.
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty ScrollProperty;

        /// <summary>
        /// Defines the <see cref="SelectedItems"/> property.
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty SelectedItemsProperty;

        /// <summary>
        /// Defines the <see cref="SelectionMode"/> property.
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty SelectionModeProperty;

        /// <summary>
        /// Defines the <see cref="VirtualizationMode"/> property.
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty VirtualizationModeProperty;

    private:
        std::shared_ptr<UI::ScrollViewer> _scroll;
        std::shared_ptr<UI::Panel> _panel;

    public:
        /// <summary>
        /// Initializes static members of the <see cref="ItemsControl"/> class.
        /// </summary>
        static void StaticClassInitializer();
        ListBox();
        virtual ~ListBox();

        /// <summary>
        /// Gets the scroll information for the <see cref="ListBox"/>.
        /// </summary>
        PROPERTY_(UI::ScrollViewer * , Scroll);
        UI::ScrollViewer *GetScroll();
    private:
        void SetScroll(UI::ScrollViewer *value);

    public:
        /// <inheritdoc/>
        PROPERTY_GET_ONLY(System::Collection::Generic::IContainer * , SelectedItems);
        System::Collection::Generic::IContainer *GetSelectedItems();

        /// <inheritdoc/>
        PROPERTY_(UI::SelectionMode  , SelectionMode);
        UI::SelectionMode GetSelectionMode();
        void SetSelectionMode(UI::SelectionMode value);

        /// <summary>
        /// Gets or sets the virtualization mode for the items.
        /// </summary>
        PROPERTY_(UI::Presenters::ItemVirtualizationMode , VirtualizationMode);
        UI::Presenters::ItemVirtualizationMode GetVirtualizationMode();
        void SetVirtualizationMode(UI::Presenters::ItemVirtualizationMode value);

    protected:
        /// <inheritdoc/>
        std::shared_ptr<Generators::IItemContainerGenerator> CreateItemContainerGenerator() override;

        /// <inheritdoc/>
        void OnGotFocus(Interfaces::GotFocusEventArgs& e) override;

        /// <inheritdoc/>
        void OnPointerPressed(Interfaces::PointerPressedEventArgs& e) override;

        void OnTemplateApplied(UI::Controls::TemplateAppliedEventArgs const& e) override;
    };
}


#endif	//#ifndef __XUI_LISTBOX_H__
