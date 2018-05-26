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

#ifndef __XUI_DATAGRIDROWSPRESENTER_H__
#define __XUI_DATAGRIDROWSPRESENTER_H__

#include "UI/DataGrid/DataGridControl.h"
#include "UI/VirtualizingStackPanel.h"

namespace XUI::UI
{
    /// <summary>
    /// Panel that lays out individual rows top to bottom.  
    /// </summary>
    class DataGridRowsPresenter : public UI::VirtualizingStackPanel
    {
#pragma region Scroll Methods

        /// <summary>
        ///     Calls the protected method BringIndexIntoView.
        /// </summary>
        /// <param name="index">The index of the row to scroll into view.</param>
        /// <remarks>
        ///     BringIndexIntoView should be callable either from the ItemsControl
        ///     or directly on the panel. This was not done in WPF, so we are
        ///     building this internally for the DataGrid. However, if a public
        ///     way of calling BringIndexIntoView becomes a reality, then
        ///     this method is no longer needed.
        /// </remarks>
    public:
        void InternalBringIndexIntoView(int index);

    protected:
        /// <summary>
        ///     This method is invoked when the IsItemsHost property changes.
        /// </summary>
        /// <param name="oldIsItemsHost">The old value of the IsItemsHost property.</param>
        /// <param name="newIsItemsHost">The new value of the IsItemsHost property.</param>
        //void OnIsItemsHostChanged(bool oldIsItemsHost, bool newIsItemsHost) override;

        /// <summary>
        ///     Override of ViewportSizeChanged method which enqueues a dispatcher operation to redistribute
        ///     the width among columns.
        /// </summary>
        /// <param name="oldViewportSize">viewport size before the change</param>
        /// <param name="newViewportSize">viewport size after the change</param>
        //void OnViewportSizeChanged(Size *oldViewportSize, Size *newViewportSize) override;

#pragma endregion

#pragma region Column Virtualization

        /// <summary>
        ///     Measure
        /// </summary>
        Core::Media::Size MeasureOverride(Core::Media::Size const& constraint) override;

        /// <summary>
        ///     Property which returns the last measure input size
        /// </summary>
    public:
		PROPERTY_GET_ONLY(Core::Media::Size , AvailableSize);
        Core::Media::Size GetAvailableSize() const;

#pragma endregion

#pragma region Row Virtualization

    protected:
        /// <summary>
        ///     Override method to suppress the cleanup of rows
        ///     with validation errors.
        /// </summary>
        /// <param name="e"></param>
        //void OnCleanUpVirtualizedItem(CleanUpVirtualizedItemEventArgs *e) override;

#pragma endregion

#pragma region Helpers

    public:
		PROPERTY_GET_ONLY(DataGrid *, Owner);
        DataGrid *GetOwner() const;

#pragma endregion

#pragma region Data

    private:
        std::shared_ptr<DataGrid> _owner;
        Core::Media::Size _availableSize;

#pragma endregion
    };
}


#endif	//#ifndef __XUI_DATAGRIDROWSPRESENTER_H__
