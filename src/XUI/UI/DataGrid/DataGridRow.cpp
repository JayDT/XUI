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
#include "DataGridRow.h"
#include "UI/VirtualizingStackPanel.h"
#include "DataGrid/DataGridControl.h"
#include "DataGrid/DataGridRowHeader.h"
#include "DataGrid/DataGridDetailsPresenter.h"
#include "DataGrid/DataGridCellsPresenter.h"
#include "DataGrid/DataGridCellsPanel.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;
using namespace XUI::UI;

Core::Dependency::RefDependencyProperty DataGridRow::ItemProperty;
Core::Dependency::RefDependencyProperty DataGridRow::ItemsPanelProperty;
Core::Dependency::RefDependencyProperty DataGridRow::HeaderProperty;
Core::Dependency::RefDependencyProperty DataGridRow::HeaderStyleProperty;
Core::Dependency::RefDependencyProperty DataGridRow::HeaderTemplateProperty;
Core::Dependency::RefDependencyProperty DataGridRow::DetailTemplateProperty;
Core::Dependency::RefDependencyProperty DataGridRow::DetailsVisibilityProperty;
Core::Dependency::RefDependencyProperty DataGridRow::AlternationIndexProperty;
Core::Dependency::RefDependencyProperty DataGridRow::IsSelectedProperty;
Core::Dependency::RefDependencyProperty DataGridRow::IsEditingProperty;

Core::Observer::RefRoutedEvent DataGridRow::SelectedEvent;
Core::Observer::RefRoutedEvent DataGridRow::UnselectedEvent;

STATIC_CLASS_INITIALIZER(DataGridRow, XUI::UI::DataGridRow::StaticClassInitializer);

void XUI::UI::DataGridRow::StaticClassInitializer()
{
	ItemProperty = (Core::Dependency::BindProperty<System::Reflection::Variant>::Register<DataGridRow>(nameof(Item)));
	ItemsPanelProperty = (Core::Dependency::BindProperty<std::shared_ptr<XamlCPP::Interfaces::ITemplate>>::Register<DataGridRow>(nameof(ItemsPanel), std::make_shared<DataTemplates::FuncTemplate<std::shared_ptr<XUI::UI::Panel>>>([]()
	{
        auto panel = std::make_shared<DataGridCellsPanel>();
        panel->Orientation = Orientation::Horizontal;
        return panel;
    })));
	HeaderProperty = (Core::Dependency::BindProperty<System::Reflection::RefObject>::Register<DataGridRow>(nameof(Header), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<DataGridRow*>(d);
        control->NotifyPropertyChanged(control, e, NotificationTarget(uint32(NotificationTarget::Rows) | uint32(NotificationTarget::RowHeaders)));
    }, nullptr));
	HeaderStyleProperty = (Core::Dependency::BindProperty<std::shared_ptr<XUI::Styling::Style>>::Register<DataGridRow>(nameof(HeaderStyle), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<DataGridRow*>(d);
        control->NotifyPropertyChanged(control, e, NotificationTarget(uint32(NotificationTarget::Rows) | uint32(NotificationTarget::RowHeaders)));
    }, [](Core::Dependency::DependencyObject* d, System::Reflection::Variant const& baseValue)
    {
        auto column = static_cast<DataGridRow*>(d);
        return DataGridHelper::GetCoercedTransferPropertyValue(d, baseValue, HeaderStyleProperty.get(), column->DataGridOwner(), DataGrid::RowHeaderStyleProperty.get());
    }));
	HeaderTemplateProperty = (Core::Dependency::BindProperty<std::shared_ptr<XUI::UI::DataTemplates::ContentTemplate>>::Register<DataGridRow>(nameof(HeaderTemplate), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<DataGridRow*>(d);
        control->NotifyPropertyChanged(control, e, NotificationTarget(uint32(NotificationTarget::Rows) | uint32(NotificationTarget::RowHeaders)));
    }, [](Core::Dependency::DependencyObject* d, System::Reflection::Variant const& baseValue)
    {
        auto column = static_cast<DataGridRow*>(d);
        return DataGridHelper::GetCoercedTransferPropertyValue(d, baseValue, HeaderTemplateProperty.get(), column->DataGridOwner(), DataGrid::RowHeaderTemplateProperty.get());
    }));
	DetailTemplateProperty = (Core::Dependency::BindProperty<std::shared_ptr<XUI::UI::DataTemplates::ContentTemplate>>::Register<DataGridRow>(nameof(DetailTemplate), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<DataGridRow*>(d);
        control->NotifyPropertyChanged(control, e, NotificationTarget(uint32(NotificationTarget::Rows) | uint32(NotificationTarget::DetailsPresenter)));

        // It only makes sense to fire UnloadingRowDetails if the row details are already loaded. The same is true for LoadingRowDetails,
        // since making row details visible will take care of firing LoadingRowDetails.
        //if (control->DetailsLoaded &&
        //    d->GetValue(e.Property) == e.NewValue)
        //{
        //    if (control->DataGridOwner())
        //    {
        //        control->DataGridOwner()->OnUnloadingRowDetailsWrapper(control);
        //    }
        //    if (e.NewValue.GetBase())
        //    {
        //        // Invoke LoadingRowDetails, but only after the details template is expanded (so DetailsElement will be available).
        //        Dispatcher.CurrentDispatcher.BeginInvoke(new DispatcherOperationCallback(DataGrid.DelayedOnLoadingRowDetails), DispatcherPriority.Loaded, row);
        //    }
        //}
    }, [](Core::Dependency::DependencyObject* d, System::Reflection::Variant const& baseValue)
    {
        auto column = static_cast<DataGridRow*>(d);
        return DataGridHelper::GetCoercedTransferPropertyValue(d, baseValue, HeaderTemplateProperty.get(), column->DataGridOwner(), DataGrid::RowHeaderTemplateProperty.get());
    }));
	DetailsVisibilityProperty = (Core::Dependency::BindProperty<UI::Visibility>::Register<DataGridRow>(nameof(DetailsVisibility), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<DataGridRow*>(d);

        // Notify the DataGrid at Loaded priority so the template has time to expland.
        //Dispatcher.CurrentDispatcher.BeginInvoke(new DispatcherOperationCallback(DelayedRowDetailsVisibilityChanged), DispatcherPriority.Loaded, row);

        control->NotifyPropertyChanged(control, e, NotificationTarget(uint32(NotificationTarget::Rows) | uint32(NotificationTarget::DetailsPresenter)));
    }, [](Core::Dependency::DependencyObject* d, System::Reflection::Variant const& baseValue)
    {
        auto row = static_cast<DataGridRow*>(d);
        auto visibility = DataGridHelper::GetCoercedTransferPropertyValue(d, baseValue, DetailsVisibilityProperty.get(), row->DataGridOwner(), DataGrid::RowDetailsVisibilityModeProperty.get());

        //if (visibility.GetType()->IsAssignableFrom<DataGridRowDetailsVisibilityMode>())
        //{
        //    auto visibilityMode = visibility.GetValue<DataGridRowDetailsVisibilityMode>();
        //    auto hasDetailsTemplate = row->DetailsTemplate != null || row.DetailsTemplateSelector != null;
        //    auto isRealItem = row->Item != CollectionView.NewItemPlaceholder;
        //    switch (visibilityMode)
        //    {
        //    case DataGridRowDetailsVisibilityMode.Collapsed:
        //        visibility = Visibility.Collapsed;
        //        break;
        //    case DataGridRowDetailsVisibilityMode.Visible:
        //        visibility = hasDetailsTemplate && isRealItem ? Visibility.Visible : Visibility.Collapsed;
        //        break;
        //    case DataGridRowDetailsVisibilityMode.VisibleWhenSelected:
        //        visibility = row.IsSelected && hasDetailsTemplate && isRealItem ? Visibility.Visible : Visibility.Collapsed;
        //        break;
        //    default:
        //        visibility = Visibility.Collapsed;
        //        break;
        //    }
        //}

        return visibility;
    }, UI::Visibility::Collapsed));
    AlternationIndexProperty = Controls::ItemsControl::AlternationIndexProperty->AddOwner<DataGridRow>()->shared_from_base_static<Core::Dependency::DependencyProperty>();
	IsSelectedProperty = (Core::Dependency::BindProperty<bool>::Register<DataGridRow>(nameof(IsSelected), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<DataGridRow*>(d);
        bool isSelected = e.NewValue.GetValue<bool>();

        if (isSelected && !control->IsSelectable())
        {
            throw System::InvalidOperationException(""); // SR.Get(SRID.DataGridRow_CannotSelectRowWhenCells));
        }

        control->NotifyPropertyChanged(control, e, NotificationTarget(uint32(NotificationTarget::Rows) | uint32(NotificationTarget::RowHeaders)));

        // This will raise the appropriate selection event, which will
        // bubble to the DataGrid. The base class Selector code will listen
        // for these events and will update SelectedItems as necessary.
        control->RaiseSelectionChangedEvent(isSelected);
    }, nullptr));
	IsEditingProperty = (Core::Dependency::BindProperty<bool>::Register<DataGridRow>(nameof(IsEditing)));

    AlternationIndexProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const & e)
    {
        DataGridRow* control = static_cast<DataGridRow*>(e.Sender);
        if (control)
            control->NotifyPropertyChanged(control, nameof(AlternationIndex), (Core::Dependency::DependencyPropertyChangedEventArgs&)e, NotificationTarget::Rows);
    });

	ItemProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const & e)
	{
		DataGridRow* control = static_cast<DataGridRow*>(e.Sender);
		if (control)
			control->OnItemChanged((Core::Dependency::DependencyPropertyChangedEventArgs&)e);
	});

    IsVisibleProperty->OverrideMetadata<DataGridRow>(std::make_shared<Core::Dependency::BindPropertyMetadata>(
        bool(true),
        nullptr,
        Core::Dependency::BindingMode::OneWay,
        [](XamlCPP::Core::Dependency::IDependencyObject* d, System::Reflection::Variant const& baseValue) -> System::Reflection::Variant
        {
            auto row = static_cast<DataGridRow*>(d);
            auto owningDataGrid = row->DataGridOwner();
            /*if (row->Item == CollectionView.NewItemPlaceholder && owningDataGrid)
            {
                return owningDataGrid->PlaceholderVisibility;
            }*/
            return baseValue;
        }));

    BackgroundProperty->OverrideMetadata<DataGridRow>(std::make_shared<Core::Dependency::BindPropertyMetadata>(
        std::shared_ptr<Interfaces::IBrush>(),
        nullptr,
        Core::Dependency::BindingMode::OneWay,
        [](XamlCPP::Core::Dependency::IDependencyObject* d, System::Reflection::Variant const& baseValue) -> System::Reflection::Variant
        {
            auto row = static_cast<DataGridRow*>(d);
            auto coercedValue = baseValue;

            switch (row->AlternationIndex)
            {
            case 0:
                coercedValue = DataGridHelper::GetCoercedTransferPropertyValue(row, baseValue, BackgroundProperty.get(), row->DataGridOwner(), DataGrid::RowBackgroundProperty.get());
                break;
            case 1:
                coercedValue = DataGridHelper::GetCoercedTransferPropertyValue(row, baseValue, BackgroundProperty.get(), row->DataGridOwner(), DataGrid::AlternatingRowBackgroundProperty.get());
                break;
            }
            return coercedValue;
        }));

    BackgroundProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const & e)
    {
        auto control = static_cast<DataGridRow*>(e.Sender);
        control->NotifyPropertyChanged(control, (XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs&)e, NotificationTarget::Rows);
    });
}

XUI::UI::DataGridRow::DataGridRow()
{
}

XUI::UI::DataGridRow::~DataGridRow()
{
}

DataGridCell * XUI::UI::DataGridRow::GetCell(int columnIndex)
{
    if (GetCellsPresenter() && GetCellsPresenter()->Presenter && GetCellsPresenter()->Presenter->Panel && GetCellsPresenter()->Presenter->Panel->Children.size() > columnIndex)
        return GetCellsPresenter()->Presenter->Panel->Children[columnIndex]->Static_As<DataGridCell>();
    return nullptr;
}

void XUI::UI::DataGridRow::ItemChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs & e)
{
}

void XUI::UI::DataGridRow::ItemsPanelChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs & e)
{
}

/// <summary>
///     Prepares a row container for active use.
/// </summary>
/// <remarks>
///     Instantiates or updates a MultipleCopiesCollection ItemsSource in
///     order that cells be generated.
/// </remarks>
/// <param name="item">The data item that the row represents.</param>
/// <param name="owningDataGrid">The DataGrid owner.</param>

void XUI::UI::DataGridRow::PrepareRow(System::Reflection::Variant const & item, DataGrid * owningDataGrid)
{
    bool fireOwnerChanged = (_owner.lock().get() != owningDataGrid);
    ASSERT(_owner.expired() || !fireOwnerChanged, "_owner should be null before PrepareRow is called or the same as the owningDataGrid.");
    bool forcePrepareCells = false;
    _owner = owningDataGrid->shared_from_base_static<DataGrid>();

    if (this != item.ToObject())
    {
        if (Item != item)
        {
            Item = item;
        }
        else
        {
            forcePrepareCells = true;
        }
    }

    if (IsEditing)
    {
        // If IsEditing was left on and this container was recycled, reset it here.
        IsEditing = false;
    }

    // Since we just changed _owner we need to invalidate all child properties that rely on a value supplied by the DataGrid.
    // A common scenario is when a recycled Row was detached from the visual tree and has just been reattached (we always clear out the 
    // owner when recycling a container).
    if (fireOwnerChanged)
    {
        SyncProperties(forcePrepareCells);
    }

    // Re-run validation, but wait until Binding has occured.
    //Dispatcher.BeginInvoke(new DispatcherOperationCallback(DelayedValidateWithoutUpdate), DispatcherPriority.DataBind, BindingGroup);
}

/// <summary>
///     Clears the row of references.
/// </summary>

void XUI::UI::DataGridRow::ClearRow(DataGrid* owningDataGrid)
{
    ASSERT(_owner.lock().get() == owningDataGrid && "_owner should be the same as the DataGrid that is clearing the row.");

    auto cellsPresenter = CellsPresenter;
    if (cellsPresenter != nullptr)
    {
        PersistAttachedItemValue(cellsPresenter, DataGridCellsPresenter::HeightProperty.get());
    }

    PersistAttachedItemValue(this, DetailsVisibilityProperty.get());

    _owner.reset();
}

void XUI::UI::DataGridRow::OnRowResizeStarted()
{
    auto cellsPresenter = CellsPresenter;
    if (cellsPresenter != nullptr)
    {
        _cellsPresenterResizeHeight = cellsPresenter->Height;
    }
}

void XUI::UI::DataGridRow::OnRowResize(double changeAmount)
{
    auto cellsPresenter = CellsPresenter;
    if (cellsPresenter != nullptr)
    {
        double newHeight = cellsPresenter->ActualHeight() + changeAmount;

        // clamp the CellsPresenter size to the RowHeader size or MinHeight because the header wont shrink any smaller.
        double minHeight = std::max(RowHeader->DesiredSize.Height, MinHeight);
        if (newHeight < minHeight)
        {
            newHeight = minHeight;
        }

        // clamp the CellsPresenter size to the MaxHeight of Row, because row wouldn't grow any larger
        double maxHeight = MaxHeight;
        if (newHeight > maxHeight)
        {
            newHeight = maxHeight;
        }

        cellsPresenter->Height = newHeight;
    }
}

void XUI::UI::DataGridRow::OnRowResizeCompleted(bool canceled)
{
    auto cellsPresenter = CellsPresenter;
    if (cellsPresenter != nullptr && canceled)
    {
        cellsPresenter->Height = _cellsPresenterResizeHeight;
    }
}

void XUI::UI::DataGridRow::OnRowResizeReset()
{
    auto cellsPresenter = CellsPresenter;
    if (cellsPresenter != nullptr)
    {
        cellsPresenter->ClearValue(DataGridCellsPresenter::HeightProperty.get());
		if (auto depObj = Item.ToObject()->Static_As<Core::Dependency::DependencyObject>())
			depObj->ClearValue(DataGridCellsPresenter::HeightProperty.get());
    }
}

/// <summary>
///     General notification for DependencyProperty changes from the grid or from columns.
/// </summary>

void XUI::UI::DataGridRow::NotifyPropertyChanged(DependencyObject * d, Core::Dependency::DependencyPropertyChangedEventArgs & e, NotificationTarget target)
{
    NotifyPropertyChanged(d, "", e, target);
}

/// <summary>
///     General notification for DependencyProperty changes from the grid or from columns.
/// </summary>

void XUI::UI::DataGridRow::NotifyPropertyChanged(DependencyObject * d, std::string const & propertyName, Core::Dependency::DependencyPropertyChangedEventArgs & e, NotificationTarget target)
{
    if (target & NotificationTarget::Rows)
    {
        if (e.Property == DataGrid::RowBackgroundProperty.get() || e.Property == DataGrid::AlternatingRowBackgroundProperty.get() ||
            e.Property == BackgroundProperty.get() || e.Property == AlternationIndexProperty.get())
        {
            CoerceValue(BackgroundProperty.get());
        }
        else if (e.Property == DataGrid::RowHeaderStyleProperty.get() || e.Property == HeaderStyleProperty.get())
        {
            CoerceValue(HeaderStyleProperty.get());
        }
        else if (e.Property == DataGrid::RowHeaderTemplateProperty.get() || e.Property == HeaderTemplateProperty.get())
        {
            CoerceValue(HeaderTemplateProperty.get());
        }
        /*else if (e.Property == DataGrid::RowHeaderTemplateSelectorProperty.get() || e.Property == HeaderTemplateSelectorProperty.get())
        {
            CoerceValue(HeaderTemplateSelectorProperty.get());
        }
        else if (e.Property == DataGrid::RowValidationErrorTemplateProperty.get() || e.Property == ValidationErrorTemplateProperty.get())
        {
            CoerceValue(ValidationErrorTemplateProperty.get());
        }
        else if (e.Property == DataGrid::RowDetailsTemplateProperty.get() || e.Property == DetailsTemplateProperty.get())
        {
            CoerceValue(DetailsTemplateProperty.get());
            CoerceValue(DetailsVisibilityProperty.get());
        }
        else if (e.Property == DataGrid::RowDetailsTemplateSelectorProperty.get() || e.Property == DetailsTemplateSelectorProperty.get())
        {
            CoerceValue(DetailsTemplateSelectorProperty.get());
            CoerceValue(DetailsVisibilityProperty.get());
        }*/
        else if (e.Property == DataGrid::RowDetailsVisibilityModeProperty.get() || e.Property == DetailsVisibilityProperty.get() || e.Property == IsSelectedProperty.get())
        {
            CoerceValue(DetailsVisibilityProperty.get());
        }
        else if (e.Property == ItemProperty.get())
        {
            OnItemChanged(e);
        }
        else if (e.Property == HeaderProperty.get())
        {
            OnHeaderChanged(e);
        }
        /*else if (e.Property == BindingGroupProperty.get())
        {
            // Re-run validation, but wait until Binding has occured.
            Dispatcher.BeginInvoke(new DispatcherOperationCallback(DelayedValidateWithoutUpdate), DispatcherPriority.DataBind, e.NewValue);
        }*/
    }
    
    if (target & NotificationTarget::DetailsPresenter)
    {
        if (DetailsPresenter != nullptr)
        {
            DetailsPresenter->NotifyPropertyChanged(d, e);
        }
    }
    
    if (target & (NotificationTarget::CellsPresenter | NotificationTarget::Cells | NotificationTarget::RefreshCellContent))
    {
        DataGridCellsPresenter* cellsPresenter = CellsPresenter;
        if (!cellsPresenter)
            cellsPresenter = Template->FindNameByManagedType<DataGridCellsPresenter>("PART_CellPresenter", this).get();
        if (cellsPresenter != nullptr)
        {
            cellsPresenter->NotifyPropertyChanged(d, propertyName, e, target);
        }
    }
    
    if (target & NotificationTarget::RowHeaders && RowHeader != nullptr)
    {
        RowHeader->NotifyPropertyChanged(d, e);
    }
}

System::Reflection::Variant XUI::UI::DataGridRow::DelayedValidateWithoutUpdate(System::Reflection::Variant const & arg)
{
    //// Only validate if we have an Item.
    //var bindingGroup = (BindingGroup)arg;
    //if (bindingGroup != null && bindingGroup.Items.Count > 0)
    //{
    //    bindingGroup.ValidateWithoutUpdate();
    //}

    return {};
}


/// <summary>
///     Notification from the DataGrid that the columns collection has changed.
/// </summary>
/// <param name="columns">The columns collection.</param>
/// <param name="e">The event arguments from the collection's change event.</param>


/// <summary>
///     Fired when the Row is attached to the DataGrid.  The scenario here is if the user is scrolling and
///     the Row is a recycled container that was just added back to the visual tree.  Properties that rely on a value from
///     the Grid should be reevaluated because they may be stale.  
/// </summary>
/// <remarks>
///     Properties can obviously be stale if the DataGrid's value changes while the row is disconnected.  They can also
///     be stale for unobvious reasons.
/// 
///     For example, the Style property is invalidated when we detect a new Visual parent.  This happens for 
///     elements in the row (such as the RowHeader) before Prepare is called on the Row.  The coercion callback
///     will thus be unable to find the DataGrid and will return the wrong value.  
/// 
///     There is a potential for perf work here.  If we know a DP isn't invalidated when the visual tree is reconnected
///     and we know that the Grid hasn't modified that property then its value is likely fine.  We could also cache whether
///     or not the Grid's property is the one that's winning.  If not, no need to redo the coercion.  This notification 
///     is pretty fast already and thus not worth the work for now.
/// </remarks>

void XUI::UI::DataGridRow::SyncProperties(bool forcePrepareCells)
{
    // Coerce all properties on Row that depend on values from the DataGrid
    // Style is ok since it's equivalent to ItemContainerStyle and has already been invalidated.
    CoerceValue(BackgroundProperty.get());
    CoerceValue(HeaderStyleProperty.get());
    CoerceValue(HeaderTemplateProperty.get());
    //CoerceValue(HeaderTemplateSelectorProperty.get());
    //CoerceValue(ValidationErrorTemplateProperty.get());
    //CoerceValue(DetailsTemplateProperty.get());
    //CoerceValue(DetailsTemplateSelectorProperty.get());
    CoerceValue(DetailsVisibilityProperty.get());
    
    CoerceValue(IsVisibleProperty.get()); // Handle NewItemPlaceholder case
    
    RestoreAttachedItemValue(this, DetailsVisibilityProperty.get());
    
    auto cellsPresenter = CellsPresenter;
    if (cellsPresenter != nullptr)
    {
        cellsPresenter->SyncProperties(forcePrepareCells);
        RestoreAttachedItemValue(cellsPresenter, DataGridCellsPresenter::HeightProperty.get());
    }
    
    if (DetailsPresenter != nullptr)
    {
        DetailsPresenter->SyncProperties();
    }
    
    if (RowHeader != nullptr)
    {
        RowHeader->SyncProperties();
    }
}

void XUI::UI::DataGridRow::OnIsSelectedChanged(System::Reflection::Object * sender, Core::Dependency::DependencyPropertyChangedEventArgs & e)
{
    DataGridRow* row = sender->Static_As<DataGridRow>();
    bool isSelected = e.NewValue.GetValue<bool>();
    
    if (isSelected && !row->IsSelectable())
    {
		throw System::InvalidOperationException(""); // SR.Get(SRID.DataGridRow_CannotSelectRowWhenCells));
    }
    
    DataGrid* grid = row->DataGridOwner();
    if (grid != nullptr && row->DataContext != nullptr)
    {
        //DataGridAutomationPeer gridPeer = UIElementAutomationPeer.FromElement(grid) as DataGridAutomationPeer;
        //if (gridPeer != null)
        //{
        //    DataGridItemAutomationPeer rowItemPeer = gridPeer.GetOrCreateItemPeer(row.DataContext);
        //    if (rowItemPeer != null)
        //    {
        //        rowItemPeer.RaisePropertyChangedEvent(
        //            System.Windows.Automation.SelectionItemPatternIdentifiers.IsSelectedProperty,
        //            (bool)e.OldValue,
        //            isSelected);
        //    }
        //}
    }
    
    // Update the header's IsRowSelected property
    row->NotifyPropertyChanged(row, e, UI::NotificationTarget(uint32(UI::NotificationTarget::Rows) | uint32(UI::NotificationTarget::RowHeaders)));
    
    // This will raise the appropriate selection event, which will
    // bubble to the DataGrid. The base class Selector code will listen
    // for these events and will update SelectedItems as necessary.
    row->RaiseSelectionChangedEvent(isSelected);
}

/// <summary>
///     Method which tries to scroll a cell for given index into the scroll view
/// </summary>
/// <param name="index"></param>

void XUI::UI::DataGridRow::ScrollCellIntoView(int index)
{
    DataGridCellsPresenter* cellsPresenter = _cellsPresenter.get();
    if (cellsPresenter != nullptr)
    {
        cellsPresenter->ScrollCellIntoView(index);
    }
}

Core::Media::Size XUI::UI::DataGridRow::ArrangeOverride(Core::Media::Size const & availableSize)
{
    DataGrid* dataGrid = DataGridOwner();
    if (dataGrid != nullptr)
        dataGrid->QueueInvalidateCellsPanelHorizontalOffset();

    return Controls::Control::ArrangeOverride(availableSize);

}

/// <summary>
///     Returns the index of this row within the DataGrid's list of item containers.
/// </summary>
/// <remarks>
///     This method performs a linear search.
/// </remarks>
/// <returns>The index, if found, -1 otherwise.</returns>

int XUI::UI::DataGridRow::GetIndex()
{
    DataGrid* dataGridOwner = DataGridOwner();
    if (dataGridOwner != nullptr)
    {
        return dataGridOwner->GetItemContainerGenerator()->IndexFromContainer(this);
    }

    return -1;
}

/// <summary>
///     Searchs up the visual parent chain from the given element until
///     a DataGridRow element is found.
/// </summary>
/// <param name="element">The descendent of a DataGridRow.</param>
/// <returns>
///     The first ancestor DataGridRow of the element parameter.
///     Returns null of none is found.
/// </returns>

DataGridRow * XUI::UI::DataGridRow::GetRowContainingElement(UI::Controls::Visual * element)
{
    return nullptr; // DataGridHelper.FindVisualParent<DataGridRow>(element);
}

/// <summary>
/// Returns true if the DetailsPresenter is supposed to draw gridlines for the row.  Only true
/// if the DetailsPresenter hooked itself up properly to the Row.
/// </summary>

bool XUI::UI::DataGridRow::DetailsPresenterDrawsGridLines() const
{
    return _detailsPresenter != nullptr && _detailsPresenter->IsVisible;
}

/// <summary>
///     Acceses the CellsPresenter and attempts to get the cell at the given index.
///     This is not necessarily the display order.
/// </summary>

DataGridCell * XUI::UI::DataGridRow::TryGetCell(int index)
{
    DataGridCellsPresenter* cellsPresenter = CellsPresenter;
    if (cellsPresenter != nullptr)
    {
        return cellsPresenter->GetItemContainerGenerator()->ContainerFromIndex(index)->Static_As<DataGridCell>();
    }

    return nullptr;
}

/// <summary>
///     Determines if a row can be selected, based on the DataGrid's SelectionUnit property.
/// </summary>

bool XUI::UI::DataGridRow::IsSelectable() const
{
	DataGrid* dataGrid = DataGridOwner();
	if (dataGrid != nullptr)
	{
		DataGridSelectionUnit unit = dataGrid->SelectionUnit;
		return (unit == DataGridSelectionUnit::FullRow) ||
			(unit == DataGridSelectionUnit::CellOrRowHeader);
	}

	return true;
}

void XUI::UI::DataGridRow::OnColumnsChanged(System::Collection::Vector<std::shared_ptr<DataGridColumn>> const & columns, System::Collection::NotifyCollectionChangedEventArgs & e)
{
    DataGridCellsPresenter* cellsPresenter = CellsPresenter;
    if (cellsPresenter != nullptr)
    {
        cellsPresenter->OnColumnsChanged(columns, e);
    }
}

void XUI::UI::DataGridRow::OnNotifyRowPropertyChanged(DependencyObject * d, Core::Dependency::DependencyPropertyChangedEventArgs & e)
{
    static_cast<DataGridRow*>(d)->NotifyPropertyChanged(d, e, NotificationTarget::Rows);
}

void XUI::UI::DataGridRow::OnNotifyRowAndRowHeaderPropertyChanged(DependencyObject * d, Core::Dependency::DependencyPropertyChangedEventArgs & e)
{
	static_cast<DataGridRow*>(d)->NotifyPropertyChanged(d, e, NotificationTarget(uint32(NotificationTarget::Rows) | uint32(NotificationTarget::RowHeaders)));
}

void XUI::UI::DataGridRow::OnNotifyDetailsTemplatePropertyChanged(DependencyObject * d, Core::Dependency::DependencyPropertyChangedEventArgs & e)
{
    DataGridRow* row = static_cast<DataGridRow*>(d);
    row->NotifyPropertyChanged(row, e, NotificationTarget(uint32(NotificationTarget::Rows) | uint32(NotificationTarget::DetailsPresenter)));

    // It only makes sense to fire UnloadingRowDetails if the row details are already loaded. The same is true for LoadingRowDetails,
    // since making row details visible will take care of firing LoadingRowDetails.
    if (row->_detailsLoaded && d->GetValue(e.Property) == e.NewValue)
    {
        if (row->DataGridOwner() != nullptr)
        {
            //row->DataGridOwner()->OnUnloadingRowDetailsWrapper(row);
        }
        if (e.NewValue.GetBase())
        {
            // Invoke LoadingRowDetails, but only after the details template is expanded (so DetailsElement will be available).
            //Core::UIDispatcher::OnRender += //(new DispatcherOperationCallback(DataGrid::DelayedOnLoadingRowDetails), DispatcherPriority.Loaded, row);
        }
    }
}

void XUI::UI::DataGridRow::OnNotifyDetailsVisibilityChanged(DependencyObject * d, Core::Dependency::DependencyPropertyChangedEventArgs & e)
{
    auto row = static_cast<DataGridRow*>(d);

    // Notify the DataGrid at Loaded priority so the template has time to expland.
	//Core::UIDispatcher::OnRender += //(new DispatcherOperationCallback(DelayedRowDetailsVisibilityChanged), DispatcherPriority.Loaded, row);

    row->NotifyPropertyChanged(d, e, NotificationTarget(uint32(NotificationTarget::Rows) | uint32(NotificationTarget::DetailsPresenter)));
}

/// <summary>
///     Notifies the DataGrid that the visibility is changed.  This is intended to be Invoked at lower than Layout priority to give the template time to expand.
/// </summary>

System::Reflection::Object * XUI::UI::DataGridRow::DelayedRowDetailsVisibilityChanged(System::Reflection::Object * arg)
{
    auto row = arg->Static_As<DataGridRow>();
    auto dataGrid = row->DataGridOwner();
    auto detailsElement = row->DetailsPresenter != nullptr ? row->DetailsPresenter->DetailsElement() : nullptr;
    if (dataGrid != nullptr && dataGrid->RowDetailsVisibilityChanged)
    {
		DataGridRowDetailsEventArgs detailsEventArgs; // = new DataGridRowDetailsEventArgs(row, detailsElement);
		detailsEventArgs.Row = row;
		detailsEventArgs.DetailsElement = detailsElement;
		dataGrid->RowDetailsVisibilityChanged(dataGrid, detailsEventArgs);
    }

    return nullptr;
}

void XUI::UI::DataGridRow::OnItemChanged(Core::Dependency::DependencyPropertyChangedEventArgs & e)
{
	DataGridCellsPresenter* cellsPresenter = CellsPresenter;
	if (cellsPresenter != nullptr)
	{
		cellsPresenter->Item = e.NewValue;
	}
}

void XUI::UI::DataGridRow::OnTemplateApplied(Controls::TemplateAppliedEventArgs const & e)
{
    Controls::Control::OnTemplateApplied(e);

    _cellsPresenter = Template->FindNameByManagedType<DataGridCellsPresenter>(PART_CELLPRESENTER, this);
    _detailsPresenter = Template->FindNameByManagedType<DataGridDetailsPresenter>(PART_DETAILPRESENTER, this);
    _rowHeader = Template->FindNameByManagedType<DataGridRowHeader>(PART_HEADER, this);
}

void XUI::UI::DataGridRow::PersistAttachedItemValue(DependencyObject * objectWithProperty, Core::Dependency::DependencyProperty* property)
{
    //ValueSource valueSource = DependencyPropertyHelper.GetValueSource(objectWithProperty, property);
    //if (valueSource.BaseValueSource == BaseValueSource.Local)
    //{
    //    // attach the local value to the item so it can be restored later.
    //    _owner.ItemAttachedStorage.SetValue(Item, property, objectWithProperty.GetValue(property));
    //    objectWithProperty.ClearValue(property);
    //}
}

void XUI::UI::DataGridRow::RestoreAttachedItemValue(DependencyObject * objectWithProperty, Core::Dependency::DependencyProperty* property)
{
    //object value;
    //if (_owner.ItemAttachedStorage.TryGetValue(Item, property, out value))
    //{
    //    objectWithProperty.SetValue(property, value);
    //}
}

void XUI::UI::DataGridRow::SetRowHeader(DataGridRowHeader * value)
{
    _rowHeader = value->shared_from_base_static<DataGridRowHeader>();
}
