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
#include "DataGridColumnHeader.h"
#include "DataGrid/DataGridControl.h"
#include "DataGrid/DataGridColumn.h"
#include "DataGrid/DataGridColumnHeaderPresenter.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;
using namespace XUI::UI;

Core::Dependency::RefDependencyProperty DataGridColumnHeader::SeparatorBrushProperty;
Core::Dependency::RefDependencyProperty DataGridColumnHeader::SeparatorVisibilityProperty;
Core::Dependency::RefDependencyProperty DataGridColumnHeader::DisplayIndexProperty;
Core::Dependency::RefDependencyProperty DataGridColumnHeader::CanUserSortProperty;
Core::Dependency::RefDependencyProperty DataGridColumnHeader::SortDirectionProperty;
Core::Dependency::RefDependencyProperty DataGridColumnHeader::IsFrozenProperty;

STATIC_CLASS_INITIALIZER(DataGridColumnHeader, XUI::UI::DataGridColumnHeader::StaticClassInitializer);

/// <summary>
/// Prepares a column header to be used.  Sets up the association between the column header and its column.
/// </summary>

void XUI::UI::DataGridColumnHeader::StaticClassInitializer()
{
    SeparatorBrushProperty = (Core::Dependency::BindProperty<std::shared_ptr<Interfaces::IBrush>>::Register<DataGridColumnHeader>(nameof(SeparatorBrush)));
    SeparatorVisibilityProperty = (Core::Dependency::BindProperty<UI::Visibility>::Register<DataGridColumnHeader>(nameof(SeparatorVisibility), UI::Visibility::Visible));;
    DisplayIndexProperty = (Core::Dependency::BindProperty<int>::Register<DataGridColumnHeader>(nameof(DisplayIndex), [](XamlCPP::Core::Dependency::IDependencyObject* d, XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const & e)
    {
        auto control = static_cast<DataGridColumnHeader*>(e.Sender);
        control->NotifyPropertyChanged(control, (XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs&)e);
    }, [](Core::Dependency::DependencyObject* d, System::Reflection::Variant const& baseValue)
    {
        auto header = static_cast<DataGridColumnHeader*>(d);
        if (header->Column() != nullptr)
        {
            return header->Column()->DisplayIndex;
        }

        return -1;
    }));
    CanUserSortProperty = (Core::Dependency::BindProperty<bool>::Register<DataGridColumnHeader>(nameof(CanUserSort), nullptr, [](Core::Dependency::DependencyObject* d, System::Reflection::Variant const& baseValue) -> System::Reflection::Variant
    {
        auto header = static_cast<DataGridColumnHeader*>(d);
        if (header->Column() != nullptr)
        {
            return header->Column()->CanUserSort;
        }

        return baseValue;
    }));
    SortDirectionProperty = (Core::Dependency::BindProperty<UI::ListSortDirection>::Register<DataGridColumnHeader>(nameof(SortDirection), nullptr, [](Core::Dependency::DependencyObject* d, System::Reflection::Variant const& baseValue) -> System::Reflection::Variant
    {
        auto header = static_cast<DataGridColumnHeader*>(d);
        if (header->Column() != nullptr)
        {
            return header->Column()->SortDirection;
        }

        return baseValue;
    }));
    IsFrozenProperty = (Core::Dependency::BindProperty<bool>::Register<DataGridColumnHeader>(nameof(IsFrozen), nullptr, [](Core::Dependency::DependencyObject* d, System::Reflection::Variant const& baseValue) -> System::Reflection::Variant
    {
        auto header = static_cast<DataGridColumnHeader*>(d);
        if (header->Column() != nullptr)
        {
            return header->Column()->IsFrozen;
        }

        return baseValue;
    }));

    // Missing
    //ContentStringFormatProperty

    ContentProperty->OverrideMetadata<DataGridColumnHeader>(std::make_shared<Core::Dependency::BindPropertyMetadata>(
        System::Reflection::RefObject(),
        nullptr,
        Core::Dependency::BindingMode::OneWay,
        [](XamlCPP::Core::Dependency::IDependencyObject* d, System::Reflection::Variant const& baseValue) -> System::Reflection::Variant
        {
            auto header = static_cast<DataGridColumnHeader*>(d);
            return DataGridHelper::GetCoercedTransferPropertyValue(header, baseValue, ContentProperty.get(), header->Column(), DataGridColumn::HeaderProperty.get());
        }));

    ContentProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const & e)
    {
        auto control = e.Sender->Static_As<DataGridColumnHeader>();
        if (control)
            control->NotifyPropertyChanged(control, (XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs&)e);
    });

    ContentTemplateProperty->OverrideMetadata<DataGridColumnHeader>(std::make_shared<Core::Dependency::BindPropertyMetadata>(
        std::shared_ptr<UI::DataTemplates::DataTemplate>(),
        nullptr,
        Core::Dependency::BindingMode::OneWay,
        [](XamlCPP::Core::Dependency::IDependencyObject* d, System::Reflection::Variant const& baseValue) -> System::Reflection::Variant
    {
        auto header = static_cast<DataGridColumnHeader*>(d);
        return DataGridHelper::GetCoercedTransferPropertyValue(header, baseValue, ContentTemplateProperty.get(), header->Column(), DataGridColumn::HeaderTemplateProperty.get());
    }));

    ContentTemplateProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const & e)
    {
        auto control = e.Sender->Static_As<DataGridColumnHeader>();
        if (control)
            control->NotifyPropertyChanged(control, (XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs&)e);
    });

    StyleProperty->OverrideMetadata<DataGridColumnHeader>(std::make_shared<Core::Dependency::BindPropertyMetadata>(
        std::shared_ptr<XamlCPP::Interfaces::IStyle>(),
        nullptr,
        Core::Dependency::BindingMode::OneWay,
        [](XamlCPP::Core::Dependency::IDependencyObject* d, System::Reflection::Variant const& baseValue) -> System::Reflection::Variant
    {
        auto header = static_cast<DataGridColumnHeader*>(d);
        DataGridColumn* column = header->Column();
        DataGrid* dataGrid = nullptr;

        // Propagate style changes to any filler column headers.
        if (column == nullptr)
        {
            if (header->TemplatedParent != nullptr)
            {
                DataGridColumnHeadersPresenter* presenter = header->TemplatedParent->Static_As<DataGridColumnHeadersPresenter>();
                if (presenter != nullptr)
                {
                    dataGrid = presenter->ParentDataGrid();
                }
            }
        }
        else
        {
            dataGrid = column->DataGridOwner();
        }

        return DataGridHelper::GetCoercedTransferPropertyValue(header, baseValue, ContentTemplateProperty.get(), column, DataGridColumn::HeaderStyleProperty.get(), dataGrid, DataGrid::ColumnHeaderStyleProperty.get());
    }));

    StyleProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const & e)
    {
        auto control = e.Sender->Static_As<DataGridColumnHeader>();
        if (control)
            control->NotifyPropertyChanged(control, (XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs&)e);
    });

    HeightProperty->OverrideMetadata<DataGridColumnHeader>(std::make_shared<Core::Dependency::BindPropertyMetadata>(
        double(0.0),
        nullptr,
        Core::Dependency::BindingMode::OneWay,
        [](XamlCPP::Core::Dependency::IDependencyObject* d, System::Reflection::Variant const& baseValue) -> System::Reflection::Variant
        {
            auto header = static_cast<DataGridColumnHeader*>(d);
            DataGridColumn* column = header->Column();
            DataGrid* dataGrid = nullptr;

            // Propagate style changes to any filler column headers.
            if (column == nullptr)
            {
                if (header->TemplatedParent != nullptr)
                {
                    DataGridColumnHeadersPresenter* presenter = header->TemplatedParent->Static_As<DataGridColumnHeadersPresenter>();
                    if (presenter != nullptr)
                    {
                        dataGrid = presenter->ParentDataGrid();
                    }
                }
            }
            else
            {
                dataGrid = column->DataGridOwner();
            }

            return DataGridHelper::GetCoercedTransferPropertyValue(header, baseValue, HeightProperty.get(), dataGrid, DataGrid::ColumnHeaderHeightProperty.get());
        }));

    HeightProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const & e)
    {
        auto control = e.Sender->Static_As<DataGridColumnHeader>();
        if (control)
            control->NotifyPropertyChanged(control, (XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs&)e);
    });

    WidthProperty->OverrideMetadata<DataGridColumnHeader>(std::make_shared<Core::Dependency::BindPropertyMetadata>(
        double(0.0),
        nullptr,
        Core::Dependency::BindingMode::OneWay,
        [](XamlCPP::Core::Dependency::IDependencyObject* d, System::Reflection::Variant const& baseValue) -> System::Reflection::Variant
        {
            auto header = static_cast<DataGridColumnHeader*>(d);
            if (header)
                return DataGridHelper::GetCoercedTransferPropertyValue(header, baseValue, WidthProperty.get(), header->Column(), DataGridColumn::ActualWidthProperty.get());
            return baseValue;
        }));
}

XUI::UI::DataGridColumnHeader::DataGridColumnHeader()
{
}

XUI::UI::DataGridColumnHeader::~DataGridColumnHeader()
{
}

DataGridColumn * XUI::UI::DataGridColumnHeader::Column() const
{
    return _column.lock().get();
}

Panel * XUI::UI::DataGridColumnHeader::ParentPanel() const
{
    return VisualParent->Static_As<Panel>();
}

double XUI::UI::DataGridColumnHeader::ColumnActualWidth() const
{
    return Column() ? Column()->ActualWidth : ActualWidth();
}

GridLength XUI::UI::DataGridColumnHeader::ColumnWidth() const
{
    return Column() ? Column()->Width : GridLength::GetAuto();
}

DataGridColumnHeadersPresenter * XUI::UI::DataGridColumnHeader::ParentPresenter() const
{
    return TemplatedParent->Static_As<DataGridColumnHeadersPresenter>();
}

void XUI::UI::DataGridColumnHeader::PrepareColumnHeader(System::Reflection::Variant const & item, DataGridColumn * column)
{
    ASSERT(column != nullptr && "This header must have been generated with for a particular column");
    //ASSERT(column->Header == item && "The data item for a ColumnHeader is the Header property of a column");
    _column = column->shared_from_base_static<DataGridColumn>();
    TabIndex = column->DisplayIndex;

    CoerceValue(ContentProperty.get());
    CoerceValue(ContentTemplateProperty.get());
    //CoerceValue(ContentTemplateSelectorProperty.get());
    //CoerceValue(ContentStringFormatProperty.get());
    CoerceValue(StyleProperty.get());
    CoerceValue(HeightProperty.get());
    CoerceValue(WidthProperty.get());
	
    CoerceValue(CanUserSortProperty.get());
    CoerceValue(SortDirectionProperty.get());
    CoerceValue(IsFrozenProperty.get());
    CoerceValue(ClipProperty.get());
    CoerceValue(DisplayIndexProperty.get());
}

void XUI::UI::DataGridColumnHeader::ClearHeader()
{
    _column.reset();
}

/// <summary>
///     Notification for column header-related DependencyProperty changes from the grid or from columns.
/// </summary>

void XUI::UI::DataGridColumnHeader::NotifyPropertyChanged(Core::Dependency::DependencyObject * d, Core::Dependency::DependencyPropertyChangedEventArgs & e)
{
    DataGridColumn* column = d->Static_As<DataGridColumn>();
    if ((column != nullptr) && (column != Column()))
    {
        // This notification does not apply to this column header
        return;
    }
    
    if (e.Property == DataGridColumn::WidthProperty.get())
    {
        DataGridHelper::OnColumnWidthChanged(this, e);
    }
    else if (e.Property == DataGridColumn::ActualWidthProperty.get())
    {
        CoerceValue(WidthProperty.get());
        InvalidateMeasure();
    }
    else if (e.Property == DataGridColumn::HeaderProperty.get() || e.Property == ContentProperty.get())
    {
        CoerceValue(ContentProperty.get());
    }
    else if (e.Property == DataGridColumn::HeaderTemplateProperty.get() || e.Property == ContentTemplateProperty.get())
    {
        CoerceValue(ContentTemplateProperty.get());
    }
    /*else if (e.Property == DataGridColumn::HeaderTemplateSelectorProperty.get() || e.Property == ContentTemplateSelectorProperty.get())
    {
        CoerceValue(ContentTemplateSelectorProperty.get());
    }*/
    /*else if (e.Property == DataGridColumn::HeaderStringFormatProperty.get() || e.Property == ContentStringFormatProperty.get())
    {
        CoerceValue(ContentStringFormatProperty.get());
    }*/
    else if (e.Property == DataGrid::ColumnHeaderStyleProperty.get() || e.Property == DataGridColumn::HeaderStyleProperty.get() || e.Property == StyleProperty.get())
    {
        CoerceValue(StyleProperty.get());
    }
    else if (e.Property == DataGrid::ColumnHeaderHeightProperty.get() || e.Property == HeightProperty.get())
    {
        CoerceValue(HeightProperty.get());
    }
    else if (e.Property == DataGridColumn::DisplayIndexProperty.get())
    {
        CoerceValue(DisplayIndexProperty.get());
        TabIndex = column->DisplayIndex;
    }
    else if (e.Property == DataGrid::CanUserResizeColumnsProperty.get())
    {
        OnCanUserResizeColumnsChanged();
    }
    else if (e.Property == DataGridColumn::CanUserSortProperty.get())
    {
        CoerceValue(CanUserSortProperty.get());
    }
    else if (e.Property == DataGridColumn::SortDirectionProperty.get())
    {
        CoerceValue(SortDirectionProperty.get());
    }
    else if (e.Property == DataGridColumn::IsFrozenProperty.get())
    {
        CoerceValue(IsFrozenProperty.get());
    }
    else if (e.Property == DataGridColumn::CanUserResizeProperty.get())
    {
        OnCanUserResizeChanged();
    }
    else if (e.Property == DataGridColumn::VisibilityProperty.get())
    {
        OnColumnVisibilityChanged(e);
    }
}

/// <summary>
/// The override of ButtonBase.OnClick.
/// Informs the owning datagrid to sort itself after the execution of usual button stuff
/// </summary>

void XUI::UI::DataGridColumnHeader::OnClick(Core::Observer::RoutedEventArgs & e)
{
    if (!_suppressClickEvent)
    {
		Button::OnClick(e);

        if (!_column.expired() && !_column.lock()->_dataGridOwner.expired())
        {
			_column.lock()->DataGridOwner()->PerformSort(_column.lock().get());
        }
    }
}

void XUI::UI::DataGridColumnHeader::OnPointerPressed(Interfaces::PointerPressedEventArgs & e)
{
    Button::OnPointerPressed(e);
    
    // If clickmode is hover then during the mouse move the hover events will be sent
    // all the headers in the path. To avoid that we are using a capture
    if (ClickMode == ClickMode::Hover && e.InputModifiers == Interfaces::InputModifiers::LeftMouseButton)
    {
        e.Device->Capture(this);
    }
    
    _parentPresenter->OnHeaderMouseLeftButtonDown(e);
    e.Handled = true;
}

void XUI::UI::DataGridColumnHeader::OnPointerMoved(Interfaces::PointerEventArgs & e)
{
	Button::OnPointerMoved(e);
	_parentPresenter->OnHeaderMouseMove(e);
    e.Handled = true;
}

void XUI::UI::DataGridColumnHeader::OnPointerReleased(Interfaces::PointerReleasedEventArgs & e)
{
	Button::OnPointerReleased(e);
    
    if (ClickMode == UI::ClickMode::Release && e.Device->Captured == this)
    {
        e.Device->Capture(nullptr);
    }
    
	_parentPresenter->OnHeaderMouseLeftButtonUp(e);
    e.Handled = true;
}

void XUI::UI::DataGridColumnHeader::OnTemplateApplied(Controls::TemplateAppliedEventArgs const & e)
{
    Button::OnTemplateApplied(e);
    HookupGripperEvents();
}

/// <summary>
/// Find grippers and register drag events
///
/// The default style for DataGridHeader is
/// +-------------------------------+
/// +---------+           +---------+
/// + Gripper +  Header   + Gripper +
/// +         +           +         +
/// +---------+           +---------+
/// +-------------------------------+
/// 
/// The reason we have two grippers is we can't extend the right gripper to straddle the line between two 
/// headers; the header to the right would render on top of it.
/// We resize a column by grabbing the gripper to the right; the leftmost gripper thus adjusts the width of
/// the column to its left.
/// </summary>

void XUI::UI::DataGridColumnHeader::HookupGripperEvents()
{
    UnhookGripperEvents();
    
    _leftGripper = Template->FindNameByManagedType<Thumb>(LeftHeaderGripperTemplateName, this);
    _rightGripper = Template->FindNameByManagedType<Thumb>(RightHeaderGripperTemplateName, this);
    
    if (_leftGripper != nullptr)
    {
        _leftGripper->DragStarted += System::make_eventhandler(&DataGridColumnHeader::OnColumnHeaderGripperDragStarted, this);
        _leftGripper->DragDelta += System::make_eventhandler(&DataGridColumnHeader::OnColumnHeaderResize, this);
        _leftGripper->DragCompleted += System::make_eventhandler(&DataGridColumnHeader::OnColumnHeaderGripperDragCompleted, this);
        _leftGripper->DoubleTapped += System::make_eventhandler(&DataGridColumnHeader::OnGripperDoubleClicked, this);
        SetLeftGripperVisibility();
    }
    
    if (_rightGripper != nullptr)
    {
        _rightGripper->DragStarted += System::make_eventhandler(&DataGridColumnHeader::OnColumnHeaderGripperDragStarted, this);
        _rightGripper->DragDelta += System::make_eventhandler(&DataGridColumnHeader::OnColumnHeaderResize, this);
        _rightGripper->DragCompleted += System::make_eventhandler(&DataGridColumnHeader::OnColumnHeaderGripperDragCompleted, this);
        _rightGripper->DoubleTapped += System::make_eventhandler(&DataGridColumnHeader::OnGripperDoubleClicked, this);
        SetRightGripperVisibility();
    }
}

/// <summary>
/// Clear gripper event
/// </summary>

void XUI::UI::DataGridColumnHeader::UnhookGripperEvents()
{
    if (_leftGripper != nullptr)
    {
        _leftGripper->DragStarted -= System::make_eventhandler(&DataGridColumnHeader::OnColumnHeaderGripperDragStarted, this);
        _leftGripper->DragDelta -= System::make_eventhandler(&DataGridColumnHeader::OnColumnHeaderResize, this);
        _leftGripper->DragCompleted -= System::make_eventhandler(&DataGridColumnHeader::OnColumnHeaderGripperDragCompleted, this);
        _leftGripper->DoubleTapped -= System::make_eventhandler(&DataGridColumnHeader::OnGripperDoubleClicked, this);
        _leftGripper = nullptr;
    }
    
    if (_rightGripper != nullptr)
    {
        _rightGripper->DragStarted -= System::make_eventhandler(&DataGridColumnHeader::OnColumnHeaderGripperDragStarted, this);
        _rightGripper->DragDelta -= System::make_eventhandler(&DataGridColumnHeader::OnColumnHeaderResize, this);
        _rightGripper->DragCompleted -= System::make_eventhandler(&DataGridColumnHeader::OnColumnHeaderGripperDragCompleted, this);
        _rightGripper->DoubleTapped -= System::make_eventhandler(&DataGridColumnHeader::OnGripperDoubleClicked, this);
        _rightGripper = nullptr;
    }
}

/// <summary>
/// Returns either this header or the one before it depending on which Gripper fired the event.
/// </summary>
/// <param name="sender"></param>

DataGridColumnHeader* XUI::UI::DataGridColumnHeader::HeaderToResize(System::Reflection::Object * gripper)
{
    return (gripper == _rightGripper.get()) ? this : PreviousVisibleHeader();
}

// Save the original widths before header resize

void XUI::UI::DataGridColumnHeader::OnColumnHeaderGripperDragStarted(void * sender, Interfaces::VectorEventArgs& e)
{
    DataGridColumnHeader* header = HeaderToResize(GENERIC_SAFE_CAST(Thumb*, sender));

    if (header != nullptr)
    {
        if (header->Column() != nullptr)
        {
            DataGrid* dataGrid = header->Column()->DataGridOwner();
            if (dataGrid != nullptr)
            {
                dataGrid->GetColumns().OnColumnResizeStarted();
            }
        }

        e.Handled = true;
    }
}

void XUI::UI::DataGridColumnHeader::OnColumnHeaderResize(void * sender, Interfaces::VectorEventArgs& e)
{
    DataGridColumnHeader* header = HeaderToResize(GENERIC_SAFE_CAST(Thumb*, sender));
    if (header != nullptr)
    {
        RecomputeColumnWidthsOnColumnResize(header, e.Vector.x);
        e.Handled = true;
    }
}

/// <summary>
/// Method which recomputes the widths of columns on resize of a header
/// </summary>

void XUI::UI::DataGridColumnHeader::RecomputeColumnWidthsOnColumnResize(DataGridColumnHeader * header, double horizontalChange)
{
    ASSERT(header != nullptr && "Header should not be nullptr");

    DataGridColumn* resizingColumn = header->Column();
    if (resizingColumn == nullptr)
    {
        return;
    }

    DataGrid* dataGrid = resizingColumn->DataGridOwner();
    if (dataGrid == nullptr)
    {
        return;
    }

    dataGrid->GetColumns().RecomputeColumnWidthsOnColumnResize(resizingColumn, horizontalChange, false);
}

void XUI::UI::DataGridColumnHeader::OnColumnHeaderGripperDragCompleted(void * sender, Interfaces::VectorEventArgs& e)
{
    DataGridColumnHeader* header = HeaderToResize(GENERIC_SAFE_CAST(Thumb*, sender));

    if (header != nullptr)
    {
        if (header->Column() != nullptr)
        {
            DataGrid* dataGrid = header->Column()->DataGridOwner();
            if (dataGrid != nullptr)
            {
                dataGrid->GetColumns().OnColumnResizeCompleted(/*e.Canceled*/false);
            }
        }

        e.Handled = true;
    }
}

void XUI::UI::DataGridColumnHeader::OnGripperDoubleClicked(void * sender, Interfaces::PointerPressedEventArgs & e)
{
    DataGridColumnHeader* header = HeaderToResize(GENERIC_SAFE_CAST(Thumb*, sender));

    if (header != nullptr && header->Column() != nullptr)
    {
        // DataGridLength is a struct, so setting to Auto resets desired and display widths to 0.0.
        header->Column()->Width = GridLength::GetAuto();
        e.Handled = true;
    }
}

/// <summary>
///     Notifies the Header of a property change.
/// </summary>

void XUI::UI::DataGridColumnHeader::OnNotifyPropertyChanged(DependencyObject * d, Core::Dependency::DependencyPropertyChangedEventArgs & e)
{
    static_cast<DataGridColumnHeader*>(d)->NotifyPropertyChanged(d, e);
}

void XUI::UI::DataGridColumnHeader::OnCanUserResizeColumnsChanged()
{
    ASSERT(Column() != nullptr && "column can't be nullptr if we got a notification for this property change");
    if (Column()->DataGridOwner() != nullptr)
    {
        SetLeftGripperVisibility();
        SetRightGripperVisibility();
    }
}

void XUI::UI::DataGridColumnHeader::OnCanUserResizeChanged()
{
	ASSERT(Column() != nullptr && "column can't be nullptr if we got a notification for this property change");
	DataGrid* dataGrid = Column()->DataGridOwner();
    if (dataGrid != nullptr)
    {
        SetNextHeaderLeftGripperVisibility(Column()->CanUserResize);
        SetRightGripperVisibility();
    }
}

void XUI::UI::DataGridColumnHeader::SetLeftGripperVisibility()
{
    if (_leftGripper == nullptr || Column() == nullptr)
    {
        return;
    }

    DataGrid* dataGridOwner = Column()->DataGridOwner();
    bool canPrevColumnResize = false;
    for (int index = DisplayIndex - 1; index >= 0; index--)
    {
        DataGridColumn* column = dataGridOwner->ColumnFromDisplayIndex(index);
        if (column->IsVisible())
        {
            canPrevColumnResize = column->CanUserResize;
            break;
        }
    }
    SetLeftGripperVisibility(canPrevColumnResize);
}

void XUI::UI::DataGridColumnHeader::SetLeftGripperVisibility(bool canPreviousColumnResize)
{
    if (_leftGripper == nullptr || Column() == nullptr)
    {
        return;
    }

    DataGrid* dataGrid = Column()->DataGridOwner();
    if (dataGrid != nullptr && dataGrid->CanUserResizeColumns && canPreviousColumnResize)
    {
        _leftGripper->IsVisible = true;
    }
    else
    {
        _leftGripper->IsVisible = false;
    }
}

void XUI::UI::DataGridColumnHeader::SetRightGripperVisibility()
{
    if (_rightGripper == nullptr || Column() == nullptr)
    {
        return;
    }

    DataGrid* dataGrid = Column()->DataGridOwner();
    if (dataGrid != nullptr && dataGrid->CanUserResizeColumns && Column()->CanUserResize)
    {
        _rightGripper->IsVisible = true;
    }
    else
    {
        _rightGripper->IsVisible = false;
    }
}

void XUI::UI::DataGridColumnHeader::SetNextHeaderLeftGripperVisibility(bool canUserResize)
{
    DataGrid* dataGrid = Column()->DataGridOwner();
    int columnCount = dataGrid->GetColumns().size();
    for (int index = DisplayIndex + 1; index < columnCount; index++)
    {
        if (dataGrid->ColumnFromDisplayIndex(index)->IsVisible())
        {
            DataGridColumnHeader* nextHeader = dataGrid->ColumnHeaderFromDisplayIndex(index);
            if (nextHeader != nullptr)
            {
                nextHeader->SetLeftGripperVisibility(canUserResize);
            }
            break;
        }
    }
}

void XUI::UI::DataGridColumnHeader::OnColumnVisibilityChanged(Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
    ASSERT(Column() != nullptr  &&  "column can't be nullptr if we got a notification for this property change");
    DataGrid* dataGrid = Column()->DataGridOwner();
    if (dataGrid != nullptr)
    {
        bool oldIsVisible = (e.OldValue.GetValue<Visibility>() == Visibility::Visible);
        bool newIsVisible = (e.NewValue.GetValue<Visibility>() == Visibility::Visible);

        if (oldIsVisible != newIsVisible)
        {
            if (newIsVisible)
            {
                SetLeftGripperVisibility();
                SetRightGripperVisibility();
                SetNextHeaderLeftGripperVisibility(Column()->CanUserResize);
            }
            else
            {
                bool canPrevColumnResize = false;
                for (int index = DisplayIndex - 1; index >= 0; index--)
                {
                    DataGridColumn* column = dataGrid->ColumnFromDisplayIndex(index);
                    if (column->IsVisible())
                    {
                        canPrevColumnResize = column->CanUserResize;
                        break;
                    }
                }
                SetNextHeaderLeftGripperVisibility(canPrevColumnResize);
            }
        }
    }
}

/// <summary>
///     Used by the resize code -- this is the header that the left gripper should be resizing.
/// </summary>

DataGridColumnHeader * XUI::UI::DataGridColumnHeader::PreviousVisibleHeader()
{
	// TODO: we need to be able to find the corner header too. 
	auto column = _column.lock();
	if (column != nullptr)
	{
		auto dataGridOwner = column->_dataGridOwner.lock();
		if (dataGridOwner != nullptr)
		{
			for (int index = DisplayIndex - 1; index >= 0; index--)
			{
				if (dataGridOwner->ColumnFromDisplayIndex(index)->IsVisible())
				{
					return dataGridOwner->ColumnHeaderFromDisplayIndex(index);
				}
			}
		}
	}

	return nullptr;
}
