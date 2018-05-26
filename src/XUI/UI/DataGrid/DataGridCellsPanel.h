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

#ifndef __XUI_DATAGRID_CELL_PANEL_H__
#define __XUI_DATAGRID_CELL_PANEL_H__

#include "UI/DataGrid/DataGridControl.h"
#include "UI/VirtualizingStackPanel.h"

namespace XUI::UI
{
    /// <summary>
    /// Panel that lays out individual rows top to bottom.  
    /// </summary>
    class DataGridCellsPanel : public UI::VirtualizingStackPanel
    {
    public:

        static void StaticClassInitializer();

        virtual ~DataGridCellsPanel();

        PROPERTY_GET_ONLY(DataGrid *, Owner);
        DataGrid *GetOwner() const;

        PROPERTY_GET_ONLY(Core::Media::Size, AvailableSize);
        Core::Media::Size GetAvailableSize() const;

    protected:

        /// <summary>
        ///     Measure
        /// </summary>
        Core::Media::Size MeasureOverride(Core::Media::Size const& constraint) override;

    private:
        std::shared_ptr<DataGrid> _owner;
        Core::Media::Size _availableSize;
    };
}


#endif	//#ifndef __XUI_DATAGRID_CELL_PANEL_H__
