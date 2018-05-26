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

#ifndef __XUI_SCROLLBAR_H__
#define __XUI_SCROLLBAR_H__

#include "UI/Detail/RangeBase.h"
#include "UI/Button.h"

namespace XUI::UI
{
    enum class ScrollBarVisibility : uint8;
    enum class Orientation : uint8;

    /// <summary>
    /// A scrollbar control.
    /// </summary>
    class TC_XUI_API ScrollBar : public RangeBase
    {
    public:
        /// <summary>
        /// Defines the <see cref="ViewportSize"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty ViewportSizeProperty;

        /// <summary>
        /// Defines the <see cref="Visibility"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty VisibilityProperty;

        /// <summary>
        /// Defines the <see cref="Orientation"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty OrientationProperty;

    private:
        std::shared_ptr<Button> _lineUpButton;
        std::shared_ptr<Button> _lineDownButton;
        std::shared_ptr<Button> _pageUpButton;
        std::shared_ptr<Button> _pageDownButton;

    public:
        /// <summary>
        /// Initializes static members of the <see cref="ScrollBar"/> class. 
        /// </summary>
        virtual ~ScrollBar()
        {
        }

        static void StaticClassInitializer();


        /// <summary>
        /// Initializes a new instance of the <see cref="ScrollBar"/> class.
        /// </summary>
    public:
        ScrollBar();

        /// <summary>
        /// Gets or sets the amount of the scrollable content that is currently visible.
        /// </summary>
        PROPERTY_(double, ViewportSize);
        double GetViewportSize() const;
        void SetViewportSize(const double &value);

        /// <summary>
        /// Gets or sets a value that indicates whether the scrollbar should hide itself when it
        /// is not needed.
        /// </summary>
        PROPERTY_(ScrollBarVisibility, Visibility);
        ScrollBarVisibility GetVisibility() const;
        void SetVisibility(ScrollBarVisibility value);

        /// <summary>
        /// Gets or sets the orientation of the scrollbar.
        /// </summary>
        PROPERTY_(UI::Orientation, Orientation);
        UI::Orientation GetOrientation() const;
        void SetOrientation(UI::Orientation value);

        /// <summary>
        /// Calculates whether the scrollbar should be visible.
        /// </summary>
        /// <returns>The scrollbar's visibility.</returns>
    private:
        bool CalculateIsVisible();

    protected:
        void OnTemplateApplied(Controls::TemplateAppliedEventArgs const& e) override;

    private:
        void LineUpClick(void *sender, Core::Observer::RoutedEventArgs& e);

        void LineDownClick(void *sender, Core::Observer::RoutedEventArgs& e);

        void PageUpClick(void *sender, Core::Observer::RoutedEventArgs& e);

        void PageDownClick(void *sender, Core::Observer::RoutedEventArgs& e);

        void SmallDecrement();

        void SmallIncrement();

        void LargeDecrement();

        void LargeIncrement();
    };
}


#endif	//#ifndef __XUI_SCROLLBAR_H__
