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

#ifndef __XUI_SCROLLVIEWER_H__
#define __XUI_SCROLLVIEWER_H__

#include "UI/Controls/ContentControl.h"
#include "UI/Presenters/ScrollContentPresenter.h"
#include "Interfaces/ICommand.h"
#include "Core/Input/KeyGesture.h"
#include "UI/DataGrid/DataGridDefines.h"

namespace XUI::UI
{
    /// <summary>
    /// A control scrolls its content if the content is bigger than the space available.
    /// </summary>
    class TC_XUI_API ScrollViewer : public Controls::ContentControl//, public IScrollable
    {
    public:
        /// <summary>
        /// Defines the <see cref="CanScrollHorizontally"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty CanScrollHorizontallyProperty;

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
        /// Defines the HorizontalScrollBarMaximum property.
        /// </summary>
        /// <remarks>
        /// There is no public C# accessor for this property as it is intended to be bound to by a 
        /// <see cref="ScrollContentPresenter"/> in the control's template.
        /// </remarks>
        static Core::Dependency::RefDependencyProperty HorizontalScrollBarMaximumProperty;

        /// <summary>
        /// Defines the HorizontalScrollBarValue property.
        /// </summary>
        /// <remarks>
        /// There is no public C# accessor for this property as it is intended to be bound to by a 
        /// <see cref="ScrollContentPresenter"/> in the control's template.
        /// </remarks>
        static Core::Dependency::RefDependencyProperty HorizontalScrollBarValueProperty;

        /// <summary>
        /// Defines the HorizontalScrollBarViewportSize property.
        /// </summary>
        /// <remarks>
        /// There is no public C# accessor for this property as it is intended to be bound to by a 
        /// <see cref="ScrollContentPresenter"/> in the control's template.
        /// </remarks>
        static Core::Dependency::RefDependencyProperty HorizontalScrollBarViewportSizeProperty;

        /// <summary>
        /// Defines the <see cref="HorizontalScrollBarVisibility"/> property.
        /// </summary>
        /// <remarks>
        /// There is no public C# accessor for this property as it is intended to be bound to by a 
        /// <see cref="ScrollContentPresenter"/> in the control's template.
        /// </remarks>
        static Core::Dependency::RefDependencyProperty HorizontalScrollBarVisibilityProperty;

        /// <summary>
        /// Defines the VerticalScrollBarMaximum property.
        /// </summary>
        /// <remarks>
        /// There is no public C# accessor for this property as it is intended to be bound to by a 
        /// <see cref="ScrollContentPresenter"/> in the control's template.
        /// </remarks>
        static Core::Dependency::RefDependencyProperty VerticalScrollBarMaximumProperty;

        /// <summary>
        /// Defines the VerticalScrollBarValue property.
        /// </summary>
        /// <remarks>
        /// There is no public C# accessor for this property as it is intended to be bound to by a 
        /// <see cref="ScrollContentPresenter"/> in the control's template.
        /// </remarks>
        static Core::Dependency::RefDependencyProperty VerticalScrollBarValueProperty;

        /// <summary>
        /// Defines the VerticalScrollBarViewportSize property.
        /// </summary>
        /// <remarks>
        /// There is no public C# accessor for this property as it is intended to be bound to by a 
        /// <see cref="ScrollContentPresenter"/> in the control's template.
        /// </remarks>
        static Core::Dependency::RefDependencyProperty VerticalScrollBarViewportSizeProperty;

        /// <summary>
        /// Defines the <see cref="VerticalScrollBarVisibility"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty VerticalScrollBarVisibilityProperty;

    private:
        std::shared_ptr<Presenters::ScrollContentPresenter> _presenter = nullptr;
        Core::Media::Size _extent;
        Core::Media::Vector _offset = Core::Media::Vector(0, 0);
        Core::Media::Size _viewport;

    public:
        /// <summary>
        /// Initializes static members of the <see cref="ScrollViewer"/> class.
        /// </summary>
        static void StaticClassInitializer();

        virtual ~ScrollViewer()
        {}

        /// <summary>
        /// Initializes a new instance of the <see cref="ScrollViewer"/> class.
        /// </summary>
        ScrollViewer();

        std::shared_ptr<Presenters::ScrollContentPresenter> const& GetPresenter() { return _presenter; }

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
        PROPERTY_(bool, CanScrollHorizontally);
        bool GetCanScrollHorizontally() const;
        void SetCanScrollHorizontally(const bool &value);

        /// <summary>
        /// Gets or sets the horizontal scrollbar visibility.
        /// </summary>
        PROPERTY_(ScrollBarVisibility, HorizontalScrollBarVisibility);
        ScrollBarVisibility GetHorizontalScrollBarVisibility() const;
        void SetHorizontalScrollBarVisibility(ScrollBarVisibility value);

        /// <summary>
        /// Gets or sets the vertical scrollbar visibility.
        /// </summary>
        PROPERTY_(ScrollBarVisibility, VerticalScrollBarVisibility);
        ScrollBarVisibility GetVerticalScrollBarVisibility() const;
        void SetVerticalScrollBarVisibility(ScrollBarVisibility value);

        /// <summary>
        /// Gets the maximum horizontal scrollbar value.
        /// </summary>
    protected:
        PROPERTY_GET_ONLY(double, HorizontalScrollBarMaximum);
        double GetHorizontalScrollBarMaximum() const;

        /// <summary>
        /// Gets or sets the horizontal scrollbar value.
        /// </summary>
        PROPERTY_(double, HorizontalScrollBarValue);
        double GetHorizontalScrollBarValue() const;
        void SetHorizontalScrollBarValue(const double &value);

        /// <summary>
        /// Gets the size of the horizontal scrollbar viewport.
        /// </summary>
        PROPERTY_GET_ONLY(double, HorizontalScrollBarViewportSize);
        double GetHorizontalScrollBarViewportSize() const;

        /// <summary>
        /// Gets the maximum vertical scrollbar value.
        /// </summary>
        PROPERTY_GET_ONLY(double, VerticalScrollBarMaximum);
        double GetVerticalScrollBarMaximum() const;

        /// <summary>
        /// Gets or sets the vertical scrollbar value.
        /// </summary>
        PROPERTY_(double, VerticalScrollBarValue);
        double GetVerticalScrollBarValue() const;
        void SetVerticalScrollBarValue(const double &value);

        /// <summary>
        /// Gets the size of the vertical scrollbar viewport.
        /// </summary>
        PROPERTY_GET_ONLY(double, VerticalScrollBarViewportSize);
        double GetVerticalScrollBarViewportSize() const;

        /// <summary>
        /// Gets the value of the HorizontalScrollBarVisibility attached property.
        /// </summary>
        /// <param name="control">The control to read the value from.</param>
        /// <returns>The value of the property.</returns>
    public:
        static ScrollBarVisibility GetHorizontalScrollBarVisibility(Controls::Control *control);

        /// <summary>
        /// Gets the value of the HorizontalScrollBarVisibility attached property.
        /// </summary>
        /// <param name="control">The control to set the value on.</param>
        /// <param name="value">The value of the property.</param>
        static void SetHorizontalScrollBarVisibility(Controls::Control *control, ScrollBarVisibility value);

        /// <summary>
        /// Gets the value of the VerticalScrollBarVisibility attached property.
        /// </summary>
        /// <param name="control">The control to read the value from.</param>
        /// <returns>The value of the property.</returns>
        static ScrollBarVisibility GetVerticalScrollBarVisibility(Controls::Control *control);

        /// <summary>
        /// Gets the value of the VerticalScrollBarVisibility attached property.
        /// </summary>
        /// <param name="control">The control to set the value on.</param>
        /// <param name="value">The value of the property.</param>
        static void SetVerticalScrollBarVisibility(Controls::Control *control, ScrollBarVisibility value);

        static Core::Media::Vector CoerceOffset(Core::Media::Size const& extent, Core::Media::Size const& viewport, Core::Media::Vector const& offset);

    protected:

        void OnTemplateApplied(Controls::TemplateAppliedEventArgs const& e);

    private:
        static double Clamp(double value, double min, double max);

        static double Max(double x, double y);

        static Core::Media::Vector ValidateOffset(Core::Dependency::DependencyObject *o, Core::Media::Vector const& value);

        void CalculatedPropertiesChanged();
    };
}


#endif	//#ifndef __XUI_SCROLLVIEWER_H__
