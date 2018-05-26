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

#include "Core/GlobalInitializerScheduler.h"
#include "DataGridCellsPanel.h"
#include "Core/Media/MediaTypes.h"

using namespace XUI::UI;

void XUI::UI::DataGridCellsPanel::StaticClassInitializer()
{
}

XUI::UI::DataGridCellsPanel::~DataGridCellsPanel()
{
}

XUI::Core::Media::Size DataGridCellsPanel::MeasureOverride(XUI::Core::Media::Size const& constraint)
{
    _availableSize = constraint;
    return VirtualizingStackPanel::MeasureOverride(constraint);
}

XUI::Core::Media::Size DataGridCellsPanel::GetAvailableSize() const
{
    return _availableSize;
}

DataGrid *DataGridCellsPanel::GetOwner() const
{
    return _owner.get();
}
