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

#ifndef __XUI_UI_DATACOLUMN_COLLECTION_H__
#define __XUI_UI_DATACOLUMN_COLLECTION_H__

#include "UI/DataGrid/DataGridColumn.h"
#include "UI/DataGrid/DataGridDefines.h"

//#include <optional>

namespace XUI::UI
{
    class DataGrid;
    class DataGridRow;

    struct RealizedColumnsBlock
    {
        int StartIndex;
        int EndIndex;
        int StartIndexOffset;
    };

    class DataGridColumnCollection: public System::Collection::Vector<std::shared_ptr<DataGridColumn>>
    {
        friend class DataGrid;

    private:

        DataGrid* _dataGridOwner = nullptr;
        bool _isUpdatingDisplayIndex = false;               // true if we're in the middle of updating the display index of each column.
        std::vector<int> _displayIndexMap;          // maps a DisplayIndex to an index in the _columns collection.
        bool _displayIndexMapInitialized = false;           // Flag is used to delay the validation of DisplayIndex until the first measure
        bool _isClearingDisplayIndex = false;               // Flag indicating that we're currently clearing the display index.  We should not coerce default display index's during this time. 
        bool _columnWidthsComputationPending = false;       // Flag indicating whether the columns width computaion operation is pending
        bool _hasVisibleStarColumns = false;
        std::map<DataGridColumn*, GridLength> _originalWidthsForResize; // Dictionary to hold the original widths of columns for resize operation
		//Meta(Disable) std::optional<double> _averageColumnWidth;       // average width of all visible columns
        std::vector<RealizedColumnsBlock> _realizedColumnsBlockListForNonVirtualizedRows; // Realized columns for non-virtualized rows
        std::vector<RealizedColumnsBlock> _realizedColumnsBlockListForVirtualizedRows; // Realized columns for virtualized rows

    public:

        DataGridColumnCollection(DataGrid* owner);

        virtual ~DataGridColumnCollection();

        void OnColumnsChanged(void * sender, System::Collection::NotifyCollectionChangedEventArgs & e);

        std::vector<int>& DisplayIndexMap()
        {
            if (!_displayIndexMapInitialized)
                InitializeDisplayIndexMap();

            return _displayIndexMap;
        }

        bool RebuildRealizedColumnsBlockListForVirtualizedRows = false;
        bool RebuildRealizedColumnsBlockListForNonVirtualizedRows = false;
        bool RefreshAutoWidthColumns = false;

        /// <summary>
        ///     Returns the DataGridColumn with the given DisplayIndex
        /// </summary>
        DataGridColumn* ColumnFromDisplayIndex(int displayIndex)
        {
            EXCEPT(displayIndex >= 0 && displayIndex < _displayIndexMap.size() && "displayIndex should have already been validated");
            return (*this)[_displayIndexMap[displayIndex]].get();
        }


        // This method is called in first DataGrid measure call
        // It needs to populate DisplayIndexMap and validate the DisplayIndex of all columns
        void InitializeDisplayIndexMap()
        {
            int resultIndex = -1;
            InitializeDisplayIndexMap(nullptr, -1, resultIndex);
        }

        /// <summary>
        ///     Throws an ArgumentOutOfRangeException if the given displayIndex is invalid for the given column.
        /// </summary>
        void ValidateDisplayIndex(DataGridColumn* column, int displayIndex)
        {
            ValidateDisplayIndex(column, displayIndex, false);
        }

        /// <summary>
        ///     Throws an ArgumentOutOfRangeException if the given displayIndex is invalid for the given column.
        /// </summary>
        void ValidateDisplayIndex(DataGridColumn* column, int displayIndex, bool isAdding)
        {
            if (!IsDisplayIndexValid(column, displayIndex, isAdding))
            {
                throw System::ArgumentOutOfRangeException("displayIndex"/*, displayIndex, SR.Get(SRID.DataGrid_ColumnDisplayIndexOutOfRange, column.Header)*/);
            }
        }

		/// <summary>
		///     Method which is called when user resize of column starts
		/// </summary>
		void OnColumnResizeStarted()
		{
			for(auto const& column : *this)
			{
				_originalWidthsForResize[column.get()] = column->Width;
			}
		}

        /// <summary>
        ///     Method which is called when user resize of column ends
        /// </summary>
        void OnColumnResizeCompleted(bool cancel)
        {
            if (cancel && !_originalWidthsForResize.empty())
            {
                for(auto const& column : *this)
                {
                    if (_originalWidthsForResize.find(column.get()) != _originalWidthsForResize.end())
                    {
                        column->Width = _originalWidthsForResize[column.get()];
                    }
                }
            }

            _originalWidthsForResize.clear();
        }

        void InvalidateHasVisibleStarColumns()
        {
            _hasVisibleStarColumns = HasVisibleStarColumnsInternal(nullptr);
        }

		/// <summary>
		///     Method which recomputes the widths of columns on resize of column
		/// </summary>
		void RecomputeColumnWidthsOnColumnResize(DataGridColumn* resizingColumn, double horizontalChange, bool retainAuto);

		/// <summary>
		///     Method which computes the widths of the columns
		/// </summary>
		void ComputeColumnWidths();

        void RedistributeColumnWidthsOnWidthChangeOfColumn(DataGridColumn* changedColumn, double oldWidth);

        /// <summary>
        ///     Method which redistributes the column widths based on change in MinWidth of a column
        /// </summary>
        void RedistributeColumnWidthsOnMinWidthChangeOfColumn(DataGridColumn* changedColumn, double oldMinWidth);

        /// <summary>
        ///     Method which redistributes the column widths based on change in MaxWidth of a column
        /// </summary>
        void RedistributeColumnWidthsOnMaxWidthChangeOfColumn(DataGridColumn* changedColumn, double oldMaxWidth);

        void NotifyPropertyChanged(Core::Dependency::DependencyObject * d, std::string const & propertyName, Core::Dependency::DependencyPropertyChangedEventArgs & e, NotificationTarget target);

        /// <summary>
        ///     Called when properties which affect the realized columns namely
        ///     Column Width, FrozenColumnCount, DisplayIndex etc. are changed.
        /// </summary>
        void InvalidateColumnRealization(bool invalidateForNonVirtualizedRows)
        {
            RebuildRealizedColumnsBlockListForVirtualizedRows = true;
            if (invalidateForNonVirtualizedRows)
            {
                RebuildRealizedColumnsBlockListForNonVirtualizedRows = true;
            }
        }

        /// <summary>
        ///     Method which redistributes the width of star columns among them selves
        /// </summary>
        void RecomputeStarColumnWidths();

        /// <summary>
        ///     Helper method which determines the average with of all the columns
        /// </summary>
        double ComputeAverageColumnWidth();
    private:

        /// <summary>
        ///     Method which handles the column widths computation for CellsPanelHorizontalOffset change
        /// </summary>
        void OnCellsPanelHorizontalOffsetChanged(Core::Dependency::DependencyPropertyChangedEventArgs& e);

        /// <summary>
        ///     Method which redistributes the column widths based on change in available space of a column
        /// </summary>
        void RedistributeColumnWidthsOnAvailableSpaceChange(double availableSpaceChange, double newTotalAvailableSpace);

        /// <summary>
        ///     Method which redistributes widths of columns on change of a column's width
        ///     when datagrid itself has star columns, but neither the oldwidth or the newwidth
        ///     of changed column is star.
        /// </summary>
        void RedistributeColumnWidthsOnNonStarWidthChange(DataGridColumn* changedColumn, double oldWidth);

        /// <summary>
        ///     Method which tries to give away the given amount of width 
        ///     among all the columns except the ignored column
        /// </summary>
        /// <param name="ignoredColumn">The column which is giving away the width</param>
        /// <param name="giveAwayWidth">The amount of giveaway width</param>
        double GiveAwayWidthToColumns(DataGridColumn* ignoredColumn, double giveAwayWidth, bool recomputeStars);

        /// <summary>
        ///     Helper method which gives away width to scroll viewer
        ///     if its extent width is greater than viewport width
        /// </summary>
        double GiveAwayWidthToScrollViewerExcess(double giveAwayWidth, bool includedInColumnsWidth);

        /// <summary>
        ///     Method which tries to give away the given amount of width
        ///     among all non star columns except the ignored column
        /// </summary>
        double GiveAwayWidthToNonStarColumns(DataGridColumn* ignoredColumn, double giveAwayWidth);

        /// <summary>
        ///     Helper method which finds the minimum non-zero difference between displayvalue and desiredvalue
        ///     among all non star columns
        /// </summary>
        double FindMinimumLaggingWidthOfNonStarColumns(
            DataGridColumn* ignoredColumn,
            int& countOfParticipatingColumns);

        /// <summary>
        ///     Helper method which gives away the given amount of width to
        ///     every non star column whose display value is less than its desired value
        /// </summary>
        void GiveAwayWidthToEveryNonStarColumn(DataGridColumn* ignoredColumn, double perColumnGiveAwayWidth);

		/// <summary>
		///     Method which initializes the column width's diplay value to its desired value
		/// </summary>
		void InitializeColumnDisplayValues();

		/// <summary>
		///     Method which distributes a given amount of width among all the columns
		/// </summary>
		void DistributeSpaceAmongColumns(double availableSpace);

		/// <summary>
		///     Method which expands the display values of widths of all columns to
		///     their desired values. Usually used when the last star column's width
		///     is changed to non-star
		/// </summary>
		void ExpandAllColumnWidthsToDesiredValue();

		/// <summary>
		///     Helper method which distributes a given amount of width among all non star columns
		/// </summary>
		double DistributeSpaceAmongNonStarColumns(double availableSpace);

        void InitializeDisplayIndexMap(DataGridColumn* changingColumn, int oldDisplayIndex, int& resultDisplayIndex);

        /// <summary>
        ///     Updates the display index for all columns affected by the removal of a set of columns.  
        /// </summary>
        void UpdateDisplayIndexForRemovedColumns(DataGridColumn* column, int startingIndex);

        /// <summary>
        ///     Updates the display index for the column that was just replaced and adjusts the other columns if necessary
        /// </summary>
        void UpdateDisplayIndexForReplacedColumn(DataGridColumn* oldColumn, DataGridColumn* newColumn);

        /// <summary>
        ///     Clears the DisplayIndexProperty on each of the columns.
        /// </summary>
        void ClearDisplayIndex(DataGridColumn* oldColumns, DataGridColumn* newColumns);

        /// <summary>
        ///     Returns true if the display index is valid for the given column
        /// </summary>
        bool IsDisplayIndexValid(DataGridColumn* column, int displayIndex, bool isAdding);

        /// <summary>
        ///     Inserts the given columnIndex in the DisplayIndexMap at the given display index.
        /// </summary>
        void InsertInDisplayIndexMap(int newDisplayIndex, int columnIndex);

        /// <summary>
        ///     Removes the given column index from the DisplayIndexMap
        /// </summary>
        int RemoveFromDisplayIndexMap(int columnIndex);

        int CoerceDefaultDisplayIndex(DataGridColumn* column);

        /// <summary>
        ///     This takes a column and checks that if its DisplayIndex is the default value.  If so, it coerces
        ///     the DisplayIndex to be its location in the columns collection.
        ///     We can't do this in CoerceValue because the callback isn't called for default values.  Instead we call this
        ///     whenever a column is added or replaced in the collection or when the DisplayIndex of an existing column has changed.
        /// </summary>
        /// <param name="column">The column</param>
        /// <param name="newDisplayIndex">The DisplayIndex the column should have</param>
        /// <returns>The DisplayIndex of the column</returns>
        int CoerceDefaultDisplayIndex(DataGridColumn* column, int newDisplayIndex);

        /// <summary>
        ///     Called when a column's display index has changed.  
        /// <param name="oldDisplayIndex">the old display index of the column</param>
        /// <param name="newDisplayIndex">the new display index of the column</param>
        void OnColumnDisplayIndexChanged(DataGridColumn* column, int oldDisplayIndex, int newDisplayIndex);

        void Debug_VerifyDisplayIndexMap();

        /// <summary>
        ///     Called when the DisplayIndex for a single column has changed.  The other columns may have conflicting display indices, so
        ///     we walk through them and adjust.  This method does nothing if we're already updating display index as part of a larger
        ///     operation (such as add or remove).  This is both for re-entrancy and to avoid modifying the display index map as we walk over
        ///     the columns.
        /// </summary>
        void UpdateDisplayIndexForChangedColumn(int oldDisplayIndex, int newDisplayIndex);

        void UpdateDisplayIndexForMovedColumn(int oldColumnIndex, int newColumnIndex);

        /// <summary>
        ///     Sets the DisplayIndex on all newly inserted or added columns and updates the existing columns as necessary.  
        /// </summary>
        void UpdateDisplayIndexForNewColumns(DataGridColumn* column, int startingIndex);

        /// <summary>
        ///     Method which sets / resets the IsFrozen property of columns based on DataGrid's FrozenColumnCount.
        ///     It is possible that the FrozenColumnCount change could be a result of column count itself, in
        ///     which case only the columns which are in the collection at the moment are to be considered.
        /// </summary>
        /// <param name="oldFrozenCount"></param>
        /// <param name="newFrozenCount"></param>
        void OnDataGridFrozenColumnCountChanged(int oldFrozenCount, int newFrozenCount);

		/// <summary>
		///     Method which determines if there are any
		///     star columns in datagrid and also returns perStarWidth
		/// </summary>
		bool HasVisibleStarColumnsInternal(double& perStarWidth)
		{
			return HasVisibleStarColumnsInternal(nullptr, perStarWidth);
		}

		/// <summary>
		///     Method which determines if there are any
		///     star columns in datagrid except the given column
		/// </summary>
		bool HasVisibleStarColumnsInternal(DataGridColumn* ignoredColumn)
		{
			double perStarWidth;
			return HasVisibleStarColumnsInternal(ignoredColumn, perStarWidth);
		}

		/// <summary>
		///     Method which determines if there are any
		///     star columns in datagrid except the given column and also returns perStarWidth
		/// </summary>
		bool HasVisibleStarColumnsInternal(DataGridColumn* ignoredColumn, double& perStarWidth);

		/// <summary>
		///     Method which computes widths of columns on positive resize of a column
		/// </summary>
		void RecomputeColumnWidthsOnColumnPositiveResize(
			double horizontalChange,
			int resizingColumnIndex,
			bool retainAuto);

#pragma region Column Resizing Helper

		/// <summary>
		///     Method which resizes the widths of star columns on positive resize of a column
		/// </summary>
		double RecomputeStarColumnWidthsOnColumnPositiveResize(
			double horizontalChange,
			int resizingColumnIndex,
			double perStarWidth,
			bool retainAuto);

		static bool CanColumnParticipateInResize(DataGridColumn* column);

		/// <summary>
		///     Method which returns the total of star factors of the columns which could be resized on positive resize of a column
		/// </summary>
		double GetStarFactorsForPositiveResize(int startIndex, double& minPerStarExcessRatio);

		/// <summary>
		///     Method which reallocated the star factors of star columns on 
		///     positive resize of a column
		/// </summary>
		double ReallocateStarValuesForPositiveResize(
			int startIndex,
			double horizontalChange,
			double perStarExcessRatio,
			double totalStarFactors,
			double perStarWidth,
			bool retainAuto);

		/// <summary>
		///     Method which recomputes widths of non star columns on positive resize of a column
		/// </summary>
		double RecomputeNonStarColumnWidthsOnColumnPositiveResize(
			double horizontalChange,
			int resizingColumnIndex,
			bool retainAuto,
			bool onlyShrinkToDesiredWidth);

		/// <summary>
		///     Method which recomputes the widths of columns on negative resize of a column
		/// </summary>
		void RecomputeColumnWidthsOnColumnNegativeResize(
			double horizontalChange,
			int resizingColumnIndex,
			bool retainAuto);

		/// <summary>
		///     Method which recomputes widths of non star columns on negative resize of a column
		/// </summary>
		double RecomputeNonStarColumnWidthsOnColumnNegativeResize(
			double horizontalChange,
			int resizingColumnIndex,
			bool retainAuto,
			bool expandBeyondDesiredWidth);

		/// <summary>
		///     Method which recomputes widths on star columns on negative resize of a column
		/// </summary>
		double RecomputeStarColumnWidthsOnColumnNegativeResize(
			double horizontalChange,
			int resizingColumnIndex,
			double perStarWidth,
			bool retainAuto);

		/// <summary>
		///     Method which returns the total star factors of columns which resize of negative resize of a column
		/// </summary>
		double GetStarFactorsForNegativeResize(int startIndex, double& minPerStarLagRatio);

		/// <summary>
		///     Method which reallocates star factors of columns on negative resize of a column
		/// </summary>
		double ReallocateStarValuesForNegativeResize(
			int startIndex,
			double horizontalChange,
			double perStarLagRatio,
			double totalStarFactors,
			double perStarWidth,
			bool retainAuto);

		/// <summary>
		///     Helper method which sets the width of the column which is currently getting resized
		/// </summary>
		static void SetResizedColumnWidth(DataGridColumn* column, double widthDelta, bool retainAuto);

#pragma endregion

#pragma region Width Take Away Methods

		/// <summary>
		///     Method which tries to get the unused column space when another column tries to positive resize
		/// </summary>
		double TakeAwayUnusedSpaceOnColumnPositiveResize(double horizontalChange, int resizingColumnIndex, bool retainAuto)
		{
			double spaceNeeded = TakeAwayWidthFromUnusedSpace(false, horizontalChange);
			if (spaceNeeded < horizontalChange)
			{
				DataGridColumn* resizingColumn = ColumnFromDisplayIndex(resizingColumnIndex);
				SetResizedColumnWidth(resizingColumn, horizontalChange - spaceNeeded, retainAuto);
			}

			return spaceNeeded;
		}

		/// <summary>
		///     Helper method which tries to take away width from unused space
		/// </summary>
		double TakeAwayWidthFromUnusedSpace(bool spaceAlreadyUtilized, double takeAwayWidth, double totalAvailableWidth);

		/// <summary>
		///     Helper method which tries to take away width from unused space
		/// </summary>
		double TakeAwayWidthFromUnusedSpace(bool spaceAlreadyUtilized, double takeAwayWidth);

		/// <summary>
		///     Method which tries to take away the given amount of width from columns
		///     except the ignored column
		/// </summary>
		double TakeAwayWidthFromColumns(DataGridColumn* ignoredColumn, double takeAwayWidth, bool widthAlreadyUtilized);

		/// <summary>
		///     Method which tries to take away the given amount of width from columns
		///     except the ignored column
		/// </summary>
		double TakeAwayWidthFromColumns(DataGridColumn* ignoredColumn, double takeAwayWidth, bool widthAlreadyUtilized, double totalAvailableWidth);

		/// <summary>
		///     Method which tries to take away the given amount of width form
		///     the star columns
		/// </summary>
		double TakeAwayWidthFromStarColumns(DataGridColumn* ignoredColumn, double takeAwayWidth);

		/// <summary>
		///     Helper method which computes the widths of all the star columns
		/// </summary>
		double ComputeStarColumnWidths(double availableStarSpace);

		/// <summary>
		///     Method which tries to take away the given amount of width
		///     among all non star columns except the ignored column
		/// </summary>
		double TakeAwayWidthFromNonStarColumns(DataGridColumn* ignoredColumn, double takeAwayWidth);

		/// <summary>
		///     Helper method which finds the minimum non-zero difference between displayvalue and minwidth
		///     among all non star columns
		/// </summary>
		double FindMinimumExcessWidthOfNonStarColumns(
			DataGridColumn* ignoredColumn,
			int& countOfParticipatingColumns);

		/// <summary>
		///     Helper method which takes away the given amount of width from
		///     every non star column whose display value is greater than its minwidth
		/// </summary>
		void TakeAwayWidthFromEveryNonStarColumn(
			DataGridColumn* ignoredColumn,
			double perColumnTakeAwayWidth);

#pragma endregion
    };
}

#endif //! __XUI_UI_DATACOLUMN_COLLECTION_H__