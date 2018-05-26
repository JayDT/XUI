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
#include "DataGridCellsPresenter.h"
#include "DataGrid/DataGridCell.h"
#include "DataGrid/DataGridRow.h"
#include "DataGrid/DataGridControl.h"
#include "Core/Media/Pen.h"
#include "UI/VirtualizingStackPanel.h"
#include "XamlCPP/DataBind/Binding.h"
#include "Core/GlobalInitializerScheduler.h"

STATIC_CLASS_INITIALIZER(DataGridCellsPresenter, XUI::UI::DataGridCellsPresenter::StaticClassInitializer);

using namespace XUI;
using namespace XUI::UI;

Core::Dependency::RefDependencyProperty DataGridCellsPresenter::ItemProperty;

void XUI::UI::DataGridCellsPresenter::StaticClassInitializer()
{
	ItemProperty = (Core::Dependency::BindProperty<System::Reflection::Variant>::Register<DataGridCellsPresenter>(nameof(Item)));

	ItemProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const & e)
	{
		DataGridCellsPresenter* control = static_cast<DataGridCellsPresenter*>(e.Sender);
		if (control)
			control->OnItemChanged((Core::Dependency::DependencyPropertyChangedEventArgs&)e);
	});

    MinHeightProperty->OverrideMetadata<DataGridCellsPresenter>(std::make_shared<Core::Dependency::BindPropertyMetadata>(
        double(0.0),
        nullptr,
        Core::Dependency::BindingMode::OneWay,
        [](IDependencyObject* d, System::Reflection::Variant const& baseValue) -> System::Reflection::Variant
        {
            auto control = d->Static_As<DataGridCellsPresenter>();
            if (control)
                return DataGridHelper::GetCoercedTransferPropertyValue(control, baseValue, MinHeightProperty.get(), control->DataGridOwner(), DataGrid::MinRowHeightProperty.get());
            return System::Reflection::Variant::Empty;
        }));

    MinHeightProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const & e)
    {
        DataGridCellsPresenter* control = static_cast<DataGridCellsPresenter*>(e.Sender);
        if (control)
            control->NotifyPropertyChanged(control, "", (XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs&)e, NotificationTarget::CellsPresenter);
    });

    HeightProperty->OverrideMetadata<DataGridCellsPresenter>(std::make_shared<Core::Dependency::BindPropertyMetadata>(
        double(0.0),
        nullptr,
        Core::Dependency::BindingMode::OneWay,
        [](IDependencyObject* d, System::Reflection::Variant const& baseValue) -> System::Reflection::Variant
    {
        auto control = d->Static_As<DataGridCellsPresenter>();
        if (control)
            return DataGridHelper::GetCoercedTransferPropertyValue(control, baseValue, HeightProperty.get(), control->DataGridOwner(), DataGrid::RowHeightProperty.get());
        return System::Reflection::Variant::Empty;
    }));

    HeightProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const & e)
    {
        DataGridCellsPresenter* control = e.Sender->Static_As<DataGridCellsPresenter>();
        if (control)
            control->NotifyPropertyChanged(control, "", (XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs&)e, NotificationTarget::CellsPresenter);
    });

    FocusableProperty->OverrideDefaultValue<DataGridCellsPresenter>(false);
}

XUI::UI::DataGridCellsPresenter::DataGridCellsPresenter()
{
    Items = std::make_shared<System::Collection::DynamicList>();
}

XUI::UI::DataGridCellsPresenter::~DataGridCellsPresenter()
{
}

DataGrid * XUI::UI::DataGridCellsPresenter::DataGridOwner() const
{
    return DataGridRowOwner() ? DataGridRowOwner()->DataGridOwner() : nullptr;
}

/// <summary>
///     The DataGridRow that owns this control.
/// </summary>

DataGridRow * XUI::UI::DataGridCellsPresenter::DataGridRowOwner() const
{
    return _dataGridRowOwner.lock().get();
}

void XUI::UI::DataGridCellsPresenter::OnTemplateApplied(Controls::TemplateAppliedEventArgs const & e)
{
//#if BindingGroups
//    if (ItemBindingGroup == null)
//    {
//        ItemBindingGroup = new BindingGroup();
//    }
//#endif

	Controls::ItemsControl::OnTemplateApplied(e);

    auto rowOwnerObj = (VisitLogicalAncestors([](Controls::Control* parent)
    {
        return parent->Is<DataGridRow>();
    }));

    if (rowOwnerObj)
        _dataGridRowOwner = rowOwnerObj->shared_from_base_static<DataGridRow>();

    DataGridRow* owningRow = DataGridRowOwner();
    if (owningRow != nullptr)
    {
        owningRow->CellsPresenter = this;
        Item = owningRow->Item;
    }

    // At the time that a Row is prepared we can't Sync because the CellsPresenter isn't created yet.
    // Doing it here ensures that the CellsPresenter is in the visual tree.
    SyncProperties(false);
}

/// <summary>
///     Update all properties that get a value from the DataGrid
/// </summary>
/// <remarks>
///     See comment on DataGridRow.SyncProperties
/// </remarks>

void XUI::UI::DataGridCellsPresenter::SyncProperties(bool forcePrepareCells)
{
    auto dataGridOwner = DataGridOwner();
    if (dataGridOwner == nullptr)
    {
        return;
    }
    
    CoerceValue(HeightProperty.get());
    CoerceValue(MinHeightProperty.get());
    CoerceValue(VirtualizingStackPanel::IsVirtualizingProperty.get());
    
    // This is a convenient way to walk through all cells and force them to call CoerceValue(StyleProperty)
    Core::Dependency::DependencyPropertyChangedEventArgs e(nullptr, DataGrid::CellStyleProperty.get(), System::Reflection::Variant::Empty, System::Reflection::Variant::Empty);
    NotifyPropertyChanged(this, e, NotificationTarget::Cells);
    
    // We may have missed an Add / Remove of a column from the grid (DataGridRow.OnColumnsChanged)
    // Sync the MultipleCopiesCollection count and update the Column on changed cells
    if (Items != nullptr)
    {
        DataGridCell* cell;
        auto const& columns = dataGridOwner->GetColumns();
        int newColumnCount = columns.size();
        int oldColumnCount = Items->size();
        int dirtyCount = 0;
    
        if (newColumnCount != oldColumnCount)
        {
            System::Collection::NotifyCollectionChangedEventArgs e;
            OnColumnsChanged(columns, e);
    
            // Newly added or removed containers will be updated by the generator via PrepareContainer. 
            // All others may have a different column
            dirtyCount = std::min(newColumnCount, oldColumnCount);
        }
        else if (forcePrepareCells)
        {
            dirtyCount = newColumnCount;
        }
    
        DataGridRow* row = DataGridRowOwner();
        bool arrangeInvalidated = false;

        // Prepare the cells until dirtyCount is reached. Also invalidate the cells panel's measure
        // and arrange if there is a mismatch between cell.ActualWidth and Column.Width.DisplayValue
        for (int i = 0; i < dirtyCount; i++)
        {
            auto cellObj = GetItemContainerGenerator()->ContainerFromIndex(i);
            if (cellObj != nullptr)
            {
                cell = cellObj->Static_As<DataGridCell>();
                if (cell != nullptr)
                {
                    cell->PrepareCell(row->Item, this, row);
                    if (!arrangeInvalidated && cell->ActualWidth() != columns[i]->ActualWidth)
                    {
                        InvalidateDataGridCellsPanelMeasureAndArrange();
                        arrangeInvalidated = true;
                    }
                }
            }
        }
    
        // Keep searching for the mismatch between cell.ActualWidth
        // and Column.Width.DisplayValue
        if (!arrangeInvalidated)
        {
            for (int i = dirtyCount; i < newColumnCount; i++)
            {
                auto cellObj = GetItemContainerGenerator()->ContainerFromIndex(i);
                if (cellObj != nullptr)
                {
                    cell = cellObj->Static_As<DataGridCell>();
                    if (cell != nullptr)
                    {
                        if (cell->ActualWidth() != columns[i]->ActualWidth)
                        {
                            InvalidateDataGridCellsPanelMeasureAndArrange();
                            arrangeInvalidated = true;
                            break;
                        }
                    }
                }
            }
        }
    
        if (!arrangeInvalidated && InvalidateCellsPanelOnColumnChange())
        {
            InvalidateDataGridCellsPanelMeasureAndArrange();
        }
    }
}

/// <summary>
/// Notification of Height & MinHeight changes.
/// </summary>

void XUI::UI::DataGridCellsPresenter::OnNotifyHeightPropertyChanged(DependencyObject * d, Core::Dependency::DependencyPropertyChangedEventArgs & e)
{
    ((DataGridCellsPresenter*)d)->NotifyPropertyChanged(d, e, NotificationTarget::CellsPresenter);
}

/// <summary>
///     General notification for DependencyProperty changes from the grid or from columns.
/// </summary>

void XUI::UI::DataGridCellsPresenter::NotifyPropertyChanged(DependencyObject * d, Core::Dependency::DependencyPropertyChangedEventArgs & e, NotificationTarget target)
{
    NotifyPropertyChanged(d, "", e, target);
}

/// <summary>
///     General notification for DependencyProperty changes from the grid or from columns.
/// </summary>

void XUI::UI::DataGridCellsPresenter::NotifyPropertyChanged(DependencyObject * d, std::string const & propertyName, Core::Dependency::DependencyPropertyChangedEventArgs & e, NotificationTarget target)
{
    if (target & NotificationTarget::CellsPresenter)
    {
        if (e.Property == DataGridColumn::WidthProperty.get() ||
            e.Property == DataGridColumn::DisplayIndexProperty.get())
        {
            if (static_cast<DataGridColumn*>(d)->IsVisible())
            {
                InvalidateDataGridCellsPanelMeasureAndArrange();
            }

            if (Presenter && Presenter->Panel)
            {
                Presenter->Panel->Children.GetNativeEnumerator().sort([](std::shared_ptr<Control> const& lhs, std::shared_ptr<Control> const& rhs) -> bool
                {
                    DataGridCell* leftheader = lhs->Static_As<DataGridCell>();
                    DataGridCell* rightheader = rhs->Static_As<DataGridCell>();
                    if (leftheader && leftheader->Column && rightheader && rightheader->Column)
                        return leftheader->Column->DisplayIndex < rightheader->Column->DisplayIndex;
                    return false;
                });

                Presenter->Panel->InvalidateArrange();
            }
        }
        else if (e.Property == DataGrid::FrozenColumnCountProperty.get() ||
            e.Property == DataGridColumn::VisibilityProperty.get() ||
            e.Property == DataGrid::CellsPanelHorizontalOffsetProperty.get() ||
            e.Property == DataGrid::HorizontalScrollOffsetProperty.get() ||
            propertyName == "ViewportWidth" ||
            propertyName == "DelayedColumnWidthComputation")
        {
            InvalidateDataGridCellsPanelMeasureAndArrange();
        }
        else if (propertyName == "RealizedColumnsBlockListForNonVirtualizedRows")
        {
            InvalidateDataGridCellsPanelMeasureAndArrange(/* withColumnVirtualization */ false);
        }
        else if (propertyName == "RealizedColumnsBlockListForVirtualizedRows")
        {
            InvalidateDataGridCellsPanelMeasureAndArrange(/* withColumnVirtualization */ true);
        }
        else if (e.Property == DataGrid::RowHeightProperty.get() || e.Property == HeightProperty.get())
        {
            CoerceValue(HeightProperty.get());
        }
        else if (e.Property == DataGrid::MinRowHeightProperty.get() || e.Property == MinHeightProperty.get())
        {
            CoerceValue(MinHeightProperty.get());
        }
        else if (e.Property == DataGrid::EnableColumnVirtualizationProperty.get())
        {
            CoerceValue(VirtualizingStackPanel::IsVirtualizingProperty.get());
        }
    }
    
    if (target & (NotificationTarget::Cells | NotificationTarget::RefreshCellContent))
    {
        for (auto iCell = DataGridRowOwner()->GetCells().begin(); iCell != DataGridRowOwner()->GetCells().end();)
        {
            auto cell = iCell->getTarget();
            ++iCell;

            cell->NotifyPropertyChanged(d, propertyName, e, target);
        }
    }
}

/// <summary>
///     Method which tries to scroll a cell for given index into the scroll view
/// </summary>
/// <param name="index"></param>

void XUI::UI::DataGridCellsPresenter::ScrollCellIntoView(int index)
{
    //DataGridCellsPanel itemsHost = InternalItemsHost as DataGridCellsPanel;
    //if (itemsHost != null)
    //{
    //    itemsHost.InternalBringIndexIntoView(index);
    //    return;
    //}
}

DataGridColumnCollection* XUI::UI::DataGridCellsPresenter::GetColumns()
{
    DataGridRow* owningRow = DataGridRowOwner();
    if (owningRow != nullptr && owningRow->DataGridOwner())
		return &owningRow->DataGridOwner()->GetColumns();
	return nullptr;
}

/// <summary>
///     Called when the value of the Item property changes.
/// </summary>
/// <param name="oldItem">The old value of Item.</param>
/// <param name="newItem">The new value of Item.</param>

void XUI::UI::DataGridCellsPresenter::OnItemChanged(Core::Dependency::DependencyPropertyChangedEventArgs & e)
{
	DataGridColumnCollection* columns = GetColumns();
    
    // ToDo: Hax for the win :D
    if (columns != nullptr && e.NewValue.GetBase())
    {
        if (e.NewValue.GetType()->IsAssignableFrom<System::Collection::Generic::IContainer>())
        {
            if (e.NewValue.GetBase()->IsSharedPointer())
            {
                Items = e.NewValue.TryGetValue<std::shared_ptr<System::Collection::Generic::IContainer>>(); // For WinForm like usage
            }
        }
        else // Binding base usage
        {
            Items->Clear();

            for (std::shared_ptr<DataGridColumn> const& column : *columns)
                Items->Add(e.NewValue);
        }
    }
    else
    {
        Items->Clear();
    }
}

std::shared_ptr<Generators::IItemContainerGenerator> XUI::UI::DataGridCellsPresenter::CreateItemContainerGenerator()
{
    auto itemGenerator = std::make_shared<Generators::ItemContainerGenerator<DataGridCell>>(this, Controls::ContentControl::DataContextProperty.get(), Controls::ContentControl::ContentTemplateProperty.get());

    itemGenerator->Materialized += System::make_eventhandler(&DataGridCellsPresenter::OnMaterializedHeader, this);
    itemGenerator->Dematerialized += System::make_eventhandler(&DataGridCellsPresenter::OnDematerializedHeader, this);

    return itemGenerator;
}

void XUI::UI::DataGridCellsPresenter::OnMaterializedHeader(void * sender, Interfaces::ItemContainerEventArgs & e)
{
    auto items = static_cast<System::Collection::Vector<Interfaces::ItemContainerInfo>*>(e.Container);
    for (int i = 0; i < items->size(); ++i)
    {
        auto item = (*items)[e.StartingIndex + i];
        auto Cell = item.ContainerControl->Static_As<UI::DataGridCell>();
        if (Cell)
        {
            Cell->_cellReference.link(Cell, DataGridRowOwner());
            Cell->_presenter = this;
            Cell->PrepareCell(item.Item, DataGridRowOwner(), item.Index);
        }
    }
}

void XUI::UI::DataGridCellsPresenter::OnDematerializedHeader(void * sender, Interfaces::ItemContainerEventArgs & e)
{
    auto items = static_cast<System::Collection::Vector<Interfaces::ItemContainerInfo>*>(e.Container);
    for (int i = 0; i < items->size(); ++i)
    {
        auto item = (*items)[e.StartingIndex + i];
        auto Cell = item.ContainerControl->Static_As<UI::DataGridCell>();
        if (Cell)
        {
            Cell->_cellReference.unlink();
            Cell->_presenter = nullptr;
            Cell->Content = nullptr;
            Cell->Cleanup();
        }
    }
}

/// <summary>
///     Determines if an item is its own container.
/// </summary>
/// <param name="item">The item to test.</param>
/// <returns>true if the item is a DataGridCell, false otherwise.</returns>

bool XUI::UI::DataGridCellsPresenter::IsItemItsOwnContainerOverride(System::Reflection::Variant const & item)
{
    return true; // item is DataGridCell;
}

/// <summary>
///     Instantiates an instance of a container.
/// </summary>
/// <returns>A new DataGridCell.</returns>

std::shared_ptr<Core::Dependency::DependencyObject> XUI::UI::DataGridCellsPresenter::GetContainerForItemOverride() //override
{
    return std::make_shared<DataGridCell>();
}

/// <summary>
///     Prepares a new container for a given item.
/// </summary>
/// <param name="element">The new container.</param>
/// <param name="item">The item that the container represents.</param>

void XUI::UI::DataGridCellsPresenter::PrepareContainerForItemOverride(DependencyObject* element, System::Reflection::Variant const & item) //override
{
    //DataGridCell cell = (DataGridCell)element;
    //DataGridRow rowOwner = DataGridRowOwner;
    //
    //if (cell.RowOwner != rowOwner)
    //{
    //    cell.Tracker.StartTracking(ref _cellTrackingRoot);
    //}
    //
    //cell.PrepareCell(item, this, rowOwner);
}

/// <summary>
///     Clears a container of references.
/// </summary>
/// <param name="element">The container being cleared.</param>
/// <param name="item">The data item that the container represented.</param>

void XUI::UI::DataGridCellsPresenter::ClearContainerForItemOverride(DependencyObject* element, System::Reflection::Variant const & item) //override
{
    //DataGridCell cell = (DataGridCell)element;
    //DataGridRow rowOwner = DataGridRowOwner;
    //
    //if (cell.RowOwner == rowOwner)
    //{
    //    cell.Tracker.StopTracking(ref _cellTrackingRoot);
    //}
    //
    //cell.ClearCell(rowOwner);
}


/// <summary>
///     Notification from the DataGrid that the columns collection has changed.
/// </summary>
/// <param name="columns">The columns collection.</param>
/// <param name="e">The event arguments from the collection's change event.</param>

void XUI::UI::DataGridCellsPresenter::OnColumnsChanged(System::Collection::Vector<std::shared_ptr<DataGridColumn>> const & column, System::Collection::NotifyCollectionChangedEventArgs & e)
{
    DataGridColumnCollection* columns = GetColumns();

    // ToDo: Hax for the win :D
    if (columns != nullptr)
    {
        if (DataGridRowOwner()->Item.GetType()->IsAssignableFrom<System::Collection::Generic::IContainer>())
        {
            if (DataGridRowOwner()->Item.GetBase()->IsSharedPointer())
            {
                Items = DataGridRowOwner()->Item.TryGetValue<std::shared_ptr<System::Collection::Generic::IContainer>>(); // For WinForm like usage
            }
        }
        else // Binding base usage
        {
            Items->Clear();
    
            for (std::shared_ptr<DataGridColumn> const& column : *columns)
                Items->Add(DataGridRowOwner()->Item);
        }
    }
    else
    {
        Items->Clear();
    }
    
    // For a reset event the only thing the MultipleCopiesCollection can do is set its count to 0.
    ASSERT(
        e.Action != System::Collection::NotifyCollectionChangedAction::Reset || columns->empty() &&
        "A Reset event should only be fired for a Clear event from the columns collection");
}

/// <summary>
///     Measure.  This is overridden so that the row can extend its size to account for a grid line on the bottom.
/// </summary>
/// <param name="availableSize"></param>
/// <returns></returns>

Core::Media::Size XUI::UI::DataGridCellsPresenter::MeasureOverride(Core::Media::Size const & availableSize)
{
    // Make space for the GridLine on the bottom.
    // Remove space from the constraint (since it implicitly includes the GridLine's thickness), 
    // call the base implementation, and add the thickness back for the returned size.
    auto row = DataGridRowOwner();
    if (!row)
    {
        return Controls::ItemsControl::MeasureOverride(availableSize);
    }
    
    auto dataGrid = row->DataGridOwner();
    if (!dataGrid)
    {
        return Controls::ItemsControl::MeasureOverride(availableSize);
    }
    
    if (dataGrid->IsGridLineVisible(/*isHorizontal = */ true))
    {
        double thickness = dataGrid->HorizontalGridLineThickness;
		Core::Media::Size desiredSize = Controls::ItemsControl::MeasureOverride(availableSize.Deflate(Core::Media::Thickness(0, 0, 0, thickness))); //DataGridHelper.SubtractFromSize(availableSize, thickness, /*height = */ true));
        desiredSize.Height += thickness;
        return desiredSize;
    }
    else
    {
        return Controls::ItemsControl::MeasureOverride(availableSize);
    }
}

/// <summary>
///     Arrange.  This is overriden so that the row can position its content to account for a grid line on the bottom.
/// </summary>
/// <param name="finalSize">Arrange size</param>

Core::Media::Size XUI::UI::DataGridCellsPresenter::ArrangeOverride(Core::Media::Size const & finalSize)
{
    // We don't need to adjust the Arrange position of the content.  By default it is arranged at 0,0 and we're
    // adding a line to the bottom.  All we have to do is compress and extend the size, just like Measure.
    auto row = DataGridRowOwner();
    if (!row)
    {
        return Controls::ItemsControl::ArrangeOverride(finalSize);
    }
    
	auto dataGrid = row->DataGridOwner();
	if (!dataGrid)
	{
        return Controls::ItemsControl::ArrangeOverride(finalSize);
    }
    
	if (dataGrid->IsGridLineVisible(/*isHorizontal = */ true))
	{
        double thickness = dataGrid->HorizontalGridLineThickness;
		Core::Media::Size returnSize = Controls::ItemsControl::ArrangeOverride(finalSize.Deflate(Core::Media::Thickness(0, 0, 0, thickness))); //DataGridHelper.SubtractFromSize(finalSize, thickness, /*height = */ true));
        returnSize.Height += thickness;
        return returnSize;
    }
    else
    {
        return Controls::ItemsControl::ArrangeOverride(finalSize);
    }
}


/// <summary>
///     OnRender.  Overriden to draw a horizontal line underneath the content.
/// </summary>
/// <param name="drawingContext"></param>

void XUI::UI::DataGridCellsPresenter::Render(Interfaces::IDrawingContextImpl * drawingContext)
{
    Controls::ItemsControl::Render(drawingContext);

    auto row = DataGridRowOwner();
    if (!row)
    {
        return;
    }
    
    auto dataGrid = row->DataGridOwner();
    if (!dataGrid)
    {
        return;
    }
    
    if (dataGrid->IsGridLineVisible(/*isHorizontal = */ true))
    {
        double thickness = dataGrid->HorizontalGridLineThickness;
        Core::Media::Rect rect(Core::Media::Size(_bounds.Width, thickness));
        rect.Y = _bounds.Height - thickness;
        rect.Height = 0;
    
        drawingContext->DrawRectangle(Core::Media::Pen(dataGrid->HorizontalGridLinesBrush.get()), rect);
    }
}

bool XUI::UI::DataGridCellsPresenter::InvalidateCellsPanelOnColumnChange()
{
    //if (InternalItemsHost == null)
    //    return false;
    //
    //bool isVirtualizing = VirtualizingStackPanel.GetIsVirtualizing(this);
    //List<RealizedColumnsBlock> blockList = null;
    //
    //if (isVirtualizing && !DataGridOwner.InternalColumns.RebuildRealizedColumnsBlockListForVirtualizedRows)
    //{
    //    blockList = DataGridOwner.InternalColumns.RealizedColumnsBlockListForVirtualizedRows;
    //}
    //else if (!isVirtualizing && !DataGridOwner.InternalColumns.RebuildRealizedColumnsBlockListForNonVirtualizedRows)
    //{
    //    blockList = DataGridOwner.InternalColumns.RealizedColumnsBlockListForNonVirtualizedRows;
    //}
    //
    //// either RebuildRealizedColumnsBlockListForNonVirtualizedRows or RebuildRealizedColumnsBlockListForVirtualizedRows is true 
    //if (blockList == null)
    //{
    //    return true;
    //}
    //
    //IList children = InternalItemsHost.Children;
    //int lastRealizedColumnBlockIndex = 0, lastChildrenIndex = 0;
    //int childrenCount = children.Count;
    //int blockListCount = blockList.Count;
    //int allColumnsCount = DataGridOwner.Columns.Count;
    //bool foundInRealizedColumns, foundInCells;
    //
    //for (int i = 0; i < allColumnsCount; i++)
    //{
    //    foundInRealizedColumns = false;
    //    foundInCells = false;
    //
    //    // find column in RealizedColumns
    //    if (lastRealizedColumnBlockIndex < blockListCount)
    //    {
    //        RealizedColumnsBlock rcb = blockList[lastRealizedColumnBlockIndex];
    //        if ((rcb.StartIndex <= i) && (i <= rcb.EndIndex))
    //        {
    //            foundInRealizedColumns = true;
    //            // reached end of this block, start from next block
    //            if (i == rcb.EndIndex)
    //            {
    //                lastRealizedColumnBlockIndex++;
    //            }
    //        }
    //    }
    //
    //    // find column in Children
    //    if (lastChildrenIndex < childrenCount)
    //    {
    //        DataGridCell cell = children[lastChildrenIndex] as DataGridCell;
    //        if (DataGridOwner.Columns[i] == cell.Column)
    //        {
    //            foundInCells = true;
    //            lastChildrenIndex++;
    //        }
    //    }
    //
    //    // if found in one but not the other or reached end of either list.
    //    bool reachedEndofBlockList = (lastRealizedColumnBlockIndex == blockListCount);
    //    bool reachedEndofChildrenList = (lastChildrenIndex == childrenCount);
    //    if ((foundInCells != foundInRealizedColumns) || (reachedEndofBlockList != reachedEndofChildrenList))
    //    {
    //        return true;
    //    }
    //    else if (reachedEndofBlockList == true)
    //    {
    //        //reached end of both columns and children lists
    //        break;
    //    }
    //}
    return false;
}

/// <summary>
///     Property changed callback for VirtualizingStackPanel.IsVirtualizing property
/// </summary>

void XUI::UI::DataGridCellsPresenter::OnIsVirtualizingPropertyChanged(DependencyObject * d, Core::Dependency::DependencyPropertyChangedEventArgs & e)
{
    //DataGridCellsPresenter cellsPresenter = (DataGridCellsPresenter)d;
    //DataGridHelper.TransferProperty(cellsPresenter, VirtualizingStackPanel.IsVirtualizingProperty);
    //if (e.OldValue != cellsPresenter.GetValue(VirtualizingStackPanel.IsVirtualizingProperty))
    //{
    //    cellsPresenter.InvalidateDataGridCellsPanelMeasureAndArrange();
    //}
}

/// <summary>
///     Helper method which invalidate the underlying itemshost's measure and arrange
/// </summary>

void XUI::UI::DataGridCellsPresenter::InvalidateDataGridCellsPanelMeasureAndArrange()
{
    if (_internalItemsHost != nullptr)
    {
        _internalItemsHost->InvalidateMeasure();
        _internalItemsHost->InvalidateArrange();
    }
}

/// <summary>
///     Helper method which invalidate the underlying itemshost's measure and arrange
/// </summary>
/// <param name="withColumnVirtualization">
///     True to invalidate only when virtualization is on.
///     False to invalidate only when virtualization is off.
/// </param>

void XUI::UI::DataGridCellsPresenter::InvalidateDataGridCellsPanelMeasureAndArrange(bool withColumnVirtualization)
{
    // Invalidates measure and arrange if the flag and the virtualization
    // are either both true or both false.
    //if (withColumnVirtualization == VirtualizingStackPanel::GetIsVirtualizing(this))
    //{
    //    InvalidateDataGridCellsPanelMeasureAndArrange();
    //}
}
