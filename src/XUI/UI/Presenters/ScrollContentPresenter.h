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

#ifndef __XUI_SCROLLCONTENTPRESENTER_H__
#define __XUI_SCROLLCONTENTPRESENTER_H__

#include "UI/Presenters/ContentPresenter.h"
#include "Core/Media/MediaTypes.h"

namespace XUI::UI
{
    class ScrollViewer;
}

namespace XUI::UI::Presenters
{
    /// <summary>
    /// Presents a scrolling view of content inside a <see cref="ScrollViewer"/>.
    /// </summary>
    class TC_XUI_API ScrollContentPresenter : public ContentPresenter
    {
        friend ScrollViewer;

    public:
        /// <summary>
        /// Defines the <see cref="Extent"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty ExtentProperty;

        /// <summary>
        /// Defines the <see cref="Offset"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty OffsetProperty;

        /// <summary>
        /// Defines the <see cref="Viewport"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty ViewportProperty;

        /// <summary>
        /// Defines the <see cref="CanScrollHorizontally"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty CanScrollHorizontallyProperty;

        static Core::Dependency::RefDependencyProperty CanScrollVerticallyProperty;

    private:
        Core::Media::Size _extent;
        Core::Media::Size _measuredExtent;
        Core::Media::Vector _offset = Core::Media::Vector(0, 0);
        rxcpp::subscription _logicalScrollSubscription;
        Core::Media::Size _viewport;

        /// <summary>
        /// Initializes static members of the <see cref="ScrollContentPresenter"/> class.
        /// </summary>
    public:
        static void StaticClassInitializer();

        /// <summary>
        /// Initializes a new instance of the <see cref="ScrollContentPresenter"/> class.
        /// </summary>
        ScrollContentPresenter();
        virtual ~ScrollContentPresenter();

        /// <summary>
        /// Gets the extent of the scrollable content.
        /// </summary>
        PROPERTY_(Core::Media::Size, Extent);
        Core::Media::Size const& GetExtent() const;
    private:
        void SetExtent(Core::Media::Size const& value);

    public:
        /// <summary>
        /// Gets or sets the current scroll offset.
        /// </summary>
        PROPERTY_(Core::Media::Vector, Offset);
        Core::Media::Vector const& GetOffset() const;
        void SetOffset(Core::Media::Vector const& value);

        /// <summary>
        /// Gets the size of the viewport on the scrollable content.
        /// </summary>
        PROPERTY_(Core::Media::Size, Viewport);
        Core::Media::Size const& GetViewport() const;
    private:
        void SetViewport(Core::Media::Size const& value);

    public:
        /// <summary>
        /// Gets a value indicating whether the content can be scrolled horizontally.
        /// </summary>
        PROPERTY_GET_ONLY(bool, CanScrollHorizontally);
        bool GetCanScrollHorizontally() const;

        PROPERTY_GET_ONLY(bool, CanScrollVertically);
        bool GetCanScrollVertically() const;

        /// <summary>
        /// Attempts to bring a portion of the target visual into view by scrolling the content.
        /// </summary>
        /// <param name="target">The target visual.</param>
        /// <param name="targetRect">The portion of the target visual to bring into view.</param>
        /// <returns>True if the scroll offset was changed; otherwise false.</returns>
        bool BringDescendantIntoView(Visual *target, Core::Media::Rect const& targetRect);

    protected:
        /// <inheritdoc/>
        Core::Media::Size MeasureOverride(Core::Media::Size const& availableSize) override;
        /// <inheritdoc/>
        Core::Media::Size ArrangeOverride(Core::Media::Size const& availableSize) override;
        /// <inheritdoc/>
        void OnPointerWheelChanged(Interfaces::PointerWheelEventArgs& e) override;

    private:
        void BringIntoViewRequested(void *sender, Controls::RequestBringIntoViewEventArgs& e);

        void ChildChanged(const XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e);

        void UpdateScrollableSubscription(Controls::Control *child);

        void UpdateFromScrollable(Controls::Control *scrollable);
    };
}

#endif	//#ifndef __XUI_SCROLLCONTENTPRESENTER_H__
