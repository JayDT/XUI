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

#ifndef __XUI_ITEMSPRESENTER_H__
#define __XUI_ITEMSPRESENTER_H__

#include "UI/Presenters/ItemsPresenterBase.h"
#include <functional>

MetaAssemblyBegin
Meta(modul: ReflInclude("UI/Presenters/ItemVirtualizer.h"))
Meta(modul: ReflInclude("UI/Panel.h"))
MetaAssemblyEnd

namespace XUI::UI::Presenters
{
    class ItemVirtualizer;

    /// <summary>
    /// Displays items inside an <see cref="ItemsControl"/>.
    /// </summary>
    class ItemsPresenter : public ItemsPresenterBase
    {
    private:
        std::shared_ptr<ItemVirtualizer> _virtualizer;
        std::function<void()> _invalidateScroll;
        bool _canHorizontallyScroll = true;
        bool _canVerticallyScroll = true;

    public:
        /// <summary>
        /// Defines the <see cref="VirtualizationMode"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty VirtualizationModeProperty;

        /// <summary>
        /// Initializes static members of the <see cref="ItemsPresenter"/> class.
        /// </summary>
        ItemsPresenter();
        virtual ~ItemsPresenter()
        {
        }

        static void StaticClassInitializer();

        /// <summary>
        /// Gets or sets the virtualization mode for the items.
        /// </summary>
    public:
        PROPERTY_(ItemVirtualizationMode, VirtualizationMode);
        ItemVirtualizationMode GetVirtualizationMode() const;
        void SetVirtualizationMode(ItemVirtualizationMode value);

        /// <inheritdoc/>
        PROPERTY_GET_ONLY(bool, IsLogicalScrollEnabled);
        bool GetIsLogicalScrollEnabled();

        /// <inheritdoc/>
        PROPERTY_GET_ONLY(Core::Media::Size, Extent);
        Core::Media::Size GetExtent();

        /// <inheritdoc/>
        PROPERTY_(Core::Media::Vector, Offset);
        Core::Media::Vector GetOffset();
        void SetOffset(Core::Media::Vector const& value);

        /// <inheritdoc/>
        PROPERTY_GET_ONLY(Core::Media::Size, Viewport);
        Core::Media::Size GetViewport();

        /// <inheritdoc/>
        PROPERTY_(std::function<void()>, InvalidateScroll);
        std::function<void()> GetInvalidateScroll();
        void SetInvalidateScroll(std::function<void()> const& value);

        /// <inheritdoc/>
        PROPERTY_GET_ONLY(Core::Media::Size, ScrollSize);
        Core::Media::Size GetScrollSize();

        /// <inheritdoc/>
        PROPERTY_GET_ONLY(Core::Media::Size, PageScrollSize);
        Core::Media::Size GetPageScrollSize();

        PROPERTY_(bool, CanHorizontallyScroll);
        bool GetCanHorizontallyScroll() const { return _canHorizontallyScroll; }
        void SetCanHorizontallyScroll(bool value) { _canHorizontallyScroll = value; }

        PROPERTY_(bool, CanVerticallyScroll);
        bool GetCanVerticallyScroll() const { return _canVerticallyScroll; }
        void SetCanVerticallyScroll(bool value) { _canVerticallyScroll = value; }


        /// <inheritdoc/>
        bool BringIntoView(Controls::Control *target, Core::Media::Rect const& targetRect);

        /// <inheritdoc/>
        Controls::Control *GetControlInDirection(NavigationDirection direction, Controls::Control *from);

        void ScrollIntoView(System::Reflection::Variant const& item) override;

        /// <inheritdoc/>
    protected:
        Core::Media::Size MeasureOverride(Core::Media::Size const& availableSize) override;
        Core::Media::Size ArrangeOverride(Core::Media::Size const& availableSize) override;

        /// <inheritdoc/>
        void PanelCreated(UI::Panel *panel) override;

        void ItemsChanged(System::Collection::NotifyCollectionChangedEventArgs& e) override;

    private:
        Core::Media::Vector CoerceOffset(Core::Media::Vector const& value);

        void VirtualizationModeChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e);
    };
}


#endif	//#ifndef __XUI_ITEMSPRESENTER_H__
