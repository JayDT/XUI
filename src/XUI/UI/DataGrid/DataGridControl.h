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

#ifndef __XUI_UI_DATAGRID_H__
#define __XUI_UI_DATAGRID_H__

#include "UI/DataGrid/DataGridDefines.h"
#include "UI/DataGrid/DataGridEventArgs.h"
#include "UI/Detail/SelectingItemsControl.h"
#include "UI/DataGrid/DataGridColumnCollection.h"
#include "UI/DataGrid/DataGridCell.h"
#include "UI/Detail/GridLength.h"
#include "Interfaces/ICommand.h"
#include "Core/Input/KeyGesture.h"
#include "Core/DispatcherTimer.h"
#include "Core/Observer/RoutedCommand.h"
#include "standard/LinkedReference/RefManager.h"

//#include <optional>

namespace XUI::UI
{
    class DataGridColumnHeadersPresenter;
    class ScrollViewer;
    class DataGridColumnHeader;
	class ChangingSelectedCellsHelper;
    struct DataGridCellInfo;

    namespace Presenters
    {
        class ScrollContentPresenter;
    }

    class DataGrid : public SelectingItemsControl
    {
		friend class ChangingSelectedCellsHelper;

    private:


#pragma region Data

        static System::Reflection::Variant _newItemPlaceholder;                                                // Used as an alternate data item

        GridLength _columWidth;
        rxcpp::composite_subscription _scrollViewBindingGroup;
        DataGridColumnCollection _columns;                                                              // Stores the columns
        System::Collection::RefManager<DataGridRow, DataGrid> _rowTrackingRoot;                         // Root of a linked list of active row containers
        Meta(Disable) std::shared_ptr<DataGridColumnHeadersPresenter> _columnHeadersPresenter;          // headers presenter for sending down notifications
        System::Reflection::Variant _addedRowItem;                                                      // Current new row item
        System::Reflection::Variant _editedRowItem;                                                     // Last edited row item
        System::Reflection::Variant _editingRowItem;                                                    // Current editing row item
        std::shared_ptr<DataGridCell> _currentCellContainer;                                            // Reference to the cell container corresponding to CurrentCell (use CurrentCellContainer property instead)
        std::shared_ptr<DataGridCell> _editingCellContainer;                                           
        std::unordered_set<DataGridCell*> _selectedCells;                                               // Stores the selected cells
        std::shared_ptr<DataGridCell> _selectionAnchor;                                                 // For doing extended selection
        std::shared_ptr<DataGridRow> _placeHolderRow;                                                 // For doing extended selection
        std::shared_ptr<Panel> _internalItemsHost;                                                      // Workaround for not having access to ItemsHost
		Meta(Disable) std::shared_ptr<ScrollViewer> _internalScrollHost;                                // Scroll viewer of the datagrid
		Meta(Disable) std::shared_ptr<Presenters::ScrollContentPresenter> _internalScrollContentPresenter;// Scroll Content Presenter of DataGrid's ScrollViewer
        std::shared_ptr<DataGridCell> _focusedCell;                                                     // Holds the cell which has logical focus.
		Meta(Disable) Core::DispatcherTimer _autoScrollTimer;                                           // Timer to tick auto-scroll
        Visibility _placeholderVisibility = Visibility::Collapsed;                                      // The visibility used for the Placeholder container.  It may not exist at all times, so it's stored on the DG.
        Core::Media::Point _dragPoint;                                                                  // Used to detect if a drag actually occurred
        std::list<int> _groupingSortDescriptionIndices;                                                 // List to hold the indices of SortDescriptions added for the sake of GroupDescriptions.
        double _originalViewportWidth = 0.0;                                                            // Holds the original viewport width between multiple viewport width changes
        double _finalViewportWidth = 0.0;                                                               // Holds the final viewport width between multiple viewport width changes
        NewItemPlaceholderPosition _newItemPlaceholderPosition = NewItemPlaceholderPosition::AtEnd;
        int _editingRowIndex = -1;                                                                      // Current editing row index
        bool _dataGridMeasureInitialized = false;
        bool _dataGridColumnsInvalidated = false;
		//bool _cellsPanelHorizontalOffsetComputationPending = false;
        bool _isDraggingSelection = false;                                                              // Whether a drag select is being performed
        bool _isRowDragging = false;                                                                    // Whether a drag select is being done on rows
        bool _deferAutoGeneration = false;                                                              // The flag which determines whether the columns generation is deferred
        bool _measureNeverInvoked = true;                                                               // Flag used to track if measure was invoked atleast once. Particularly used for AutoGeneration.
        bool _updatingSelectedCells = false;                                                            // Whether to defer notifying that SelectedCells changed.
        bool _ignoreSortDescriptionsChange = false;                                                     // Flag used to neglect the SortDescriptionCollection changes in the CollectionChanged listener.
        bool _sortingStarted = false;                                                                   // Flag used to track if Sorting ever started or not.
        bool _hasCellValidationError = false;                                                           // An unsuccessful cell commit occurred
        bool _hasRowValidationError = false;                                                            // An unsuccessful row commit occurred
        bool _viewportWidthChangeNotificationPending = false;                                           // Flag to indicate if a viewport width change reuest is already queued.
		bool _autoCreateColumns = true;
        bool _hasAutoScrolled = false;                                                                  // Whether an auto-scroll has occurred since starting the tick

#pragma endregion

    public:

        static Core::Dependency::RefDependencyProperty CanUserResizeColumnsProperty;
        static Core::Dependency::RefDependencyProperty ColumnWidthProperty;
        static Core::Dependency::RefDependencyProperty MinColumnWidthProperty;
        static Core::Dependency::RefDependencyProperty MaxColumnWidthProperty;
        static Core::Dependency::RefDependencyProperty GridLinesVisibilityProperty;
        static Core::Dependency::RefDependencyProperty HorizontalGridLinesBrushProperty;
        static Core::Dependency::RefDependencyProperty VerticalGridLinesBrushProperty;
        static Core::Dependency::RefDependencyProperty HorizontalGridLineThicknessProperty;
        static Core::Dependency::RefDependencyProperty VerticalGridLineThicknessProperty;
        static Core::Dependency::RefDependencyProperty RowStyleProperty;
        static Core::Dependency::RefDependencyProperty RowValidationErrorTemplateProperty;
        static Core::Dependency::RefDependencyProperty RowStyleSelectorProperty;
        static Core::Dependency::RefDependencyProperty RowBackgroundProperty;
        static Core::Dependency::RefDependencyProperty AlternatingRowBackgroundProperty;
        static Core::Dependency::RefDependencyProperty RowHeightProperty;
        static Core::Dependency::RefDependencyProperty MinRowHeightProperty;
        static Core::Dependency::RefDependencyProperty RowHeaderWidthProperty;
        static Core::Dependency::RefDependencyProperty RowHeaderActualWidthProperty;
        static Core::Dependency::RefDependencyProperty ColumnHeaderHeightProperty;
        static Core::Dependency::RefDependencyProperty HeadersVisibilityProperty;
        static Core::Dependency::RefDependencyProperty CellStyleProperty; 
        static Core::Dependency::RefDependencyProperty ColumnHeaderStyleProperty;
        static Core::Dependency::RefDependencyProperty RowHeaderStyleProperty;
        static Core::Dependency::RefDependencyProperty RowHeaderTemplateProperty;
        static Core::Dependency::RefDependencyProperty RowHeaderTemplateSelectorProperty;
        static Core::Dependency::RefDependencyProperty HorizontalScrollBarVisibilityProperty;
        static Core::Dependency::RefDependencyProperty VerticalScrollBarVisibilityProperty;
        static Core::Dependency::RefDependencyProperty IsReadOnlyProperty;
        static Core::Dependency::RefDependencyProperty CurrentItemProperty;
        static Core::Dependency::RefDependencyProperty CurrentColumnProperty;
        static Core::Dependency::RefDependencyProperty CurrentCellProperty;
        static Core::Dependency::RefDependencyProperty CanUserAddRowsProperty;
        static Core::Dependency::RefDependencyProperty CanUserDeleteRowsProperty;
        static Core::Dependency::RefDependencyProperty RowDetailsVisibilityModeProperty;
        static Core::Dependency::RefDependencyProperty AreRowDetailsFrozenProperty;
        static Core::Dependency::RefDependencyProperty RowDetailsTemplateProperty;
        static Core::Dependency::RefDependencyProperty RowDetailsTemplateSelectorProperty;
        static Core::Dependency::RefDependencyProperty CanUserResizeRowsProperty;
        static Core::Dependency::RefDependencyProperty SelectionModeProperty;
        static Core::Dependency::RefDependencyProperty SelectionUnitProperty;
        static Core::Dependency::RefDependencyProperty CanUserSortColumnsProperty;
        static Core::Dependency::RefDependencyProperty AutoGenerateColumnsProperty;
        static Core::Dependency::RefDependencyProperty FrozenColumnCountProperty;
        static Core::Dependency::RefDependencyProperty NonFrozenColumnsViewportHorizontalOffsetProperty;
        static Core::Dependency::RefDependencyProperty EnableRowVirtualizationProperty;
        static Core::Dependency::RefDependencyProperty EnableColumnVirtualizationProperty;
        static Core::Dependency::RefDependencyProperty CanUserReorderColumnsProperty;
        static Core::Dependency::RefDependencyProperty DragIndicatorStyleProperty;
        static Core::Dependency::RefDependencyProperty DropLocationIndicatorStyleProperty;
        static Core::Dependency::RefDependencyProperty ClipboardCopyModeProperty;
        static Core::Dependency::RefDependencyProperty CellsPanelHorizontalOffsetProperty;
        static Core::Dependency::RefDependencyProperty HorizontalScrollOffsetProperty;

        static Core::Observer::RefRoutedCommand BeginEditCommand;
        static Core::Observer::RefRoutedCommand CommitEditCommand;
        static Core::Observer::RefRoutedCommand CancelEditCommand;
        static Core::Observer::RefRoutedCommand DeleteCommand;
        static Core::Observer::RefRoutedCommand SelectAllCommand;

        static void StaticClassInitializer();

        /// <summary>
        ///     Event that is fired when the DisplayIndex on one of the DataGrid's Columns changes.
        /// </summary>
        System::Events::EventHandler<DataGridSortingEventArgs> Sorting;
        System::Events::EventHandler<DataGridColumnEventArgs> ColumnDisplayIndexChanged;
		System::Events::EventHandler<DataGridPreparingCellForEditEventArgs> PreparingCellForEdit;
        System::Events::EventHandler<System::Collection::NotifyCollectionChangedEventArgs> ColumnChanged;

        /// <summary>
        ///     Called just before a cell will change to edit mode
        ///     to allow handlers to prevent the cell from entering edit mode.
        /// </summary>
        System::Events::EventHandler<DataGridBeginningEditEventArgs> BeginningEdit;
        System::Events::EventHandler<DataGridCellEditEndingEventArgs> CellEditEnding;
        System::Events::EventHandler<DataGridRowEditEndingEventArgs> RowEditEnding;

        System::Collection::RefManager<DataGridRow, DataGrid>& GetRows() { return _rowTrackingRoot; }

		/// <summary>
		///     Event that is fired just before the details of a Row is shown
		/// </summary>
		System::Events::EventHandler<DataGridRowDetailsEventArgs> LoadingRowDetails;
		/// <summary>
		///     Event that is fired just before the details of a Row is hidden
		/// </summary>
		System::Events::EventHandler<DataGridRowDetailsEventArgs> UnloadingRowDetails;
		/// <summary>
		///     Event that is fired when the visibility of a Rows details changes.
		/// </summary>
		System::Events::EventHandler<DataGridRowDetailsEventArgs> RowDetailsVisibilityChanged;

        System::Events::EventHandler<DataGridColumnReorderingEventArgs> ColumnReordering;
        System::Events::EventHandler<DataGridColumnEventArgs> ColumnReordered;
        System::Events::EventHandler<Interfaces::VectorEventArgs> ColumnHeaderDragStarted;
        System::Events::EventHandler<Interfaces::VectorEventArgs> ColumnHeaderDragDelta;
        System::Events::EventHandler<Interfaces::VectorEventArgs> ColumnHeaderDragCompleted;
        System::Events::EventHandler<System::Events::EventArg> CurrentCellChanged;
        System::Events::EventHandler<InitializingNewItemEventArgs> InitializingNewItem;
        

        DataGrid();
        virtual ~DataGrid();

		PROPERTY_(bool, AutoGenerateColumns);
		bool GetAutoGenerateColumns() const { return _autoCreateColumns; }
		void SetAutoGenerateColumns(bool const& value) { SetAndRaise(AutoGenerateColumnsProperty, _autoCreateColumns, value); }

		PROPERTY_(double, CellsPanelHorizontalOffset);
		double GetCellsPanelHorizontalOffset() const { return GetValue<double>(CellsPanelHorizontalOffsetProperty); }
		void SetCellsPanelHorizontalOffset(double const& value) { SetValue(CellsPanelHorizontalOffsetProperty, value); }

		PROPERTY_(double, RowHeaderWidth);
		double GetRowHeaderWidth() const { return GetValue<double>(RowHeaderWidthProperty); }
		void SetRowHeaderWidth(double const& value) { SetValue(RowHeaderWidthProperty, value); }

		PROPERTY_(DataGridSelectionUnit, SelectionUnit);
		DataGridSelectionUnit GetSelectionUnit() const { return GetValue<DataGridSelectionUnit>(SelectionUnitProperty); }
		void SetSelectionUnit(DataGridSelectionUnit const& value) { SetValue(SelectionUnitProperty, value); }

		PROPERTY_(DataGridGridLinesVisibility, GridLinesVisibility);
		DataGridGridLinesVisibility GetGridLinesVisibility() const { return GetValue<DataGridGridLinesVisibility>(GridLinesVisibilityProperty); }
		void SetGridLinesVisibility(DataGridGridLinesVisibility const& value) { SetValue(GridLinesVisibilityProperty, value); }

		PROPERTY_(double, HorizontalGridLineThickness);
		double GetHorizontalGridLineThickness() const { return GetValue<double>(HorizontalGridLineThicknessProperty); }
		void SetHorizontalGridLineThickness(double const& value) { SetValue(HorizontalGridLineThicknessProperty, value); }

		PROPERTY_(double, VerticalGridLineThickness);
		double GetVerticalGridLineThickness() const { return GetValue<double>(VerticalGridLineThicknessProperty); }
		void SetVerticalGridLineThickness(double const& value) { SetValue(VerticalGridLineThicknessProperty, value); }

		PROPERTY_(bool, CanUserResizeColumns);
		bool GetCanUserResizeColumns() const { return GetValue<bool>(CanUserResizeColumnsProperty); }
		void SetCanUserResizeColumns(bool const& value) { SetValue(CanUserResizeColumnsProperty, value); }

        PROPERTY_(GridLength, ColumnWidth);
        GridLength const& GetColumnWidth() const;
        void SetColumnWidth(GridLength& value);

		PROPERTY_(System::Reflection::Variant, CurrentItem);
        System::Reflection::Variant GetCurrentItem() const { return GetValue(CurrentItemProperty.get()); }
		void SetCurrentItem(System::Reflection::Variant const& value) { SetValue(CurrentItemProperty, value); }

        PROPERTY_(std::shared_ptr<DataGridCell>, CurrentCell);
        std::shared_ptr<DataGridCell> const& GetCurrentCell() const { return _currentCellContainer; }
        void SetCurrentCell(std::shared_ptr<DataGridCell> const& value) { SetAndRaise(CurrentCellProperty, _currentCellContainer, value); }

        DEPENDENCY_PROPERTY_(double, MinColumnWidth)
        DEPENDENCY_PROPERTY_(double, MaxColumnWidth)
        DEPENDENCY_PROPERTY_(std::shared_ptr<XUI::Styling::Style>, ColumnHeaderStyle)
        DEPENDENCY_PROPERTY_(std::shared_ptr<XUI::Styling::Style>, CellStyle)
        DEPENDENCY_PROPERTY_(std::shared_ptr<XUI::Styling::Style>, DragIndicatorStyle)
        DEPENDENCY_PROPERTY_(bool, IsReadOnly)
        DEPENDENCY_PROPERTY_(bool, CanUserAddRows)
        DEPENDENCY_PROPERTY_(bool, CanUserSortColumns)
        DEPENDENCY_PROPERTY_(bool, CanUserReorderColumns)
        DEPENDENCY_PROPERTY_(bool, CanUserResizeRows)
        DEPENDENCY_PROPERTY_(int, FrozenColumnCount)
        DEPENDENCY_PROPERTY_(std::shared_ptr<Interfaces::IBrush>, HorizontalGridLinesBrush)
        DEPENDENCY_PROPERTY_(std::shared_ptr<Interfaces::IBrush>, VerticalGridLinesBrush)
        DEPENDENCY_PROPERTY_(double, RowHeaderActualWidth)
        DEPENDENCY_PROPERTY_(bool, CanUserDeleteRows)
        DEPENDENCY_PROPERTY_(double, HorizontalScrollOffset);
        DEPENDENCY_PROPERTY_(UI::SelectionMode, SelectionMode);
        DEPENDENCY_PROPERTY_(std::shared_ptr<DataGridColumn>, CurrentColumn);

        static System::Reflection::Variant const& NewPlaceHolder() { return _newItemPlaceholder; }
        std::unordered_set<DataGridCell*> const& GetSelectedCells() const { return _selectedCells; }

        System::Events::EventHandler<DataGridAutoGeneratingColumnEventArgs> AutoGeneratingColumn;

        DataGridColumnCollection& GetColumns() { return _columns; }
        DataGridColumnCollection const& GetColumns() const { return _columns; }

        System::Reflection::Variant const& GetAddedRowItem() const { return _addedRowItem; }
        System::Reflection::Variant const& GetEditedRowItem()  const { return _editedRowItem; }
        System::Reflection::Variant const& GetEditingRowItem() const { return  _editingRowItem; }
        DataGridCell* GetEditingCellContainer() const { return _editingCellContainer.get(); }

		DataGridRow* AddNewRow();
		DataGridRow* GetRow(long id);

        /// <summary>
        ///     Updates the reference to this DataGrid on the list of columns.
        /// </summary>
        /// <param name="list">The list of affected columns.</param>
        /// <param name="clear">Whether to add or remove the reference to this grid.</param>
        void UpdateDataGridReference(std::shared_ptr<DataGridColumn>& column, bool clear);

        /// <summary>
        ///     Returns the DataGridColumn with the given DisplayIndex
        /// </summary>
        DataGridColumn* ColumnFromDisplayIndex(int displayIndex);

        /// <summary>
        ///     A map of display index (key) to index in the column collection (value).  
        ///     Used by the CellsPanel to quickly find a child from a column display index.
        /// </summary>
        std::vector<int>& DisplayIndexMap();

            /// <summary>
            ///     Throws an ArgumentOutOfRangeException if the given displayIndex is invalid.
            /// </summary>
        void ValidateDisplayIndex(DataGridColumn* column, int displayIndex);

        /// <summary>
        ///     Returns the index of a column from the given DisplayIndex
        /// </summary>
        int ColumnIndexFromDisplayIndex(int displayIndex);

        /// <summary>
        ///     Given the DisplayIndex of a column returns the DataGridColumnHeader for that column.
        ///     Used by DataGridColumnHeader to find its previous sibling.
        /// </summary>
        /// <param name="displayIndex"></param>
        /// <returns></returns>
        DataGridColumnHeader* ColumnHeaderFromDisplayIndex(int displayIndex);

		/// <summary>
		///     Called by DataGridColumnCollection when columns' DisplayIndex changes
		/// </summary>
		/// <param name="e"></param>
		void UpdateColumnsOnVirtualizedCellInfoCollections(System::Collection::NotifyCollectionChangedAction action, int oldDisplayIndex, DataGridColumn* oldColumn, int newDisplayIndex);

        /// <summary>
        ///     Called when the DisplayIndex of a column is modified.
        /// </summary>
        /// <remarks>
        ///     A column's DisplayIndex may be modified as the result of another column's DisplayIndex changing.  This is because the 
        ///     DataGrid enforces that the DisplayIndex of all Columns are unique integers from 0 to Columns.Count -1.
        /// </remarks>
        virtual void OnColumnDisplayIndexChanged(DataGridColumnEventArgs& e)
        {
            if (ColumnDisplayIndexChanged)
            {
                ColumnDisplayIndexChanged(this, e);
            }
        }

		/// <summary>
		///     Helper method which queue a request to dispatcher to
		///     invalidate the cellspanel offset if not already queued
		/// </summary>
		void QueueInvalidateCellsPanelHorizontalOffset();

		/// <summary>
		/// Helper method which return any one of the cells or column headers
		/// </summary>
		/// <returns></returns>
		UI::Controls::Control* GetAnyCellOrColumnHeader();

		DataGridCell* GetFocusedCell() const { return _focusedCell.get(); }
        void SetFocusedCell(std::shared_ptr<DataGridCell> const& cell);

		/// <summary>
		///     Test if either the vertical or horizontal gridlines are visible.
		/// </summary>
		bool IsGridLineVisible(bool isHorizontal)
		{
			DataGridGridLinesVisibility visibility = GridLinesVisibility;

			switch (visibility)
			{
				case DataGridGridLinesVisibility::All:
					return true;
				case DataGridGridLinesVisibility::Horizontal:
					return isHorizontal;
				case DataGridGridLinesVisibility::None:
					return false;
				case DataGridGridLinesVisibility::Vertical:
					return !isHorizontal;
			}
			return false;
		}

		/// <summary>
		/// Helper method which returns the width of the viewport which is available for the columns to render
		/// </summary>
		/// <returns></returns>
		double GetViewportWidthForColumns();

        void OnMaterializedHeader(void* sender, Interfaces::ItemContainerEventArgs& e);
        void OnDematerializedHeader(void* sender, Interfaces::ItemContainerEventArgs& e);

        /// <summary>
        ///     Notification that a particular cell's IsSelected property changed.
        /// </summary>
        void CellIsSelectedChanged(DataGridCell* cell, bool isSelected)
        {
            if (!_updatingSelectedCells)
            {
                if (isSelected)
                {
                    _selectedCells.insert(cell);
                }
                else
                {
                    _selectedCells.erase(cell);
                }
            }
        }

        Meta(Disable) void SetColumnHeaderPresenterInternal(DataGridColumnHeadersPresenter* presenter);

        void NotifyPropertyChanged(Core::Dependency::DependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e, NotificationTarget target);

        void OnInternalScrollContentPresenterSizeChanged(void* m, XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e);
        void CleanUpInternalScrollControls();

        static void OnCanExecuteBeginEdit(void* sender, Core::Observer::CanExecuteRoutedEventArgs& e);

        static void OnExecutedBeginEdit(void* sender, Core::Observer::ExecutedRoutedEventArgs& e);

        /// <summary>
        ///     Invoked to determine if the BeginEdit command can be executed.
        /// </summary>
        virtual void OnCanExecuteBeginEdit(Core::Observer::CanExecuteRoutedEventArgs& e);

        /// <summary>
        ///     Invoked when the BeginEdit command is executed.
        /// </summary>
        virtual void OnExecutedBeginEdit(Core::Observer::ExecutedRoutedEventArgs& e);

        static void OnCanExecuteCommitEdit(void* sender, Core::Observer::CanExecuteRoutedEventArgs& e);

        static void OnExecutedCommitEdit(void* sender, Core::Observer::ExecutedRoutedEventArgs& e);

        DataGridCell* GetEventCellOrCurrentCell(Core::Observer::RoutedEventArgs& e);

        bool CanEndEdit(Core::Observer::CanExecuteRoutedEventArgs& e, bool commit);

        /// <summary>
        ///     Invoked to determine if the CommitEdit command can be executed.
        /// </summary>
        virtual void OnCanExecuteCommitEdit(Core::Observer::CanExecuteRoutedEventArgs& e);

        /// <summary>
        ///     Invoked when the CommitEdit command is executed.
        /// </summary>
        virtual void OnExecutedCommitEdit(Core::Observer::ExecutedRoutedEventArgs& e);

        static void OnCanExecuteCancelEdit(void* sender, Core::Observer::CanExecuteRoutedEventArgs& e);

        static void OnExecutedCancelEdit(void* sender, Core::Observer::ExecutedRoutedEventArgs& e);

        /// <summary>
        ///     Invoked to determine if the CancelEdit command can be executed.
        /// </summary>
        virtual void OnCanExecuteCancelEdit(Core::Observer::CanExecuteRoutedEventArgs& e);

        /// <summary>
        ///     Invoked when the CancelEdit command is executed.
        /// </summary>
        virtual void OnExecutedCancelEdit(Core::Observer::ExecutedRoutedEventArgs& e);

        static void OnCanExecuteDelete(void* sender, Core::Observer::CanExecuteRoutedEventArgs& e);

        static void OnExecutedDelete(void* sender, Core::Observer::ExecutedRoutedEventArgs& e);

        /// <summary>
        ///     Invoked to determine if the Delete command can be executed.
        /// </summary>
        virtual void OnCanExecuteDelete(Core::Observer::CanExecuteRoutedEventArgs& e);

        /// <summary>
        ///     Invoked when the Delete command is executed.
        /// </summary>
        virtual void OnExecutedDelete(Core::Observer::ExecutedRoutedEventArgs& e);

        static void OnCanExecuteSelectAll(void * sender, Core::Observer::CanExecuteRoutedEventArgs & e);

        static void OnExecutedSelectAll(void * sender, Core::Observer::ExecutedRoutedEventArgs & e);

        /// <summary>
        /// Method to perform sorting on datagrid
        /// </summary>
        /// <param name="sortColumn"></param>
        void PerformSort(DataGridColumn* sortColumn);

        /// <summary>
        ///     Raises the BeginEdit command, which will place the current cell or row into
        ///     edit mode.
        /// </summary>
        /// <remarks>
        ///     If the command is enabled, this will lead to the BeginningEdit and PreparingCellForEdit
        ///     overrides and events.
        /// </remarks>
        /// <param name="editingEventArgs">The event arguments, if any, that led to BeginEdit being called. May be null.</param>
        /// <returns>true if the current cell or row enters edit mode, false otherwise.</returns>
        bool BeginEdit(Core::Observer::RoutedEventArgs* editingEventArgs = nullptr)
        {
            if (!IsReadOnly)
            {
                DataGridCell* cellContainer = _currentCellContainer.get();
                if (cellContainer != nullptr)
                {
                    if (!cellContainer->IsEditing &&
                        BeginEditCommand->CanExecute(*editingEventArgs, cellContainer))
                    {
                        BeginEditCommand->Execute(*editingEventArgs, cellContainer);
                    }

                    return cellContainer->IsEditing;
                }
            }

            return false;
        }

        /// <summary>
        ///     Raises the CancelEdit command.
        ///     If a cell is currently in edit mode, cancels the cell edit, but leaves any row edits alone.
        ///     If a cell is not in edit mode, then cancels any pending row edits.
        /// </summary>
        /// <returns>true if the current cell or row exits edit mode, false otherwise.</returns>
        bool CancelEdit()
        {
            if (IsEditingCurrentCell())
            {
                return CancelEdit(DataGridEditingUnit::Cell);
            }
            else if (IsEditingRowItem() || IsAddingNewItem())
            {
                return CancelEdit(DataGridEditingUnit::Row);
            }

            return true; // No one is in edit mode
        }

        /// <summary>
        ///     Raises the CancelEdit command.
        ///     If a cell is currently in edit mode, cancels the cell edit, but leaves any row edits alone.
        /// </summary>
        /// <returns>true if the cell exits edit mode, false otherwise.</returns>
        bool CancelEdit(DataGridCell* cell)
        {
            DataGridCell* currentCell = _currentCellContainer.get();
            if (currentCell != nullptr && currentCell == cell && currentCell->IsEditing)
            {
                return CancelEdit(DataGridEditingUnit::Cell);
            }

            return true;
        }

        /// <summary>
        ///     Raises the CancelEdit command.
        ///     Reverts any pending editing changes to the desired editing unit and exits edit mode.
        /// </summary>
        /// <param name="editingUnit">Whether to cancel edit mode of the current cell or current row.</param>
        /// <returns>true if the current cell or row exits edit mode, false otherwise.</returns>
        bool CancelEdit(DataGridEditingUnit editingUnit)
        {
            return EndEdit(CancelEditCommand.get(), _currentCellContainer.get(), editingUnit, true);
        }

        /// <summary>
        ///     Raises the CommitEdit command.
        ///     If a cell is currently being edited, commits any pending changes to the cell, but
        ///     leaves any pending changes to the row. This should mean that changes are propagated
        ///     from the editing environment to the pending row.
        ///     If a cell is not currently being edited, then commits any pending rows.
        /// </summary>
        /// <returns>true if the current cell or row exits edit mode, false otherwise.</returns>
        bool CommitEdit()
        {
            if (IsEditingCurrentCell())
            {
                return CommitEdit(DataGridEditingUnit::Cell, true);
            }
            else if (IsEditingRowItem() || IsAddingNewItem())
            {
                return CommitEdit(DataGridEditingUnit::Row, true);
            }

            return true; // No one is in edit mode
        }

        /// <summary>
        ///     Raises the CommitEdit command.
        ///     Commits any pending changes for the given editing unit and exits edit mode.
        /// </summary>
        /// <param name="editingUnit">Whether to commit changes for the current cell or current row.</param>
        /// <param name="exitEditingMode">Whether to exit edit mode.</param>
        /// <returns>true if the current cell or row exits edit mode, false otherwise.</returns>
        bool CommitEdit(DataGridEditingUnit editingUnit, bool exitEditingMode)
        {
            return EndEdit(CommitEditCommand.get(), _currentCellContainer.get(), editingUnit, exitEditingMode);
        }

        bool IsEditingCurrentCell() const
        {
            if (_currentCellContainer)
                return _currentCellContainer->IsEditing;
            return false;
        }

        bool IsCurrentCellReadOnly() const
        {
            if (_currentCellContainer)
                return _currentCellContainer->IsReadOnly;
            return false;
        }

        bool IsAddingNewItem() const
        {
            return _addedRowItem.GetBase();
        }

        bool IsEditingRowItem() const
        {
            return _editingRowItem.GetBase();
        }

        bool IsAddingOrEditingRowItem(System::Reflection::Variant const& item)
        {
            return //IsEditingItem(item) ||
                (IsAddingNewItem() && (_addedRowItem/* EditableItems.CurrentAddItem */ == item));
        }

        bool CanCancelAddingOrEditingRowItem(DataGridEditingUnit editingUnit, System::Reflection::Variant const& item)
        {
            return (editingUnit == DataGridEditingUnit::Row) &&
                ((IsEditingItem(item) /*&& EditableItems.CanCancelEdit*/) ||
                (IsAddingNewItem() && (_addedRowItem/* EditableItems.CurrentAddItem */ == item)));
        }

        bool IsEditingItem(System::Reflection::Variant const& item)
        {
            return IsEditingRowItem() && (_editingRowItem /* EditableItems.CurrentEditItem */ == item);
        }

        bool IsNewItemPlaceholder(System::Reflection::Variant const& item)
        {
            return (item == _newItemPlaceholder);
        }

        int DataItemsSelected() const
        {
            int itemsSelected = _selectedItems.size();

            if (_newItemPlaceholderPosition != NewItemPlaceholderPosition::None && GetSelectedItems().Contains(_newItemPlaceholder))
            {
                itemsSelected--;
            }

            return itemsSelected;
        }

        void HandleSelectionForCellInput(DataGridCell* cell, bool startDragging, bool allowsExtendSelect, bool allowsMinimalSelect);

        /// <summary>
        ///     Processes selection for a row.
        ///     Depending on the current keyboard state, this may mean
        ///     - Selecting the row
        ///     - Deselecting the row
        ///     - Deselecting other rows
        ///     - Extending selection to the row
        /// </summary>
        /// <remarks>
        ///     ADO.Net has a bug (#524977) where if the row is in edit mode
        ///     and atleast one of the cells are edited and committed without
        ///     commiting the row itself, DataView.IndexOf for that row returns -1
        ///     and DataView.Contains returns false. The Workaround to this problem 
        ///     is to try to use the previously computed row index if the operations 
        ///     are in the same row scope.
        /// </remarks>
        void MakeFullRowSelection(System::Reflection::Variant const& dataItem, bool allowsExtendSelect, bool allowsMinimalSelect);

        /// <summary>
        ///     Process selection on a cell.
        ///     Depending on the current keyboard state, this may mean
        ///     - Selecting the cell
        ///     - Deselecting the cell
        ///     - Deselecting other cells
        ///     - Extending selection to the cell
        /// </summary>
        /// <remarks>
        ///     ADO.Net has a bug (#524977) where if the row is in edit mode
        ///     and atleast one of the cells are edited and committed without
        ///     commiting the row itself, DataView.IndexOf for that row returns -1
        ///     and DataView.Contains returns false. The Workaround to this problem 
        ///     is to try to use the previously computed row index if the operations 
        ///     are in the same row scope.
        /// </remarks>
        void MakeCellSelection(DataGridCell* cell, bool allowsExtendSelect, bool allowsMinimalSelect);


        /// <summary>
        ///     There was general input on a row header that indicated that
        ///     selection should occur on the given row.
        /// </summary>
        /// <param name="row">The target row.</param>
        /// <param name="startDragging">Whether the input also indicated that dragging should start.</param>
        void HandleSelectionForRowHeaderAndDetailsInput(DataGridRow* row, bool startDragging, bool allowsMinimalSelect);

        void SelectItem(System::Reflection::Variant const& item);

        void SelectItem(System::Reflection::Variant const& item, bool selectCells);

        void OnItemsSourceCollectionChanged(void* sender, System::Collection::NotifyCollectionChangedEventArgs& e);

        void UpdateNewItemPlaceholder(bool isAddingNewItem);

    protected:

        /// <summary>
        /// Measures the control.
        /// </summary>
        /// <param name="availableSize">The available size for the control.</param>
        /// <returns>The desired size.</returns>
        Core::Media::Size MeasureOverride(Core::Media::Size const& availableSize) override;
        Core::Media::Size ArrangeOverride(Core::Media::Size const& arrangeSize) override;

		/// <inheritdoc/>
		std::shared_ptr<Generators::IItemContainerGenerator> CreateItemContainerGenerator() override;

		void ItemsChanged(Core::Dependency::DependencyPropertyChangedEventArgs& e) override;

        void OnTemplateApplied(Controls::TemplateAppliedEventArgs const& e) override;

        void ApplyTemplate() override;

        void OnPointerMoved(Interfaces::PointerEventArgs& e) override;

        RelativeMousePositions GetRelativeMousePosition();

	private:

        bool CanSelectRows()
        {
            switch (SelectionUnit)
            {
                case DataGridSelectionUnit::FullRow:
                case DataGridSelectionUnit::CellOrRowHeader:
                    return true;

                case DataGridSelectionUnit::Cell:
                    return false;
            }

            //LOG("Unknown SelectionUnit encountered.");
            return false;
        }

        System::Reflection::Variant AddNewItem();

        void CancelAnyEdit()
        {
            if (IsAddingNewItem() || IsEditingRowItem())
            {
                // There is a row edit in progress, cancel it, which will also cancel the cell edit.
                CancelEdit(DataGridEditingUnit::Row);
            }
            else if (IsEditingCurrentCell())
            {
                // Cancel the current cell edit.
                CancelEdit(DataGridEditingUnit::Cell);
            }
        }

        void EditRowItem(System::Reflection::Variant const& rowItem)
        {
            _editedRowItem = rowItem;

            // CancelEdit and CommitEdit rely on IsEditingRowItem
            Core::Observer::RoutedCommand::InvalidateRequerySuggested();
        }

        bool CommitAnyEdit();

        bool EndEdit(Core::Observer::RoutedCommand* command, DataGridCell* cellContainer, DataGridEditingUnit editingUnit, bool exitEditMode);

        void EnsureInternalScrollControls();

		void BeginUpdateSelectedCells()
		{
			ASSERT(!_updatingSelectedCells);
			_updatingSelectedCells = true;
		}

		void EndUpdateSelectedCells()
		{
			ASSERT(_updatingSelectedCells);

			UpdateIsSelected();
			_updatingSelectedCells = false;

			NotifySelectedCellsChanged();
		}

        void NotifySelectedCellsChanged();

        void UpdateIsSelected();

		/// <summary>
		///     Dispatcher call back method which recomputes the CellsPanelOffset
		/// </summary>
		void InvalidateCellsPanelHorizontalOffset()
		{
            if (!std::isnan(RowHeaderWidth) && RowHeaderWidth > 0.0)
			{
				CellsPanelHorizontalOffset = RowHeaderWidth + 2.0; // ToDo: row border thickness
			}
			else
			{
				CellsPanelHorizontalOffset = 0.0;
			}
		}

        bool ShouldExtendSelection() const
        {
            return (_selectionAnchor != nullptr);
        }

        int GetDataItemCount() const;

		void DeleteAutoColumns();
        void GenerateAutoColumns();

        /// <summary>
        /// Validates that the minimum column width is an acceptable value
        /// </summary>
        static bool ValidateMinColumnWidth(System::Reflection::Variant const& v);

        /// <summary>
        /// Validates that the maximum column width is an acceptable value
        /// </summary>
        static bool ValidateMaxColumnWidth(System::Reflection::Variant const& v);

        /// <summary>
        ///     Called when the Columns collection changes.
        /// </summary>
        void OnColumnsChanged(void* sender, System::Collection::NotifyCollectionChangedEventArgs& e);

        /// <summary>
        ///     Updates the transferred size constraints from DataGrid on the columns.
        /// </summary>
        /// <param name="list">The list of affected columns.</param>
        static void UpdateColumnSizeConstraints(std::shared_ptr<DataGridColumn>& column);

        void UnselectItem(System::Reflection::Variant const& item);
        void SelectAll();

        void BeginRowDragging();

        void BeginDragging();

        void EndDragging();

        DataGridEditingUnit GetEditingUnit(System::Reflection::Variant const& parameter)
        {
            // If the parameter contains a DataGridEditingUnit, then use it.
            // Otherwise, choose Cell if a cell is currently being edited, or Row if not.
            
            return ((parameter.GetBase()) && (parameter.GetType()->IsAssignableFrom<DataGridEditingUnit>())) ?
                parameter.GetValue<DataGridEditingUnit>() :
                IsEditingCurrentCell() ? DataGridEditingUnit::Cell : DataGridEditingUnit::Row;
        }

        /// <summary>
        /// Clears the sort directions for all the columns except the column to be sorted upon
        /// </summary>
        /// <param name="sortColumn"></param>
        void PrepareForSort(DataGridColumn* sortColumn)
        {
            if (!_columns.empty())
            {
                for(auto const& column : _columns)
                {
                    if (column.get() != sortColumn)
                    {
                        column->SortDirection = ListSortDirection::UnSorted;
                    }
                }
            }
        }

        /// <summary>
        /// Determines the sort direction and sort property name and adds a sort
        /// description to the Items>SortDescriptions Collection. Clears all the 
        /// existing sort descriptions.
        /// </summary>
        /// <param name="column"></param>
        /// <param name="clearExistingSortDescriptions"></param>
        void DefaultSort(DataGridColumn* column, bool clearExistingSortDescriptions);

        void OnCurrentCellChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e);

        void UpdateRowEditing(DataGridCell* cell);

        DataGridCell* GetCellNearMouse();

        DataGridRow* GetRowNearMouse();

        static bool CalculateCellDistance(UI::Controls::Visual* cell, DataGridRow* rowOwner, Panel* itemsHost, Core::Media::Rect const& itemsHostBounds, bool isMouseInCorner, double& distance);
    };

	/// <summary>
	///     Handles tracking defered selection change notifications for selected cells.
	/// </summary>
	struct ChangingSelectedCellsHelper
	{
		ChangingSelectedCellsHelper(DataGrid* dataGrid)
		{
			_dataGrid = dataGrid;
			_wasUpdatingSelectedCells = _dataGrid->_updatingSelectedCells;
			if (!_wasUpdatingSelectedCells)
			{
				_dataGrid->BeginUpdateSelectedCells();
			}
		}

		~ChangingSelectedCellsHelper()
		{
			//GC.SuppressFinalize(this);
			if (!_wasUpdatingSelectedCells)
			{
				_dataGrid->EndUpdateSelectedCells();
			}
		}

		DataGrid* _dataGrid;
		bool _wasUpdatingSelectedCells = false;
	};
}

#endif //!__XUI_UI_DATAGRID_H__
