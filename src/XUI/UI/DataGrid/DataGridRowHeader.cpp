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
#include "DataGridRowHeader.h"
#include "DataGrid/DataGridControl.h"
#include "DataGrid/DataGridRow.h"
#include "Core/Input/KeyboardDevice.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;
using namespace XUI::UI;

Core::Dependency::RefDependencyProperty DataGridRowHeader::SeparatorBrushProperty;
Core::Dependency::RefDependencyProperty DataGridRowHeader::SeparatorVisibilityProperty;
Core::Dependency::RefDependencyProperty DataGridRowHeader::IsRowSelectedProperty;

STATIC_CLASS_INITIALIZER(DataGridRowHeader, XUI::UI::DataGridRowHeader::StaticClassInitializer)

/// <summary>
/// Measure this element and it's child elements.
/// </summary>
/// <remarks>
/// DataGridRowHeader needs to update the DataGrid's RowHeaderActualWidth & use this as it's width so that they all end up the
/// same size.
/// </remarks>
/// <param name="availableSize"></param>
/// <returns></returns>

void XUI::UI::DataGridRowHeader::StaticClassInitializer()
{
    SeparatorBrushProperty = (Core::Dependency::BindProperty<std::shared_ptr<Interfaces::IBrush>>::Register<DataGridRowHeader>(nameof(SeparatorBrush)));
    SeparatorVisibilityProperty = (Core::Dependency::BindProperty<UI::Visibility>::Register<DataGridRowHeader>(nameof(SeparatorVisibility)));
    IsRowSelectedProperty = (Core::Dependency::BindProperty<bool>::Register<DataGridRowHeader>(nameof(IsRowSelected), nullptr, [](Core::Dependency::DependencyObject* d, System::Reflection::Variant const& baseValue) -> System::Reflection::Variant
    {
        auto header = static_cast<DataGridRowHeader*>(d);
        DataGridRow* parent = header->RowOwner();
        if (parent)
        {
            return parent->IsSelected;
        }

        return baseValue;
    }));

    ContentProperty->OverrideMetadata<DataGridRowHeader>(std::make_shared<Core::Dependency::BindPropertyMetadata>(
        System::Reflection::RefObject(),
        nullptr,
        Core::Dependency::BindingMode::OneWay,
        [](XamlCPP::Core::Dependency::IDependencyObject* d, System::Reflection::Variant const& baseValue) -> System::Reflection::Variant
    {
        auto header = static_cast<DataGridRowHeader*>(d);
        return DataGridHelper::GetCoercedTransferPropertyValue(header, baseValue, ContentProperty.get(), header->RowOwner(), DataGridRow::HeaderProperty.get());
    }));

    ContentProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const & e)
    {
        auto control = e.Sender->Static_As<DataGridRowHeader>();
        if (control)
            control->NotifyPropertyChanged(control, (XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs&)e);
    });

    ContentTemplateProperty->OverrideMetadata<DataGridRowHeader>(std::make_shared<Core::Dependency::BindPropertyMetadata>(
        std::shared_ptr<UI::DataTemplates::DataTemplate>(),
        nullptr,
        Core::Dependency::BindingMode::OneWay,
        [](XamlCPP::Core::Dependency::IDependencyObject* d, System::Reflection::Variant const& baseValue) -> System::Reflection::Variant
    {
        auto header = static_cast<DataGridRowHeader*>(d);
        auto dataGrid = header->RowOwner() ? header->RowOwner()->DataGridOwner() : nullptr;
        return DataGridHelper::GetCoercedTransferPropertyValue(header, baseValue, ContentTemplateProperty.get(), header->RowOwner(), DataGridRow::HeaderTemplateProperty.get(), dataGrid, DataGrid::RowHeaderTemplateProperty.get());
    }));

    ContentTemplateProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const & e)
    {
        auto control = e.Sender->Static_As<DataGridRowHeader>();
        if (control)
            control->NotifyPropertyChanged(control, (XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs&)e);
    });

    StyleProperty->OverrideMetadata<DataGridRowHeader>(std::make_shared<Core::Dependency::BindPropertyMetadata>(
        std::shared_ptr<XamlCPP::Interfaces::IStyle>(),
        nullptr,
        Core::Dependency::BindingMode::OneWay,
        [](XamlCPP::Core::Dependency::IDependencyObject* d, System::Reflection::Variant const& baseValue) -> System::Reflection::Variant
    {
        auto header = static_cast<DataGridRowHeader*>(d);
        DataGridRow* column = header->RowOwner();
        DataGrid* dataGrid = header->DataGridOwner();

        return DataGridHelper::GetCoercedTransferPropertyValue(header, baseValue, ContentTemplateProperty.get(), column, DataGridRow::HeaderStyleProperty.get(), dataGrid, DataGrid::RowHeaderStyleProperty.get());
    }));

    StyleProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const & e)
    {
        auto control = e.Sender->Static_As<DataGridRowHeader>();
        if (control)
            control->NotifyPropertyChanged(control, (XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs&)e);
    });

    WidthProperty->OverrideMetadata<DataGridRowHeader>(std::make_shared<Core::Dependency::BindPropertyMetadata>(
        double(0.0),
        nullptr,
        Core::Dependency::BindingMode::OneWay,
        [](XamlCPP::Core::Dependency::IDependencyObject* d, System::Reflection::Variant const& baseValue) -> System::Reflection::Variant
    {
        auto header = static_cast<DataGridRowHeader*>(d);
        DataGridRow* column = header->RowOwner();
        DataGrid* dataGrid = header->DataGridOwner();

        return DataGridHelper::GetCoercedTransferPropertyValue(header, baseValue, HeightProperty.get(), dataGrid, DataGrid::RowHeaderWidthProperty.get());
    }));

    WidthProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const & e)
    {
        auto control = e.Sender->Static_As<DataGridRowHeader>();
        if (control)
            control->NotifyPropertyChanged(control, (XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs&)e);
    });

    Button::ClickModeProperty->OverrideDefaultValue<DataGridRowHeader>(XUI::UI::ClickMode::Press);
}

XUI::UI::DataGridRowHeader::DataGridRowHeader()
{
}

XUI::UI::DataGridRowHeader::~DataGridRowHeader()
{
}

DataGrid * XUI::UI::DataGridRowHeader::DataGridOwner() const
{
    if (_owner.expired())
        return nullptr;
    return _owner.lock()->DataGridOwner();
}

DataGridRow * XUI::UI::DataGridRowHeader::RowOwner() const
{
    return _owner.lock().get();
}

/// <summary>
///     Notification for column header-related DependencyProperty changes from the grid or from columns.
/// </summary>

void XUI::UI::DataGridRowHeader::NotifyPropertyChanged(Core::Dependency::DependencyObject * d, Core::Dependency::DependencyPropertyChangedEventArgs & e)
{
    if (e.Property == DataGridRow::HeaderProperty.get() || e.Property == ContentProperty.get())
    {
        CoerceValue(ContentProperty.get());
    }
    else if (e.Property == DataGrid::RowHeaderStyleProperty.get() || e.Property == DataGridRow::HeaderStyleProperty.get() || e.Property == StyleProperty.get())
    {
        CoerceValue(StyleProperty.get());
    }
    else if (e.Property == DataGrid::RowHeaderTemplateProperty.get() || e.Property == DataGridRow::HeaderTemplateProperty.get() || e.Property == ContentTemplateProperty.get())
    {
        CoerceValue(ContentTemplateProperty.get());
    }
    /*else if (e.Property == DataGrid::RowHeaderTemplateSelectorProperty.get() || e.Property == DataGridRow::HeaderTemplateSelectorProperty.get() || e.Property == ContentTemplateSelectorProperty.get())
    {
        CoerceValue(ContentTemplateSelectorProperty.get());
    }*/
    else if (e.Property == DataGrid::RowHeaderWidthProperty.get() || e.Property == WidthProperty.get())
    {
        CoerceValue(WidthProperty.get());
    }
    else if (e.Property == DataGridRow::IsSelectedProperty.get())
    {
        CoerceValue(IsRowSelectedProperty.get());
    }
    else if (e.Property == DataGrid::CanUserResizeRowsProperty.get())
    {
        OnCanUserResizeRowsChanged();
    }
    else if (e.Property == DataGrid::RowHeaderActualWidthProperty.get())
    {
        // When the RowHeaderActualWidth changes we need to re-measure to pick up the new value for DesiredSize
        InvalidateMeasure();
        InvalidateArrange();

        // If the DataGrid has not run layout the headers parent may not position the cells correctly when the header size changes.
        // This will cause the cells to be out of sync with the columns. To avoid this we will force a layout of the headers parent panel.
        auto parent = Parent;
        if (parent != nullptr)
        {
            parent->InvalidateMeasure();
            parent->InvalidateArrange();
        }
    }
}

void XUI::UI::DataGridRowHeader::SyncProperties()
{
    CoerceValue(ContentProperty.get());
    CoerceValue(StyleProperty.get());
    CoerceValue(ContentTemplateProperty.get());
    //CoerceValue(ContentTemplateSelectorProperty.get());
    CoerceValue(WidthProperty.get());
    CoerceValue(IsRowSelectedProperty.get());

    // We could be the first row now, so reset the thumb visibility.
    OnCanUserResizeRowsChanged();
}

// Save the original height before resize

void XUI::UI::DataGridRowHeader::OnRowHeaderGripperDragStarted(void * sender, Interfaces::VectorEventArgs & e)
{
    DataGridRow* rowToResize = RowToResize(GENERIC_SAFE_CAST(Thumb*, sender));
    if (rowToResize)
    {
        rowToResize->OnRowResizeStarted();
        e.Handled = true;
    }
}

void XUI::UI::DataGridRowHeader::OnRowHeaderResize(void * sender, Interfaces::VectorEventArgs & e)
{
    DataGridRow* rowToResize = RowToResize(GENERIC_SAFE_CAST(Thumb*, sender));
    if (rowToResize)
    {
        rowToResize->OnRowResize(e.Vector.y);
        e.Handled = true;
    }
}

// Restores original height if canceled.

void XUI::UI::DataGridRowHeader::OnRowHeaderGripperDragCompleted(void * sender, Interfaces::VectorEventArgs & e)
{
    DataGridRow* rowToResize = RowToResize(GENERIC_SAFE_CAST(Thumb*, sender));
    if (rowToResize)
    {
        rowToResize->OnRowResizeCompleted(/*e.Canceled*/false);
        e.Handled = true;
    }
}

void XUI::UI::DataGridRowHeader::OnGripperDoubleClicked(void * sender, Interfaces::PointerPressedEventArgs & e)
{
    DataGridRow* rowToResize = RowToResize(GENERIC_SAFE_CAST(Thumb*, sender));
    if (rowToResize)
    {
        rowToResize->OnRowResizeReset();
        e.Handled = true;
    }
}

void XUI::UI::DataGridRowHeader::OnCanUserResizeRowsChanged()
{
    SetTopGripperVisibility();
    SetBottomGripperVisibility();
}

void XUI::UI::DataGridRowHeader::SetTopGripperVisibility()
{
    if (_topGripper != nullptr)
    {
        DataGrid* dataGrid = DataGridOwner();
        DataGridRow* parent = RowOwner();
        if (dataGrid && parent &&
            dataGrid->CanUserResizeRows && dataGrid->Items->size() > 1 &&
            !parent->Item.IsFastEqual(dataGrid->Items->ElementAt(0)))
        {
            _topGripper->IsVisible = true;
        }
        else
        {
            _topGripper->IsVisible = false;
        }
    }
}

void XUI::UI::DataGridRowHeader::SetBottomGripperVisibility()
{
    if (_bottomGripper != nullptr)
    {
        DataGrid* dataGrid = DataGridOwner();
        if (dataGrid && dataGrid->CanUserResizeRows)
        {
            _bottomGripper->IsVisible = true;
        }
        else
        {
            _bottomGripper->IsVisible = false;
        }
    }
}

Core::Media::Size XUI::UI::DataGridRowHeader::MeasureOverride(Core::Media::Size const & availableSize)
{
    auto baseSize = Button::MeasureOverride(availableSize);
    
    DataGrid* dataGridOwner = DataGridOwner();
    if (dataGridOwner == nullptr)
    {
        return baseSize;
    }
    
    if ((std::isnan(dataGridOwner->RowHeaderWidth) || dataGridOwner->RowHeaderWidth == 0.0) &&
        baseSize.Width > dataGridOwner->RowHeaderActualWidth)
    {
        dataGridOwner->RowHeaderActualWidth = baseSize.Width;
    }
    
    // Regardless of how width the Header wants to be, we use 
    // DataGridOwner.RowHeaderActualWidth to ensure they're all the same size.
    return Core::Media::Size(dataGridOwner->RowHeaderActualWidth, baseSize.Height);
}

/// <summary>
///     Called when the header is clicked.
/// </summary>

void XUI::UI::DataGridRowHeader::OnClick(Core::Observer::RoutedEventArgs & e)
{
    Button::OnClick(e);

    // The base implementation took capture. This prevents us from doing
    // drag selection, so release it.
    //if (e.Device->Captured == this)
    //{
    //    ReleaseMouseCapture();
    //}
    
    DataGrid* dataGridOwner = DataGridOwner();
    DataGridRow* parentRow = RowOwner();
    if (dataGridOwner && parentRow)
    {
        dataGridOwner->HandleSelectionForRowHeaderAndDetailsInput(parentRow, /* startDragging = */ true, Core::Input::KeyboardDevice::Modifiers & Interfaces::InputModifiers::Shift);
    }
}

void XUI::UI::DataGridRowHeader::OnTemplateApplied(Controls::TemplateAppliedEventArgs const & e)
{
    Button::OnTemplateApplied(e);

    // Give the Row a pointer to the RowHeader so that it can propagate down change notifications
    DataGridRow* rowOwner = static_cast<DataGridRow*>(VisitLogicalAncestors([](Controls::Control* x) { return x->Is<DataGridRow>(); }));
    if (rowOwner)
    {
        _owner = rowOwner->shared_from_base_static<DataGridRow>();
        rowOwner->RowHeader = this;
        SyncProperties();
    }

    HookupGripperEvents();
}

/// <summary>
/// Find grippers and register drag events
///
/// The default style for DataGridRowHeader is
/// +-------------------------------+
/// +-------------------------------+ 
/// +           Gripper             + 
/// +-------------------------------+
/// +            Header             +
/// +-------------------------------+ 
/// +           Gripper             + 
/// +-------------------------------+
/// +-------------------------------+
/// 
/// The reason we have two grippers is we can't extend the bottom gripper to straddle the line between two 
/// headers; the header below would render on top of it.
/// We resize a Row by grabbing the gripper to the bottom; the top gripper thus adjusts the height of
/// the row above it.
/// </summary>

void XUI::UI::DataGridRowHeader::HookupGripperEvents()
{
    UnhookGripperEvents();

    _topGripper = Template->FindNameByManagedType<Thumb>(TopHeaderGripperTemplateName, this);
    _bottomGripper = Template->FindNameByManagedType<Thumb>(BottomHeaderGripperTemplateName, this);

    if (_topGripper != nullptr)
    {
        _topGripper->DragStarted += System::make_eventhandler(&DataGridRowHeader::OnRowHeaderGripperDragStarted, this);
        _topGripper->DragDelta += System::make_eventhandler(&DataGridRowHeader::OnRowHeaderResize, this);
        _topGripper->DragCompleted += System::make_eventhandler(&DataGridRowHeader::OnRowHeaderGripperDragCompleted, this);
        _topGripper->DoubleTapped += System::make_eventhandler(&DataGridRowHeader::OnGripperDoubleClicked, this);
        SetTopGripperVisibility();
    }

    if (_bottomGripper != nullptr)
    {
        _bottomGripper->DragStarted += System::make_eventhandler(&DataGridRowHeader::OnRowHeaderGripperDragStarted, this);
        _bottomGripper->DragDelta += System::make_eventhandler(&DataGridRowHeader::OnRowHeaderResize, this);
        _bottomGripper->DragCompleted += System::make_eventhandler(&DataGridRowHeader::OnRowHeaderGripperDragCompleted, this);
        _bottomGripper->DoubleTapped += System::make_eventhandler(&DataGridRowHeader::OnGripperDoubleClicked, this);
        SetBottomGripperVisibility();
    }
}

/// <summary>
/// Clear gripper event
/// </summary>

void XUI::UI::DataGridRowHeader::UnhookGripperEvents()
{
    if (_topGripper)
    {
        _topGripper->DragStarted -= System::make_eventhandler(&DataGridRowHeader::OnRowHeaderGripperDragStarted, this);
        _topGripper->DragDelta -= System::make_eventhandler(&DataGridRowHeader::OnRowHeaderResize, this);
        _topGripper->DragCompleted -= System::make_eventhandler(&DataGridRowHeader::OnRowHeaderGripperDragCompleted, this);
        _topGripper->DoubleTapped -= System::make_eventhandler(&DataGridRowHeader::OnGripperDoubleClicked, this);
        _topGripper = nullptr;
    }

    if (_bottomGripper)
    {
        _bottomGripper->DragStarted -= System::make_eventhandler(&DataGridRowHeader::OnRowHeaderGripperDragStarted, this);
        _bottomGripper->DragDelta -= System::make_eventhandler(&DataGridRowHeader::OnRowHeaderResize, this);
        _bottomGripper->DragCompleted -= System::make_eventhandler(&DataGridRowHeader::OnRowHeaderGripperDragCompleted, this);
        _bottomGripper->DoubleTapped -= System::make_eventhandler(&DataGridRowHeader::OnGripperDoubleClicked, this);
        _bottomGripper = nullptr;
    }
}

DataGridRow * XUI::UI::DataGridRowHeader::PreviousRow()
{
    DataGridRow* row = RowOwner();
    if (row)
    {
        DataGrid* dataGrid = row->DataGridOwner();
        if (dataGrid)
        {
            int index = dataGrid->GetItemContainerGenerator()->IndexFromContainer(row);
            if (index > 0)
            {
                return static_cast<DataGridRow*>(dataGrid->GetItemContainerGenerator()->ContainerFromIndex(index - 1));
            }
        }
    }

    return nullptr;
}
