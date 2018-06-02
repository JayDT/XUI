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

#ifndef __XUI_DROPDOWN_H__
#define __XUI_DROPDOWN_H__

#include "UI/Detail/SelectingItemsControl.h"
#include "UI/Detail/Popup.h"

namespace XUI::UI
{
    /// <summary>
    /// A drop-down list control.
    /// </summary>
    class TC_XUI_API DropDown : public SelectingItemsControl
    {
    public:
        /// <summary>
        /// Defines the <see cref="IsDropDownOpen"/> property.
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty IsDropDownOpenProperty;

        /// <summary>
        /// Defines the <see cref="MaxDropDownHeight"/> property.
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty MaxDropDownHeightProperty;

        /// <summary>
        /// Defines the <see cref="SelectionBoxItem"/> property.
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty SelectionBoxItemProperty;

    private:
        bool _isDropDownOpen = false;
        std::shared_ptr<Popup> _popup;
        System::Reflection::Variant _selectionBoxItem;

    public:
        /// <summary>
        /// Initializes static members of the <see cref="DropDown"/> class.
        /// </summary>
        static void StaticClassInitializer();

        DropDown() {}
        virtual ~DropDown() {}

        /// <summary>
        /// Gets or sets a value indicating whether the dropdown is currently open.
        /// </summary>
        PROPERTY_(bool , IsDropDownOpen);
        bool GetIsDropDownOpen() const;
        void SetIsDropDownOpen(const bool &value);

        /// <summary>
        /// Gets or sets the maximum height for the dropdown list.
        /// </summary>
        PROPERTY_(double , MaxDropDownHeight);
        double GetMaxDropDownHeight() const;
        void SetMaxDropDownHeight(const double &value);

    protected:
        /// <summary>
        /// Gets or sets the item to display as the control's content.
        /// </summary>
        PROPERTY_(System::Reflection::Variant, SelectionBoxItem);
        System::Reflection::Variant const& GetSelectionBoxItem() const;
        void SetSelectionBoxItem(System::Reflection::Variant const& value);

        /// <inheritdoc/>
        std::shared_ptr<Generators::IItemContainerGenerator> CreateItemContainerGenerator() override;

        /// <inheritdoc/>
        void OnAttachedToLogicalTree(Controls::LogicalTreeAttachmentEventArgs& e) override;

        void OnGotFocus(Interfaces::GotFocusEventArgs& e) override;

        /// <inheritdoc/>
        void OnKeyDown(Interfaces::KeyEventArgs& e) override;

        /// <inheritdoc/>
        void OnPointerPressed(Interfaces::PointerPressedEventArgs& e) override;

        /// <inheritdoc/>
        void OnTemplateApplied(Controls::TemplateAppliedEventArgs const& e) override;

    private:
        void PopupOpened(void* sender, System::Events::EventArg& e);

        void SelectedItemChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e);

        void UpdateSelectionBoxItem(System::Reflection::Variant const& item);
    };
}


#endif	//#ifndef __XUI_DROPDOWN_H__
