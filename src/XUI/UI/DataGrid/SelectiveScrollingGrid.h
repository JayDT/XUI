/*
* Copyright (C) 2017-2018 Tauri JayD <https://www.>
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

#ifndef __XUI_SELECTIVESCROLLINGGRID_H__
#define __XUI_SELECTIVESCROLLINGGRID_H__

#include "UI/Grid.h"

namespace XUI::UI
{
    enum class SelectiveScrollingOrientation
    {
        /// <summary>
        /// The cell will not be allowed to get
        /// sctolled in any direction
        /// </summary>
        None = 0,

        /// <summary>
        /// The cell will be allowed to
        /// get scrolled only in horizontal direction
        /// </summary>
        Horizontal = 1,

        /// <summary>
        /// The cell will be allowed to
        /// get scrolled only in vertical directions
        /// </summary>
        Vertical = 2,

        /// <summary>
        /// The cell will be allowed to get
        /// scrolled in all directions
        /// </summary>
        Both = 3
    };

    /// <summary>
    /// Subclass of Grid that knows how to freeze certain cells in place when scrolled.
    /// Used as the panel for the DataGridRow to hold the header, cells, and details.
    /// </summary>
    class SelectiveScrollingGrid : public Grid
    {
    public:
        static void StaticClassInitializer();

        /// <summary>
        /// Attached property to specify the selective scroll behaviour of cells
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty SelectiveScrollingOrientationProperty;

        /// <summary>
        /// Getter for the SelectiveScrollingOrientation attached property
        /// </summary>
        /// <param name="obj"></param>
        /// <returns></returns>
        static UI::SelectiveScrollingOrientation GetSelectiveScrollingOrientation(Core::Dependency::DependencyObject *obj);

        /// <summary>
        /// Setter for the SelectiveScrollingOrientation attached property
        /// </summary>
        /// <param name="obj"></param>
        /// <param name="value"></param>
        static void SetSelectiveScrollingOrientation(DependencyObject *obj, UI::SelectiveScrollingOrientation value);

        /// <summary>
        /// Property changed call back for SelectiveScrollingOrientation property
        /// </summary>
        /// <param name="d"></param>
        /// <param name="e"></param>
    private:
        static void OnSelectiveScrollingOrientationChanged(void *d, Core::Dependency::DependencyPropertyChangedEventArgs const& e);
    };
}


#endif	//#ifndef __XUI_SELECTIVESCROLLINGGRID_H__
