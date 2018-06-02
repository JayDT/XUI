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

#ifndef __XUI_CAROUSEL_H__
#define __XUI_CAROUSEL_H__

#include "UI/Detail/SelectingItemsControl.h"

namespace XUI::UI
{
    /// <summary>
    /// An items control that displays its items as pages that fill the control.
    /// </summary>
    class TC_XUI_API Carousel : public SelectingItemsControl
    {
    public:
        /// <summary>
        /// Defines the <see cref="IsVirtualized"/> property.
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty IsVirtualizedProperty;

        /// <summary>
        /// Defines the <see cref="Transition"/> property.
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty TransitionProperty;

    private:
        /// <summary>
        /// The default value of <see cref="ItemsControl.ItemsPanelProperty"/> for 
        /// <see cref="Carousel"/>.
        /// </summary>
        //static XUI::Core::Dependency::RefDependencyProperty PanelTemplate;

    public:
        /// <summary>
        /// Initializes static members of the <see cref="Carousel"/> class.
        /// </summary>
        static void StaticClassInitializer();
        Carousel() {}
        virtual ~Carousel() {}

        /// <summary>
        /// Gets or sets a value indicating whether the items in the carousel are virtualized.
        /// </summary>
        /// <remarks>
        /// When the carousel is virtualized, only the active page is held in memory.
        /// </remarks>
        PROPERTY_(bool , IsVirtualized);
        bool GetIsVirtualized() const;
        void SetIsVirtualized(const bool &value);

        /// <summary>
        /// Gets or sets the transition to use when moving between pages.
        /// </summary>
        //PROPERTY_(IPageTransition * , Transition);
        //IPageTransition *GetTransition() const;
        //void SetTransition(IPageTransition *value);

        /// <summary>
        /// Moves to the next item in the carousel.
        /// </summary>
        void Next();

        /// <summary>
        /// Moves to the previous item in the carousel.
        /// </summary>
        void Previous();

        /// <inheritdoc/>
    protected:
        void OnKeyDown(Interfaces::KeyEventArgs& e) override;

        /// <inheritdoc/>
        void OnPointerPressed(Interfaces::PointerPressedEventArgs& e) override;
    };
}


#endif	//#ifndef __XUI_CAROUSEL_H__
