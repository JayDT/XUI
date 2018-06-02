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

#include "StdAfx.h"
#include "DataGridColumnCollection.h"
#include "DataGrid/DataGridColumn.h"
#include "UI/DataGrid/DataGridControl.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;
using namespace XUI::UI;

/// <summary>
///     Updates the display index for the column that was just replaced and adjusts the other columns if necessary
/// </summary>


/// <summary>
///     Method which recomputes the widths of columns on resize of column
/// </summary>

XUI::UI::DataGridColumnCollection::DataGridColumnCollection(DataGrid * owner)
    : _dataGridOwner(owner)
{
    _displayIndexMap.reserve(10);

    CollectionChanged += System::make_eventhandler(&DataGridColumnCollection::OnColumnsChanged, this);

    //RealizedColumnsBlockListForNonVirtualizedRows = null;
    //RealizedColumnsDisplayIndexBlockListForNonVirtualizedRows = null;
    //RebuildRealizedColumnsBlockListForNonVirtualizedRows = true;
    //
    //RealizedColumnsBlockListForVirtualizedRows = null;
    //RealizedColumnsDisplayIndexBlockListForVirtualizedRows = null;
    //RebuildRealizedColumnsBlockListForVirtualizedRows = true;
}

XUI::UI::DataGridColumnCollection::~DataGridColumnCollection()
{
}

void XUI::UI::DataGridColumnCollection::OnColumnsChanged(void * sender, System::Collection::NotifyCollectionChangedEventArgs & e)
{
    // Update the reference to this DataGrid on the affected column(s)
    // and update the SelectedCells collection.
    switch (e.Action)
    {
    case System::Collection::NotifyCollectionChangedAction::Add:
        if (_displayIndexMapInitialized)
            UpdateDisplayIndexForNewColumns((*this)[e.NewStartingIndex].get(), e.NewStartingIndex);

        _hasVisibleStarColumns = HasVisibleStarColumnsInternal(nullptr);
        break;

    case System::Collection::NotifyCollectionChangedAction::Move:
        if (_displayIndexMapInitialized)
            UpdateDisplayIndexForMovedColumn(e.OldStartingIndex, e.NewStartingIndex);
        break;

    case System::Collection::NotifyCollectionChangedAction::Remove:
        if (_displayIndexMapInitialized)
            UpdateDisplayIndexForRemovedColumns((*this)[e.OldStartingIndex].get(), e.OldStartingIndex);

        ClearDisplayIndex((*this)[e.OldStartingIndex].get(), (*this)[e.NewStartingIndex].get());
        _hasVisibleStarColumns = HasVisibleStarColumnsInternal((*this)[e.OldStartingIndex].get());
        break;

    case System::Collection::NotifyCollectionChangedAction::Replace:
        if (_displayIndexMapInitialized)
            UpdateDisplayIndexForReplacedColumn((*this)[e.OldStartingIndex].get(), (*this)[e.NewStartingIndex].get());

        ClearDisplayIndex((*this)[e.OldStartingIndex].get(), (*this)[e.NewStartingIndex].get());
        _hasVisibleStarColumns = HasVisibleStarColumnsInternal((*this)[e.OldStartingIndex].get());
        break;

    case System::Collection::NotifyCollectionChangedAction::Reset:
        // We dont ClearDisplayIndex here because we no longer have access to the old items.
        // Instead this is handled in ClearItems.
        if (_displayIndexMapInitialized)
        {
            _displayIndexMap.clear();
            _dataGridOwner->UpdateColumnsOnVirtualizedCellInfoCollections(e.Action, -1, nullptr, -1);
        }

        _hasVisibleStarColumns = false;
        break;
    }
}

void XUI::UI::DataGridColumnCollection::RecomputeColumnWidthsOnColumnResize(DataGridColumn * resizingColumn, double horizontalChange, bool retainAuto)
{
	GridLength resizingColumnWidth = resizingColumn->Width;
	double expectedRezingColumnWidth = resizingColumn->ActualWidth + horizontalChange;

	if (expectedRezingColumnWidth < resizingColumn->MinWidth)
	{
		horizontalChange = resizingColumn->MinWidth - resizingColumn->ActualWidth;
	}
	else if (expectedRezingColumnWidth > resizingColumn->MaxWidth)
	{
		horizontalChange = resizingColumn->MaxWidth - resizingColumn->ActualWidth;
	}

	int resizingColumnIndex = resizingColumn->DisplayIndex;

	if (horizontalChange > 0.0)
	{
		RecomputeColumnWidthsOnColumnPositiveResize(horizontalChange, resizingColumnIndex, retainAuto);
	}
	else if (horizontalChange < 0.0)
	{
		RecomputeColumnWidthsOnColumnNegativeResize(-horizontalChange, resizingColumnIndex, retainAuto);
	}
}


/// <summary>
///     Method which computes the widths of the columns
/// </summary>

void XUI::UI::DataGridColumnCollection::ComputeColumnWidths()
{
	if (_hasVisibleStarColumns)
	{
		InitializeColumnDisplayValues();
		DistributeSpaceAmongColumns(_dataGridOwner->GetViewportWidthForColumns());
	}
	else
	{
		ExpandAllColumnWidthsToDesiredValue();
	}
	
	if (RefreshAutoWidthColumns)
	{
		for(auto const& column : *this)
		{
			if (column->Width.IsAuto)
			{
				// This operation resets desired and display widths to 0.0.
				column->Width = GridLength::GetAuto();
                column->DesiredWidth = 0.0;
                column->ActualWidth = 0.0;
			}
		}
	
		RefreshAutoWidthColumns = false;
	}
	
	_columnWidthsComputationPending = false;
}

void XUI::UI::DataGridColumnCollection::RedistributeColumnWidthsOnWidthChangeOfColumn(DataGridColumn * changedColumn, double oldWidth)
{
    GridLength width = changedColumn->Width;
    bool hasStarColumns = _hasVisibleStarColumns;
    if (changedColumn->_invalidatedStarWidth && !width.IsStar && !hasStarColumns)
    {
        ExpandAllColumnWidthsToDesiredValue();
    }
    else if (width.IsStar && changedColumn->_invalidatedStarWidth)
    {
        if (!HasVisibleStarColumnsInternal(changedColumn))
        {
            ComputeColumnWidths();
        }
        else
        {
            double minWidth = changedColumn->MinWidth;
            double leftOverSpace = GiveAwayWidthToNonStarColumns(nullptr, changedColumn->ActualWidth - minWidth);
            changedColumn->SetWidthInternal(width, minWidth + leftOverSpace, changedColumn->DesiredWidth);
            RecomputeStarColumnWidths();
        }
    }
    else if (width.IsStar && !changedColumn->_invalidatedStarWidth)
    {
        RecomputeStarColumnWidths();
    }
    else if (hasStarColumns)
    {
        RedistributeColumnWidthsOnNonStarWidthChange(
            changedColumn,
            oldWidth);
    }
}

/// <summary>
///     Method which redistributes the column widths based on change in MinWidth of a column
/// </summary>

void XUI::UI::DataGridColumnCollection::RedistributeColumnWidthsOnMinWidthChangeOfColumn(DataGridColumn * changedColumn, double oldMinWidth)
{
    GridLength width = changedColumn->Width;
    double minWidth = changedColumn->MinWidth;
    if (minWidth > changedColumn->ActualWidth)
    {
        if (_hasVisibleStarColumns)
        {
            TakeAwayWidthFromColumns(changedColumn, minWidth - changedColumn->ActualWidth, false);
        }

        changedColumn->SetWidthInternal(width, minWidth, changedColumn->DesiredWidth);
    }
    else if (minWidth < oldMinWidth)
    {
        if (width.IsStar)
        {
            if (changedColumn->ActualWidth == oldMinWidth)
            {
                GiveAwayWidthToColumns(changedColumn, oldMinWidth - minWidth, true);
            }
        }
        else if (oldMinWidth > changedColumn->DesiredWidth)
        {
            double displayValue = std::max(changedColumn->DesiredWidth, minWidth);
            if (_hasVisibleStarColumns)
            {
                GiveAwayWidthToColumns(changedColumn, oldMinWidth - displayValue, false);
            }

            changedColumn->SetWidthInternal(width, displayValue, changedColumn->DesiredWidth);
        }
    }
}

/// <summary>
///     Method which redistributes the column widths based on change in MaxWidth of a column
/// </summary>

void XUI::UI::DataGridColumnCollection::RedistributeColumnWidthsOnMaxWidthChangeOfColumn(DataGridColumn * changedColumn, double oldMaxWidth)
{
    GridLength width = changedColumn->Width;
    double maxWidth = changedColumn->MaxWidth;
    if (maxWidth < changedColumn->ActualWidth)
    {
        if (_hasVisibleStarColumns)
        {
            GiveAwayWidthToColumns(changedColumn, changedColumn->ActualWidth - maxWidth, false);
        }

        changedColumn->SetWidthInternal(width, maxWidth, changedColumn->DesiredWidth);
    }
    else if (maxWidth > oldMaxWidth)
    {
        if (width.IsStar)
        {
            RecomputeStarColumnWidths();
        }
        else if (oldMaxWidth < changedColumn->DesiredWidth)
        {
            double displayValue = std::min(changedColumn->DesiredWidth, maxWidth);
            if (_hasVisibleStarColumns)
            {
                double leftOverSpace = TakeAwayWidthFromUnusedSpace(false, displayValue - oldMaxWidth);
                leftOverSpace = TakeAwayWidthFromStarColumns(changedColumn, leftOverSpace);
                displayValue -= leftOverSpace;
            }

            changedColumn->SetWidthInternal(width, displayValue, changedColumn->DesiredWidth);
        }
    }
}

void XUI::UI::DataGridColumnCollection::NotifyPropertyChanged(Core::Dependency::DependencyObject * d, std::string const & propertyName, Core::Dependency::DependencyPropertyChangedEventArgs & e, NotificationTarget target)
{
    if (target & NotificationTarget::ColumnCollection)
    {
        if (e.Property == DataGridColumn::DisplayIndexProperty.get())
        {
            OnColumnDisplayIndexChanged(static_cast<DataGridColumn*>(d), e.OldValue.GetValue<int>(), e.NewValue.GetValue<int>());
            if (static_cast<DataGridColumn*>(d)->IsVisible())
            {
                InvalidateColumnRealization(true);
            }
        }
        else if (e.Property == DataGridColumn::WidthProperty.get())
        {
            if (static_cast<DataGridColumn*>(d)->IsVisible())
            {
                InvalidateColumnRealization(false);
            }
        }
        else if (e.Property == DataGrid::FrozenColumnCountProperty.get())
        {
            InvalidateColumnRealization(false);
            OnDataGridFrozenColumnCountChanged(e.OldValue.GetValue<int>(), e.NewValue.GetValue<int>());
        }
        else if (e.Property == DataGridColumn::VisibilityProperty.get())
        {
            InvalidateHasVisibleStarColumns();
            if (!_columnWidthsComputationPending)
            {
                Core::UIDispatcher::OnRender += [this, dataGrid = _dataGridOwner->weak_from_base_static<DataGrid>()]()
                {
                    if (dataGrid.expired())
                        return;

                    ComputeColumnWidths();
                    Core::Dependency::DependencyPropertyChangedEventArgs e(nullptr, nullptr, System::Reflection::Variant::Empty, System::Reflection::Variant::Empty);
                    _dataGridOwner->NotifyPropertyChanged(
                        _dataGridOwner,
                        e,
                        NotificationTarget::CellsPresenter | NotificationTarget::ColumnHeadersPresenter);
                };
                _columnWidthsComputationPending = true;
            }
            InvalidateColumnRealization(true);
        }
        else if (e.Property == DataGrid::EnableColumnVirtualizationProperty.get())
        {
            InvalidateColumnRealization(true);
        }
        else if (e.Property == DataGrid::CellsPanelHorizontalOffsetProperty.get())
        {
            OnCellsPanelHorizontalOffsetChanged(e);
        }
        else if (e.Property == DataGrid::HorizontalScrollOffsetProperty.get() || propertyName == "ViewportWidth")
        {
            InvalidateColumnRealization(false);
        }
    }

    if (target & NotificationTarget::Columns)
    {
        int count = size();
        for (int i = 0; i < count; i++)
        {
            // Passing in NotificationTarget.Columns directly to ensure the notification doesn't
            // bounce back to the collection.
            (*this)[i]->NotifyPropertyChanged(d, e, NotificationTarget::Columns);
        }
    }
}

/// <summary>
///     Method which handles the column widths computation for CellsPanelHorizontalOffset change
/// </summary>

void XUI::UI::DataGridColumnCollection::OnCellsPanelHorizontalOffsetChanged(Core::Dependency::DependencyPropertyChangedEventArgs & e)
{
    InvalidateColumnRealization(false);

    // Change in CellsPanelOffset width has an opposite effect on Column
    // width distribution. Hence widthChange is (oldvalue - newvalue)
    double totalAvailableWidth = _dataGridOwner->GetViewportWidthForColumns();
    RedistributeColumnWidthsOnAvailableSpaceChange(e.OldValue.GetValue<double>() - e.NewValue.GetValue<double>(), totalAvailableWidth);
}

/// <summary>
///     Method which redistributes the column widths based on change in available space of a column
/// </summary>

void XUI::UI::DataGridColumnCollection::RedistributeColumnWidthsOnAvailableSpaceChange(double availableSpaceChange, double newTotalAvailableSpace)
{
    if (!_columnWidthsComputationPending && _hasVisibleStarColumns)
    {
        if (availableSpaceChange > 0.0)
        {
            GiveAwayWidthToColumns(nullptr, availableSpaceChange, false);
        }
        else if (availableSpaceChange < 0.0)
        {
            TakeAwayWidthFromColumns(nullptr, std::abs(availableSpaceChange), false, newTotalAvailableSpace);
        }
    }
}

/// <summary>
///     Method which redistributes widths of columns on change of a column's width
///     when datagrid itself has star columns, but neither the oldwidth or the newwidth
///     of changed column is star.
/// </summary>

void XUI::UI::DataGridColumnCollection::RedistributeColumnWidthsOnNonStarWidthChange(DataGridColumn * changedColumn, double oldWidth)
{
    GridLength width = changedColumn->Width;
    if (changedColumn->ActualWidth > oldWidth)
    {
        double nonRetrievableSpace = TakeAwayWidthFromColumns(changedColumn, changedColumn->DesiredWidth - changedColumn->ActualWidth, changedColumn != nullptr);
        if (nonRetrievableSpace > 0.0)
        {
            changedColumn->SetWidthInternal(width,
                std::max(changedColumn->ActualWidth - nonRetrievableSpace, changedColumn->MinWidth),
                changedColumn->DesiredWidth);
        }
    }
    else if (changedColumn->DesiredWidth < oldWidth)
    {
        double newDesiredValue = std::min(std::max(changedColumn->ActualWidth, changedColumn->MinWidth), changedColumn->MaxWidth);
        GiveAwayWidthToColumns(changedColumn, changedColumn->ActualWidth - newDesiredValue, false);
    }
}

/// <summary>
///     Method which tries to give away the given amount of width 
///     among all the columns except the ignored column
/// </summary>
/// <param name="ignoredColumn">The column which is giving away the width</param>
/// <param name="giveAwayWidth">The amount of giveaway width</param>

double XUI::UI::DataGridColumnCollection::GiveAwayWidthToColumns(DataGridColumn * ignoredColumn, double giveAwayWidth, bool recomputeStars)
{
    double originalGiveAwayWidth = giveAwayWidth;
    giveAwayWidth = GiveAwayWidthToScrollViewerExcess(giveAwayWidth, /*includedInColumnsWidth*/ ignoredColumn != nullptr);
    giveAwayWidth = GiveAwayWidthToNonStarColumns(ignoredColumn, giveAwayWidth);

    if (giveAwayWidth > 0.0 || recomputeStars)
    {
        double sumOfStarDisplayWidths = 0.0;
        double sumOfStarMaxWidths = 0.0;
        bool giveAwayWidthIncluded = false;
        for (auto const& column : *this)
        {
            GridLength width = column->Width;
            if (width.IsStar && column->IsVisible())
            {
                if (column.get() == ignoredColumn)
                {
                    giveAwayWidthIncluded = true;
                }

                sumOfStarDisplayWidths += column->ActualWidth;
                sumOfStarMaxWidths += column->MaxWidth;
            }
        }

        double expectedStarSpace = sumOfStarDisplayWidths;
        if (!giveAwayWidthIncluded)
        {
            expectedStarSpace += giveAwayWidth;
        }
        else if (originalGiveAwayWidth != giveAwayWidth)
        {
            expectedStarSpace -= (originalGiveAwayWidth - giveAwayWidth);
        }

        double usedStarSpace = ComputeStarColumnWidths(std::min(expectedStarSpace, sumOfStarMaxWidths));
        giveAwayWidth = std::max(usedStarSpace - expectedStarSpace, 0.0);
    }

    return giveAwayWidth;
}

/// <summary>
///     Helper method which gives away width to scroll viewer
///     if its extent width is greater than viewport width
/// </summary>

double XUI::UI::DataGridColumnCollection::GiveAwayWidthToScrollViewerExcess(double giveAwayWidth, bool includedInColumnsWidth)
{
    double totalSpace = _dataGridOwner->GetViewportWidthForColumns();
    double usedSpace = 0.0;
    for (auto const& column : *this)
    {
        if (column->IsVisible())
        {
            usedSpace += column->ActualWidth;
        }
    }

    if (includedInColumnsWidth)
    {
        if (usedSpace > totalSpace)
        {
            double contributingSpace = usedSpace - totalSpace;
            giveAwayWidth -= std::min(contributingSpace, giveAwayWidth);
        }
    }
    else
    {
        // If the giveAwayWidth is not included in columns, then the new
        // giveAwayWidth should be derived the total available space and used space
        giveAwayWidth = std::min(giveAwayWidth, std::max(0.0, totalSpace - usedSpace));
    }

    return giveAwayWidth;
}

/// <summary>
///     Method which redistributes the width of star columns among them selves
/// </summary>

void XUI::UI::DataGridColumnCollection::RecomputeStarColumnWidths()
{
    double totalDisplaySpace = _dataGridOwner->GetViewportWidthForColumns();
    double nonStarSpace = 0.0;
    for (auto const& column : *this)
    {
        GridLength width = column->Width;
        if (column->IsVisible() && !width.IsStar)
        {
            nonStarSpace += column->ActualWidth;
        }
    }

    if (std::isnan(nonStarSpace))
    {
        return;
    }

    ComputeStarColumnWidths(totalDisplaySpace - nonStarSpace);
}

/// <summary>
///     Helper method which determines the average with of all the columns
/// </summary>

double XUI::UI::DataGridColumnCollection::ComputeAverageColumnWidth()
{
    double eligibleDisplayValue = 0.0;
    int totalFactors = 0;
    for (auto const& column : *this)
    {
        if (column->IsVisible() && !std::isnan(column->ActualWidth) && column->ActualWidth != -1.0)
        {
            eligibleDisplayValue += column->ActualWidth;
            totalFactors++;
        }
    }

    if (totalFactors != 0)
    {
        return eligibleDisplayValue / totalFactors;
    }

    return 0.0;
}

/// <summary>
///     Method which tries to give away the given amount of width
///     among all non star columns except the ignored column
/// </summary>

double XUI::UI::DataGridColumnCollection::GiveAwayWidthToNonStarColumns(DataGridColumn * ignoredColumn, double giveAwayWidth)
{
    while (giveAwayWidth > 0.0)
    {
        int countOfParticipatingColumns = 0;
        double minLagWidth = FindMinimumLaggingWidthOfNonStarColumns(
            ignoredColumn,
            countOfParticipatingColumns);

        if (countOfParticipatingColumns == 0)
        {
            break;
        }

        double minTotalLagWidth = minLagWidth * countOfParticipatingColumns;
        if (minTotalLagWidth >= giveAwayWidth)
        {
            minLagWidth = giveAwayWidth / countOfParticipatingColumns;
            giveAwayWidth = 0.0;
        }
        else
        {
            giveAwayWidth -= minTotalLagWidth;
        }

        GiveAwayWidthToEveryNonStarColumn(ignoredColumn, minLagWidth);
    }

    return giveAwayWidth;
}

/// <summary>
///     Helper method which finds the minimum non-zero difference between displayvalue and desiredvalue
///     among all non star columns
/// </summary>

double XUI::UI::DataGridColumnCollection::FindMinimumLaggingWidthOfNonStarColumns(DataGridColumn * ignoredColumn, int & countOfParticipatingColumns)
{
    double minLagWidth = std::numeric_limits<double>::max();
    countOfParticipatingColumns = 0;
    for (auto const& column : *this)
    {
        if (ignoredColumn == column.get() ||
            !column->IsVisible())
        {
            continue;
        }

        GridLength width = column->Width;
        if (width.IsStar)
        {
            continue;
        }

        double columnMaxWidth = column->MaxWidth;
        if (column->ActualWidth < column->DesiredWidth &&
            column->ActualWidth != columnMaxWidth)
        {
            countOfParticipatingColumns++;
            double lagWidth = std::min(column->DesiredWidth, columnMaxWidth) - column->ActualWidth;
            if (lagWidth < minLagWidth)
            {
                minLagWidth = lagWidth;
            }
        }
    }

    return minLagWidth;
}


/// <summary>
///     Method which initializes the column width's diplay value to its desired value
/// </summary>


/// <summary>
///     Helper method which gives away the given amount of width to
///     every non star column whose display value is less than its desired value
/// </summary>

void XUI::UI::DataGridColumnCollection::GiveAwayWidthToEveryNonStarColumn(DataGridColumn * ignoredColumn, double perColumnGiveAwayWidth)
{
    for (auto const& column : *this)
    {
        if (ignoredColumn == column.get() ||
            !column->IsVisible())
        {
            continue;
        }

        GridLength width = column->Width;
        if (width.IsStar)
        {
            continue;
        }

        if (column->ActualWidth < std::min(column->DesiredWidth, column->MaxWidth))
        {
            column->SetWidthInternal(width, column->ActualWidth + perColumnGiveAwayWidth, column->DesiredWidth);
        }
    }
}

void XUI::UI::DataGridColumnCollection::InitializeColumnDisplayValues()
{
	for (auto const& column : *this)
	{
		if (!column->IsVisible())
		{
			continue;
		}

		GridLength width = column->Width;
		if (!width.IsStar)
		{
			double minWidth = column->MinWidth;
			double displayValue = std::max(std::min(width.Value, minWidth), column->MaxWidth);
			if (width.Value != displayValue)
			{
				column->SetWidthInternal(width, displayValue, width.Value);
			}
		}
	}
}

/// <summary>
///     Method which expands the display values of widths of all columns to
///     their desired values. Usually used when the last star column's width
///     is changed to non-star
/// </summary>
void XUI::UI::DataGridColumnCollection::ExpandAllColumnWidthsToDesiredValue()
{
	for (auto const& column : *this)
	{
		if (!column->IsVisible())
		{
			continue;
		}

		GridLength width = column->Width;
		double maxWidth = column->MaxWidth;
		if (column->DesiredWidth < column->ActualWidth &&
			column->ActualWidth != maxWidth)
		{
			column->SetWidthInternal(width, column->ActualWidth, column->DesiredWidth);
		}
	}
}

/// <summary>
///     Method which distributes a given amount of width among all the columns
/// </summary>
void XUI::UI::DataGridColumnCollection::DistributeSpaceAmongColumns(double availableSpace)
{
	double sumOfMinWidths = 0.0;
	double sumOfMaxWidths = 0.0;
	double sumOfStarMinWidths = 0.0;
	for (auto const& column : *this)
	{
		if (!column->IsVisible())
		{
			continue;
		}

		sumOfMinWidths += column->MinWidth;
		sumOfMaxWidths += column->MaxWidth;
		if (column->Width.IsStar)
		{
			sumOfStarMinWidths += column->MinWidth;
		}
	}

	if (availableSpace < sumOfMinWidths)
	{
		availableSpace = sumOfMinWidths;
	}

	if (availableSpace > sumOfMaxWidths)
	{
		availableSpace = sumOfMaxWidths;
	}

	double nonStarSpaceLeftOver = DistributeSpaceAmongNonStarColumns(availableSpace - sumOfStarMinWidths);

	ComputeStarColumnWidths(sumOfStarMinWidths + nonStarSpaceLeftOver);
}

/// <summary>
///     Helper method which distributes a given amount of width among all non star columns
/// </summary>
double XUI::UI::DataGridColumnCollection::DistributeSpaceAmongNonStarColumns(double availableSpace)
{
	double requiredSpace = 0.0;
	for (auto const& column : *this)
	{
		GridLength width = column->Width;
		if (!column->IsVisible() ||
			width.IsStar)
		{
			continue;
		}

		requiredSpace += column->ActualWidth;
	}

	if (availableSpace < requiredSpace)
	{
		double spaceDeficit = requiredSpace - availableSpace;
		TakeAwayWidthFromNonStarColumns(nullptr, spaceDeficit);
	}

	return std::max(availableSpace - requiredSpace, 0.0);
}

void XUI::UI::DataGridColumnCollection::InitializeDisplayIndexMap(DataGridColumn * changingColumn, int oldDisplayIndex, int & resultDisplayIndex)
{
    resultDisplayIndex = oldDisplayIndex;
    if (_displayIndexMapInitialized)
        return;

    _displayIndexMapInitialized = true;

    EXCEPT(_displayIndexMap.empty() && "DisplayIndexMap should be empty until first measure call.");
    int columnCount = size();
    std::map<int, int> assignedDisplayIndexMap; // <DisplayIndex, ColumnIndex>

    if (changingColumn != nullptr && oldDisplayIndex >= columnCount)
    {
        throw System::ArgumentOutOfRangeException("displayIndex"/*, oldDisplayIndex, SR.Get(SRID.DataGrid_ColumnDisplayIndexOutOfRange, changingColumn.Header)*/);
    }

    // First loop:
    // 1. Validate all columns DisplayIndex
    // 2. Add columns with DisplayIndex!=default to the assignedDisplayIndexMap
    for (int columnIndex = 0; columnIndex < columnCount; columnIndex++)
    {
        DataGridColumn* currentColumn = (*this)[columnIndex].get();
        int currentColumnDisplayIndex = currentColumn->DisplayIndex;

        ValidateDisplayIndex(currentColumn, currentColumnDisplayIndex);

        if (currentColumn == changingColumn)
        {
            currentColumnDisplayIndex = oldDisplayIndex;
        }

        if (currentColumnDisplayIndex >= 0)
        {
            if (assignedDisplayIndexMap.find(currentColumnDisplayIndex) != assignedDisplayIndexMap.end())
            {
                throw System::ArgumentException("" /*SR.Get(SRID.DataGrid_DuplicateDisplayIndex)*/);
            }

            assignedDisplayIndexMap[currentColumnDisplayIndex] = columnIndex;
        }
    }

    // Second loop:
    // Assign DisplayIndex to the columns with default values
    int nextAvailableColumnIndex = 0;
    for (int columnIndex = 0; columnIndex < columnCount; columnIndex++)
    {
        DataGridColumn* currentColumn = (*this)[columnIndex].get();
        int currentColumnDisplayIndex = currentColumn->DisplayIndex;
        bool hasDefaultDisplayIndex = currentColumn->IsDefaultValue(DataGridColumn::DisplayIndexProperty.get());
        if (currentColumn == changingColumn)
        {
            if (oldDisplayIndex == -1)
            {
                hasDefaultDisplayIndex = true;
            }
            currentColumnDisplayIndex = oldDisplayIndex;
        }

        if (hasDefaultDisplayIndex)
        {
            while (assignedDisplayIndexMap.find(nextAvailableColumnIndex) != assignedDisplayIndexMap.end())
            {
                nextAvailableColumnIndex++;
            }

            CoerceDefaultDisplayIndex(currentColumn, nextAvailableColumnIndex);
            assignedDisplayIndexMap[nextAvailableColumnIndex] = columnIndex;
            if (currentColumn == changingColumn)
            {
                resultDisplayIndex = nextAvailableColumnIndex;
            }
            nextAvailableColumnIndex++;
        }
    }

    // Third loop:
    // Copy generated assignedDisplayIndexMap into DisplayIndexMap
    for (int displayIndex = 0; displayIndex < columnCount; displayIndex++)
    {
        ASSERT(assignedDisplayIndexMap.find(displayIndex) != assignedDisplayIndexMap.end());
        _displayIndexMap.push_back(assignedDisplayIndexMap[displayIndex]);
    }
}

/// <summary>
///     Updates the display index for all columns affected by the removal of a set of columns.  
/// </summary>

void XUI::UI::DataGridColumnCollection::UpdateDisplayIndexForRemovedColumns(DataGridColumn * column, int startingIndex)
{
    ASSERT(_isUpdatingDisplayIndex == false && "We don't remove columns as part of a display index update operation");

    System::ScopeGuard g([&]()
    {
        _isUpdatingDisplayIndex = false;
    });

    try
    {
        _isUpdatingDisplayIndex = true;
        ASSERT(_displayIndexMap.size() > size() && "Columns were just removed: the display index map shouldn't have yet been updated");

        int removedDisplayIndex = RemoveFromDisplayIndexMap(startingIndex);

        // Removing the column in the map means that all columns with display index >= the new column's display index
        // were given a lower display index.  This is perfect, except that the column indices have changed due to the insert
        // in the column collection.  We need to iterate over the column indices and decrement them appropriately.  We also
        // need to give each changed column a new display index.
        for (int i = 0; i < _displayIndexMap.size(); i++)
        {
            if (i >= removedDisplayIndex)
            {
                // All columns with DisplayIndex higher than the newly deleted columns need to have their DisplayIndex adiusted
                // (we use >= because a column will have been decremented to have the same display index as the deleted column).
                column = ColumnFromDisplayIndex(i);
                column->DisplayIndex = column->DisplayIndex - 1;
            }
        }

        Debug_VerifyDisplayIndexMap();

		_dataGridOwner->UpdateColumnsOnVirtualizedCellInfoCollections(System::Collection::NotifyCollectionChangedAction::Remove, removedDisplayIndex, column, -1);
    }
    catch (...)
    {
    }
}

void XUI::UI::DataGridColumnCollection::UpdateDisplayIndexForReplacedColumn(DataGridColumn * oldColumn, DataGridColumn * newColumn)
{
    if (oldColumn != nullptr && newColumn != nullptr)
    {
        int newDisplayIndex = CoerceDefaultDisplayIndex(newColumn);

        if (oldColumn->DisplayIndex != newDisplayIndex)
        {
            // Update the display index of other columns to adjust for that of the new one.
            UpdateDisplayIndexForChangedColumn(oldColumn->DisplayIndex, newDisplayIndex);
        }

        _dataGridOwner->UpdateColumnsOnVirtualizedCellInfoCollections(System::Collection::NotifyCollectionChangedAction::Replace, newDisplayIndex, oldColumn, newDisplayIndex);
    }
}

/// <summary>
///     Clears the DisplayIndexProperty on each of the columns.
/// </summary>

void XUI::UI::DataGridColumnCollection::ClearDisplayIndex(DataGridColumn * oldColumns, DataGridColumn * newColumns)
{
    if (oldColumns != nullptr)
    {
        try
        {
            _isClearingDisplayIndex = true;

            auto column = oldColumns;

            // Only clear the old column's index if its not in newColumns
            if (newColumns != nullptr && oldColumns != newColumns)
            {
                return;
            }

            if (column)
                column->ClearValue(DataGridColumn::DisplayIndexProperty.get());

            _isClearingDisplayIndex = false;
        }
        catch (...)
        {
            _isClearingDisplayIndex = false;
        }
    }
}

/// <summary>
///     Returns true if the display index is valid for the given column
/// </summary>

bool XUI::UI::DataGridColumnCollection::IsDisplayIndexValid(DataGridColumn * column, int displayIndex, bool isAdding)
{
    // -1 is legal only as a default value
    if (displayIndex == -1 && column->IsDefaultValue(DataGridColumn::DisplayIndexProperty.get()))
        return true;

    // If we're adding a column the count will soon be increased by one -- so a DisplayIndex == size() is ok.
    return displayIndex >= 0 && (isAdding ? displayIndex <= size() : displayIndex < size());
}

/// <summary>
///     Inserts the given columnIndex in the DisplayIndexMap at the given display index.
/// </summary>

void XUI::UI::DataGridColumnCollection::InsertInDisplayIndexMap(int newDisplayIndex, int columnIndex)
{
    _displayIndexMap.insert(_displayIndexMap.begin() + newDisplayIndex, columnIndex);

    for (int i = 0; i < _displayIndexMap.size(); i++)
    {
        if (_displayIndexMap[i] >= columnIndex && i != newDisplayIndex)
        {
            // These are columns that are after the inserted item in the column collection; we have to adiust
            // to account for the shifted column index.
            _displayIndexMap[i]++;
        }
    }
}

/// <summary>
///     Removes the given column index from the DisplayIndexMap
/// </summary>

int XUI::UI::DataGridColumnCollection::RemoveFromDisplayIndexMap(int columnIndex)
{
    auto itr = std::find(_displayIndexMap.begin(), _displayIndexMap.end(), columnIndex);
    if (itr == _displayIndexMap.end())
    {
        //Assert(false);
        return -1;
    }

    size_t removedDisplayIndex = itr - _displayIndexMap.begin();

    _displayIndexMap.erase(itr);

    for (int i = 0; i < _displayIndexMap.size(); i++)
    {
        if (_displayIndexMap[i] >= columnIndex)
        {
            // These are columns that are after the removed item in the column collection; we have to adiust
            // to account for the shifted column index.
            _displayIndexMap[i]--;
        }
    }

    return removedDisplayIndex;
}

int XUI::UI::DataGridColumnCollection::CoerceDefaultDisplayIndex(DataGridColumn * column)
{
    return CoerceDefaultDisplayIndex(column, IndexOf(column->shared_from_base_static<DataGridColumn>()));
}

/// <summary>
///     This takes a column and checks that if its DisplayIndex is the default value.  If so, it coerces
///     the DisplayIndex to be its location in the columns collection.
///     We can't do this in CoerceValue because the callback isn't called for default values.  Instead we call this
///     whenever a column is added or replaced in the collection or when the DisplayIndex of an existing column has changed.
/// </summary>
/// <param name="column">The column</param>
/// <param name="newDisplayIndex">The DisplayIndex the column should have</param>
/// <returns>The DisplayIndex of the column</returns>

int XUI::UI::DataGridColumnCollection::CoerceDefaultDisplayIndex(DataGridColumn * column, int newDisplayIndex)
{
    if (column->IsDefaultValue(DataGridColumn::DisplayIndexProperty.get()))
    {
        bool isUpdating = _isUpdatingDisplayIndex;
        System::ScopeGuard g([&]()
        {
            _isUpdatingDisplayIndex = isUpdating;
        });
        try
        {
            _isUpdatingDisplayIndex = true;
            column->DisplayIndex = newDisplayIndex;
        }
        catch (...)
        {
        }

        return newDisplayIndex;
    }

    return column->DisplayIndex;
}

/// <summary>
///     Called when a column's display index has changed.  
/// <param name="oldDisplayIndex">the old display index of the column</param>
/// <param name="newDisplayIndex">the new display index of the column</param>

void XUI::UI::DataGridColumnCollection::OnColumnDisplayIndexChanged(DataGridColumn * column, int oldDisplayIndex, int newDisplayIndex)
{
    int originalOldDisplayIndex = oldDisplayIndex;
    if (!_displayIndexMapInitialized)
    {
        InitializeDisplayIndexMap(column, oldDisplayIndex, oldDisplayIndex);
    }

    // Handle ClearValue.
    if (_isClearingDisplayIndex)
    {
        // change from -1 to the new value; the OnColumnDisplayIndexChanged further down the stack (from old value to -1) will handle
        // notifying the user and updating columns.
        return;
    }

    // The DisplayIndex may have changed to the default value.  
    newDisplayIndex = CoerceDefaultDisplayIndex(column);

    if (newDisplayIndex == oldDisplayIndex)
    {
        return;
    }

    // Our coerce value callback should have validated the DisplayIndex.  Fire the virtual.
    ASSERT(newDisplayIndex >= 0 && newDisplayIndex < size() && "The new DisplayIndex should have already been validated");

    // -1 is the default value and really means 'DisplayIndex should be the index of the column in the column collection'.
    // We immediately replace the display index without notifying anyone.
    if (originalOldDisplayIndex != -1)
    {
		DataGridColumnEventArgs e;
		e.Column = column;
        _dataGridOwner->OnColumnDisplayIndexChanged(e);
    }

    // Call our helper to walk through all other columns and adjust their display indices.
    UpdateDisplayIndexForChangedColumn(oldDisplayIndex, newDisplayIndex);
}

void XUI::UI::DataGridColumnCollection::Debug_VerifyDisplayIndexMap()
{
    EXCEPT(size() == _displayIndexMap.size() && "Display Index map is of the wrong size");
    for (int i = 0; i < _displayIndexMap.size(); i++)
    {
        EXCEPT(_displayIndexMap[i] >= 0 && _displayIndexMap[i] < size() && "DisplayIndex map entry doesn't point to a valid column");
        EXCEPT(ColumnFromDisplayIndex(i)->DisplayIndex == i && "DisplayIndex map doesn't match column indices");
    }
}

/// <summary>
///     Called when the DisplayIndex for a single column has changed.  The other columns may have conflicting display indices, so
///     we walk through them and adjust.  This method does nothing if we're already updating display index as part of a larger
///     operation (such as add or remove).  This is both for re-entrancy and to avoid modifying the display index map as we walk over
///     the columns.
/// </summary>

void XUI::UI::DataGridColumnCollection::UpdateDisplayIndexForChangedColumn(int oldDisplayIndex, int newDisplayIndex)
{
    // The code below adjusts the DisplayIndex of other columns and shouldn't happen if this column's display index is changed
    // to account for the change in another.
    if (_isUpdatingDisplayIndex)
    {
        // Avoid re-entrancy; setting DisplayIndex on columns causes their OnDisplayIndexChanged to fire.
        return;
    }

    System::ScopeGuard g([&]()
    {
        _isUpdatingDisplayIndex = false;
    });

    try
    {
        _isUpdatingDisplayIndex = true;

        ASSERT(oldDisplayIndex != newDisplayIndex && "A column's display index must have changed for us to call OnColumnDisplayIndexChanged");
        ASSERT(oldDisplayIndex >= 0 && oldDisplayIndex < size() && "The old DisplayIndex should be valid");

        // Update the display index mapping for all affected columns.
        int columnIndex = _displayIndexMap[oldDisplayIndex];
        _displayIndexMap.erase(_displayIndexMap.begin() + oldDisplayIndex);
        _displayIndexMap.insert(_displayIndexMap.begin() + newDisplayIndex, columnIndex);

        // Update the display index of other columns.
        if (newDisplayIndex < oldDisplayIndex)
        {
            // DisplayIndex decreased. All columns with DisplayIndex >= newDisplayIndex and < oldDisplayIndex
            // get their DisplayIndex incremented.
            for (int i = newDisplayIndex + 1; i <= oldDisplayIndex; i++)
                ColumnFromDisplayIndex(i)->DisplayIndex = ColumnFromDisplayIndex(i)->DisplayIndex + 1;
        }
        else
        {
            // DisplayIndex increased. All columns with DisplayIndex <= newDisplayIndex and > oldDisplayIndex get their DisplayIndex decremented.
            for (int i = oldDisplayIndex; i < newDisplayIndex; i++)
                ColumnFromDisplayIndex(i)->DisplayIndex = ColumnFromDisplayIndex(i)->DisplayIndex - 1;
        }

        Debug_VerifyDisplayIndexMap();

        _dataGridOwner->UpdateColumnsOnVirtualizedCellInfoCollections(System::Collection::NotifyCollectionChangedAction::Move, oldDisplayIndex, nullptr, newDisplayIndex);
    }
    catch (...)
    {
    }
}

void XUI::UI::DataGridColumnCollection::UpdateDisplayIndexForMovedColumn(int oldColumnIndex, int newColumnIndex)
{
    int displayIndex = RemoveFromDisplayIndexMap(oldColumnIndex);
    InsertInDisplayIndexMap(displayIndex, newColumnIndex);
    _dataGridOwner->UpdateColumnsOnVirtualizedCellInfoCollections(System::Collection::NotifyCollectionChangedAction::Move, oldColumnIndex, nullptr, newColumnIndex);
}

/// <summary>
///     Sets the DisplayIndex on all newly inserted or added columns and updates the existing columns as necessary.  
/// </summary>

void XUI::UI::DataGridColumnCollection::UpdateDisplayIndexForNewColumns(DataGridColumn * column, int startingIndex)
{
    int newDisplayIndex, columnIndex;

    EXCEPT(_isUpdatingDisplayIndex == false && "We don't add new columns as part of a display index update operation");

    System::ScopeGuard g([&]()
    {
        _isUpdatingDisplayIndex = false;
    });

    try
    {
        _isUpdatingDisplayIndex = true;

        // Set the display index of the new columns and add them to the DisplayIndexMap
        columnIndex = startingIndex;

        newDisplayIndex = CoerceDefaultDisplayIndex(column, columnIndex);

        // Inserting the column in the map means that all columns with display index >= the new column's display index
        // were given a higher display index.  This is perfect, except that the column indices have changed due to the insert
        // in the column collection.  We need to iterate over the column indices and increment them appropriately.  We also
        // need to give each changed column a new display index.
        InsertInDisplayIndexMap(newDisplayIndex, columnIndex);

        for (int i = 0; i < _displayIndexMap.size(); i++)
        {
            if (i > newDisplayIndex)
            {
                // All columns with DisplayIndex higher than the newly inserted columns
                // need to have their DisplayIndex adiusted.
                column = ColumnFromDisplayIndex(i);
                column->DisplayIndex = column->DisplayIndex + 1;
            }
        }

        Debug_VerifyDisplayIndexMap();

        _dataGridOwner->UpdateColumnsOnVirtualizedCellInfoCollections(System::Collection::NotifyCollectionChangedAction::Add, -1, nullptr, newDisplayIndex);
    }
    catch (...)
    {
    }
}

/// <summary>
///     Method which sets / resets the IsFrozen property of columns based on DataGrid's FrozenColumnCount.
///     It is possible that the FrozenColumnCount change could be a result of column count itself, in
///     which case only the columns which are in the collection at the moment are to be considered.
/// </summary>
/// <param name="oldFrozenCount"></param>
/// <param name="newFrozenCount"></param>

void XUI::UI::DataGridColumnCollection::OnDataGridFrozenColumnCountChanged(int oldFrozenCount, int newFrozenCount)
{
    if (newFrozenCount > oldFrozenCount)
    {
        int columnCount = std::min(newFrozenCount, int(size()));
        for (int i = oldFrozenCount; i < columnCount; i++)
        {
            ColumnFromDisplayIndex(i)->IsFrozen = true;
        }
    }
    else
    {
        int columnCount = std::min(oldFrozenCount, int(size()));
        for (int i = newFrozenCount; i < columnCount; i++)
        {
            ColumnFromDisplayIndex(i)->IsFrozen = false;
        }
    }
}

/// <summary>
///     Method which determines if there are any
///     star columns in datagrid except the given column and also returns perStarWidth
/// </summary>

bool XUI::UI::DataGridColumnCollection::HasVisibleStarColumnsInternal(DataGridColumn * ignoredColumn, double & perStarWidth)
{
	bool hasStarColumns = false;
	perStarWidth = 0.0;
	for (auto const& column : *this)
	{
		if (column.get() == ignoredColumn || !column->IsVisible())
			continue;

		GridLength width = column->Width;
		if (width.IsStar)
		{
			hasStarColumns = true;
			if (width.Value != 0.0 && column->DesiredWidth != 0.0)
			{
				perStarWidth = column->DesiredWidth / width.Value;
				break;
			}
		}
	}

	return hasStarColumns;
}

/// <summary>
///     Method which computes widths of columns on positive resize of a column
/// </summary>

void XUI::UI::DataGridColumnCollection::RecomputeColumnWidthsOnColumnPositiveResize(double horizontalChange, int resizingColumnIndex, bool retainAuto)
{
	double perStarWidth = 0.0;
	if (HasVisibleStarColumnsInternal(perStarWidth))
	{
		// reuse unused space
		horizontalChange = TakeAwayUnusedSpaceOnColumnPositiveResize(horizontalChange, resizingColumnIndex, retainAuto);

		// reducing columns to the right which are greater than the desired size
		horizontalChange = RecomputeNonStarColumnWidthsOnColumnPositiveResize(horizontalChange, resizingColumnIndex, retainAuto, true);

		// reducing star columns to right
		horizontalChange = RecomputeStarColumnWidthsOnColumnPositiveResize(horizontalChange, resizingColumnIndex, perStarWidth, retainAuto);

		// reducing columns to the right which are greater than the min size
		horizontalChange = RecomputeNonStarColumnWidthsOnColumnPositiveResize(horizontalChange, resizingColumnIndex, retainAuto, false);
	}
	else
	{
		DataGridColumn* column = ColumnFromDisplayIndex(resizingColumnIndex);
		SetResizedColumnWidth(column, horizontalChange, retainAuto);
	}
}

/// <summary>
///     Method which resizes the widths of star columns on positive resize of a column
/// </summary>

double XUI::UI::DataGridColumnCollection::RecomputeStarColumnWidthsOnColumnPositiveResize(double horizontalChange, int resizingColumnIndex, double perStarWidth, bool retainAuto)
{
	while (horizontalChange > 0.0)
	{
		double minPerStarExcessRatio = std::numeric_limits<double>::max();
		double rightStarFactors = GetStarFactorsForPositiveResize(resizingColumnIndex + 1, minPerStarExcessRatio);

		if (rightStarFactors > 0.0)
		{
			horizontalChange = ReallocateStarValuesForPositiveResize(
				resizingColumnIndex,
				horizontalChange,
				minPerStarExcessRatio,
				rightStarFactors,
				perStarWidth,
				retainAuto);

			if (horizontalChange == 0.0)
			{
				break;
			}
		}
		else
		{
			break;
		}
	}

	return horizontalChange;
}

bool XUI::UI::DataGridColumnCollection::CanColumnParticipateInResize(DataGridColumn * column)
{
	return column->IsVisible() && column->CanUserResize;
}

/// <summary>
///     Method which returns the total of star factors of the columns which could be resized on positive resize of a column
/// </summary>

double XUI::UI::DataGridColumnCollection::GetStarFactorsForPositiveResize(int startIndex, double & minPerStarExcessRatio)
{
	minPerStarExcessRatio = std::numeric_limits<double>::max();
	double rightStarFactors = 0.0;
	for (int i = startIndex, count = size(); i < count; i++)
	{
		DataGridColumn* column = ColumnFromDisplayIndex(i);
		if (!CanColumnParticipateInResize(column))
		{
			continue;
		}

		GridLength width = column->Width;
		if (width.IsStar && width.Value !=  0.0)
		{
			if (column->ActualWidth /* .DisplayValue */ >  column->MinWidth)
			{
				rightStarFactors += width.Value;
				double excessRatio = (column->ActualWidth /* .DisplayValue */ - column->MinWidth) / width.Value;
				if (excessRatio < minPerStarExcessRatio)
				{
					minPerStarExcessRatio = excessRatio;
				}
			}
		}
	}

	return rightStarFactors;
}

/// <summary>
///     Method which reallocated the star factors of star columns on 
///     positive resize of a column
/// </summary>

double XUI::UI::DataGridColumnCollection::ReallocateStarValuesForPositiveResize(int startIndex, double horizontalChange, double perStarExcessRatio, double totalStarFactors, double perStarWidth, bool retainAuto)
{
	double changePerStar = 0.0;
	double horizontalChangeForIteration = 0.0;
	if (horizontalChange < perStarExcessRatio * totalStarFactors)
	{
		changePerStar = horizontalChange / totalStarFactors;
		horizontalChangeForIteration = horizontalChange;
		horizontalChange = 0.0;
	}
	else
	{
		changePerStar = perStarExcessRatio;
		horizontalChangeForIteration = changePerStar * totalStarFactors;
		horizontalChange -= horizontalChangeForIteration;
	}
	
	for (int i = startIndex, count = size(); i < count; i++)
	{
		DataGridColumn* column = ColumnFromDisplayIndex(i);
		GridLength width = column->Width;
		if (i == startIndex)
		{
			SetResizedColumnWidth(column, horizontalChangeForIteration, retainAuto);
		}
		else if (column->Width.IsStar && CanColumnParticipateInResize(column) && column->ActualWidth /* .DisplayValue */ >  column->MinWidth)
		{
			double columnDesiredWidth = column->ActualWidth /* .DisplayValue */ - (width.Value * changePerStar);
			column->UpdateWidthForStarColumn(std::max(columnDesiredWidth, column->MinWidth), columnDesiredWidth, columnDesiredWidth / perStarWidth);
		}
	}

	return horizontalChange;
}

/// <summary>
///     Method which recomputes widths of non star columns on positive resize of a column
/// </summary>

double XUI::UI::DataGridColumnCollection::RecomputeNonStarColumnWidthsOnColumnPositiveResize(double horizontalChange, int resizingColumnIndex, bool retainAuto, bool onlyShrinkToDesiredWidth)
{
	if (horizontalChange >  0.0)
	{
		double totalExcessWidth = 0.0;
		bool iterationNeeded = true;
		for (int i = size() - 1; iterationNeeded && i > resizingColumnIndex; i--)
		{
			DataGridColumn* column = ColumnFromDisplayIndex(i);
			if (!CanColumnParticipateInResize(column))
			{
				continue;
			}
	
			GridLength width = column->Width;
			double minWidth = column->MinWidth;
			double columnExcessWidth = onlyShrinkToDesiredWidth ? column->ActualWidth /* .DisplayValue */ - std::max(column->DesiredWidth, column->MinWidth) : column->ActualWidth /* .DisplayValue */ - column->MinWidth;
	
			if (!width.IsStar &&
				columnExcessWidth >  0.0)
			{
				if (totalExcessWidth + columnExcessWidth >= horizontalChange)
				{
					columnExcessWidth = horizontalChange - totalExcessWidth;
					iterationNeeded = false;
				}
	
				column->SetWidthInternal(width, column->ActualWidth /* .DisplayValue */ - columnExcessWidth, column->DesiredWidth);
				totalExcessWidth += columnExcessWidth;
			}
		}
	
		if (totalExcessWidth >  0.0)
		{
			DataGridColumn* column = ColumnFromDisplayIndex(resizingColumnIndex);
			SetResizedColumnWidth(column, totalExcessWidth, retainAuto);
			horizontalChange -= totalExcessWidth;
		}
	}

	return horizontalChange;
}

/// <summary>
///     Method which recomputes the widths of columns on negative resize of a column
/// </summary>

void XUI::UI::DataGridColumnCollection::RecomputeColumnWidthsOnColumnNegativeResize(double horizontalChange, int resizingColumnIndex, bool retainAuto)
{
	double perStarWidth = 0.0;
	if (HasVisibleStarColumnsInternal(perStarWidth))
	{
		// increasing columns to the right which are less than the desired size
		horizontalChange = RecomputeNonStarColumnWidthsOnColumnNegativeResize(horizontalChange, resizingColumnIndex, retainAuto, false);
	
		// increasing star columns to the right
		horizontalChange = RecomputeStarColumnWidthsOnColumnNegativeResize(horizontalChange, resizingColumnIndex, perStarWidth, retainAuto);
	
		// increasing columns to the right which are less than the maximum size
		horizontalChange = RecomputeNonStarColumnWidthsOnColumnNegativeResize(horizontalChange, resizingColumnIndex, retainAuto, true);
	
		if (horizontalChange >  0.0)
		{
			DataGridColumn* resizingColumn = ColumnFromDisplayIndex(resizingColumnIndex);
			if (!resizingColumn->Width.IsStar)
			{
				SetResizedColumnWidth(resizingColumn, -horizontalChange, retainAuto);
			}
		}
	}
	else
	{
		DataGridColumn* column = ColumnFromDisplayIndex(resizingColumnIndex);
		SetResizedColumnWidth(column, -horizontalChange, retainAuto);
	}
}

/// <summary>
///     Method which recomputes widths of non star columns on negative resize of a column
/// </summary>

double XUI::UI::DataGridColumnCollection::RecomputeNonStarColumnWidthsOnColumnNegativeResize(double horizontalChange, int resizingColumnIndex, bool retainAuto, bool expandBeyondDesiredWidth)
{
	if (horizontalChange >  0.0)
	{
		double totalLagWidth = 0.0;
		bool iterationNeeded = true;
		for (int i = resizingColumnIndex + 1, count = size(); iterationNeeded && i < count; i++)
		{
			DataGridColumn* column = ColumnFromDisplayIndex(i);
			if (!CanColumnParticipateInResize(column))
			{
				continue;
			}
	
			GridLength width = column->Width;
			double maxColumnResizeWidth = expandBeyondDesiredWidth ? column->MaxWidth : std::min(column->DesiredWidth, column->MaxWidth);
	
			if (!width.IsStar &&
                column->ActualWidth /* .DisplayValue */ < maxColumnResizeWidth)
			{
				double columnLagWidth = maxColumnResizeWidth - column->ActualWidth /* .DisplayValue */;
				if (totalLagWidth + columnLagWidth >= horizontalChange)
				{
					columnLagWidth = horizontalChange - totalLagWidth;
					iterationNeeded = false;
				}
	
				column->SetWidthInternal(width, column->ActualWidth /* .DisplayValue */ + columnLagWidth, column->DesiredWidth);
				totalLagWidth += columnLagWidth;
			}
		}
	
		if (totalLagWidth >  0.0)
		{
			DataGridColumn* column = ColumnFromDisplayIndex(resizingColumnIndex);
			SetResizedColumnWidth(column, -totalLagWidth, retainAuto);
			horizontalChange -= totalLagWidth;
		}
	}

	return horizontalChange;
}

/// <summary>
///     Method which recomputes widths on star columns on negative resize of a column
/// </summary>

double XUI::UI::DataGridColumnCollection::RecomputeStarColumnWidthsOnColumnNegativeResize(double horizontalChange, int resizingColumnIndex, double perStarWidth, bool retainAuto)
{
	while (horizontalChange >  0.0)
	{
		double minPerStarLagRatio = std::numeric_limits<double>::max();
		double rightStarFactors = GetStarFactorsForNegativeResize(resizingColumnIndex + 1, minPerStarLagRatio);

		if (rightStarFactors >  0.0)
		{
			horizontalChange = ReallocateStarValuesForNegativeResize(
				resizingColumnIndex,
				horizontalChange,
				minPerStarLagRatio,
				rightStarFactors,
				perStarWidth,
				retainAuto);

			if (horizontalChange == 0.0)
			{
				break;
			}
		}
		else
		{
			break;
		}
	}

	return horizontalChange;
}

/// <summary>
///     Method which returns the total star factors of columns which resize of negative resize of a column
/// </summary>

double XUI::UI::DataGridColumnCollection::GetStarFactorsForNegativeResize(int startIndex, double & minPerStarLagRatio)
{
	minPerStarLagRatio = std::numeric_limits<double>::max();
	double rightStarFactors = 0.0;
	for (int i = startIndex, count = size(); i < count; i++)
	{
		DataGridColumn* column = ColumnFromDisplayIndex(i);
		if (!CanColumnParticipateInResize(column))
		{
			continue;
		}

		GridLength width = column->Width;
		if (width.IsStar && width.Value !=  0.0)
		{
			if (column->ActualWidth /* .DisplayValue */ < column->MaxWidth)
			{
				rightStarFactors += width.Value;
				double lagRatio = (column->MaxWidth - column->ActualWidth /* .DisplayValue */) / width.Value;
				if (lagRatio < minPerStarLagRatio)
				{
					minPerStarLagRatio = lagRatio;
				}
			}
		}
	}

	return rightStarFactors;
}

/// <summary>
///     Method which reallocates star factors of columns on negative resize of a column
/// </summary>

double XUI::UI::DataGridColumnCollection::ReallocateStarValuesForNegativeResize(int startIndex, double horizontalChange, double perStarLagRatio, double totalStarFactors, double perStarWidth, bool retainAuto)
{
	double changePerStar = 0.0;
	double horizontalChangeForIteration = 0.0;
	if (horizontalChange < perStarLagRatio * totalStarFactors)
	{
		changePerStar = horizontalChange / totalStarFactors;
		horizontalChangeForIteration = horizontalChange;
		horizontalChange = 0.0;
	}
	else
	{
		changePerStar = perStarLagRatio;
		horizontalChangeForIteration = changePerStar * totalStarFactors;
		horizontalChange -= horizontalChangeForIteration;
	}
	
	for (int i = startIndex, count = size(); i < count; i++)
	{
		DataGridColumn* column = ColumnFromDisplayIndex(i);
		GridLength width = column->Width;
		if (i == startIndex)
		{
			SetResizedColumnWidth(column, -horizontalChangeForIteration, retainAuto);
		}
		else if (column->Width.IsStar && CanColumnParticipateInResize(column) && column->ActualWidth /* .DisplayValue */ < column->MaxWidth)
		{
			double columnDesiredWidth = column->ActualWidth /* .DisplayValue */ + (width.Value * changePerStar);
			column->UpdateWidthForStarColumn(std::min(columnDesiredWidth, column->MaxWidth), columnDesiredWidth, columnDesiredWidth / perStarWidth);
		}
	}

	return horizontalChange;
}

/// <summary>
///     Helper method which sets the width of the column which is currently getting resized
/// </summary>

void XUI::UI::DataGridColumnCollection::SetResizedColumnWidth(DataGridColumn * column, double widthDelta, bool retainAuto)
{
	GridLength width = column->Width;
	double columnDisplayWidth = std::max(std::min(column->ActualWidth + widthDelta, column->MaxWidth), column->MinWidth);
	
	if (width.IsStar)
	{
		double starValue = column->DesiredWidth / width.Value;
		column->UpdateWidthForStarColumn(columnDisplayWidth, columnDisplayWidth, columnDisplayWidth / starValue);
	}
	else if (!width.IsAbsolute && retainAuto)
	{
		column->SetWidthInternal(width, columnDisplayWidth, column->DesiredWidth);
	}
	else
	{
        column->SetWidthInternal(GridLength(columnDisplayWidth, GridUnitType::Pixel), columnDisplayWidth, columnDisplayWidth);
	}
}

/// <summary>
///     Helper method which tries to take away width from unused space
/// </summary>

double XUI::UI::DataGridColumnCollection::TakeAwayWidthFromUnusedSpace(bool spaceAlreadyUtilized, double takeAwayWidth, double totalAvailableWidth)
{
	double usedSpace = 0.0;
	for (auto const& column : *this)
	{
		if (column->IsVisible())
		{
			usedSpace += column->ActualWidth; // DisplayValue;
		}
	}

	if (spaceAlreadyUtilized)
	{
		if (totalAvailableWidth >= usedSpace)
		{
			return 0.0;
		}
		else
		{
			return std::min(usedSpace - totalAvailableWidth, takeAwayWidth);
		}
	}
	else
	{
		double unusedSpace = totalAvailableWidth - usedSpace;
		if (unusedSpace > 0.0)
		{
			takeAwayWidth = std::max(0.0, takeAwayWidth - unusedSpace);
		}

		return takeAwayWidth;
	}
}

/// <summary>
///     Helper method which tries to take away width from unused space
/// </summary>

double XUI::UI::DataGridColumnCollection::TakeAwayWidthFromUnusedSpace(bool spaceAlreadyUtilized, double takeAwayWidth)
{
	double totalAvailableWidth = _dataGridOwner->GetViewportWidthForColumns();
	if (totalAvailableWidth >  0.0)
	{
		return TakeAwayWidthFromUnusedSpace(spaceAlreadyUtilized, takeAwayWidth, totalAvailableWidth);
	}

	return takeAwayWidth;
}

/// <summary>
///     Method which tries to take away the given amount of width from columns
///     except the ignored column
/// </summary>

double XUI::UI::DataGridColumnCollection::TakeAwayWidthFromColumns(DataGridColumn * ignoredColumn, double takeAwayWidth, bool widthAlreadyUtilized)
{
	double totalAvailableWidth = _dataGridOwner->GetViewportWidthForColumns();
	return TakeAwayWidthFromColumns(ignoredColumn, takeAwayWidth, widthAlreadyUtilized, totalAvailableWidth);
}

/// <summary>
///     Method which tries to take away the given amount of width from columns
///     except the ignored column
/// </summary>

double XUI::UI::DataGridColumnCollection::TakeAwayWidthFromColumns(DataGridColumn * ignoredColumn, double takeAwayWidth, bool widthAlreadyUtilized, double totalAvailableWidth)
{
	takeAwayWidth = TakeAwayWidthFromUnusedSpace(widthAlreadyUtilized, takeAwayWidth, totalAvailableWidth);

	takeAwayWidth = TakeAwayWidthFromStarColumns(ignoredColumn, takeAwayWidth);

	takeAwayWidth = TakeAwayWidthFromNonStarColumns(ignoredColumn, takeAwayWidth);
	return takeAwayWidth;
}

/// <summary>
///     Method which tries to take away the given amount of width form
///     the star columns
/// </summary>

double XUI::UI::DataGridColumnCollection::TakeAwayWidthFromStarColumns(DataGridColumn * ignoredColumn, double takeAwayWidth)
{
	if (takeAwayWidth > 0.0)
	{
		double sumOfStarDisplayWidths = 0.0;
		double sumOfStarMinWidths = 0.0;
		for (auto const& column : *this)
		{
			GridLength width = column->Width;
			if (width.IsStar && column->IsVisible())
			{
				if (column.get() == ignoredColumn)
				{
					sumOfStarDisplayWidths += takeAwayWidth;
				}

				sumOfStarDisplayWidths += column->ActualWidth; // .Value /* .DisplayValue */;
				sumOfStarMinWidths += column->MinWidth;
			}
		}

		double expectedStarSpace = sumOfStarDisplayWidths - takeAwayWidth;
		double usedStarSpace = ComputeStarColumnWidths(std::max(expectedStarSpace, sumOfStarMinWidths));
		takeAwayWidth = std::max(usedStarSpace - expectedStarSpace, 0.0);
	}

	return takeAwayWidth;
}

/// <summary>
///     Helper method which computes the widths of all the star columns
/// </summary>

double XUI::UI::DataGridColumnCollection::ComputeStarColumnWidths(double availableStarSpace)
{
	ASSERT(
		!std::isnan(availableStarSpace) && !std::isinf(availableStarSpace) && "availableStarSpace is not valid");

	std::vector<DataGridColumn*> unResolvedColumns;
	std::vector<DataGridColumn*> partialResolvedColumns;
	double totalFactors = 0.0;
	double totalMinWidths = 0.0;
	double totalMaxWidths = 0.0;
	double utilizedStarSpace = 0.0;

	// Accumulate all the star columns into unResolvedColumns in the beginning
	for (auto const& column : *this)
	{
		GridLength width = column->Width;
		if (column->IsVisible() && width.IsStar)
		{
			unResolvedColumns.push_back(column.get());
			totalFactors += width.Value;
			totalMinWidths += column->MinWidth;
			totalMaxWidths += column->MaxWidth;
		}
	}

	if (availableStarSpace < totalMinWidths)
	{
		availableStarSpace = totalMinWidths;
	}

	if (availableStarSpace > totalMaxWidths)
	{
		availableStarSpace = totalMaxWidths;
	}

	while (unResolvedColumns.size() > 0)
	{
		double starValue = availableStarSpace / totalFactors;

		// Find all the columns whose star share is less than thier min width and move such columns
		// into partialResolvedColumns giving them atleast the minwidth and there by reducing the availableSpace and totalFactors
		for (int i = 0, count = unResolvedColumns.size(); i < count; i++)
		{
			DataGridColumn* column = unResolvedColumns[i];
			GridLength width = column->Width;

			double columnMinWidth = column->MinWidth;
			double starColumnWidth = availableStarSpace * width.Value / totalFactors;

			if (columnMinWidth > starColumnWidth)
			{
				availableStarSpace = std::max(0.0, availableStarSpace - columnMinWidth);
				totalFactors -= width.Value;
				unResolvedColumns.erase(unResolvedColumns.begin() + i);
				i--;
				count--;
				partialResolvedColumns.push_back(column);
			}
		}

		// With the remaining space determine in any columns star share is more than maxwidth.
		// If such columns are found give them their max width and remove them from unResolvedColumns
		// there by reducing the availablespace and totalfactors. If such column is found, the remaining columns are to be recomputed
		bool iterationRequired = false;
		for (int i = 0, count = unResolvedColumns.size(); i < count; i++)
		{
			DataGridColumn* column = unResolvedColumns[i];
			GridLength width = column->Width;

			double columnMaxWidth = column->MaxWidth;
			double starColumnWidth = availableStarSpace * width.Value / totalFactors;

			if (columnMaxWidth < starColumnWidth)
			{
				iterationRequired = true;
				unResolvedColumns.erase(unResolvedColumns.begin() + i);
				availableStarSpace -= columnMaxWidth;
				utilizedStarSpace += columnMaxWidth;
				totalFactors -= width.Value;
				column->UpdateWidthForStarColumn(columnMaxWidth, starValue * width.Value, width.Value);
				break;
			}
		}

		// If it was determined by the previous step that another iteration is needed
		// then move all the partialResolvedColumns back to unResolvedColumns and there by
		// restoring availablespace and totalfactors.
		// If another iteration is not needed then allocate min widths to all columns in 
		// partial resolved columns and star share to all unresolved columns there by
		// ending the loop
		if (iterationRequired)
		{
			for (int i = 0, count = partialResolvedColumns.size(); i < count; i++)
			{
				DataGridColumn* column = partialResolvedColumns[i];

				unResolvedColumns.push_back(column);
				availableStarSpace += column->MinWidth;
				totalFactors += column->Width.Value;
			}

			partialResolvedColumns.clear();
		}
		else
		{
			for (int i = 0, count = partialResolvedColumns.size(); i < count; i++)
			{
				DataGridColumn* column = partialResolvedColumns[i];
				GridLength width = column->Width;
				double columnMinWidth = column->MinWidth;
				column->UpdateWidthForStarColumn(columnMinWidth, width.Value * starValue, width.Value);
				utilizedStarSpace += columnMinWidth;
			}

			partialResolvedColumns.clear();
			for (int i = 0, count = unResolvedColumns.size(); i < count; i++)
			{
				DataGridColumn* column = unResolvedColumns[i];
				GridLength width = column->Width;
				double starColumnWidth = availableStarSpace * width.Value / totalFactors;
				column->UpdateWidthForStarColumn(starColumnWidth, width.Value * starValue, width.Value);
				utilizedStarSpace += starColumnWidth;
			}

			unResolvedColumns.clear();
		}
	}

	return utilizedStarSpace;
}

/// <summary>
///     Helper method which takes away the given amount of width from
///     every non star column whose display value is greater than its minwidth
/// </summary>

void XUI::UI::DataGridColumnCollection::TakeAwayWidthFromEveryNonStarColumn(DataGridColumn * ignoredColumn, double perColumnTakeAwayWidth)
{
	for (auto const& column : *this)
	{
		if (ignoredColumn == column.get() ||
			!column->IsVisible())
		{
			continue;
		}
	
		GridLength width = column->Width;
		if (width.IsStar)
		{
			continue;
		}
	
		if (column->ActualWidth > column->MinWidth)
		{
			column->SetWidthInternal(width, column->ActualWidth - perColumnTakeAwayWidth, column->DesiredWidth); 
		}
	}
}

/// <summary>
///     Method which tries to get the unused column space when another column tries to positive resize
/// </summary>


/// <summary>
///     Method which tries to take away the given amount of width
///     among all non star columns except the ignored column
/// </summary>
double XUI::UI::DataGridColumnCollection::TakeAwayWidthFromNonStarColumns(DataGridColumn * ignoredColumn, double takeAwayWidth)
{
	while (takeAwayWidth >  0.0)
	{
		int countOfParticipatingColumns = 0;
		double minExcessWidth = FindMinimumExcessWidthOfNonStarColumns(
			ignoredColumn,
			countOfParticipatingColumns);

		if (countOfParticipatingColumns == 0)
		{
			break;
		}

		double minTotalExcessWidth = minExcessWidth * countOfParticipatingColumns;
		if (minTotalExcessWidth >= takeAwayWidth)
		{
			minExcessWidth = takeAwayWidth / countOfParticipatingColumns;
			takeAwayWidth = 0.0;
		}
		else
		{
			takeAwayWidth -= minTotalExcessWidth;
		}

		TakeAwayWidthFromEveryNonStarColumn(ignoredColumn, minExcessWidth);
	}

	return takeAwayWidth;
}

/// <summary>
///     Helper method which finds the minimum non-zero difference between displayvalue and minwidth
///     among all non star columns
/// </summary>
double XUI::UI::DataGridColumnCollection::FindMinimumExcessWidthOfNonStarColumns(DataGridColumn * ignoredColumn, int& countOfParticipatingColumns)
{
	double minExcessWidth = std::numeric_limits<double>::max();
	countOfParticipatingColumns = 0;
	for(auto const& column : *this)
	{
		if (ignoredColumn == column.get() ||
			!column->IsVisible())
		{
			continue;
		}

		GridLength width = column->Width;
		if (width.IsStar)
		{
			continue;
		}

		double minWidth = column->MinWidth;
		if (column->ActualWidth > minWidth)
		{
			countOfParticipatingColumns++;
			double excessWidth = column->ActualWidth - minWidth;
			if (excessWidth < minExcessWidth)
			{
				minExcessWidth = excessWidth;
			}
		}
	}

	return minExcessWidth;
}
