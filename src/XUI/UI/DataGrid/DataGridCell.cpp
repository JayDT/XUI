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
#include "DataGridCell.h"
#include "DataGrid/DataGridControl.h"
#include "DataGrid/DataGridRow.h"
#include "Platform/UIRenderer.h"
#include "Core/Media/Pen.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;
using namespace XUI::UI;

Core::Dependency::RefDependencyProperty DataGridCell::ColumnProperty;
Core::Dependency::RefDependencyProperty DataGridCell::IsEditingProperty;
Core::Dependency::RefDependencyProperty DataGridCell::IsReadOnlyProperty;
Core::Dependency::RefDependencyProperty DataGridCell::IsSelectedProperty;

Core::Observer::RefRoutedEvent DataGridCell::SelectedEvent;
Core::Observer::RefRoutedEvent DataGridCell::UnselectedEvent;

STATIC_CLASS_INITIALIZER(DataGridCell, XUI::UI::DataGridCell::StaticClassInitializer);

void XUI::UI::DataGridCell::StaticClassInitializer()
{
    ColumnProperty = (Core::Dependency::BindProperty<std::shared_ptr<DataGridColumn>>::Register<DataGridCell>(nameof(Column), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto cell = static_cast<DataGridCell*>(d);
        // We need to call BuildVisualTree after changing the column (PrepareCell does this).
        cell->Content = nullptr;
        cell->CoerceValue(StyleProperty.get());
        cell->CoerceValue(IsReadOnlyProperty.get());
    }, nullptr));
    IsEditingProperty = (Core::Dependency::BindProperty<bool>::Register<DataGridCell>(nameof(IsEditing), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto cell = static_cast<DataGridCell*>(d);
        if (e.NewValue.GetValue<bool>())
            cell->Focus();
        cell->BuildVisualTree(cell->RowOwner()->Item);
    }, nullptr));
    IsReadOnlyProperty = (Core::Dependency::BindProperty<bool>::Register<DataGridCell>(nameof(IsReadOnly), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto cell = static_cast<DataGridCell*>(d);
        auto dataGrid = cell->DataGridOwner();
        if (e.NewValue.GetValue<bool>() && dataGrid != nullptr)
        {
            dataGrid->CancelEdit(cell);
        }

        // re-evalutate the BeginEdit command's CanExecute.
        Core::Observer::RoutedCommand::InvalidateRequerySuggested();

        cell->NotifyPropertyChanged(cell, "", e, NotificationTarget::Cells);
    }, [](Core::Dependency::DependencyObject* d, System::Reflection::Variant const& baseValue)
    {
        auto column = static_cast<DataGridCell*>(d);
        return DataGridHelper::GetCoercedTransferPropertyValue(d, baseValue, IsReadOnlyProperty.get(), column, DataGridColumn::IsReadOnlyProperty.get(), column->DataGridOwner(), DataGrid::IsReadOnlyProperty.get());
    }));
    IsSelectedProperty = (Core::Dependency::BindProperty<bool>::Register<DataGridCell>(nameof(IsSelected), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto cell = static_cast<DataGridCell*>(d);
        bool isSelected = e.NewValue.GetValue<bool>();

        // There is no reason to notify the DataGrid if IsSelected's value came
        // from the DataGrid.
        if (!cell->_syncingIsSelected)
        {
            DataGrid* dataGrid = cell->DataGridOwner();
            if (dataGrid != nullptr)
            {
                // Notify the DataGrid that a cell's IsSelected property changed
                // in case it was done programmatically instead of by the 
                // DataGrid itself.
                dataGrid->CellIsSelectedChanged(cell, isSelected);
            }
        }

        cell->RaiseSelectionChangedEvent(isSelected);
    }, nullptr));

    WidthProperty->OverrideMetadata<DataGridCell>(std::make_shared<Core::Dependency::BindPropertyMetadata>(
        double(0.0),
        nullptr,
        Core::Dependency::BindingMode::OneWay,
        [](XamlCPP::Core::Dependency::IDependencyObject* d, System::Reflection::Variant const& baseValue) -> System::Reflection::Variant
        {
            auto cell = d->Static_As<DataGridCell>();
            if (cell)
                return DataGridHelper::GetCoercedTransferPropertyValue(cell, baseValue, WidthProperty.get(), cell->Column.get(), DataGridColumn::ActualWidthProperty.get());
            return baseValue;
        }));

    SelectedEvent = (Core::Observer::RoutedEvent::Register<DataGridCell, Core::Observer::RoutedEventArgs>(nameof(Selected), Core::Observer::RoutingStrategies::Bubble));
    UnselectedEvent = (Core::Observer::RoutedEvent::Register<DataGridCell, Core::Observer::RoutedEventArgs>(nameof(Unselected), Core::Observer::RoutingStrategies::Bubble));

    FocusableProperty->OverrideDefaultValue<DataGridCell>(true);
    VerticalContentAlignmentProperty->OverrideDefaultValue<DataGridCell>(Controls::VerticalAlignment::Center);
}

XUI::UI::DataGridCell::DataGridCell()
{
}

XUI::UI::DataGridCell::~DataGridCell()
{
}

DataGrid * XUI::UI::DataGridCell::DataGridOwner() const
{
	if (_owner.expired())
		return nullptr;
    return _owner.lock()->DataGridOwner();
}

Panel * XUI::UI::DataGridCell::ParentPanel() const
{
    return nullptr;
}

System::Reflection::Variant XUI::UI::DataGridCell::RowDataItem() const
{
    if (_owner.expired())
        return System::Reflection::Variant::Empty;
    return _owner.lock()->Item;
}

DataGridCellsPresenter * XUI::UI::DataGridCell::CellsPresenter() const
{
    return _presenter;
}

bool XUI::UI::DataGridCell::NeedsVisualTree() const
{
    return true;
}

std::shared_ptr<DataGridColumn> XUI::UI::DataGridCell::GetColumn() const
{
	return GetValue<std::shared_ptr<DataGridColumn>>(ColumnProperty);
}

void XUI::UI::DataGridCell::SetColumn(std::shared_ptr<DataGridColumn> const & value)
{
	SetValue(ColumnProperty, value);
}

/// <summary>
///     Prepares a cell for use.
/// </summary>
/// <remarks>
///     Updates the column reference.
///     This overload computes the column index from the ItemContainerGenerator.
/// </remarks>

void XUI::UI::DataGridCell::PrepareCell(System::Reflection::Variant const & item, Controls::ItemsControl * cellsPresenter, DataGridRow * ownerRow)
{
    PrepareCell(item, ownerRow, cellsPresenter->GetItemContainerGenerator()->IndexFromContainer(this));
}

/// <summary>
///     Prepares a cell for use.
/// </summary>
/// <remarks>
///     Updates the column reference.
/// </remarks>

void XUI::UI::DataGridCell::PrepareCell(System::Reflection::Variant const & item, DataGridRow * ownerRow, int index)
{
	auto owner = _owner.lock();
    ASSERT(owner == nullptr || owner.get() == ownerRow && "_owner should be null before PrepareCell is called or the same value as the ownerRow.");

    _owner = ownerRow->shared_from_base_static<DataGridRow>();

    DataGrid* dataGrid = ownerRow->DataGridOwner();
    if (dataGrid != nullptr)
    {
        // The index of the container should correspond to the index of the column
        if ((index >= 0) && (index < dataGrid->GetColumns().size()))
        {
            // Retrieve the column definition and pass it to the cell container
            auto& column = dataGrid->GetColumns()[index];
            Column = column;
            TabIndex = column->DisplayIndex;
            CoerceValue(WidthProperty.get());
        }

        if (IsEditing)
        {
            // If IsEditing was left on and this container was recycled, reset it here.
            // Setting this property will result in BuildVisualTree being called.
            IsEditing = false;
        }
        else if (!typeof(Controls::UIElement).IsAssignableFrom(*Content.GetType()))
        {
            // If there isn't already a visual tree, then create one.
            BuildVisualTree(item);

            if (!NeedsVisualTree())
            {
                Content = item.ToSharedObject();
            }
        }

        // Update cell Selection
        bool isSelected = dataGrid->GetSelectedCells().find(this) != dataGrid->GetSelectedCells().end();
        SyncIsSelected(isSelected);
    }
}

/// <summary>
///     Clears the cell of references.
/// </summary>

void XUI::UI::DataGridCell::ClearCell(DataGridRow * ownerRow)
{
    ASSERT(_owner.lock().get() == ownerRow, "_owner should be the same as the DataGridRow that is clearing the cell.");
    _owner.reset();
}

/// <summary>
///     Measure.  This is overridden so that the cell can extend its size to account for a grid line on the right.
/// </summary>

Core::Media::Size XUI::UI::DataGridCell::MeasureOverride(Core::Media::Size const & constraint)
{
    // Make space for the GridLine on the right:
    // Remove space from the constraint (since it implicitly includes the GridLine's thickness), 
    // call the base implementation, and add the thickness back for the returned size.
    if (DataGridOwner()->IsGridLineVisible(/*isHorizontal = */ false))
    {
        double thickness = DataGridOwner()->VerticalGridLineThickness;
		
        Core::Media::Size desiredSize = Controls::ContentControl::MeasureOverride(constraint.Deflate(Core::Media::Thickness(0, 0, thickness, 0)));
        desiredSize.Width += thickness;
        return desiredSize;
    }
    else
    {
        return Controls::ContentControl::MeasureOverride(constraint);
    }
}

/// <summary>
///     Arrange.  This is overriden so that the cell can position its content to account for a grid line on the right.
/// </summary>
/// <param name="arrangeSize">Arrange size</param>

Core::Media::Size XUI::UI::DataGridCell::ArrangeOverride(Core::Media::Size const & arrangeSize)
{
    // We don't need to adjust the Arrange position of the content.  By default it is arranged at 0,0 and we're
    // adding a line to the right.  All we have to do is compress and extend the size, just like Measure.
    if (DataGridOwner()->IsGridLineVisible(/*isHorizontal = */ false))
    {
		double thickness = DataGridOwner()->VerticalGridLineThickness;

		Core::Media::Size returnSize = Controls::ContentControl::ArrangeOverride(arrangeSize.Deflate(Core::Media::Thickness(0, 0, thickness, 0)));
        returnSize.Width += thickness;
        return returnSize;
    }
    else
    {
        return Controls::ContentControl::ArrangeOverride(arrangeSize);
    }
}

/// <summary>
///     OnRender.  Overriden to draw a vertical line on the right.
/// </summary>
/// <param name="drawingContext"></param>

void XUI::UI::DataGridCell::Render(Interfaces::IDrawingContextImpl * drawingContext)
{
    Controls::ContentControl::Render(drawingContext);
    
    if (DataGridOwner()->IsGridLineVisible(/*isHorizontal = */ false))
    {
        double thickness = DataGridOwner()->VerticalGridLineThickness;
        Core::Media::Rect rect(Core::Media::Size(thickness, _bounds.Height));
        rect.X = _bounds.Width - thickness;
        rect.Width = 0;
    
        drawingContext->DrawRectangle(Core::Media::Pen(DataGridOwner()->VerticalGridLinesBrush.get()), rect);
    }
}

void XUI::UI::DataGridCell::OnAnyMouseLeftButtonDown(Interfaces::PointerPressedEventArgs & e)
{
    bool focusWithin = IsFocused;
    bool isCtrlKeyPressed = e.InputModifiers & Interfaces::InputModifiers::Control;
    
    if (focusWithin && !isCtrlKeyPressed && !e.Handled && !IsEditing && !IsReadOnly && IsSelected)
    {
        // The cell is focused and there are no other special selection gestures,
        // enter edit mode.
        DataGrid* dataGridOwner = DataGridOwner();
        if (dataGridOwner != nullptr)
        {
            // The cell was clicked, which means that other cells may
            // need to be de-selected, let the DataGrid handle that.
            dataGridOwner->HandleSelectionForCellInput(this, /* startDragging = */ false, /* allowsExtendSelect = */ e.InputModifiers & Interfaces::InputModifiers::Shift, /* allowsMinimalSelect = */ false);
    
            // Enter edit mode
            dataGridOwner->BeginEdit(&e);
            e.Handled = true;
        }
    }
    else if (!focusWithin || !IsSelected || isCtrlKeyPressed)
    {
        if (!focusWithin)
        {
            auto uiService = Platform::UIService::Instance()->GetCache<Platform::UIRender>();
            // The cell should receive focus on click
            uiService->SetFocusedElement(this);
        }
    
        DataGrid* dataGridOwner = DataGridOwner();
        if (dataGridOwner != nullptr)
        {
            // Let the DataGrid process selection
            dataGridOwner->HandleSelectionForCellInput(this, /* startDragging = */ e.Device->Captured == nullptr, /* allowsExtendSelect = */ e.InputModifiers & Interfaces::InputModifiers::Shift, /* allowsMinimalSelect = */ e.InputModifiers & Interfaces::InputModifiers::Control);
        }
    
        e.Handled = true;
    }
    else
    {
        SendInputToColumn(e);
    }
}

/// <summary>
///     Reporting text composition.
/// </summary>

void XUI::UI::DataGridCell::OnTextInput(Interfaces::TextInputEventArgs & e)
{
    Controls::ContentControl::OnTextInput(e);
    SendInputToColumn(e);
}

/// <summary>
///     Reporting a key was pressed.
/// </summary>

void XUI::UI::DataGridCell::OnKeyDown(Interfaces::KeyEventArgs & e)
{
    Controls::ContentControl::OnKeyDown(e);
    SendInputToColumn(e);
}

/// <summary>
///     Reporting a key was released
/// </summary>

void XUI::UI::DataGridCell::OnKeyUp(Interfaces::KeyEventArgs & e)
{
    Controls::ContentControl::OnKeyUp(e);
    SendInputToColumn(e);
}

/// <summary>
///     Reporting the mouse button was released
/// </summary>

void XUI::UI::DataGridCell::OnPointerReleased(Interfaces::PointerReleasedEventArgs & e)
{
    Controls::ContentControl::OnPointerReleased(e);
    SendInputToColumn(e);
}

void XUI::UI::DataGridCell::OnPointerPressed(Interfaces::PointerPressedEventArgs & e)
{
    Controls::ContentControl::OnPointerPressed(e);
    if (e.InputModifiers & Interfaces::InputModifiers::LeftMouseButton)
        OnAnyMouseLeftButtonDown(e);
}

void XUI::UI::DataGridCell::OnPreviewPointerPressed(Interfaces::PointerPressedEventArgs & e)
{
    Controls::ContentControl::OnPointerPressed(e);
    //SendInputToColumn(e);
}

void XUI::UI::DataGridCell::OnGotFocus(Interfaces::GotFocusEventArgs & e)
{
    Controls::ContentControl::OnGotFocus(e);
    OnAnyGotFocus(this, e);
}

void XUI::UI::DataGridCell::OnLostFocus(Core::Observer::RoutedEventArgs & e)
{
    Controls::ContentControl::OnLostFocus(e);
    OnAnyLostFocus(this, e);
}

void XUI::UI::DataGridCell::SendInputToColumn(Core::Observer::RoutedEventArgs & e)
{
    auto column = Column;
    if (column)
        column->OnInput(e);
}

/// <summary>
///     Notifies the Cell of a property change.
/// </summary>

void XUI::UI::DataGridCell::OnNotifyPropertyChanged(DependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs & e)
{
    static_cast<DataGridCell*>(d)->NotifyPropertyChanged(d, "", e, NotificationTarget::Cells);
}

void XUI::UI::DataGridCell::OnAnyLostFocus(System::Reflection::Object * sender, Core::Observer::RoutedEventArgs & e)
{
    // Get the ancestor cell of old focused element.
    // Set DataGrid.FocusedCell to null, if the cell doesn't
    // have keyboard focus.

    DataGridCell* cell = static_cast<DataGridCell*>(e.Source->VisitVisualAncestors([](Controls::Visual* x) { return x->Is<DataGridCell>(); }));
    if (!cell && e.Source->Is<Controls::Control>())
        cell = static_cast<DataGridCell*>(static_cast<Controls::Control*>(e.Source)->VisitLogicalAncestors([](Controls::Control* x) { return x->Is<DataGridCell>(); }));

    if (cell != nullptr && cell != static_cast<DataGridCell*>(sender))
    {
        DataGrid* owner = cell->DataGridOwner();
        if (owner != nullptr && !cell->IsKeyboardFocused && owner->GetFocusedCell() == cell)
        {
            owner->SetFocusedCell(nullptr);
        }
    }
}

void XUI::UI::DataGridCell::OnAnyGotFocus(System::Reflection::Object * sender, Core::Observer::RoutedEventArgs & e)
{
    DataGridCell* cell = static_cast<DataGridCell*>(e.Source->VisitVisualAncestors([](Controls::Visual* x) { return x->Is<DataGridCell>(); }));
    if (!cell && e.Source->Is<Controls::Control>())
        cell = static_cast<DataGridCell*>(static_cast<Controls::Control*>(e.Source)->VisitLogicalAncestors([](Controls::Control* x) { return x->Is<DataGridCell>(); }));

	if (cell != nullptr && cell == static_cast<DataGridCell*>(sender))
	{
		DataGrid* owner = cell->DataGridOwner();
        if (owner != nullptr)
        {
            owner->SetFocusedCell(cell->shared_from_base_static<DataGridCell>());
        }
    }
}

void XUI::UI::DataGridCell::RaisePreparingCellForEdit(Core::Observer::RoutedEventArgs & editingEventArgs)
{
    DataGrid* dataGridOwner = DataGridOwner();
    if (dataGridOwner != nullptr && dataGridOwner->PreparingCellForEdit)
    {
        Controls::Control* currentEditingElement = GetEditingElement();
		DataGridPreparingCellForEditEventArgs e;
		e.Column = Column.get();
		e.Row = RowOwner();
		e.EditingEventArgs = &editingEventArgs;
		e.EditingElement = currentEditingElement;
        dataGridOwner->PreparingCellForEdit(dataGridOwner, e);
    }
}

void XUI::UI::DataGridCell::NotifyPropertyChanged(DependencyObject* d, std::string propertyName, Core::Dependency::DependencyPropertyChangedEventArgs & e, NotificationTarget target)
{
    DataGridColumn* column = d->Static_As<DataGridColumn>();
    if ((column != nullptr) && (column != Column.get()))
    {
        // This notification does not apply to this cell
        return;
    }
    
    // All the notifications which are to be handled by the cell
    if (target & NotificationTarget::Cells)
    {
        if (e.Property == DataGridColumn::WidthProperty.get())
        {
            DataGridHelper::OnColumnWidthChanged(this, e);
        }
        else if (e.Property == DataGridColumn::ActualWidthProperty.get())
        {
            CoerceValue(WidthProperty.get());
            InvalidateMeasure();
        }
        else if (e.Property == DataGrid::CellStyleProperty.get() || e.Property == DataGridColumn::CellStyleProperty.get() || e.Property == StyleProperty.get())
        {
            CoerceValue(StyleProperty.get());
        }
        else if (e.Property == DataGrid::IsReadOnlyProperty.get() || e.Property == DataGridColumn::IsReadOnlyProperty.get() || e.Property == IsReadOnlyProperty.get())
        {
            CoerceValue(IsReadOnlyProperty.get());
        }
        else if (e.Property == DataGridColumn::DisplayIndexProperty.get())
        {
            TabIndex = column->DisplayIndex;
        }
    }
    
    // All the notifications which needs forward to columns
    if (target & NotificationTarget::RefreshCellContent)
    {
        if (column != nullptr && NeedsVisualTree())
        {
            if (!propertyName.empty())
            {
                column->RefreshCellContent(this, propertyName);
            }
            else if (e.Property != nullptr)
            {
                column->RefreshCellContent(this, e.Property->Name);
            }
        }
    }
}

/// <summary>
///     Builds a column's visual tree if not using templates.
/// </summary>

void XUI::UI::DataGridCell::BuildVisualTree(System::Reflection::Variant const & item)
{
    if (NeedsVisualTree())
    {
        auto column = Column;
        if (column != nullptr)
        {
            Content = column->BuildVisualTree(IsEditing, /*RowDataItem()*/item, this);
        }
    }
}

void XUI::UI::DataGridCell::BeginEdit(Core::Observer::RoutedEventArgs & e)
{
    ASSERT(!IsEditing && "Should not call BeginEdit when IsEditing is true.");

    IsEditing = true;

    DataGridColumn* column = Column.get();
    if (column != nullptr)
    {
        // Ask the column to store the original value
        column->BeginEdit(Content.ToObject() ? Content.ToObject()->Static_As<Controls::Control>() : nullptr, e);
    }

    RaisePreparingCellForEdit(e);
}

void XUI::UI::DataGridCell::CancelEdit()
{
	ASSERT(IsEditing && "Should not call CancelEdit when IsEditing is false.");

    DataGridColumn* column = Column.get();
    if (column != nullptr)
    {
        // Ask the column to restore the original value
        column->CancelEdit(Content.ToObject() ? Content.ToObject()->Static_As<Controls::Control>() : nullptr);
    }

    IsEditing = false;
}

bool XUI::UI::DataGridCell::CommitEdit()
{
	ASSERT(IsEditing && "Should not call CommitEdit when IsEditing is false.");

    bool validationPassed = true;
	DataGridColumn* column = Column.get();
	if (column != nullptr)
    {
        // Ask the column to access the binding and update the data source
        // If validation fails, then remain in editing mode
        validationPassed = column->CommitEdit(Content.ToObject() ? Content.ToObject()->Static_As<Controls::Control>() : nullptr);
    }

    if (validationPassed)
    {
        IsEditing = false;
    }

    return validationPassed;
}

void XUI::UI::DataGridCell::SyncIsSelected(bool isSelected)
{
    bool originalValue = _syncingIsSelected;
    _syncingIsSelected = true;
    try
    {
        IsSelected = isSelected;
    }
    catch (...)
    {
    }

    _syncingIsSelected = originalValue;
}

void XUI::UI::DataGridCellInfo::ConvertCellInfoToIndexes(DataGridCellInfo * cell, int & rowIndex, int & columnIndex)
{
    columnIndex = Column->DisplayIndex;
    rowIndex = Owner->Items->IndexOf(cell->Item);
}
