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

#ifndef __XUI_ITEMVIRTUALIZERSIMPLE_H__
#define __XUI_ITEMVIRTUALIZERSIMPLE_H__

#include "UI/Presenters/ItemVirtualizer.h"

namespace XUI::UI::Presenters
{
    /// <summary>
    /// Represents an item virtualizer for an <see cref="ItemsPresenter"/> that doesn't actually
    /// virtualize items - it just creates a container for every item.
    /// </summary>
    class ItemVirtualizerSimple : public ItemVirtualizer
    {
    public:
        ItemVirtualizerSimple(ItemsPresenter *owner);

        /// <inheritdoc/>
        PROPERTY_(bool , IsLogicalScrollEnabled);
        bool GetIsLogicalScrollEnabled() const override;

        /// <summary>
        /// This property should never be accessed because <see cref="IsLogicalScrollEnabled"/> is
        /// false.
        /// </summary>
        PROPERTY_(double , ExtentValue);
        double GetExtentValue() const override;

        /// <summary>
        /// This property should never be accessed because <see cref="IsLogicalScrollEnabled"/> is
        /// false.
        /// </summary>
        PROPERTY_(double , OffsetValue);
        double GetOffsetValue() const override;
        void SetOffsetValue(const double &value) override;

        /// <summary>
        /// This property should never be accessed because <see cref="IsLogicalScrollEnabled"/> is
        /// false.
        /// </summary>
        PROPERTY_(double , ViewportValue);
        double GetViewportValue() const override;

        /// <inheritdoc/>
        void ItemsChanged(System::Collection::Generic::IContainer *items, System::Collection::NotifyCollectionChangedEventArgs& e) override;

        /// <summary>
        /// Scrolls the specified item into view.
        /// </summary>
        /// <param name="item">The item.</param>
        void ScrollIntoView(System::Reflection::Variant const& item) override;

        /// <inheritdoc/>        
        virtual Core::Media::Size MeasureOverride(Core::Media::Size const& availableSize);

        /// <inheritdoc/>        
        virtual Core::Media::Size ArrangeOverride(Core::Media::Size const& finalSize);

        void UpdateControls() override
        {
            CreateAndRemoveContainers();
            InvalidateScroll();
        }

        Controls::Control *GetControlInDirection(NavigationDirection direction, Controls::Control *from);

    private:
        std::vector<Interfaces::ItemContainerInfo> AddContainers(int index, System::Collection::Generic::IContainer *items);
        Interfaces::ItemContainerInfo AddItem(int index, System::Reflection::Variant const& item);

        void RemoveContainers(std::vector<Interfaces::ItemContainerInfo> const&items);

        void CreateAndRemoveContainers();

        void RecycleContainers();

        void RecycleContainersForMove(int delta);

        void RecycleContainersOnRemove();

        void RemoveContainers(int count);

        Controls::Control * ScrollIntoView(int index);

        double CoerceOffset(double value)
        {
            double max = std::max(ExtentValue - ViewportValue, 0.0);
            return std::clamp(value, 0.0, max);
        }
    };
}


#endif	//#ifndef __XUI_ITEMVIRTUALIZERSIMPLE_H__
