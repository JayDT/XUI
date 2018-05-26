#ifndef __DATAGRID_DEFINES_H__
#define __DATAGRID_DEFINES_H__

#include "standard/enum.h"

namespace XUI::UI
{
    class DataGrid;
    class DataGridColumn;
	class DataGridRow;

    enum class NotificationTarget : uint16
    {
        None                   = 0x00, // this means don't send it on; likely handle it on the same object that raised the event.
        Cells                  = 0x01,
        CellsPresenter         = 0x02,
        Columns                = 0x04,
        ColumnCollection       = 0x08,
        ColumnHeaders          = 0x10,
        ColumnHeadersPresenter = 0x20,
        DataGrid               = 0x40,
        DetailsPresenter       = 0x80,
        RefreshCellContent     = 0x100,
        RowHeaders             = 0x200,
        Rows                   = 0x400,
        All                    = 0xFFF,
    };

    ENABLE_ENUM_CLASS_FLAG(NotificationTarget)

    enum class ListSortDirection : uint8
    {
        UnSorted   = 0,
        Ascending  = 1,
        Descending = 2
    };

    enum class Visibility : uint8
    {
        Visible     = 0,
        Hidden      = 1,
        Collapsed   = 2
    };

	enum class DataGridSelectionUnit : uint8
	{
		/// <summary>
		///     Only cells are selectable.
		///     Clicking on a cell will select the cell.
		///     Clicking on row or column headers does nothing.
		/// </summary>
		Cell,

		/// <summary>
		///     Only full rows are selectable.
		///     Clicking on row headers or on cells will select the whole row.
		/// </summary>
		FullRow,

		/// <summary>
		///     Cells and rows are selectable.
		///     Clicking on a cell will select the cell. Selecting all cells in the row will not select the row.
		///     Clicking on a row header will select the row and all cells in the row.
		/// </summary>
		CellOrRowHeader
	};

    /// <summary>
    ///     Used to specify which container to take out of edit mode.
    /// </summary>
    enum class DataGridEditingUnit
    {
        /// <summary>
        ///     Targets a cell container.
        /// </summary>
        Cell,

        /// <summary>
        ///     Targets a row container.
        /// </summary>
        Row
    };

	/// <summary>
	/// Defines modes that indicate how DataGrid content is copied to the Clipboard. 
	/// </summary>
	enum class DataGridClipboardCopyMode : uint8
	{
		/// <summary>
		/// Copying to the Clipboard is disabled.
		/// </summary>
		None,

		/// <summary>
		/// The text values of selected cells can be copied to the Clipboard. Column header is not included. 
		/// </summary>
		ExcludeHeader,

		/// <summary>
		/// The text values of selected cells can be copied to the Clipboard. Column header is included for columns that contain selected cells.  
		/// </summary>
		IncludeHeader,
	};

	enum class DataGridGridLinesVisibility : uint8
	{
		All,
		Horizontal,
		None,
		Vertical
	};

	/// <summary>
	/// Determines whether the row/column headers are shown or not.
	/// </summary>
	enum class DataGridHeadersVisibility : uint8
	{
		Column = 0x01,
		Row = 0x02,
		None = 0x00,
		All = (uint8(Row) | uint8(Column)),
	};

	enum class ScrollBarVisibility : uint8
	{
		Auto,
		Visible,
		Hidden,
	};

	enum class DataGridRowDetailsVisibilityMode : uint8
	{
		Collapsed,              // Show no details by default. Developer must toggle visibility
		Visible,                // Show the details section for all rows
		VisibleWhenSelected     // Show the details section only for the selected row(s)
	};

    enum class EditTriggers
    {
        None = 0x00,
        BeginEditCommand = 0x01,
        ClickOnCurrentCell = 0x02,
        SingleClick = 0x04,
        CellIsCurrent = 0x08,
        ActivationGesture = 0x10,
        RowIsCurrent = 0x20,
    };

    enum class NewItemPlaceholderPosition
    {
        None = 0,
        AtBeginning = 1,
        AtEnd = 2
    };

    enum class RelativeMousePositions
    {
        Over = 0x00,
        Above = 0x01,
        Below = 0x02,
        Left = 0x04,
        Right = 0x08,
    };

    ENABLE_ENUM_CLASS_FLAG(RelativeMousePositions)

    struct DataGridColumnEventArgs : System::Events::EventArg
    {
        DataGridColumn* Column = nullptr;
    };

    /// <summary>
    /// Event args used for Column reordering event raised after column header drag-drop
    /// </summary>
    struct DataGridColumnReorderingEventArgs : DataGridColumnEventArgs
    {
        bool Cancel = false;
        Controls::Control* DropLocationIndicator = nullptr;
        Controls::Control* DragIndicator = nullptr;
    };

	struct DataGridRowDetailsEventArgs : System::Events::EventArg
	{
		Controls::Control* DetailsElement = nullptr;
		DataGridRow* Row = nullptr;
	};

    struct DataGridBeginningEditEventArgs : System::Events::EventArg
    {
        bool _cancel = false;
        DataGridColumn* _dataGridColumn = nullptr;
        DataGridRow* _dataGridRow = nullptr;
        Core::Observer::RoutedEventArgs* _editingEventArgs = nullptr;
    };

    struct DataGridCellEditEndingEventArgs : System::Events::EventArg
    {
        bool _cancel = false;
        DataGridColumn* _dataGridColumn = nullptr;
        DataGridRow* _dataGridRow = nullptr;
        Controls::Control* _editingElement = nullptr;
        bool _commit = true;
    };

    struct DataGridRowEditEndingEventArgs : System::Events::EventArg
    {
        bool _cancel = false;
        DataGridRow* _dataGridRow = nullptr;
        bool _commit = true;
    };

    struct DataGridSortingEventArgs : DataGridColumnEventArgs
    {
        bool Handled = false;
    };

    struct InitializingNewItemEventArgs : System::Events::EventArg
    {
        /// <summary>
        ///     Instantiates a new instance of this class.
        /// </summary>
        InitializingNewItemEventArgs(System::Reflection::Variant* newItem)
            : NewItem(newItem)
        {
        }

        /// <summary>
        ///     The new item.
        /// </summary>
        System::Reflection::Variant* NewItem = nullptr;
    };

    /// <summary>
    /// Defines vertical or horizontal orientation.
    /// </summary>
    enum class Orientation : uint8
    {
        /// <summary>
        /// Vertical orientation.
        /// </summary>
        Vertical,

        /// <summary>
        /// Horizontal orientation.
        /// </summary>
        Horizontal,
    };
}

#endif // !__DATAGRID_DEFINES_H__
