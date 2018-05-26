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

#ifndef __XUI_UI_DATAGRID_CELL_H__
#define __XUI_UI_DATAGRID_CELL_H__

#include "UI/Controls/ContentControl.h"
#include "UI/Controls/ItemsControl.h"
#include "UI/DataGrid/DataGridDefines.h"
#include "UI/DataGrid/DataGridExtensions.h"

namespace XUI::UI
{
    class Panel;
    class DataGrid;
    class DataGridRow;
    class DataGridCellsPresenter;

    class DataGridCell : public Controls::ContentControl
    {
        friend class DataGridCellsPresenter;
    private:
#pragma region Data

        std::weak_ptr<DataGridRow> _owner;
        DataGridCellsPresenter* _presenter = nullptr;
        DataGridCellReference _cellReference;
        bool _syncingIsSelected = false;                    // Used to prevent unnecessary notifications

#pragma endregion
    public:

        static Core::Dependency::RefDependencyProperty ColumnProperty;
        static Core::Dependency::RefDependencyProperty IsEditingProperty;
        static Core::Dependency::RefDependencyProperty IsReadOnlyProperty;
        static Core::Dependency::RefDependencyProperty IsSelectedProperty;

        static Core::Observer::RefRoutedEvent SelectedEvent;
        static Core::Observer::RefRoutedEvent UnselectedEvent;

        static void StaticClassInitializer();

        DataGridCell();
        virtual ~DataGridCell();

        DataGrid* DataGridOwner() const;
        Panel* ParentPanel() const;
        DataGridRow* RowOwner() const { return _owner.lock().get(); }
        System::Reflection::Variant RowDataItem() const;
        DataGridCellsPresenter* CellsPresenter() const;
        bool NeedsVisualTree() const;

        PROPERTY_(std::shared_ptr<DataGridColumn>, Column);
		std::shared_ptr<DataGridColumn> GetColumn() const;
		void SetColumn(std::shared_ptr<DataGridColumn> const& value);

		PROPERTY_(bool, IsEditing);
		bool GetIsEditing() const { return GetValue<bool>(IsEditingProperty); }
		void SetIsEditing(bool const& value) { SetValue(IsEditingProperty, value); }

		PROPERTY_(bool, IsSelected);
		bool GetIsSelected() const { return GetValue<bool>(IsSelectedProperty); }
		void SetIsSelected(bool const& value) { SetValue(IsSelectedProperty, value); }

        DEPENDENCY_PROPERTY_(bool, IsReadOnly)

		Controls::Control* GetEditingElement()
		{
			return Content.ToObject() ? Content.ToObject()->Static_As<Controls::Control>() : nullptr;
		}


        /// <summary>
        ///     Prepares a cell for use.
        /// </summary>
        /// <remarks>
        ///     Updates the column reference.
        ///     This overload computes the column index from the ItemContainerGenerator.
        /// </remarks>
        void PrepareCell(System::Reflection::Variant const& item, Controls::ItemsControl* cellsPresenter, DataGridRow* ownerRow);

        /// <summary>
        ///     Prepares a cell for use.
        /// </summary>
        /// <remarks>
        ///     Updates the column reference.
        /// </remarks>
        void PrepareCell(System::Reflection::Variant const& item, DataGridRow* ownerRow, int index);

        /// <summary>
        ///     Clears the cell of references.
        /// </summary>
        void ClearCell(DataGridRow* ownerRow);

        /// <summary>
        ///     Measure.  This is overridden so that the cell can extend its size to account for a grid line on the right.
        /// </summary>
        Core::Media::Size MeasureOverride(Core::Media::Size const& constraint) override;

        /// <summary>
        ///     Arrange.  This is overriden so that the cell can position its content to account for a grid line on the right.
        /// </summary>
        /// <param name="arrangeSize">Arrange size</param>
        Core::Media::Size ArrangeOverride(Core::Media::Size const& arrangeSize) override;

        /// <summary>
        ///     OnRender.  Overriden to draw a vertical line on the right.
        /// </summary>
        /// <param name="drawingContext"></param>
        void Render(Interfaces::IDrawingContextImpl* drawingContext) override;

        /// <summary>
        ///     The left mouse button was pressed
        /// </summary>
        void OnAnyMouseLeftButtonDown(Interfaces::PointerPressedEventArgs& e);

    protected:

        /// <summary>
        ///     Reporting text composition.
        /// </summary>
        void OnTextInput(Interfaces::TextInputEventArgs& e) override;

        /// <summary>
        ///     Reporting a key was pressed.
        /// </summary>
        void OnKeyDown(Interfaces::KeyEventArgs& e) override;

        /// <summary>
        ///     Reporting a key was released
        /// </summary>
        void OnKeyUp(Interfaces::KeyEventArgs& e) override;

        /// <summary>
        ///     Reporting the mouse button was released
        /// </summary>
        void OnPointerReleased(Interfaces::PointerReleasedEventArgs& e) override;

        void OnPointerPressed(Interfaces::PointerPressedEventArgs& e) override;

        void OnPreviewPointerPressed(Interfaces::PointerPressedEventArgs& e) override;

        void OnGotFocus(Interfaces::GotFocusEventArgs& e) override;

        void OnLostFocus(Core::Observer::RoutedEventArgs& e) override;

        void SendInputToColumn(Core::Observer::RoutedEventArgs& e);

    private:

        /// <summary>
        ///     Notifies the Cell of a property change.
        /// </summary>
        static void OnNotifyPropertyChanged(DependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e);

        static void OnAnyLostFocus(System::Reflection::Object* sender, Core::Observer::RoutedEventArgs& e);

        static void OnAnyGotFocus(System::Reflection::Object* sender, Core::Observer::RoutedEventArgs& e);

        void RaisePreparingCellForEdit(Core::Observer::RoutedEventArgs& editingEventArgs);

    public:
        /// <summary>
        ///     General notification for DependencyProperty changes from the grid or from columns.
        /// </summary>
        void NotifyPropertyChanged(DependencyObject* d, std::string propertyName, Core::Dependency::DependencyPropertyChangedEventArgs& e, NotificationTarget target);

        /// <summary>
        ///     Builds a column's visual tree if not using templates.
        /// </summary>
        void BuildVisualTree(System::Reflection::Variant const & item);

        void BeginEdit(Core::Observer::RoutedEventArgs& e);

        void CancelEdit();

        bool CommitEdit();

        /// <summary>
        ///     Used to synchronize IsSelected with the DataGrid.
        ///     Prevents unncessary notification back to the DataGrid.
        /// </summary>
        void SyncIsSelected(bool isSelected);

        void RaiseSelectionChangedEvent(bool isSelected)
        {
            Core::Observer::RoutedEventArgs e;
            e.Sender = this;
            e.Source = this;

            if (isSelected)
                e.RoutedEvent = SelectedEvent.get();
            else
                e.RoutedEvent = UnselectedEvent.get();

            RaiseEvent(e);
        }
    };

    struct DataGridCellInfo : System::Reflection::Object
    {
		DataGridCellInfo()
		{
		}

        /// <summary>
        ///     Identifies a cell at the column within the row for the specified item.
        /// </summary>
        /// <param name="item">The item who's row contains the cell.</param>
        /// <param name="column">The column of the cell within the row.</param>
        /// <remarks>
        ///     This constructor will not tie the DataGridCellInfo to any particular
        ///     DataGrid.
        /// </remarks>
        DataGridCellInfo(System::Reflection::Variant& item, std::shared_ptr<DataGridColumn> const& column)
        {
            if (column == nullptr)
                throw System::ArgumentNullException("column");

            Item = item;
            Column = column;
            Owner = nullptr;
        }

        /// <summary>
        ///     Creates a structure that identifies the specific cell container.
        /// </summary>
        /// <param name="cell">
        ///     A reference to a cell.
        ///     This structure does not maintain a strong reference to the cell.
        ///     Changes to the cell will not affect this structure.
        /// </param>
        /// <remarks>
        ///     This constructor will tie the DataGridCellInfo to the specific
        ///     DataGrid that owns the cell container.
        /// </remarks>
        DataGridCellInfo(DataGridCell const* cell)
        {
            if (cell == nullptr)
                throw System::ArgumentNullException("cell");

            Item = cell->RowDataItem();
            Column = cell->Column;
            Owner = cell->DataGridOwner();
        }

        DataGridCellInfo(System::Reflection::Variant& item, std::shared_ptr<DataGridColumn> const& column, DataGrid* owner)
        {
            EXCEPT(item != nullptr && "item should not be null.");
            EXCEPT(column != nullptr && "column should not be null.");
            EXCEPT(owner != nullptr && "owner should not be null.");

            Item = item;
            Column = column;
            Owner = owner;
        }

        /// <summary>
        ///     Used to create an unset DataGridCellInfo.
        /// </summary>
        DataGridCellInfo(System::Reflection::Variant& item)
        {
            //EXCEPT(item == DependencyProperty.UnsetValue, "This should only be used to make an Unset CellInfo.");
            Item = item;
            Column = nullptr;
            Owner = nullptr;
        }

		bool operator==(DataGridCellInfo const& rhs) const
		{
			return Item == rhs.Item && Column == rhs.Column && Owner == rhs.Owner;
		}

        bool operator==(DataGridColumn const* rhs) const
        {
            return Column.get() == rhs;
        }

        void ConvertCellInfoToIndexes(DataGridCellInfo* cell, int& rowIndex, int &columnIndex);

        System::Reflection::Variant Item;
        std::shared_ptr<DataGridColumn> Column;
        DataGrid* Owner = nullptr;
    };
}

#endif //!__XUI_UI_DATAGRID_CELL_H__