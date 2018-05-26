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

#ifndef __XUI_ITEMVIRTUALIZER_H__
#define __XUI_ITEMVIRTUALIZER_H__

#include "UI/Presenters/ContentPresenter.h"
#include "Core/Media/MediaTypes.h"

#include <standard/collection.h>

namespace XUI::UI
{
    class VirtualizingStackPanel;

    namespace Controls
    {
        class Control;
        class Visual;
    }
}

namespace XUI::UI::Presenters
{
    enum class NavigationDirection : uint8;
    class ItemsPresenter;

    /// <summary>
    /// Base class for classes which handle virtualization for an <see cref="ItemsPresenter"/>.
    /// </summary>
    class ItemVirtualizer
    {
    private:
        System::Collection::Generic::IContainer *m_Items;
        int m_ItemCount = 0;

        double _crossAxisOffset = 0;
        rxcpp::subscription _subscriptions;

        /// <summary>
        /// Initializes a new instance of the <see cref="ItemVirtualizer"/> class.
        /// </summary>
        /// <param name="owner"></param>
    public:
        ItemVirtualizer(ItemsPresenter *owner);

        /// <summary>
        /// Gets the <see cref="ItemsPresenter"/> which owns the virtualizer.
        /// </summary>
        std::shared_ptr<ItemsPresenter> Owner;

        /// <summary>
        /// Gets the <see cref="IVirtualizingPanel"/> which will host the items.
        /// </summary>
        PROPERTY_GET_ONLY(UI::VirtualizingStackPanel*, VirtualizingPanel);
        UI::VirtualizingStackPanel* GetVirtualizingPanel() const;

        /// <summary>
        /// Gets the items to display.
        /// </summary>
        PROPERTY_(System::Collection::Generic::IContainer *, Items);
        System::Collection::Generic::IContainer *GetItems() const;
        void SetItems(System::Collection::Generic::IContainer *value);

        /// <summary>
        /// Gets the number of items in <see cref="Items"/>.
        /// </summary>
        PROPERTY_(int, ItemCount);
        int GetItemCount() const;
        void SetItemCount(const int &value);

        /// <summary>
        /// Gets or sets the index of the first item displayed in the panel.
        /// </summary>
        int FirstIndex = 0;

        /// <summary>
        /// Gets or sets the index of the first item beyond those displayed in the panel.
        /// </summary>
        int NextIndex = 0;

        /// <summary>
        /// Gets a value indicating whether the items should be scroll horizontally or vertically.
        /// </summary>
        PROPERTY_GET_ONLY(bool, Vertical);
        bool GetVertical() const;

        /// <summary>
        /// Gets a value indicating whether logical scrolling is enabled.
        /// </summary>
        PROPERTY_GET_ONLY(bool, IsLogicalScrollEnabled);
        virtual bool GetIsLogicalScrollEnabled() const = 0;

        /// <summary>
        /// Gets the value of the scroll extent.
        /// </summary>
        PROPERTY_GET_ONLY(double, ExtentValue);
        virtual double GetExtentValue() const = 0;

        /// <summary>
        /// Gets or sets the value of the current scroll offset.
        /// </summary>
        PROPERTY_(double, OffsetValue);
        virtual double GetOffsetValue() const = 0;
        virtual void SetOffsetValue(const double &value) = 0;

        /// <summary>
        /// Gets the value of the scrollable viewport.
        /// </summary>
        PROPERTY_GET_ONLY(double, ViewportValue);
        virtual double GetViewportValue() const = 0;

        /// <summary>
        /// Gets the <see cref="ExtentValue"/> as a <see cref="Size"/>.
        /// </summary>
        PROPERTY_GET_ONLY(Core::Media::Size, Extent);
        Core::Media::Size GetExtent() const;

        /// <summary>
        /// Gets the <see cref="ViewportValue"/> as a <see cref="Size"/>.
        /// </summary>
        PROPERTY_GET_ONLY(Core::Media::Size, Viewport);
        Core::Media::Size GetViewport() const;

        /// <summary>
        /// Gets or sets the <see cref="OffsetValue"/> as a <see cref="Vector"/>.
        /// </summary>
        PROPERTY_(Core::Media::Vector, Offset);
        Core::Media::Vector GetOffset() const;
        void SetOffset(Core::Media::Vector const& value);

        /// <summary>
        /// Creates an <see cref="ItemVirtualizer"/> based on an item presenter's 
        /// <see cref="ItemVirtualizationMode"/>.
        /// </summary>
        /// <param name="owner">The items presenter.</param>
        /// <returns>An <see cref="ItemVirtualizer"/>.</returns>
        static std::shared_ptr<ItemVirtualizer> Create(ItemsPresenter *owner);

        /// <summary>
        /// Carries out a measure for the related <see cref="ItemsPresenter"/>.
        /// </summary>
        /// <param name="availableSize">The size available to the control.</param>
        /// <returns>The desired size for the control.</returns>
        virtual Core::Media::Size MeasureOverride(Core::Media::Size const& availableSize);

        /// <summary>
        /// Carries out an arrange for the related <see cref="ItemsPresenter"/>.
        /// </summary>
        /// <param name="finalSize">The size available to the control.</param>
        /// <returns>The actual size used.</returns>
        virtual Core::Media::Size ArrangeOverride(Core::Media::Size const& finalSize);

        /// <inheritdoc/>
        virtual void UpdateControls();

        /// <summary>
        /// Gets the next control in the specified direction.
        /// </summary>
        /// <param name="direction">The movement direction.</param>
        /// <param name="from">The control from which movement begins.</param>
        /// <returns>The control.</returns>
        virtual Controls::Control *GetControlInDirection(NavigationDirection direction, Controls::Control *from);

        /// <summary>
        /// Called when the items for the presenter change, either because 
        /// <see cref="ItemsPresenterBase.Items"/> has been set, the items collection has been
        /// modified, or the panel has been created.
        /// </summary>
        /// <param name="items">The items.</param>
        /// <param name="e">A description of the change.</param>
        virtual void ItemsChanged(System::Collection::Generic::IContainer *items, System::Collection::NotifyCollectionChangedEventArgs& e);

        /// <summary>
        /// Scrolls the specified item into view.
        /// </summary>
        /// <param name="item">The item.</param>
        virtual void ScrollIntoView(System::Reflection::Variant const&  item);

        /// <inheritdoc/>
        virtual ~ItemVirtualizer();

        /// <summary>
        /// Invalidates the current scroll.
        /// </summary>
    protected:
        void InvalidateScroll();
    };
}


#endif	//#ifndef __XUI_ITEMVIRTUALIZER_H__
