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

#include "DataGridColumnHeaderPresenter.h"
#include "DataGrid/DataGridControl.h"
#include "DataGrid/DataGridColumn.h"
#include "DataGrid/DataGridColumnHeader.h"
#include "DataGrid/DataGridColumnFloatingHeader.h"
#include "UI/VirtualizingStackPanel.h"
#include "Core/GlobalInitializerScheduler.h"

STATIC_CLASS_INITIALIZER(DataGridColumnHeadersPresenter, XUI::UI::DataGridColumnHeadersPresenter::StaticClassInitializer)

using namespace XUI;
using namespace XUI::UI;

void XUI::UI::DataGridColumnHeadersPresenter::StaticClassInitializer()
{
    MemberSelectorProperty->OverrideDefaultValue<DataGridColumnHeadersPresenter>(System::Delegate<System::Reflection::Variant(System::Reflection::Variant const&)>(DataGridColumnHeadersPresenter::SelectHeader));

    ItemsPanelProperty->OverrideDefaultValue<DataGridColumnHeadersPresenter>(std::make_shared<DataTemplates::FuncTemplate<std::shared_ptr<XUI::UI::Panel>>>([]()
    {
        auto panel = std::make_shared<VirtualizingStackPanel>();
        panel->Orientation = Orientation::Horizontal;
        return panel;
    }));
}

XUI::UI::DataGridColumnHeadersPresenter::DataGridColumnHeadersPresenter()
{
    Items = std::make_shared<System::Collection::Vector<std::shared_ptr<DataGridColumn>>>();
}

XUI::UI::DataGridColumnHeadersPresenter::~DataGridColumnHeadersPresenter()
{
}

System::Reflection::Variant DataGridColumnHeadersPresenter::SelectHeader(System::Reflection::Variant const& o)
{
    if (o.GetType()->IsAssignableFrom<UI::DataGridColumn>())
    {
        auto headered = o.TryGetValue<UI::DataGridColumn*>();
        if (headered)
            return headered->Header;
    }

    return std::string("");
}

void XUI::UI::DataGridColumnHeadersPresenter::OnMaterializedHeader(void * sender, Interfaces::ItemContainerEventArgs & e)
{
    auto items = static_cast<System::Collection::Vector<Interfaces::ItemContainerInfo>*>(e.Container);
    for (int i = 0; i < items->size(); ++i)
    {
        auto item = (*items)[e.StartingIndex + i];
        auto header = item.ContainerControl->Static_As<UI::DataGridColumnHeader>();
        auto column = item.Item.GetValue<std::shared_ptr<UI::DataGridColumn>>();
        if (header)
        {
            header->_parentPresenter = this;
            header->_columnHeaderReference.link(header, this);
            header->PrepareColumnHeader(column->Header, column.get());
        }
    }
}

void XUI::UI::DataGridColumnHeadersPresenter::OnDematerializedHeader(void * sender, Interfaces::ItemContainerEventArgs & e)
{
    auto items = static_cast<System::Collection::Vector<Interfaces::ItemContainerInfo>*>(e.Container);
    for (int i = 0; i < items->size(); ++i)
    {
        auto item = (*items)[e.StartingIndex + i];
        auto header = item.ContainerControl->Static_As<UI::DataGridColumnHeader>();
        auto column = item.Item.GetValue<std::shared_ptr<UI::DataGridColumn>>();
        if (header)
        {
            header->_columnHeaderReference.unlink();
            header->ClearHeader();
        }
    }
}

/// <summary>
///     Tells the row owner about this element.
/// </summary>

void XUI::UI::DataGridColumnHeadersPresenter::OnTemplateApplied(Controls::TemplateAppliedEventArgs const & e)
{
    Controls::ItemsControl::OnTemplateApplied(e);

    //// Find the columns collection and set the ItemsSource.
    _dataGridOwner = static_cast<DataGrid*>(VisitLogicalAncestors([](Controls::Control* parent) { return parent->Is<DataGrid>(); }));
    
    if (_dataGridOwner != nullptr)
    {
        auto columsVector = static_cast<System::Collection::Vector<std::shared_ptr<DataGridColumn>>*>(Items.get());

        columsVector->Clear();
        for (auto const& column : _dataGridOwner->GetColumns())
            columsVector->Add(column);

        _dataGridOwner->ColumnChanged += System::make_eventhandler(&DataGridColumnHeadersPresenter::OnDataGridColumnsChanged, this);
        _dataGridOwner->SetColumnHeaderPresenterInternal(this);
        CoerceValue(VirtualizingStackPanel::IsVirtualizingProperty.get());
        
        DataGridColumnHeader* fillerColumnHeader = Template->FindNameByManagedType<DataGridColumnHeader>(ElementFillerColumnHeader, this).get();
        if (fillerColumnHeader != nullptr)
        {
            fillerColumnHeader->CoerceValue(DataGridColumnHeader::StyleProperty.get());
            fillerColumnHeader->CoerceValue(DataGridColumnHeader::HeightProperty.get());
        }
    }
    else
    {
        if (_dataGridOwner)
            _dataGridOwner->ColumnChanged -= System::make_eventhandler(&DataGridColumnHeadersPresenter::OnDataGridColumnsChanged, this);

        _dataGridOwner = nullptr;
        Items = nullptr;
    }
}

/// <summary>
///     Notification for column header-related DependencyProperty changes from the grid or from columns.
/// </summary>

void XUI::UI::DataGridColumnHeadersPresenter::NotifyPropertyChanged(DependencyObject * d, std::string const & propertyName, Core::Dependency::DependencyPropertyChangedEventArgs & e, NotificationTarget target)
{
    DataGridColumn* column = static_cast<DataGridColumn*>(d);
    if (target & NotificationTarget::ColumnHeadersPresenter)
    {
        if (e.Property == DataGridColumn::WidthProperty.get() ||
            e.Property == DataGridColumn::DisplayIndexProperty.get())
        {
            if (column->IsVisible())
            {
                InvalidateDataGridCellsPanelMeasureAndArrange();
            }

            if (Presenter && Presenter->Panel)
            {
                Presenter->Panel->Children.GetNativeEnumerator().sort([](std::shared_ptr<Control> const& lhs, std::shared_ptr<Control> const& rhs) -> bool
                {
                    DataGridColumnHeader* leftheader = lhs->Static_As<DataGridColumnHeader>();
                    DataGridColumnHeader* rightheader = rhs->Static_As<DataGridColumnHeader>();
                    if (leftheader && leftheader->Column() && rightheader && rightheader->Column())
                        return leftheader->Column()->DisplayIndex < rightheader->Column()->DisplayIndex;
                    return false;
                });

                Presenter->Panel->InvalidateArrange();
            }
        }
        else if (e.Property == DataGrid::FrozenColumnCountProperty.get() ||
                 e.Property == DataGridColumn::VisibilityProperty.get() ||
                 e.Property == DataGrid::CellsPanelHorizontalOffsetProperty.get() ||
                 e.Property->Name == "ViewportWidth" ||
                 e.Property->Name == "DelayedColumnWidthComputation")
        {
            InvalidateDataGridCellsPanelMeasureAndArrange();
        }
        else if (e.Property == DataGrid::HorizontalScrollOffsetProperty.get())
        {
            InvalidateArrange();
            InvalidateDataGridCellsPanelMeasureAndArrange();
        }
        else if (e.Property->Name == "RealizedColumnsBlockListForNonVirtualizedRows")
        {
            InvalidateDataGridCellsPanelMeasureAndArrange(/* withColumnVirtualization */ false);
        }
        else if (e.Property->Name == "RealizedColumnsBlockListForVirtualizedRows")
        {
            InvalidateDataGridCellsPanelMeasureAndArrange(/* withColumnVirtualization */ true);
        }
        else if (e.Property->Name == "CellsPanelActualWidthProperty" /*e.Property == DataGrid::CellsPanelActualWidthProperty.get()*/)
        {
            InvalidateArrange();
        }
        else if (e.Property == DataGrid::EnableColumnVirtualizationProperty.get())
        {
            //DataGridHelper.TransferProperty(this, VirtualizingStackPanel.IsVirtualizingProperty);
        }
    }
    
    if (target & NotificationTarget::ColumnHeaders)
    {
        if (e.Property == DataGridColumn::HeaderProperty.get())
        {
            if (Items != nullptr)
            {
                ASSERT(Items->Contains(column->shared_from_base_static<DataGridColumn>()));

                System::Collection::NotifyCollectionChangedEventArgs e;
                e.Action = System::Collection::NotifyCollectionChangedAction::Replace;
                e.NewStartingIndex = Items->IndexOf(column);
                e.OldStartingIndex = Items->IndexOf(column);

                if (Items->CollectionChanged)
                    Items->CollectionChanged(Items.get(), e);
            }
        }
        else
        {
            for (auto iHeader = GetHeaders().begin(); iHeader != GetHeaders().end();)
            {
                auto header = iHeader->getTarget();
                ++iHeader;
                header->NotifyPropertyChanged(d, e);
            }
            
            // Handle Style & Height change notification for PART_FillerColumnHeader.
            if (d->Is<DataGrid>() &&
                (e.Property == DataGrid::ColumnHeaderStyleProperty.get() || e.Property == DataGrid::ColumnHeaderHeightProperty.get()))
            {
                auto fillerColumnHeader = Template->FindNameByManagedType<DataGridColumnHeader>(ElementFillerColumnHeader, this);
                if (fillerColumnHeader != nullptr)
                {
                    fillerColumnHeader->NotifyPropertyChanged(d, e);
                }
            }
        }
    }
}

void XUI::UI::DataGridColumnHeadersPresenter::OnDataGridColumnsChanged(void * sender, System::Collection::NotifyCollectionChangedEventArgs & e)
{
    auto columsVector = static_cast<System::Collection::Vector<std::shared_ptr<DataGridColumn>>*>(Items.get());

    // Update the reference to this DataGrid on the affected column(s)
    // and update the SelectedCells collection.
    switch (e.Action)
    {
    case System::Collection::NotifyCollectionChangedAction::Add:
        columsVector->Add(_dataGridOwner->GetColumns()[e.NewStartingIndex]);
        break;

    case System::Collection::NotifyCollectionChangedAction::Remove:
        columsVector->Remove(_dataGridOwner->GetColumns()[e.NewStartingIndex]);
        break;

    case System::Collection::NotifyCollectionChangedAction::Replace:
        break;

    case System::Collection::NotifyCollectionChangedAction::Reset:
        columsVector->Clear();
        break;
    }
}

DataGrid * XUI::UI::DataGridColumnHeadersPresenter::ParentDataGrid()
{
    return _dataGridOwner;
}

std::shared_ptr<Generators::IItemContainerGenerator> XUI::UI::DataGridColumnHeadersPresenter::CreateItemContainerGenerator()
{
    auto itemGenerator =  std::make_shared<Generators::ItemContainerGenerator<UI::DataGridColumnHeader>>(this, Controls::ContentControl::ContentProperty.get(), Controls::ContentControl::ContentTemplateProperty.get());

    itemGenerator->Materialized += System::make_eventhandler(&DataGridColumnHeadersPresenter::OnMaterializedHeader, this);
    itemGenerator->Dematerialized += System::make_eventhandler(&DataGridColumnHeadersPresenter::OnDematerializedHeader, this);

    return itemGenerator;
}

/// <summary>
///     Measure
/// </summary>

Core::Media::Size XUI::UI::DataGridColumnHeadersPresenter::MeasureOverride(Core::Media::Size const & availableSize)
{
    Core::Media::Size desiredSize;
    Core::Media::Size childConstraint = availableSize;
    childConstraint.Width = std::numeric_limits<double>::max();
    
    desiredSize = Controls::ItemsControl::MeasureOverride(childConstraint);
    
    Core::Media::Size indicatorSize;
    if (_columnHeaderDragIndicator != nullptr && _isColumnHeaderDragging)
    {
        _columnHeaderDragIndicator->Measure(childConstraint);
        indicatorSize = _columnHeaderDragIndicator->DesiredSize;
        desiredSize.Width = std::max(desiredSize.Width, indicatorSize.Width);
        desiredSize.Height = std::max(desiredSize.Height, indicatorSize.Height);
    }
    
    if (_columnHeaderDropLocationIndicator != nullptr && _isColumnHeaderDragging)
    {
        _columnHeaderDropLocationIndicator->Measure(availableSize);
        indicatorSize = _columnHeaderDropLocationIndicator->DesiredSize;
        desiredSize.Width =  std::max(desiredSize.Width, indicatorSize.Width);
        desiredSize.Height = std::max(desiredSize.Height, indicatorSize.Height);
    }
    
    desiredSize.Width = std::min(availableSize.Width, desiredSize.Width);
    
    return desiredSize;
}

/// <summary>
///     Arrange
/// </summary>
/// <param name="finalSize">Arrange size</param>

Core::Media::Size XUI::UI::DataGridColumnHeadersPresenter::ArrangeOverride(Core::Media::Size const & finalSize)
{
    Controls::UIElement* child = (m_visualChildrens.size() > 0) ? (*m_visualChildrens.begin())->Static_As<UI::Controls::UIElement>() : nullptr;
    
    if (child != nullptr)
    {
        Core::Media::Rect childRect(finalSize);
        DataGrid* dataGrid = ParentDataGrid();
        if (dataGrid != nullptr)
        {
            //childRect.X = -dataGrid->HorizontalScrollOffset;
            childRect.Width = std::max(finalSize.Width, dataGrid->DesiredSize.Width);
        }
    
        child->Arrange(childRect);
    }
    
    if (_columnHeaderDragIndicator != nullptr && _isColumnHeaderDragging)
    {
        _columnHeaderDragIndicator->Arrange(Core::Media::Rect::xywh(
            _columnHeaderDragCurrentPosition.x - _columnHeaderDragStartRelativePosition.x, 0,
            _columnHeaderDragIndicator->Width, _columnHeaderDragIndicator->Height));
    }
    
    if (_columnHeaderDropLocationIndicator != nullptr && _isColumnHeaderDragging)
    {
        Core::Media::Point point = FindColumnHeaderPositionByCurrentPosition(_columnHeaderDragCurrentPosition, true);
        double dropIndicatorWidth = _columnHeaderDropLocationIndicator->Width;
        point.x -= dropIndicatorWidth * 0.5;
        _columnHeaderDropLocationIndicator->Arrange(Core::Media::Rect::xywh(point.x, point.y, dropIndicatorWidth, _columnHeaderDropLocationIndicator->Height));
    }
    
    return finalSize;
}

DataGridColumn* XUI::UI::DataGridColumnHeadersPresenter::ColumnFromContainer(DataGridColumnHeader * container)
{
    ASSERT(Items != nullptr && "This is a helper method for preparing and clearing a container; if it's called we must have a valid ItemSource");

    int index = GetItemContainerGenerator()->IndexFromContainer(container);
    return GetItemContainerGenerator()->ContainerFromIndex(index)->Static_As<DataGridColumn>();
}

/// <summary>
///     Helper method which invalidate the underlying itemshost's measure and arrange
/// </summary>

void XUI::UI::DataGridColumnHeadersPresenter::InvalidateDataGridCellsPanelMeasureAndArrange()
{
    if (Presenter != nullptr)
    {
        Presenter->InvalidateMeasure();
        Presenter->InvalidateArrange();
    }
}

/// <summary>
///     Helper method which invalidate the underlying itemshost's measure and arrange
/// </summary>
/// <param name="withColumnVirtualization">
///     True to invalidate only when virtualization is on.
///     False to invalidate only when virtualization is off.
/// </param>

void XUI::UI::DataGridColumnHeadersPresenter::InvalidateDataGridCellsPanelMeasureAndArrange(bool withColumnVirtualization)
{
    // Invalidates measure and arrange if the flag and the virtualization
    // are either both true or both false.
    if (withColumnVirtualization == VirtualizingStackPanel::GetIsVirtualizing(this))
    {
        InvalidateDataGridCellsPanelMeasureAndArrange();
    }
}

/// <summary>
///     Workaround for not being able to access the panel instance of 
///     itemscontrol directly
/// </summary>

UI::Panel * XUI::UI::DataGridColumnHeadersPresenter::GetInternalItemsHost() const
{
	return Presenter ? Presenter->Panel.get() : nullptr;
}

void XUI::UI::DataGridColumnHeadersPresenter::SetInternalItemsHost(UI::Panel * value)
{
}

/// <summary>
///     Override of GetVisualChild which accomodates the indicators as visual children
/// </summary>

Controls::Visual * XUI::UI::DataGridColumnHeadersPresenter::GetVisualChild(int index) //override
{
    int visualChildrenCount = m_visualChildrens.size();
    if (index == visualChildrenCount)
    {
        if (_columnHeaderDragIndicator != nullptr)
        {
            return _columnHeaderDragIndicator.get();
        }
        else if (_columnHeaderDropLocationIndicator != nullptr)
        {
            return _columnHeaderDropLocationIndicator.get();
        }
    }
    
    if (index == visualChildrenCount + 1)
    {
        if (_columnHeaderDragIndicator != nullptr && _columnHeaderDropLocationIndicator != nullptr)
        {
            return _columnHeaderDropLocationIndicator.get();
        }
    }
    
    return m_visualChildrens[index].get();
}

/// <summary>
///     Gets called on mouse left button down of child header, and ensures preparation for column header drag
/// </summary>

void XUI::UI::DataGridColumnHeadersPresenter::OnHeaderMouseLeftButtonDown(Interfaces::PointerPressedEventArgs & e)
{
    if (ParentDataGrid() == nullptr)
    {
        return;
    }

    if (_draggingSrcColumnHeader != nullptr)
    {
        FinishColumnHeaderDrag(true);
    }

    if (_columnHeaderDragIndicator != nullptr)
    {
        RemoveVisualChild(_columnHeaderDragIndicator.get());
        _columnHeaderDragIndicator = nullptr;
    }
    
    if (_columnHeaderDropLocationIndicator != nullptr)
    {
        RemoveVisualChild(_columnHeaderDropLocationIndicator.get());
        _columnHeaderDropLocationIndicator = nullptr;
    }
    
    Core::Media::Point mousePosition = e.GetPosition(this);
    DataGridColumnHeader* header = FindColumnHeaderByPosition(mousePosition);
    
    if (header != nullptr)
    {
        DataGridColumn* column = header->Column();
    
        if (ParentDataGrid()->CanUserReorderColumns && column->CanUserReorder)
        {
            PrepareColumnHeaderDrag(header, mousePosition, e.GetPosition(header));
        }
    }
    else
    {
        _isColumnHeaderDragging = false;
        _prepareColumnHeaderDragging = false;
        _draggingSrcColumnHeader = nullptr;
        InvalidateArrange();
    }
}

/// <summary>
///     Gets called on mouse move of child header, and ensures column header drag
/// </summary>

void XUI::UI::DataGridColumnHeadersPresenter::OnHeaderMouseMove(Interfaces::PointerEventArgs & e)
{
    if (e.InputModifiers & Interfaces::InputModifiers::LeftMouseButton)
    {
        if (_prepareColumnHeaderDragging)
        {
            _columnHeaderDragCurrentPosition = e.GetPosition(this);
    
            if (!_isColumnHeaderDragging)
            {
                if (CheckStartColumnHeaderDrag(_columnHeaderDragCurrentPosition, _columnHeaderDragStartPosition))
                {
                    StartColumnHeaderDrag();
                }
            }
            else
            {
                bool shouldDisplayDragIndicator = IsMousePositionValidForColumnDrag(2.0);
                Visibility dragIndicatorVisibility = shouldDisplayDragIndicator ? Visibility::Visible : Visibility::Collapsed;
    
                if (_columnHeaderDragIndicator != nullptr)
                {
                    _columnHeaderDragIndicator->IsVisible = shouldDisplayDragIndicator;
                }
    
                if (_columnHeaderDropLocationIndicator != nullptr)
                {
                    _columnHeaderDropLocationIndicator->IsVisible = shouldDisplayDragIndicator;
                }
    
                InvalidateArrange();
    
                if (ParentDataGrid()->ColumnHeaderDragDelta)
                {
                    Interfaces::VectorEventArgs dragDeltaEventArgs;
                    dragDeltaEventArgs.Vector = _columnHeaderDragCurrentPosition - _columnHeaderDragStartPosition;

                    _columnHeaderDragStartPosition = _columnHeaderDragCurrentPosition;
                    ParentDataGrid()->ColumnHeaderDragDelta(ParentDataGrid(), dragDeltaEventArgs);
                }
            }
        }
    }
}

/// <summary>
///     Gets called on mouse left button up of child header, and ensures reordering of columns on successful completion of drag
/// </summary>

void XUI::UI::DataGridColumnHeadersPresenter::OnHeaderMouseLeftButtonUp(Interfaces::PointerReleasedEventArgs & e)
{
    if (_isColumnHeaderDragging)
    {
        _columnHeaderDragCurrentPosition = e.GetPosition(this);
        FinishColumnHeaderDrag(false);
    }
    else
    {
        ClearColumnHeaderDragInfo();
    }
}

/// <summary>
///     Gets called on mouse lost capture of child header and ensures that when capture gets lost
///     the drag ends in appropriate state. In this case it restore the drag state to
///     the start of the operation by finishing the drag with cancel flag
/// </summary>

void XUI::UI::DataGridColumnHeadersPresenter::OnHeaderLostMouseCapture(Interfaces::PointerEventArgs & e)
{
    if (_isColumnHeaderDragging && e.InputModifiers & Interfaces::InputModifiers::LeftMouseButton)
    {
        FinishColumnHeaderDrag(true);
    }
}

/// <summary>
///     Helper method which clears the header drag state
/// </summary>

void XUI::UI::DataGridColumnHeadersPresenter::ClearColumnHeaderDragInfo()
{
    _isColumnHeaderDragging = false;
    _prepareColumnHeaderDragging = false;
    _draggingSrcColumnHeader = nullptr;
    if (_columnHeaderDragIndicator != nullptr)
    {
        RemoveVisualChild(_columnHeaderDragIndicator.get());
        _columnHeaderDragIndicator = nullptr;
    }
	
    if (_columnHeaderDropLocationIndicator != nullptr)
    {
        RemoveVisualChild(_columnHeaderDropLocationIndicator.get());
        _columnHeaderDropLocationIndicator = nullptr;
    }
}

/// <summary>
///     Method which prepares the state for the start of column header drag
/// </summary>

void XUI::UI::DataGridColumnHeadersPresenter::PrepareColumnHeaderDrag(DataGridColumnHeader * header, Core::Media::Point const & pos, Core::Media::Point const & relativePos)
{
    _prepareColumnHeaderDragging = true;
    _isColumnHeaderDragging = false;
    _draggingSrcColumnHeader = header->shared_from_base_static<DataGridColumnHeader>();
    _columnHeaderDragStartPosition = pos;
    _columnHeaderDragStartRelativePosition = relativePos;
}

/// <summary>
///     Method which checks if mouse move is sufficient to start the drag
/// </summary>

bool XUI::UI::DataGridColumnHeadersPresenter::CheckStartColumnHeaderDrag(Core::Media::Point & currentPos, Core::Media::Point & originalPos)
{
	return std::abs(currentPos.x - originalPos.x) > 2.0;
}

/// <summary>
///     Method which checks during and after the drag if the position is valid for the drop
/// </summary>

bool XUI::UI::DataGridColumnHeadersPresenter::IsMousePositionValidForColumnDrag(double dragFactor)
{
    int nearestDisplayIndex = -1;
    return IsMousePositionValidForColumnDrag(dragFactor, nearestDisplayIndex);
}

/// <summary>
///     Method which checks during and after the drag if the position is valid for the drop and returns the drop display index
/// </summary>

bool XUI::UI::DataGridColumnHeadersPresenter::IsMousePositionValidForColumnDrag(double dragFactor, int & nearestDisplayIndex)
{
    nearestDisplayIndex = -1;
    bool isDraggingColumnFrozen = false;
    if (_draggingSrcColumnHeader->Column() != nullptr)
    {
        isDraggingColumnFrozen = _draggingSrcColumnHeader->Column()->IsFrozen;
    }
    
    int frozenCount = 0;
    if (ParentDataGrid() != nullptr)
    {
        frozenCount = ParentDataGrid()->FrozenColumnCount;
    }
    
    nearestDisplayIndex = FindDisplayIndexByPosition(_columnHeaderDragCurrentPosition, true);
    if (isDraggingColumnFrozen && nearestDisplayIndex >= frozenCount)
    {
        return false;
    }
    
    if (!isDraggingColumnFrozen && nearestDisplayIndex < frozenCount)
    {
        return false;
    }
    
    double height = 0.0;
    
    if (_columnHeaderDragIndicator == nullptr)
    {
        height = _draggingSrcColumnHeader->ActualHeight();
    }
    else
    {
        height = std::max(_draggingSrcColumnHeader->ActualHeight(), _columnHeaderDragIndicator->Height);
    }
    
    return (-height * dragFactor <= _columnHeaderDragCurrentPosition.y) &&
        (_columnHeaderDragCurrentPosition.y <= height * (dragFactor + 1));
}

/// <summary>
///     Method which start the column header drag. Includes raising events and creating default ghosts
/// </summary>

void XUI::UI::DataGridColumnHeadersPresenter::StartColumnHeaderDrag()
{
    ASSERT(ParentDataGrid() != nullptr, "ParentDataGrid is nullptr");
    
    _columnHeaderDragStartPosition = _columnHeaderDragCurrentPosition;
    if (ParentDataGrid()->ColumnHeaderDragStarted)
    {
        Interfaces::VectorEventArgs dragStartedEventArgs;
        dragStartedEventArgs.Vector = _columnHeaderDragStartPosition;
        ParentDataGrid()->ColumnHeaderDragStarted(ParentDataGrid(), dragStartedEventArgs);
    }
    
    DataGridColumnReorderingEventArgs reorderingEventArgs;
    reorderingEventArgs.Column = _draggingSrcColumnHeader->Column();
    
    _columnHeaderDragIndicator = CreateColumnHeaderDragIndicator();
    _columnHeaderDropLocationIndicator = CreateColumnHeaderDropIndicator();
    
    reorderingEventArgs.DragIndicator = _columnHeaderDragIndicator.get();
    reorderingEventArgs.DropLocationIndicator = _columnHeaderDropLocationIndicator.get();
    if (ParentDataGrid()->ColumnReordering)
        ParentDataGrid()->ColumnReordering(ParentDataGrid(), reorderingEventArgs);
    
    if (!reorderingEventArgs.Cancel)
    {
        _isColumnHeaderDragging = true;
        _columnHeaderDragIndicator = reorderingEventArgs.DragIndicator ? reorderingEventArgs.DragIndicator->shared_from_base_static<Controls::Control>() : nullptr;
        _columnHeaderDropLocationIndicator = reorderingEventArgs.DropLocationIndicator ? reorderingEventArgs.DropLocationIndicator->shared_from_base_static<Controls::Control>() : nullptr;
    
        if (_columnHeaderDragIndicator != nullptr)
        {
            SetDefaultsOnDragIndicator();
            AddVisualChild(_columnHeaderDragIndicator.get());
            AddLogicalChild(_columnHeaderDragIndicator.get());
        }
    
        if (_columnHeaderDropLocationIndicator != nullptr)
        {
            SetDefaultsOnDropIndicator();
            AddVisualChild(_columnHeaderDropLocationIndicator.get());
            AddLogicalChild(_columnHeaderDropLocationIndicator.get());
        }
    
        _draggingSrcColumnHeader->_suppressClickEvent = true;
        InvalidateMeasure();
    }
    else
    {
        FinishColumnHeaderDrag(true);
    }
}

/// <summary>
///     Method which returns a default control for column header drag indicator
/// </summary>

std::shared_ptr<Controls::Control> XUI::UI::DataGridColumnHeadersPresenter::CreateColumnHeaderDragIndicator()
{
    ASSERT(_draggingSrcColumnHeader.get() != nullptr && "Dragging header is nullptr");
	
    auto floatingHeader = std::make_shared<DataGridColumnFloatingHeader>();
    floatingHeader->_referenceHeader = _draggingSrcColumnHeader;
    floatingHeader->GID = 1113;
    return floatingHeader;
}

/// <summary>
///     Method which set the default values on drag indicator
/// </summary>

void XUI::UI::DataGridColumnHeadersPresenter::SetDefaultsOnDragIndicator()
{
    ASSERT(_columnHeaderDragIndicator != nullptr && "Drag indicator is nullptr");
    ASSERT(_draggingSrcColumnHeader != nullptr && "Dragging header is nullptr");
    DataGridColumn* column = _draggingSrcColumnHeader->Column();
    std::shared_ptr<Styling::Style> style = nullptr;
    //if (column != nullptr)
    //{
    //    style = column->DragIndicatorStyle;
    //    _columnHeaderDragIndicator->Style = style;
    //}
    
    _columnHeaderDragIndicator->CoerceValue(WidthProperty.get());
    _columnHeaderDragIndicator->CoerceValue(HeightProperty.get());
}

/// <summary>
///     Method which returns the default control for the column header drop indicator
/// </summary>

std::shared_ptr<Controls::Control> XUI::UI::DataGridColumnHeadersPresenter::CreateColumnHeaderDropIndicator()
{
    ASSERT(_draggingSrcColumnHeader != nullptr && "Dragging header is nullptr");
	
    //auto indicator = std::make_shared<DataGridColumnDropSeparator>();
    //indicator.ReferenceHeader = _draggingSrcColumnHeader;
    //return indicator;
	return nullptr;
}

/// <summary>
///     Method which sets the default values on drop indicator
/// </summary>

void XUI::UI::DataGridColumnHeadersPresenter::SetDefaultsOnDropIndicator()
{
    //ASSERT(_columnHeaderDropLocationIndicator != nullptr, "Drag indicator is nullptr");
    //ASSERT(_draggingSrcColumnHeader != nullptr, "Dragging header is nullptr");
    //Style style = nullptr;
    //if (ParentDataGrid != nullptr)
    //{
    //    style = ParentDataGrid.DropLocationIndicatorStyle;
    //}
    //
    //_columnHeaderDropLocationIndicator.Style = style;
    //_columnHeaderDropLocationIndicator.CoerceValue(WidthProperty);
    //_columnHeaderDropLocationIndicator.CoerceValue(HeightProperty);
}

/// <summary>
///     Method which completes the column header drag. Includes raising of events and changing column display index if needed.
/// </summary>

void XUI::UI::DataGridColumnHeadersPresenter::FinishColumnHeaderDrag(bool isCancel)
{
    ASSERT(ParentDataGrid() != nullptr && "ParentDataGrid is nullptr");
    _prepareColumnHeaderDragging = false;
    _isColumnHeaderDragging = false;
    
    _draggingSrcColumnHeader->_suppressClickEvent = false;
    
    if (_columnHeaderDragIndicator != nullptr)
    {
        _columnHeaderDragIndicator->IsVisible = false;
        DataGridColumnFloatingHeader* floatingHeader = _columnHeaderDragIndicator->Static_As<DataGridColumnFloatingHeader>();
        if (floatingHeader != nullptr)
        {
            floatingHeader->ClearHeader();
        }
    
        RemoveLogicalChild(_columnHeaderDragIndicator.get());
        RemoveVisualChild(_columnHeaderDragIndicator.get());
    }
    
    if (_columnHeaderDropLocationIndicator != nullptr)
    {
        _columnHeaderDropLocationIndicator->IsVisible = false;
        //DataGridColumnDropSeparator separator = _columnHeaderDropLocationIndicator as DataGridColumnDropSeparator;
        //if (separator != nullptr)
        //{
        //    separator.ReferenceHeader = nullptr;
        //}
        //
        //RemoveLogicalChild(_columnHeaderDropLocationIndicator.get());
        //RemoveVisualChild(_columnHeaderDropLocationIndicator.get());
    }
    
    Interfaces::VectorEventArgs dragCompletedEventArgs;
    dragCompletedEventArgs.Vector = _columnHeaderDragCurrentPosition - _columnHeaderDragStartPosition;
    
    if (ParentDataGrid()->ColumnHeaderDragCompleted)
        ParentDataGrid()->ColumnHeaderDragCompleted(ParentDataGrid(), dragCompletedEventArgs);
    _draggingSrcColumnHeader->InvalidateArrange();
    
    if (!isCancel)
    {
        int newDisplayIndex = -1;
        bool dragEndPositionValid = IsMousePositionValidForColumnDrag(
            2.0,
            newDisplayIndex);
    
        DataGridColumn* column = _draggingSrcColumnHeader->Column();
        if (column != nullptr && dragEndPositionValid && newDisplayIndex != column->DisplayIndex)
        {
            column->DisplayIndex = newDisplayIndex;

            if (ParentDataGrid()->ColumnReordered)
            {
                DataGridColumnEventArgs columnEventArgs;
                columnEventArgs.Column = _draggingSrcColumnHeader->Column();
                ParentDataGrid()->ColumnReordered(ParentDataGrid(), columnEventArgs);
            }
        }
    }
    
    _draggingSrcColumnHeader = nullptr;
    _columnHeaderDragIndicator = nullptr;
    _columnHeaderDropLocationIndicator = nullptr;
}

/// <summary>
///     Helper method to determine the display index based on the given position
/// </summary>

int XUI::UI::DataGridColumnHeadersPresenter::FindDisplayIndexByPosition(Core::Media::Point const & startPos, bool findNearestColumn)
{
    Core::Media::Point headerPos;
    int displayIndex;
    DataGridColumnHeader* header;
    FindDisplayIndexAndHeaderPosition(startPos, findNearestColumn, displayIndex, headerPos, header);
    return displayIndex;
}

/// <summary>
///     Helper method to determine the column header based on the given position
/// </summary>

DataGridColumnHeader * XUI::UI::DataGridColumnHeadersPresenter::FindColumnHeaderByPosition(Core::Media::Point const & startPos)
{
	Core::Media::Point headerPos;
    int displayIndex;
    DataGridColumnHeader* header;
    FindDisplayIndexAndHeaderPosition(startPos, false, displayIndex, headerPos, header);
    return header;
}

/// <summary>
///     Helper method to determine the position of drop indicator based on the given mouse position
/// </summary>

Core::Media::Point XUI::UI::DataGridColumnHeadersPresenter::FindColumnHeaderPositionByCurrentPosition(Core::Media::Point const & startPos, bool findNearestColumn)
{
    Core::Media::Point headerPos;
    int displayIndex;
    DataGridColumnHeader* header;
    FindDisplayIndexAndHeaderPosition(startPos, findNearestColumn, displayIndex, headerPos, header);
    return headerPos;
}

/// <summary>
///     Helper method which estimates the column width
/// </summary>

double XUI::UI::DataGridColumnHeadersPresenter::GetColumnEstimatedWidth(DataGridColumn* column, double averageColumnWidth)
{
    double columnEstimatedWidth = column->ActualWidth;
    if (std::isnan(columnEstimatedWidth) || columnEstimatedWidth == -1.0)
    {
        columnEstimatedWidth = std::max(averageColumnWidth, column->MinWidth);
        columnEstimatedWidth = std::min(columnEstimatedWidth, column->MaxWidth);
    }
	
    return columnEstimatedWidth;
}

/// <summary>
///     Helper method to find display index, header and header start position based on given mouse position
/// </summary>

void XUI::UI::DataGridColumnHeadersPresenter::FindDisplayIndexAndHeaderPosition(Core::Media::Point const& startPos, bool findNearestColumn, int& displayIndex, Core::Media::Point& headerPos, DataGridColumnHeader*& header)
{
    ASSERT(ParentDataGrid() != nullptr && "ParentDataGrid is nullptr");
    
    Core::Media::Point originPoint(0, 0);
    headerPos = originPoint;
    displayIndex = -1;
    header = nullptr;
    
    if (startPos.x < 0.0)
    {
        if (findNearestColumn)
        {
            displayIndex = 0;
        }
    
        return;
    }
    
    double headerStartX = 0.0;
    double headerEndX = 0.0;
    int i = 0;
    DataGrid* dataGrid = ParentDataGrid();
    double averageColumnWidth = dataGrid->GetColumns().ComputeAverageColumnWidth();
    bool firstVisibleNonFrozenColumnHandled = false;
    for (i = 0; i < dataGrid->GetColumns().size(); i++)
    {
        displayIndex++;
        DataGridColumnHeader* currentHeader = dataGrid->ColumnHeaderFromDisplayIndex(i);
        if (currentHeader == nullptr)
        {
            DataGridColumn* column = dataGrid->ColumnFromDisplayIndex(i);
            if (!column->IsVisible())
            {
                continue;
            }
            else
            {
                headerStartX = headerEndX;
                if (i >= dataGrid->FrozenColumnCount &&
                    !firstVisibleNonFrozenColumnHandled)
                {
                    headerStartX -= dataGrid->HorizontalScrollOffset;
                    firstVisibleNonFrozenColumnHandled = true;
                }
    
                headerEndX = headerStartX + GetColumnEstimatedWidth(column, averageColumnWidth);
            }
        }
        else
        {
            auto transform = currentHeader->_bounds.GetPosition(); // ToDo: implement transformation
            headerStartX = transform.x;
            headerEndX = headerStartX + currentHeader->ActualWidth();
        }
    
        if (startPos.x <= headerStartX)
        {
            break;
        }
    
        if (startPos.x >= headerStartX &&
            startPos.x <= headerEndX)
        {
            if (findNearestColumn)
            {
                double headerMidX = (headerStartX + headerEndX) * 0.5;
                if (startPos.x >= headerMidX)
                {
                    headerStartX = headerEndX;
                    displayIndex++;
                }
    
                if (_draggingSrcColumnHeader != nullptr && _draggingSrcColumnHeader->Column() != nullptr && _draggingSrcColumnHeader->Column()->DisplayIndex < displayIndex)
                {
                    displayIndex--;
                }
            }
            else
            {
                header = currentHeader;
            }
    
            break;
        }
    }
    
    if (i == dataGrid->GetColumns().size())
    {
        displayIndex = dataGrid->GetColumns().size() - 1;
        headerStartX = headerEndX;
    }
    
    headerPos.x = headerStartX;
}

/// <summary>
///     Property changed callback for VirtualizingStackPanel.IsVirtualizing property
/// </summary>

void XUI::UI::DataGridColumnHeadersPresenter::OnIsVirtualizingPropertyChanged(DependencyObject * d, Core::Dependency::DependencyPropertyChangedEventArgs & e)
{
    DataGridColumnHeadersPresenter* headersPresenter = static_cast<DataGridColumnHeadersPresenter*>(d);
    headersPresenter->CoerceValue(VirtualizingStackPanel::IsVirtualizingProperty.get());
    if (e.OldValue.GetValue<bool>() != headersPresenter->GetValue<bool>(VirtualizingStackPanel::IsVirtualizingProperty))
    {
        headersPresenter->InvalidateDataGridCellsPanelMeasureAndArrange();
    }
}
