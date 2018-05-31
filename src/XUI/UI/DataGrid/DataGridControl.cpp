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

#include "DataGrid/DataGridControl.h"
#include "DataGrid/DataGridRow.h"
#include "DataGrid/DataGridColumnHeaderPresenter.h"
#include "DataGrid/DataGridEventArgs.h"
#include "DataGrid/DataGridColumnHeader.h"
#include "DataGrid/DataGridRowHeader.h"
#include "DataGrid/DataGridRowsPresenter.h"
#include "Input/KeyboardDevice.h"
#include "Input/MouseDevice.h"
#include "UI/ScrollViewer.h"
#include "UI/VirtualizingStackPanel.h"
#include "XamlCPP/DataBind/Binding.h"
#include "System/TypeComparer.h"
#include "Reflection/Runtime/TypeCreator.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XamlCPP::Core::Dependency;
using namespace XUI;
using namespace XUI::UI;

STATIC_CLASS_INITIALIZER(DataGrid, XUI::UI::DataGrid::StaticClassInitializer)

System::Reflection::Variant DataGrid::_newItemPlaceholder;

Core::Dependency::RefDependencyProperty DataGrid::CanUserResizeColumnsProperty;
Core::Dependency::RefDependencyProperty DataGrid::ColumnWidthProperty;
Core::Dependency::RefDependencyProperty DataGrid::MinColumnWidthProperty;
Core::Dependency::RefDependencyProperty DataGrid::MaxColumnWidthProperty;
Core::Dependency::RefDependencyProperty DataGrid::GridLinesVisibilityProperty;
Core::Dependency::RefDependencyProperty DataGrid::HorizontalGridLinesBrushProperty;
Core::Dependency::RefDependencyProperty DataGrid::VerticalGridLinesBrushProperty;
Core::Dependency::RefDependencyProperty DataGrid::HorizontalGridLineThicknessProperty;
Core::Dependency::RefDependencyProperty DataGrid::VerticalGridLineThicknessProperty;
Core::Dependency::RefDependencyProperty DataGrid::RowStyleProperty;
Core::Dependency::RefDependencyProperty DataGrid::RowValidationErrorTemplateProperty;
Core::Dependency::RefDependencyProperty DataGrid::RowStyleSelectorProperty;
Core::Dependency::RefDependencyProperty DataGrid::RowBackgroundProperty;
Core::Dependency::RefDependencyProperty DataGrid::AlternatingRowBackgroundProperty;
Core::Dependency::RefDependencyProperty DataGrid::RowHeightProperty;
Core::Dependency::RefDependencyProperty DataGrid::MinRowHeightProperty;
Core::Dependency::RefDependencyProperty DataGrid::RowHeaderWidthProperty;
Core::Dependency::RefDependencyProperty DataGrid::RowHeaderActualWidthProperty;
Core::Dependency::RefDependencyProperty DataGrid::ColumnHeaderHeightProperty;
Core::Dependency::RefDependencyProperty DataGrid::HeadersVisibilityProperty;
Core::Dependency::RefDependencyProperty DataGrid::CellStyleProperty;
Core::Dependency::RefDependencyProperty DataGrid::ColumnHeaderStyleProperty;
Core::Dependency::RefDependencyProperty DataGrid::RowHeaderStyleProperty;
Core::Dependency::RefDependencyProperty DataGrid::RowHeaderTemplateProperty;
Core::Dependency::RefDependencyProperty DataGrid::RowHeaderTemplateSelectorProperty;
Core::Dependency::RefDependencyProperty DataGrid::HorizontalScrollBarVisibilityProperty;
Core::Dependency::RefDependencyProperty DataGrid::VerticalScrollBarVisibilityProperty;
Core::Dependency::RefDependencyProperty DataGrid::IsReadOnlyProperty;
Core::Dependency::RefDependencyProperty DataGrid::CurrentItemProperty;
Core::Dependency::RefDependencyProperty DataGrid::CurrentColumnProperty;
Core::Dependency::RefDependencyProperty DataGrid::CurrentCellProperty;
Core::Dependency::RefDependencyProperty DataGrid::CanUserAddRowsProperty;
Core::Dependency::RefDependencyProperty DataGrid::CanUserDeleteRowsProperty;
Core::Dependency::RefDependencyProperty DataGrid::RowDetailsVisibilityModeProperty;
Core::Dependency::RefDependencyProperty DataGrid::AreRowDetailsFrozenProperty;
Core::Dependency::RefDependencyProperty DataGrid::RowDetailsTemplateProperty;
Core::Dependency::RefDependencyProperty DataGrid::RowDetailsTemplateSelectorProperty;
Core::Dependency::RefDependencyProperty DataGrid::CanUserResizeRowsProperty;
Core::Dependency::RefDependencyProperty DataGrid::SelectionModeProperty;
Core::Dependency::RefDependencyProperty DataGrid::SelectionUnitProperty;
Core::Dependency::RefDependencyProperty DataGrid::CanUserSortColumnsProperty;
Core::Dependency::RefDependencyProperty DataGrid::AutoGenerateColumnsProperty;
Core::Dependency::RefDependencyProperty DataGrid::FrozenColumnCountProperty;
Core::Dependency::RefDependencyProperty DataGrid::NonFrozenColumnsViewportHorizontalOffsetProperty;
Core::Dependency::RefDependencyProperty DataGrid::EnableRowVirtualizationProperty;
Core::Dependency::RefDependencyProperty DataGrid::EnableColumnVirtualizationProperty;
Core::Dependency::RefDependencyProperty DataGrid::CanUserReorderColumnsProperty;
Core::Dependency::RefDependencyProperty DataGrid::DragIndicatorStyleProperty;
Core::Dependency::RefDependencyProperty DataGrid::DropLocationIndicatorStyleProperty;
Core::Dependency::RefDependencyProperty DataGrid::ClipboardCopyModeProperty;
Core::Dependency::RefDependencyProperty DataGrid::CellsPanelHorizontalOffsetProperty;
Core::Dependency::RefDependencyProperty DataGrid::HorizontalScrollOffsetProperty;

Core::Observer::RefRoutedCommand DataGrid::BeginEditCommand;
Core::Observer::RefRoutedCommand DataGrid::CommitEditCommand;
Core::Observer::RefRoutedCommand DataGrid::CancelEditCommand;
Core::Observer::RefRoutedCommand DataGrid::DeleteCommand;
Core::Observer::RefRoutedCommand DataGrid::SelectAllCommand;

/// <summary>
/// Initializes static members of the <see cref="DataGrid"/> class.
/// </summary>
void XUI::UI::DataGrid::StaticClassInitializer()
{
    _newItemPlaceholder = System::Reflection::Variant(std::make_shared<System::Reflection::Object>());

	CanUserResizeColumnsProperty = (Core::Dependency::BindProperty<bool>::Register<DataGrid>(nameof(CanUserResizeColumns), true));
	ColumnWidthProperty = (Core::Dependency::DirectProperty<DataGrid, GridLength>::RegisterDirect(nameof(ColumnWidth),
        [](DataGrid* x) { return x->ColumnWidth; },
        [](DataGrid* x, GridLength& v) { x->ColumnWidth = v; })
    );
    MinColumnWidthProperty = (Core::Dependency::BindProperty<double>::Register<DataGrid>(nameof(MinColumnWidth), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<DataGrid*>(d);
        control->NotifyPropertyChanged(control, e, NotificationTarget::Columns);
    }, nullptr, 20.0));
	MaxColumnWidthProperty = (Core::Dependency::BindProperty<double>::Register<DataGrid>(nameof(MaxColumnWidth), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<DataGrid*>(d);
        control->NotifyPropertyChanged(control, e, NotificationTarget::Columns);
    }, nullptr, std::numeric_limits<double>::max()));
	GridLinesVisibilityProperty = (Core::Dependency::BindProperty<UI::DataGridGridLinesVisibility>::Register<DataGrid>(nameof(GridLinesVisibility), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<DataGrid*>(d);
        control->NotifyPropertyChanged(control, e, NotificationTarget(uint32(NotificationTarget::Columns) | uint32(NotificationTarget::Cells) | uint32(NotificationTarget::ColumnHeaders)));
    }, nullptr, DataGridGridLinesVisibility::All));
	HorizontalGridLinesBrushProperty = (Core::Dependency::BindProperty<std::shared_ptr<Interfaces::IBrush>>::Register<DataGrid>(nameof(HorizontalGridLinesBrush), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<DataGrid*>(d);
        control->NotifyPropertyChanged(control, e, NotificationTarget(uint32(NotificationTarget::Columns) | uint32(NotificationTarget::Cells) | uint32(NotificationTarget::ColumnHeaders)));
    }, nullptr, Core::Media::Brush::Parse("Black")));
	VerticalGridLinesBrushProperty = (Core::Dependency::BindProperty<std::shared_ptr<Interfaces::IBrush>>::Register<DataGrid>(nameof(VerticalGridLinesBrush), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<DataGrid*>(d);
        control->NotifyPropertyChanged(control, e, NotificationTarget(uint32(NotificationTarget::Columns) | uint32(NotificationTarget::Cells) | uint32(NotificationTarget::ColumnHeaders)));
    }, nullptr, Core::Media::Brush::Parse("Black")));
	HorizontalGridLineThicknessProperty = (Core::Dependency::BindProperty<double>::Register<DataGrid>(nameof(HorizontalGridLineThickness), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<DataGrid*>(d);
        control->NotifyPropertyChanged(control, e, NotificationTarget(uint32(NotificationTarget::Columns) | uint32(NotificationTarget::Cells) | uint32(NotificationTarget::ColumnHeaders)));
    }, nullptr, 1.0));
	VerticalGridLineThicknessProperty = (Core::Dependency::BindProperty<double>::Register<DataGrid>(nameof(VerticalGridLineThickness), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<DataGrid*>(d);
        control->NotifyPropertyChanged(control, e, NotificationTarget(uint32(NotificationTarget::Columns) | uint32(NotificationTarget::Cells) | uint32(NotificationTarget::ColumnHeaders)));
    }, nullptr, 1.0));
	RowStyleProperty = (Core::Dependency::BindProperty<std::shared_ptr<XUI::Styling::Style>>::Register<DataGrid>(nameof(RowStyle)));
	//RowValidationErrorTemplateProperty = (Core::Dependency::BindProperty<bool>::Register<DataGrid>(nameof(RowValidationErrorTemplate)));
	RowStyleSelectorProperty = (Core::Dependency::BindProperty<std::shared_ptr<XUI::Styling::Style>>::Register<DataGrid>(nameof(RowStyleSelector)));
	RowBackgroundProperty = (Core::Dependency::BindProperty<std::shared_ptr<Interfaces::IBrush>>::Register<DataGrid>(nameof(RowBackground), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<DataGrid*>(d);
        control->NotifyPropertyChanged(control, e, NotificationTarget::Rows);
    }, nullptr));
	AlternatingRowBackgroundProperty = (Core::Dependency::BindProperty<std::shared_ptr<Interfaces::IBrush>>::Register<DataGrid>(nameof(AlternatingRowBackground), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<DataGrid*>(d);
        control->NotifyPropertyChanged(control, e, NotificationTarget(uint32(NotificationTarget::Rows) | uint32(NotificationTarget::DataGrid)));
    }, nullptr));
	RowHeightProperty = (Core::Dependency::BindProperty<double>::Register<DataGrid>(nameof(RowHeight), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<DataGrid*>(d);
        control->NotifyPropertyChanged(control, e, NotificationTarget::CellsPresenter);
    }, nullptr, -1.0));
	MinRowHeightProperty = (Core::Dependency::BindProperty<double>::Register<DataGrid>(nameof(MinRowHeight), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<DataGrid*>(d);
        control->NotifyPropertyChanged(control, e, NotificationTarget::CellsPresenter);
    }, nullptr, 10.0));
	RowHeaderWidthProperty = (Core::Dependency::BindProperty<double>::Register<DataGrid>(nameof(RowHeaderWidth), 10.0));
	RowHeaderActualWidthProperty = (Core::Dependency::BindProperty<double>::Register<DataGrid>(nameof(RowHeaderActualWidth), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<DataGrid*>(d);
        control->NotifyPropertyChanged(control, e, NotificationTarget::RowHeaders);
    }, nullptr));
	ColumnHeaderHeightProperty = (Core::Dependency::BindProperty<double>::Register<DataGrid>(nameof(ColumnHeaderHeight), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<DataGrid*>(d);
        control->NotifyPropertyChanged(control, e, NotificationTarget::ColumnHeaders);
    }, nullptr, -1.0));
	HeadersVisibilityProperty = (Core::Dependency::BindProperty<UI::DataGridHeadersVisibility>::Register<DataGrid>(nameof(HeadersVisibility)));
	CellStyleProperty = (Core::Dependency::BindProperty<std::shared_ptr<XUI::Styling::Style>>::Register<DataGrid>(nameof(CellStyle), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<DataGrid*>(d);
        control->NotifyPropertyChanged(control, e, NotificationTarget(uint32(NotificationTarget::Columns) | uint32(NotificationTarget::Cells)));
    }, nullptr));
	ColumnHeaderStyleProperty = (Core::Dependency::BindProperty<std::shared_ptr<XUI::Styling::Style>>::Register<DataGrid>(nameof(ColumnHeaderStyle), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<DataGrid*>(d);
        control->NotifyPropertyChanged(control, e, NotificationTarget(uint32(NotificationTarget::Columns) | uint32(NotificationTarget::ColumnHeaders)));
    }, nullptr));
	RowHeaderStyleProperty = (Core::Dependency::BindProperty<std::shared_ptr<XUI::Styling::Style>>::Register<DataGrid>(nameof(RowHeaderStyle), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<DataGrid*>(d);
        control->NotifyPropertyChanged(control, e, NotificationTarget(uint32(NotificationTarget::Rows) | uint32(NotificationTarget::RowHeaders)));
    }, nullptr));
	RowHeaderTemplateProperty = (Core::Dependency::BindProperty<std::shared_ptr<XUI::UI::DataTemplates::ContentTemplate>>::Register<DataGrid>(nameof(RowHeaderTemplate), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<DataGrid*>(d);
        control->NotifyPropertyChanged(control, e, NotificationTarget(uint32(NotificationTarget::Rows) | uint32(NotificationTarget::RowHeaders)));
    }, nullptr));
	//RowHeaderTemplateSelectorProperty = (Core::Dependency::BindProperty<bool>::Register<DataGrid>(nameof(RowHeaderTemplateSelector)));
	HorizontalScrollBarVisibilityProperty = (Core::Dependency::BindProperty<UI::ScrollBarVisibility>::Register<DataGrid>(nameof(HorizontalScrollBarVisibility), ScrollBarVisibility::Auto));
	VerticalScrollBarVisibilityProperty = (Core::Dependency::BindProperty<UI::ScrollBarVisibility>::Register<DataGrid>(nameof(VerticalScrollBarVisibility), ScrollBarVisibility::Auto));
	IsReadOnlyProperty = (Core::Dependency::BindProperty<bool>::Register<DataGrid>(nameof(IsReadOnly), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<DataGrid*>(d);
        //if (e.NewValue.GetValue<bool>())
        //    control->CancelAnyEdit();

        control->CoerceValue(CanUserAddRowsProperty.get());
        control->CoerceValue(CanUserDeleteRowsProperty.get());

        control->NotifyPropertyChanged(control, e, NotificationTarget(uint32(NotificationTarget::Columns) | uint32(NotificationTarget::Cells)));
    }, nullptr));
	CurrentItemProperty = (Core::Dependency::BindProperty<System::Reflection::Variant>::Register<DataGrid>(nameof(CurrentItem)));
	CurrentColumnProperty = (Core::Dependency::BindProperty<std::shared_ptr<DataGridColumn>>::Register<DataGrid>(nameof(CurrentColumn)));
	CurrentCellProperty = (Core::Dependency::DirectProperty<DataGrid, std::shared_ptr<DataGridCell>>::RegisterDirect(nameof(CurrentCell),
        [](DataGrid* x) { return x->CurrentCell; },
        [](DataGrid* x, std::shared_ptr<DataGridCell> const& v) { x->CurrentCell = v; })
    );
	CanUserAddRowsProperty = (Core::Dependency::BindProperty<bool>::Register<DataGrid>(nameof(CanUserAddRows), true));
	CanUserDeleteRowsProperty = (Core::Dependency::BindProperty<bool>::Register<DataGrid>(nameof(CanUserDeleteRows), true));
	RowDetailsVisibilityModeProperty = (Core::Dependency::BindProperty<UI::DataGridRowDetailsVisibilityMode>::Register<DataGrid>(nameof(RowDetailsVisibilityMode), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<DataGrid*>(d);
        control->NotifyPropertyChanged(control, e, NotificationTarget(uint32(NotificationTarget::Rows) | uint32(NotificationTarget::DetailsPresenter)));
    }, nullptr));
	AreRowDetailsFrozenProperty = (Core::Dependency::BindProperty<bool>::Register<DataGrid>(nameof(AreRowDetailsFrozen)));
	RowDetailsTemplateProperty = (Core::Dependency::BindProperty<std::shared_ptr<XUI::UI::DataTemplates::ContentTemplate>>::Register<DataGrid>(nameof(RowDetailsTemplate), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<DataGrid*>(d);
        control->NotifyPropertyChanged(control, e, NotificationTarget(uint32(NotificationTarget::Rows) | uint32(NotificationTarget::DetailsPresenter)));
    }, nullptr));
	//RowDetailsTemplateSelectorProperty = (Core::Dependency::BindProperty<bool>::Register<DataGrid>(nameof(RowDetailsTemplateSelector)));
	CanUserResizeRowsProperty = (Core::Dependency::BindProperty<bool>::Register<DataGrid>(nameof(CanUserResizeRows), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<DataGrid*>(d);
        control->NotifyPropertyChanged(control, e, NotificationTarget::RowHeaders);
    }, nullptr, true));
	SelectionModeProperty = (Core::Dependency::BindProperty<UI::SelectionMode>::Register<DataGrid>(nameof(SelectionMode), UI::SelectionMode::Extended));
	SelectionUnitProperty = (Core::Dependency::BindProperty<UI::DataGridSelectionUnit>::Register<DataGrid>(nameof(SelectionUnit), DataGridSelectionUnit::FullRow));
	CanUserSortColumnsProperty = (Core::Dependency::BindProperty<bool>::Register<DataGrid>(nameof(CanUserSortColumns), true));
	AutoGenerateColumnsProperty = (Core::Dependency::DirectProperty<DataGrid, bool>::RegisterDirect(nameof(AutoGenerateColumns),
		[](DataGrid* x) { return x->AutoGenerateColumns; },
		[](DataGrid* x, bool v) { x->AutoGenerateColumns = v; }, true)
	);
	FrozenColumnCountProperty = (Core::Dependency::BindProperty<int>::Register<DataGrid>(nameof(FrozenColumnCount), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<DataGrid*>(d);
        control->NotifyPropertyChanged(control, e, NotificationTarget(uint32(NotificationTarget::ColumnCollection) | uint32(NotificationTarget::CellsPresenter) | uint32(NotificationTarget::ColumnHeadersPresenter)));
    }, [](Core::Dependency::DependencyObject* d, System::Reflection::Variant const& baseValue)
    {
        auto control = static_cast<DataGrid*>(d);
        if (baseValue.GetValue<int>() > control->GetColumns().size())
            return System::Reflection::Variant(control->GetColumns().size());
        return baseValue;
    }));
	NonFrozenColumnsViewportHorizontalOffsetProperty = (Core::Dependency::BindProperty<double>::Register<DataGrid>(nameof(NonFrozenColumnsViewportHorizontalOffset)));
	EnableRowVirtualizationProperty = (Core::Dependency::BindProperty<bool>::Register<DataGrid>(nameof(EnableRowVirtualization), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<DataGrid*>(d);
    }, nullptr));
	EnableColumnVirtualizationProperty = (Core::Dependency::BindProperty<bool>::Register<DataGrid>(nameof(EnableColumnVirtualization), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<DataGrid*>(d);
        control->NotifyPropertyChanged(control, e, NotificationTarget(uint32(NotificationTarget::ColumnCollection) | uint32(NotificationTarget::CellsPresenter) | uint32(NotificationTarget::ColumnHeadersPresenter)));
    }, nullptr));
	CanUserReorderColumnsProperty = (Core::Dependency::BindProperty<bool>::Register<DataGrid>(nameof(CanUserReorderColumns), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<DataGrid*>(d);
        control->NotifyPropertyChanged(control, e, NotificationTarget::Columns);
    }, nullptr, true));
	DragIndicatorStyleProperty = (Core::Dependency::BindProperty<std::shared_ptr<XUI::Styling::Style>>::Register<DataGrid>(nameof(DragIndicatorStyle), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<DataGrid*>(d);
        control->NotifyPropertyChanged(control, e, NotificationTarget::Columns);
    }, nullptr));
	DropLocationIndicatorStyleProperty = (Core::Dependency::BindProperty<std::shared_ptr<XUI::Styling::Style>>::Register<DataGrid>(nameof(DropLocationIndicatorStyle)));
	ClipboardCopyModeProperty = (Core::Dependency::BindProperty<UI::DataGridClipboardCopyMode>::Register<DataGrid>(nameof(ClipboardCopyMode), nullptr, nullptr, DataGridClipboardCopyMode::ExcludeHeader));
	CellsPanelHorizontalOffsetProperty = (Core::Dependency::BindProperty<double>::Register<DataGrid>(nameof(CellsPanelHorizontalOffset), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<DataGrid*>(d);
        control->NotifyPropertyChanged(control, e, NotificationTarget(uint32(NotificationTarget::ColumnCollection) | uint32(NotificationTarget::CellsPresenter) | uint32(NotificationTarget::ColumnHeadersPresenter)));
    }, nullptr, 0.0));
    HorizontalScrollOffsetProperty = (Core::Dependency::BindProperty<double>::Register<DataGrid>(nameof(HorizontalScrollOffset), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<DataGrid*>(d);
        control->NotifyPropertyChanged(control, e, NotificationTarget(uint32(NotificationTarget::ColumnCollection) | uint32(NotificationTarget::CellsPresenter) | uint32(NotificationTarget::ColumnHeadersPresenter)));
    }, nullptr, 0.0));

    ItemsPanelProperty->OverrideDefaultValue<DataGrid>(std::make_shared<DataTemplates::FuncTemplate<std::shared_ptr<XUI::UI::Panel>>>([]()
    {
        return std::make_shared<DataGridRowsPresenter>();
    }));

    AlternationCountProperty->OverrideMetadata<DataGrid>(std::make_shared<Core::Dependency::BindPropertyMetadata>(
        int(0),
        [](IDependencyObject* d, System::Reflection::Variant const& baseValue) -> System::Reflection::Variant
        {
            auto control = static_cast<DataGrid*>(d);
            if (baseValue.GetValue<int>() < 2)
                return 2;
            return baseValue;
        },
        Core::Dependency::BindingMode::OneWay,
        [](IDependencyObject* d, System::Reflection::Variant const& baseValue) -> System::Reflection::Variant
        {
            auto control = static_cast<DataGrid*>(d);
            if (baseValue.GetValue<int>() < 2)
                return 2;
            return baseValue;
        }));

    CurrentCellProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const & e)
    {
        auto control = static_cast<DataGrid*>(e.Sender);
        control->OnCurrentCellChanged((XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs&)e);
    });

    BeginEditCommand = (Core::Observer::RoutedCommand::Register<DataGrid>(nameof(BeginEdit)));
    CommitEditCommand = (Core::Observer::RoutedCommand::Register<DataGrid>(nameof(CommitEdit)));
    CancelEditCommand = (Core::Observer::RoutedCommand::Register<DataGrid>(nameof(CancelEdit)));
    DeleteCommand = (Core::Observer::RoutedCommand::Register<DataGrid>(nameof(Delete)));
    SelectAllCommand = (Core::Observer::RoutedCommand::Register<DataGrid>(nameof(SelectAll)));

    BeginEditCommand->AddClassInputBinding(&typeof(DataGrid), Core::Input::KeyGesture::Parse("F2"));
    CancelEditCommand->AddClassInputBinding(&typeof(DataGrid), Core::Input::KeyGesture::Parse("Escape"));
    CommitEditCommand->AddClassInputBinding(&typeof(DataGrid), Core::Input::KeyGesture::Parse("Enter"));
    DeleteCommand->AddClassInputBinding(&typeof(DataGrid), Core::Input::KeyGesture::Parse("Delete"));
    SelectAllCommand->AddClassInputBinding(&typeof(DataGrid), Core::Input::KeyGesture::Parse("Ctrl+A"));

    BeginEditCommand->AddClassHandler(&typeof(DataGrid), std::make_shared<Core::Observer::CommandBinding>(BeginEditCommand.get(), System::make_shared_eventhandler(DataGrid::OnExecutedBeginEdit), System::make_shared_eventhandler(DataGrid::OnCanExecuteBeginEdit)));
    CommitEditCommand->AddClassHandler(&typeof(DataGrid), std::make_shared<Core::Observer::CommandBinding>(CommitEditCommand.get(), System::make_shared_eventhandler(DataGrid::OnExecutedCommitEdit), System::make_shared_eventhandler(DataGrid::OnCanExecuteCommitEdit)));
    CancelEditCommand->AddClassHandler(&typeof(DataGrid), std::make_shared<Core::Observer::CommandBinding>(CancelEditCommand.get(), System::make_shared_eventhandler(DataGrid::OnExecutedCancelEdit), System::make_shared_eventhandler(DataGrid::OnCanExecuteCancelEdit)));
    DeleteCommand->AddClassHandler(&typeof(DataGrid), std::make_shared<Core::Observer::CommandBinding>(DeleteCommand.get(), System::make_shared_eventhandler(DataGrid::OnExecutedDelete), System::make_shared_eventhandler(DataGrid::OnCanExecuteDelete)));
    SelectAllCommand->AddClassHandler(&typeof(DataGrid), std::make_shared<Core::Observer::CommandBinding>(DeleteCommand.get(), System::make_shared_eventhandler(DataGrid::OnExecutedSelectAll), System::make_shared_eventhandler(DataGrid::OnCanExecuteSelectAll)));
}

XUI::UI::DataGrid::DataGrid()
	: _columns(this)
{
    _columns.CollectionChanged += System::make_eventhandler(&DataGrid::OnColumnsChanged, this);
    AddDirectBind(BoundsProperty.get(), [this](System::Reflection::Variant const& value)
    {
        _columns.RecomputeStarColumnWidths();
    });
}

XUI::UI::DataGrid::~DataGrid()
{
}

GridLength const& XUI::UI::DataGrid::GetColumnWidth() const
{
    return _columWidth;
}

void XUI::UI::DataGrid::SetColumnWidth(GridLength& value)
{
    SetAndRaise(ColumnWidthProperty, _columWidth, value);
}

Core::Media::Size XUI::UI::DataGrid::MeasureOverride(Core::Media::Size const & availableSize)
{
	if (_dataGridColumnsInvalidated && AutoGenerateColumns)
	{
        _dataGridColumnsInvalidated = false;
		_columns.ComputeColumnWidths();
	}

    if (!_dataGridMeasureInitialized)
    {
        _dataGridMeasureInitialized = true;

        InvalidateCellsPanelHorizontalOffset();
		_columns.InitializeDisplayIndexMap();
        UpdateNewItemPlaceholder(false);
    }

    return SelectingItemsControl::MeasureOverride(availableSize);
}

Core::Media::Size XUI::UI::DataGrid::ArrangeOverride(Core::Media::Size const & arrangeSize)
{
    auto finalSize = SelectingItemsControl::ArrangeOverride(arrangeSize);

    UpdateNewItemPlaceholder(false);

    return finalSize;
}

DataGridRow * XUI::UI::DataGrid::AddNewRow()
{
	return nullptr;
}

DataGridRow * XUI::UI::DataGrid::GetRow(long id)
{
	return nullptr;
}

void XUI::UI::DataGrid::ItemsChanged(Core::Dependency::DependencyPropertyChangedEventArgs & e)
{
    if (e.OldValue.GetBase())
    {
        e.OldValue.GetValue<std::shared_ptr<System::Collection::Generic::IContainer>>()->CollectionChanged -= System::make_eventhandler(&DataGrid::OnItemsSourceCollectionChanged, this);
    }

	ItemsControl::ItemsChanged(e);

    if (!Items || Items->empty())
    {
        DeleteAutoColumns();
        InvalidateMeasure();
        return;
    }

	if (_columns.empty())
    {
        DeleteAutoColumns();
        _columns.InitializeDisplayIndexMap();
        GenerateAutoColumns();
        _dataGridColumnsInvalidated = true;
        InvalidateMeasure();
    }

    Items->CollectionChanged += System::make_eventhandler(&DataGrid::OnItemsSourceCollectionChanged, this);
}

void XUI::UI::DataGrid::OnTemplateApplied(Controls::TemplateAppliedEventArgs const & e)
{
    CleanUpInternalScrollControls();
    SelectingItemsControl::OnTemplateApplied(e);

    auto selectAllButton = Template->FindNameByManagedType<UI::Button>("PART_SelectAllButton", this);
    if (selectAllButton)
        selectAllButton->Command = DataGrid::SelectAllCommand;

    _internalScrollHost = Template->FindNameByManagedType<ScrollViewer>("PART_ScrollViewer", this);
    if (_internalScrollHost)
    {
        _internalScrollContentPresenter = _internalScrollHost->GetPresenter();
        EnsureInternalScrollControls();
    }
}

void XUI::UI::DataGrid::ApplyTemplate()
{
    SelectingItemsControl::ApplyTemplate();
}

void XUI::UI::DataGrid::OnPointerMoved(Interfaces::PointerEventArgs & e)
{
    if (_isDraggingSelection)
    {
        if (e.InputModifiers == Interfaces::InputModifiers::LeftMouseButton)
        {
            // Check that the mouse has moved relative to the DataGrid.
            // This check prevents the case where a row is partially visible
            // at the bottom. If this row is clicked, then it will be scrolled
            // into view and away from the mouse. The mouse will then appear
            // (according to these messages) as if it moved over a new cell, and
            // could invoke a drag, but the actual mouse position relative to 
            // the DataGrid hasn't changed.
            Core::Media::Point currentMousePosition = e.Device->GetRelativePosition(this);
            if (currentMousePosition != _dragPoint)
            {
                _dragPoint = currentMousePosition;

                RelativeMousePositions position = GetRelativeMousePosition();
                if (position == RelativeMousePositions::Over)
                {
                    // The mouse is within the field of cells and rows, use the actual
                    // elements to determine changes to selection.
                    if (_isRowDragging)
                    {
                        DataGridRow* row = static_cast<DataGridRow*>(e.Source->VisitVisualAncestors([](Controls::Visual* x) { return x->Is<DataGridRow>(); }));
                        if (row && (row->Item != CurrentItem))
                        {
                            // Continue a row header drag to the given row
                            HandleSelectionForRowHeaderAndDetailsInput(row, /* startDragging = */ false, true);
                            CurrentItem = row->Item;
                            e.Handled = true;
                        }
                    }
                    else
                    {
                        DataGridCell* cell = static_cast<DataGridCell*>(e.Source->VisitVisualAncestors([](Controls::Visual* x) { return x->Is<DataGridCell>(); }));
                        if (!cell)
                        {
                            DataGridRow* row = static_cast<DataGridRow*>(e.Source->VisitVisualAncestors([](Controls::Visual* x) { return x->Is<DataGridRow>(); }));
                            if (row)
                            {
                                // The mouse is over a row but not necessarily a cell,
                                // such as over a header or details section. Find the
                                // nearest cell and use that.
                                cell = GetCellNearMouse();
                            }
                        }

                        if ((cell) && (cell != _currentCellContainer.get()))
                        {
                            HandleSelectionForCellInput(cell, /* startDragging = */ false, /* allowsExtendSelect = */ true, /* allowsMinimalSelect = */ true);
                            cell->Focus();
                            e.Handled = true;
                        }
                    }
                }
                else
                {
                    // The mouse is outside of the field of cells and rows.
                    if (_isRowDragging && (position == RelativeMousePositions::Left || position == RelativeMousePositions::Right))
                    {
                        // Figure out which row the mouse is in-line with and select it
                        DataGridRow* row = GetRowNearMouse();
                        if ((row) && (row->Item != CurrentItem))
                        {
                            // The mouse is directly to the left or right of the row
                            HandleSelectionForRowHeaderAndDetailsInput(row, /* startDragging = */ false, true);
                            CurrentItem = row->Item;
                            e.Handled = true;
                        }
                    }
                    //else if (_hasAutoScrolled)
                    //{
                    //    // The mouse is outside the grid, and we've started auto-scrolling.
                    //    // The user has moved the mouse and would like a quick update.
                    //    if (DoAutoScroll())
                    //    {
                    //        e.Handled = true;
                    //    }
                    //}
                    //else
                    //{
                    //    // Ensure that the auto-scroll timer has started
                    //    StartAutoScroll();
                    //}
                }
            }
        }
        else
        {
            // The mouse button is up, end the drag operation
            EndDragging();
        }
    }
}

RelativeMousePositions XUI::UI::DataGrid::GetRelativeMousePosition()
{
    RelativeMousePositions position = RelativeMousePositions::Over;

    auto itemsHost = Presenter->Panel.get();
    if (itemsHost)
    {
        Core::Input::MouseDevice* mouseDevice = Platform::UIService::Instance()->Get<Core::Input::MouseDevice>();

        Core::Media::Point pt = mouseDevice->GetRelativePosition(itemsHost);
        Core::Media::Rect bounds(itemsHost->DesiredSize);

        if (pt.x < bounds.x0())
        {
            position |= RelativeMousePositions::Left;
        }
        else if (pt.x > bounds.x1())
        {
            position |= RelativeMousePositions::Right;
        }

        if (pt.y < bounds.y0())
        {
            position |= RelativeMousePositions::Above;
        }
        else if (pt.y > bounds.y1())
        {
            position |= RelativeMousePositions::Below;
        }
    }

    return position;
}

System::Reflection::Variant XUI::UI::DataGrid::AddNewItem()
{
    // Hide the placeholder
    UpdateNewItemPlaceholder(/* isAddingNewItem = */ true);

    _addedRowItem = System::Reflection::TypeCreator::CreateShared(*Items->GetValueType());

    if (!_placeHolderRow)
    {
        _placeHolderRow = std::make_shared<DataGridRow>();
        _placeHolderRow->BeginInit();
        _placeHolderRow->EndInit();
    }

    _placeHolderRow->_rowReference.link(_placeHolderRow.get(), this);
    _placeHolderRow->PrepareRow(_addedRowItem, this);

    if (InitializingNewItem)
    {
        InitializingNewItemEventArgs e(&_addedRowItem);
        InitializingNewItem(this, e);
    }

    Core::Observer::RoutedCommand::InvalidateRequerySuggested();

    return _addedRowItem;
}

bool XUI::UI::DataGrid::CommitAnyEdit()
{
    if (_editingCellContainer == nullptr && (IsAddingNewItem() || IsEditingRowItem()))
    {
        // There is a row edit in progress, commit it, which will also commit the cell edit.
        return CommitEdit(DataGridEditingUnit::Row, /* exitEditingMode = */ true);
    }
    else if (IsEditingCurrentCell())
    {
        // Commit the current cell edit.
        return CommitEdit(DataGridEditingUnit::Cell, /* exitEditingMode = */ true);
    }

    return true;
}

bool XUI::UI::DataGrid::EndEdit(Core::Observer::RoutedCommand * command, DataGridCell * cellContainer, DataGridEditingUnit editingUnit, bool exitEditMode)
{
    bool cellLeftEditingMode = true;
    bool rowLeftEditingMode = true;

    if (cellContainer)
    {
        if (command->CanExecute(editingUnit, cellContainer))
        {
            command->Execute(editingUnit, cellContainer);
        }

        cellLeftEditingMode = !cellContainer->IsEditing;
        rowLeftEditingMode = !IsEditingRowItem() && !IsAddingNewItem();
    }

    if (!exitEditMode)
    {
        if (editingUnit == DataGridEditingUnit::Cell)
        {
            if (cellContainer != nullptr)
            {
                if (cellLeftEditingMode)
                {
                    return BeginEdit(nullptr);
                }
            }
            else
            {
                // A cell was not placed in edit mode
                return false;
            }
        }
        else
        {
            if (rowLeftEditingMode)
            {
                System::Reflection::Variant rowItem = cellContainer->RowDataItem();
                if (rowItem.GetBase())
                {
                    EditRowItem(rowItem);
                    return IsEditingRowItem();
                }
            }

            // A row item was not placed in edit mode
            return false;
        }
    }

    return cellLeftEditingMode && ((editingUnit == DataGridEditingUnit::Cell) || rowLeftEditingMode);
}

void XUI::UI::DataGrid::EnsureInternalScrollControls()
{
    if (_internalScrollContentPresenter)
    {
        _internalScrollContentPresenter->DependencyPropertyChanged += System::make_eventhandler(&DataGrid::OnInternalScrollContentPresenterSizeChanged, this);

        if (_internalScrollHost)
        {
            _scrollViewBindingGroup.add(_internalScrollHost->AddDirectBind(ScrollViewer::OffsetProperty.get(), [this](System::Reflection::Variant const& value)
            {
                auto offset = value.GetValue<Core::Media::Vector>();
                HorizontalScrollOffset = offset.x;
            }));
        }
    }
}

void XUI::UI::DataGrid::NotifySelectedCellsChanged()
{
    //if (((_pendingSelectedCells != null) && (_pendingSelectedCells.Count > 0)) ||
    //    ((_pendingUnselectedCells != null) && (_pendingUnselectedCells.Count > 0)))
    //{
    //    // Create the new event args
    //    SelectedCellsChangedEventArgs e = new SelectedCellsChangedEventArgs(this, _pendingSelectedCells, _pendingUnselectedCells);
    //
    //    // Calculate the previous and current selection counts to determine if commands need invalidating
    //    int currentSelectionCount = _selectedCells.Count;
    //    int unselectedCellCount = (_pendingUnselectedCells != null) ? _pendingUnselectedCells.Count : 0;
    //    int selectedCellCount = (_pendingSelectedCells != null) ? _pendingSelectedCells.Count : 0;
    //    int previousSelectionCount = currentSelectionCount - selectedCellCount + unselectedCellCount;
    //
    //    // Clear the pending lists
    //    _pendingSelectedCells = null;
    //    _pendingUnselectedCells = null;
    //
    //    // Fire the public event
    //    OnSelectedCellsChanged(e);
    //
    //    // If old or new selection is empty - invalidate Copy command
    //    if ((previousSelectionCount == 0) || (currentSelectionCount == 0))
    //    {
    //        // The Copy command needs to have CanExecute run
    //        CommandManager.InvalidateRequerySuggested();
    //    }
    //}
}

void XUI::UI::DataGrid::UpdateIsSelected()
{
}

void XUI::UI::DataGrid::NotifyPropertyChanged(Core::Dependency::DependencyObject * d, DependencyPropertyChangedEventArgs & e, NotificationTarget target)
{
    if (target & NotificationTarget::DataGrid)
    {
        // If the alternate row background is set, the count may be coerced to 2
        CoerceValue(AlternationCountProperty.get());
    }

    if (target & (NotificationTarget::Rows | NotificationTarget::RowHeaders | NotificationTarget::CellsPresenter | NotificationTarget::Cells | NotificationTarget::RefreshCellContent | NotificationTarget::DetailsPresenter))
    {
        for (auto const& row : GetRows())
            row->NotifyPropertyChanged(d, e, target);
    }

    if (target & (NotificationTarget::ColumnCollection | NotificationTarget::Columns))
    {
        _columns.NotifyPropertyChanged(d, e.Property->Name, e, target);
    }

    if (target & (NotificationTarget::ColumnHeadersPresenter | NotificationTarget::ColumnHeaders) && _columnHeadersPresenter)
    {
        _columnHeadersPresenter->NotifyPropertyChanged(d, e.Property->Name, e, target);
    }
}

void XUI::UI::DataGrid::OnInternalScrollContentPresenterSizeChanged(void * m, XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs & e)
{
    if (e.Property != Visual::BoundsProperty.get())
        return;

    //if (_internalScrollContentPresenter &&
    //    !_internalScrollContentPresenter->CanContentScroll)
    //{
    //    OnViewportSizeChanged(e.PreviousSize, e.NewSize);
    //}
}

void XUI::UI::DataGrid::CleanUpInternalScrollControls()
{
    if (_scrollViewBindingGroup.is_subscribed())
        _scrollViewBindingGroup.unsubscribe();

    _internalScrollHost = nullptr;
    if (_internalScrollContentPresenter)
    {
        _internalScrollContentPresenter->DependencyPropertyChanged -= System::make_eventhandler(&DataGrid::OnInternalScrollContentPresenterSizeChanged, this);
        _internalScrollContentPresenter = nullptr;
    }
}

void XUI::UI::DataGrid::OnCanExecuteBeginEdit(void * sender, Core::Observer::CanExecuteRoutedEventArgs & e)
{
    GENERIC_SAFE_AS(DataGrid, sender)->OnCanExecuteBeginEdit(e);
}

void XUI::UI::DataGrid::OnExecutedBeginEdit(void * sender, Core::Observer::ExecutedRoutedEventArgs & e)
{
    GENERIC_SAFE_AS(DataGrid, sender)->OnExecutedBeginEdit(e);
}

/// <summary>
///     Invoked to determine if the BeginEdit command can be executed.
/// </summary>

void XUI::UI::DataGrid::OnCanExecuteBeginEdit(Core::Observer::CanExecuteRoutedEventArgs & e)
{
    bool canExecute = !IsReadOnly && (_currentCellContainer != nullptr) && !IsEditingCurrentCell() && !IsCurrentCellReadOnly() && !_hasCellValidationError;

    if (canExecute && _hasRowValidationError)
    {
        DataGridCell* cellContainer = GetEventCellOrCurrentCell(e);
        if (cellContainer != nullptr)
        {
            auto rowItem = cellContainer->RowDataItem();

            // When there is a validation error, only allow editing on that row
            canExecute = IsAddingOrEditingRowItem(rowItem);
        }
        else
        {
            // Don't allow entering edit mode when there is a pending validation error
            canExecute = false;
        }
    }

    if (canExecute)
    {
        e.CanExecute = true;
        e.Handled = true;
    }
    else
    {
        e.ContinueRouting = true;
    }
}

/// <summary>
///     Invoked when the BeginEdit command is executed.
/// </summary>

void XUI::UI::DataGrid::OnExecutedBeginEdit(Core::Observer::ExecutedRoutedEventArgs & e)
{
    DataGridCell* cell = _currentCellContainer.get();
    if ((cell != nullptr) && !cell->IsReadOnly && !cell->IsEditing)
    {
        bool addedPlaceholder = false;
        bool deselectedPlaceholder = false;
        bool reselectPlaceholderCells = false;
        std::set<int> columnIndexRanges;
        int newItemIndex = -1;
        System::Reflection::Variant newItem;
        bool placeholderAtBeginning = _newItemPlaceholderPosition == NewItemPlaceholderPosition::AtBeginning;
    
        if (IsNewItemPlaceholder(cell->RowDataItem()))
        {
            // If editing the new item placeholder, then create a new item and edit that instead.
            if (SelectedItems.Contains(DataGrid::_newItemPlaceholder))
            {
                // Unselect the NewItemPlaceholder and select the new row
                UnselectItem(DataGrid::_newItemPlaceholder);
                deselectedPlaceholder = true;
            }
            else
            {
                // Cells will automatically unselect when the new item placeholder is removed, but we
                // should reselect them on the new item.
                newItemIndex = Items->IndexOf(cell->RowDataItem());

                int itemIndex = Items->IndexOf(cell->RowDataItem());
                int numColumns = _columns.size();
                if ((itemIndex >= 0) && (numColumns > 0))
                {
                    for (auto iCell = _selectedCells.begin(); iCell != _selectedCells.end();)
                    {
                        if (cell->RowDataItem() == (*iCell)->RowDataItem())
                        {
                            columnIndexRanges.insert((*iCell)->Column->DisplayIndex);
                        }

                        ++iCell;
                    }
                }

                reselectPlaceholderCells = ((newItemIndex >= 0) && !columnIndexRanges.empty());
            }
            
            newItem = AddNewItem();
            CurrentItem = newItem; // Puts focus on the added row
            _editingRowItem = newItem;

            // Display the new item placeholder again
            UpdateNewItemPlaceholder(/* isAddingNewItem = */ false);
            {
                int currentCellIndex = cell->Column->DisplayIndex;
                cell = nullptr;

                for (auto const& _cell : _placeHolderRow->GetCells())
                {
                    if (_cell->Column->DisplayIndex == currentCellIndex)
                    {
                        currentCellIndex = -1;
                        cell = _cell.getTarget();
                        SetFocusedCell(_cell->shared_from_base_static<DataGridCell>());
                        break;
                    }
                }

                // CurrentCellContainer becomes nullptr if focus moves out of the datagrid
                // Calling UpdateLayout instantiates the CurrentCellContainer
                InvalidateArrange();
            }
            
            if (deselectedPlaceholder)
            {
                // Re-select the new item if the placeholder was selected before
                SelectItem(newItem);
            }
            else if (reselectPlaceholderCells)
            {
                // Re-select placeholder cells if they were selected before
                {
                    ChangingSelectedCellsHelper g(this);
                    int rowIndex = newItemIndex;

                    UnselectItem(DataGrid::_newItemPlaceholder);

                    for (auto const& cell : _placeHolderRow->GetCells())
                    {
                        if (columnIndexRanges.find(cell->Column->DisplayIndex) != columnIndexRanges.end())
                        {
                            CellIsSelectedChanged(cell.getTarget(), true);
                            cell->SyncIsSelected(true);
                            if (!GetSelectedItems().Contains(newItem))
                                GetSelectedItems().Add(newItem);
                        }
                    }
                }
            }
            
            addedPlaceholder = true;
        }
    
        Core::Observer::RoutedEventArgs& editingEventArgs = e.Parameter.GetValue<Core::Observer::RoutedEventArgs&>();
        DataGridBeginningEditEventArgs beginningEditEventArgs;
    
        if (cell != nullptr && BeginningEdit)
        {
            // Give the callback an opportunity to cancel edit mode
            beginningEditEventArgs._dataGridColumn = cell->Column.get();
            beginningEditEventArgs._dataGridRow = cell->RowOwner();
            beginningEditEventArgs._editingEventArgs = &editingEventArgs;
            BeginningEdit(this, beginningEditEventArgs);
        }
    
        if ((cell == nullptr) || beginningEditEventArgs._cancel)
        {
            // If CurrentCellContainer is nullptr then cancel editing
            if (deselectedPlaceholder)
            {
                // If the new item placeholder was deselected and the new item was selected,
                // de-select the new item. Selecting the new item placeholder comes at the end.
                // This is to accomodate the scenario where the new item placeholder only appears
                // when not editing a new item.
                UnselectItem(newItem);
            }
            else if (reselectPlaceholderCells && placeholderAtBeginning)
            {
                // When the placeholder is at the beginning, we need to unselect the added item cells.
                UnselectItem(DataGrid::_newItemPlaceholder);
            }
    
            if (addedPlaceholder)
            {
                // The edit was canceled, cancel the new item
                _editedRowItem = System::Reflection::Variant::Empty;
            
                // Display the new item placeholder again
                UpdateNewItemPlaceholder(/* isAddingNewItem = */ false);
            
                // Put focus back on the placeholder
                CurrentItem = newItem;
                _editingRowItem = newItem;
            }
            
            if (deselectedPlaceholder)
            {
                // If the new item placeholder was deselected, then select it again.
                SelectItem(DataGrid::_newItemPlaceholder);
            }
            else if (reselectPlaceholderCells)
            {
                for (auto const& cell : _placeHolderRow->GetCells())
                {
                    if (columnIndexRanges.find(cell->Column->DisplayIndex) != columnIndexRanges.end())
                    {
                        CellIsSelectedChanged(cell.getTarget(), true);
                        cell->SyncIsSelected(true);
                        if (!GetSelectedItems().Contains(newItem))
                            GetSelectedItems().Add(newItem);
                    }
                }
            }
        }
        else
        {
            if (!addedPlaceholder && !IsEditingRowItem())
            {
                EditRowItem(cell->RowDataItem());
            
                _editingRowItem = cell->RowDataItem();
                _editingRowIndex = Items->IndexOf(_editingRowItem);
            }
    
            cell->BeginEdit(editingEventArgs);
            cell->RowOwner()->IsEditing = true;
            _editingCellContainer = _currentCellContainer;
        }
    }
    
    // CancelEdit and CommitEdit rely on IsAddingNewItem and IsEditingRowItem
    Core::Observer::RoutedCommand::InvalidateRequerySuggested();
    
    e.Handled = true;
}

void XUI::UI::DataGrid::OnCanExecuteCommitEdit(void * sender, Core::Observer::CanExecuteRoutedEventArgs & e)
{
    GENERIC_SAFE_AS(DataGrid, sender)->OnCanExecuteCommitEdit(e);
}

void XUI::UI::DataGrid::OnExecutedCommitEdit(void* sender, Core::Observer::ExecutedRoutedEventArgs & e)
{
    GENERIC_SAFE_AS(DataGrid, sender)->OnExecutedCommitEdit(e);
}

DataGridCell * XUI::UI::DataGrid::GetEventCellOrCurrentCell(Core::Observer::RoutedEventArgs & e)
{
    // If the command routed through a cell, then use that cell. Otherwise, use the current cell.
    auto source = e.Source->Static_As<Controls::Control>();
    auto cell = ((source == this) || (source == nullptr)) ? _editingCellContainer.get() : source->Static_As<DataGridCell>();
    if (!cell)
        cell = static_cast<DataGridCell*>(source->VisitLogicalAncestors([](Controls::Control* x) { return x->Is<DataGridCell>(); }));
    return cell;
}

bool XUI::UI::DataGrid::CanEndEdit(Core::Observer::CanExecuteRoutedEventArgs & e, bool commit)
{
    DataGridCell* cellContainer = GetEventCellOrCurrentCell(e);
    if (cellContainer == nullptr)
    {
        // If there is no cell, then nothing can be determined. So, no edit could end.
        return false;
    }
    
    DataGridEditingUnit editingUnit = GetEditingUnit(e.Parameter);
    //IEditableCollectionView editableItems = EditableItems;
    auto rowItem = cellContainer->RowDataItem();
    
    // Check that there is an appropriate pending add or edit.
    // - If any cell is in edit mode
    // - OR If the editing unit is row AND one of:
    //   - There is a pending add OR
    //   - There is a pending edit
    return cellContainer->IsEditing ||
        ((editingUnit == DataGridEditingUnit::Row) &&
            !_hasCellValidationError // &&
            //((editableItems.IsAddingNew && (editableItems.CurrentAddItem == rowItem)) ||
            //(editableItems.IsEditingItem && (commit || editableItems.CanCancelEdit || _hasRowValidationError) && (editableItems.CurrentEditItem == rowItem)))
            );
}

/// <summary>
///     Invoked to determine if the CommitEdit command can be executed.
/// </summary>

void XUI::UI::DataGrid::OnCanExecuteCommitEdit(Core::Observer::CanExecuteRoutedEventArgs & e)
{
    if (CanEndEdit(e, /* commit = */ true))
    {
        e.CanExecute = true;
        e.Handled = true;
    }
    else
    {
        e.ContinueRouting = true;
    }
}

/// <summary>
///     Invoked when the CommitEdit command is executed.
/// </summary>

void XUI::UI::DataGrid::OnExecutedCommitEdit(Core::Observer::ExecutedRoutedEventArgs & e)
{
    DataGridCell* cell = _editingCellContainer.get();
    bool validationPassed = true;
    if (cell != nullptr)
    {
        DataGridEditingUnit editingUnit = GetEditingUnit(e.Parameter);
    
        bool eventCanceled = false;
        if (cell->IsEditing)
        {
            DataGridCellEditEndingEventArgs cellEditEndingEventArgs;
            if (CellEditEnding)
            {
                cellEditEndingEventArgs._dataGridColumn = cell->Column.get();
                cellEditEndingEventArgs._dataGridRow = cell->RowOwner();
                cellEditEndingEventArgs._editingElement = cell;
                CellEditEnding(this, cellEditEndingEventArgs);
            }
    
            eventCanceled = cellEditEndingEventArgs._cancel;
            if (!eventCanceled)
            {
                validationPassed = cell->CommitEdit();
                //HasCellValidationError = !validationPassed;
            }
        }
    
        // Consider commiting the row if:
        // 1. Validation passed on the cell or no cell was in edit mode.
        // 2. A cell in edit mode didn't have it's ending edit event canceled.
        // 3. The row can be commited:
        //    a. The row is being edited or added and being targeted directly.
        //    b. If the row is being edited (not added), but it doesn't support
        //       pending changes, then tell the row to commit (this doesn't really
        //       do anything except update the IEditableCollectionView state)
        if (validationPassed &&
            !eventCanceled &&
            (((editingUnit == DataGridEditingUnit::Row) && IsAddingOrEditingRowItem(cell->RowDataItem())) || (/*!CanCancelEdit &&*/ IsEditingItem(cell->RowDataItem()))))
        {
            DataGridRowEditEndingEventArgs rowEditEndingEventArgs;
            if (RowEditEnding)
            {
                rowEditEndingEventArgs._dataGridRow = cell->RowOwner();
                RowEditEnding(this, rowEditEndingEventArgs);
            }
    
            if (!rowEditEndingEventArgs._cancel)
            {
                //HasRowValidationError = !validationPassed;
                if (validationPassed)
                {
                    _editedRowItem = CurrentItem;
                }
            }
        }

        if (IsAddingNewItem() || _editingRowIndex == -1)
        {
            UpdateNewItemPlaceholder(true);

            if (_placeHolderRow)
            {
                _placeHolderRow->_rowReference.link(_placeHolderRow.get(), this);
                _placeHolderRow->PrepareRow(DataGrid::_newItemPlaceholder, this);
            }

            if (validationPassed)
                Items->Add(_addedRowItem);
            _addedRowItem = System::Reflection::Variant::Empty;

            UpdateNewItemPlaceholder(false);
        }

        if (validationPassed)
        {
            // Update the state of row editing
            UpdateRowEditing(cell);
        }

        // CancelEdit and CommitEdit rely on IsAddingNewItem and IsEditingRowItem
        Core::Observer::RoutedCommand::InvalidateRequerySuggested();

        _editingCellContainer = nullptr;
    }
    
    e.Handled = true;
}

void XUI::UI::DataGrid::OnCanExecuteCancelEdit(void * sender, Core::Observer::CanExecuteRoutedEventArgs & e)
{
    GENERIC_SAFE_AS(DataGrid, sender)->OnCanExecuteCancelEdit(e);
}

void XUI::UI::DataGrid::OnExecutedCancelEdit(void * sender, Core::Observer::ExecutedRoutedEventArgs & e)
{
    GENERIC_SAFE_AS(DataGrid, sender)->OnExecutedCancelEdit(e);
}

/// <summary>
///     Invoked to determine if the CancelEdit command can be executed.
/// </summary>

void XUI::UI::DataGrid::OnCanExecuteCancelEdit(Core::Observer::CanExecuteRoutedEventArgs & e)
{
    if (CanEndEdit(e, /* commit = */ false))
    {
        e.CanExecute = true;
        e.Handled = true;
    }
    else
    {
        e.ContinueRouting = true;
    }
}

void XUI::UI::DataGrid::OnExecutedCancelEdit(Core::Observer::ExecutedRoutedEventArgs & e)
{
    DataGridCell* cell = _editingCellContainer.get();
    if (cell != nullptr)
    {
        DataGridEditingUnit editingUnit = GetEditingUnit(e.Parameter);
    
        bool eventCanceled = false;
        if (cell->IsEditing)
        {
            DataGridCellEditEndingEventArgs cellEditEndingEventArgs;
            if (CellEditEnding)
            {
                cellEditEndingEventArgs._dataGridColumn = cell->Column.get();
                cellEditEndingEventArgs._dataGridRow = cell->RowOwner();
                cellEditEndingEventArgs._editingElement = cell;
                cellEditEndingEventArgs._commit = false;
                CellEditEnding(this, cellEditEndingEventArgs);
            }
    
            eventCanceled = cellEditEndingEventArgs._cancel;
            if (!eventCanceled)
            {
                cell->CancelEdit();
                //HasCellValidationError = false;
            }
        }
    
        //auto editableItems = _editableItems;
        bool needsCommit = false; /*IsEditingItem(cell->RowDataItem()) && !editableItems.CanCancelEdit*/;
        if (!eventCanceled &&
            (CanCancelAddingOrEditingRowItem(editingUnit, cell->RowDataItem()) || needsCommit))
        {
            bool cancelAllowed = true;
    
            if (!needsCommit)
            {
                DataGridRowEditEndingEventArgs rowEditEndingEventArgs;
                if (RowEditEnding)
                {
                    rowEditEndingEventArgs._dataGridRow = cell->RowOwner();
                    rowEditEndingEventArgs._commit = false;
                    RowEditEnding(this, rowEditEndingEventArgs);
                }
                cancelAllowed = !rowEditEndingEventArgs._cancel;
            }
    
            if (cancelAllowed)
            {
                if (needsCommit)
                {
                    // If the row is being edited (not added), but it doesn't support
                    // pending changes, then tell the item to commit (this doesn't really
                    // do anything except update the IEditableCollectionView state).
                    // This allows us to exit row editing mode.
                    cell->CommitEdit();
                }

                if (IsAddingNewItem() || _editingRowIndex == -1)
                {
                    UpdateNewItemPlaceholder(true);

                    if (_placeHolderRow)
                    {
                        _placeHolderRow->_rowReference.link(_placeHolderRow.get(), this);
                        _placeHolderRow->PrepareRow(DataGrid::_newItemPlaceholder, this);
                    }

                    UpdateNewItemPlaceholder(false);
                }

                _addedRowItem = System::Reflection::Variant::Empty;
                _editedRowItem = System::Reflection::Variant::Empty;
            }
        }
    
        // Update the state of row editing
        UpdateRowEditing(cell);
    
        //if (!cell->RowOwner()->IsEditing)
        //{
        //    // Allow the user to cancel the row and avoid being locked to that row.
        //    // If the row is still not valid, it means that the source data is already
        //    // invalid, and that is OK.
        //    HasRowValidationError = false;
        //}
    
        // CancelEdit and CommitEdit rely on IsAddingNewItem and IsEditingRowItem
        Core::Observer::RoutedCommand::InvalidateRequerySuggested();

        _editingCellContainer = nullptr;
    }

    e.Handled = true;
}

void XUI::UI::DataGrid::OnCanExecuteDelete(void * sender, Core::Observer::CanExecuteRoutedEventArgs & e)
{
    GENERIC_SAFE_AS(DataGrid, sender)->OnCanExecuteDelete(e);
}

void XUI::UI::DataGrid::OnExecutedDelete(void * sender, Core::Observer::ExecutedRoutedEventArgs & e)
{
    GENERIC_SAFE_AS(DataGrid, sender)->OnExecutedDelete(e);
}

void XUI::UI::DataGrid::OnCanExecuteDelete(Core::Observer::CanExecuteRoutedEventArgs & e)
{
    e.CanExecute = CanUserDeleteRows &&  // User is allowed to delete
        (DataItemsSelected() > 0) &&       // There is a selection
        ((_currentCellContainer == nullptr) || !_currentCellContainer->IsEditing); // Not editing a cell
    e.Handled = true;
}

/// <summary>
///     Invoked when the Delete command is executed.
/// </summary>

void XUI::UI::DataGrid::OnExecutedDelete(Core::Observer::ExecutedRoutedEventArgs & e)
{
    if (DataItemsSelected() > 0)
    {
        bool shouldDelete = false;
        bool isEditingRowItem = IsEditingRowItem();
        if (isEditingRowItem || IsAddingNewItem())
        {
            // If editing or adding a row, cancel that edit.
            if (CancelEdit(DataGridEditingUnit::Row) && isEditingRowItem)
            {
                // If adding, we're done. If editing, then an actual delete
                // needs to happen.
                shouldDelete = true;
            }
        }
        else
        {
            // There is no pending edit, just delete.
            shouldDelete = true;
        }

        if (shouldDelete)
        {
            // Normally, the current item will be within the selection,
            // deteremine a new item to select once the items are removed.
            int numSelected = SelectedItems.size();
            int indexToSelect = -1;
            auto currentItem = CurrentItem;

            // The current item is in the selection
            if (SelectedItems.Contains(currentItem))
            {
                // Choose the smaller index between the anchor and the current item
                // as the index to select after the items are removed.
                indexToSelect = Items->IndexOf(currentItem);
                if (_selectionAnchor != nullptr)
                {
                    int anchorIndex = Items->IndexOf(_selectionAnchor->RowDataItem());
                    if ((anchorIndex >= 0) && (anchorIndex < indexToSelect))
                    {
                        indexToSelect = anchorIndex;
                    }
                }

                indexToSelect = std::min((int)Items->size() - numSelected - 1, indexToSelect);
            }

            // Save off the selected items. The selected items are going to be cleared
            // first as a performance optimization. When items are removed, they are checked
            // against the selected items to be removed from that collection. This can be slow
            // since each item could cause a linear search of the selected items collection.
            // Since it is known that all of the selected items are going to be deleted, they
            // can safely be unselected.
            auto itemsToRemove(SelectedItems);

            {
                ChangingSelectedCellsHelper g(this);
                UnSelectAll();
            }

            // We are not going to defer the rest of the selection change due to existing
            // Selector behavior. When an item is removed from the ItemsSource, the Selector
            // will immediately remove it from SelectedItems. In this process, it starts a 
            // defer, which asserts because this code would have already started a defer.

            // Remove the items that are selected
            for (int i = 0; i < numSelected; i++)
            {
                auto itemToRemove = itemsToRemove[i];
                if (itemToRemove != DataGrid::NewPlaceHolder())
                    Items->Remove(itemToRemove);
            }

            // Select a new item
            if (indexToSelect >= 0)
            {
                auto itemToSelect = Items->ElementAt(indexToSelect);

                // This should focus the row and bring it into view.
                CurrentItem = itemToSelect;

                // Since the current cell should be in view, there should be a container
                DataGridCell* cell = _currentCellContainer.get();
                if (cell != nullptr)
                {
                    _selectionAnchor = nullptr;
                    HandleSelectionForCellInput(cell, /* startDragging = */ false, /* allowsExtendSelect = */ false, /* allowsMinimalSelect = */ false);
                }
            }
        }
    }

    e.Handled = true;
}

void XUI::UI::DataGrid::OnCanExecuteSelectAll(void * sender, Core::Observer::CanExecuteRoutedEventArgs & e)
{
    DataGrid* dataGrid = GENERIC_SAFE_AS(DataGrid, sender);
    e.CanExecute = (dataGrid->SelectionMode == UI::SelectionMode::Extended) && dataGrid->IsEnabled;
    e.Handled = true;
}

void XUI::UI::DataGrid::OnExecutedSelectAll(void * sender, Core::Observer::ExecutedRoutedEventArgs & e)
{
    GENERIC_SAFE_AS(DataGrid, sender)->SelectAll();
}

void XUI::UI::DataGrid::PerformSort(DataGridColumn * sortColumn)
{
    ASSERT(sortColumn != nullptr && "column should not be nullptr");

    if (!CanUserSortColumns || !sortColumn->CanUserSort)
    {
        return;
    }

    if (CommitAnyEdit())
    {
        PrepareForSort(sortColumn);

        DataGridSortingEventArgs e;
        if (Sorting)
        {
            e.Column = sortColumn;
            Sorting(this, e);
        }

        if (!e.Handled)
            DefaultSort(sortColumn, false/* (Keyboard.Modifiers & ModifierKeys.Shift) != ModifierKeys.Shift */);

        InvalidateArrange();

        //if (Items.NeedsRefresh)
        //{
        //    //try
        //    {
        //        Items.Refresh();
        //    }
        //    //catch (System::InvalidOperationException invalidOperationException)
        //    //{
        //    //    Items.SortDescriptions.Clear();
        //    //    throw new InvalidOperationException(SR.Get(SRID.DataGrid_ProbableInvalidSortDescription), invalidOperationException);
        //    //}
        //}
    }
}

/// <summary>
///     Updates the reference to this DataGrid on the list of columns.
/// </summary>
/// <param name="list">The list of affected columns.</param>
/// <param name="clear">Whether to add or remove the reference to this grid.</param>

void XUI::UI::DataGrid::UpdateDataGridReference(std::shared_ptr<DataGridColumn>& column, bool clear)
{
    //int numItems = list->size();
    //for (int i = 0; i < numItems; i++)
    {
        //DataGridColumn* column = list->ElementAt(i).GetValue<DataGridColumn*>();
        if (clear)
        {
            // Set the owner to nullptr only if the current owner is this grid
            if (!column->_dataGridOwner.expired() && column->_dataGridOwner.lock().get() == this)
            {
                column->_dataGridOwner.reset();
            }
        }
        else
        {
            // Remove the column from any old owner
            if (!column->_dataGridOwner.expired() && column->_dataGridOwner.lock().get() != this)
            {
                column->_dataGridOwner.lock()->_columns.Remove(column->shared_from_base_static<DataGridColumn>());
            }

            column->_dataGridOwner = shared_from_base_static<DataGrid>();
        }
    }
}

/// <summary>
///     Returns the DataGridColumn with the given DisplayIndex
/// </summary>

DataGridColumn * XUI::UI::DataGrid::ColumnFromDisplayIndex(int displayIndex)
{
    //System::ArgumentException;
    if (displayIndex < 0 || displayIndex >= _columns.size())
        throw System::ArgumentOutOfRangeException("displayIndex"/*, displayIndex, SR.Get(SRID.DataGrid_DisplayIndexOutOfRange)*/);

    return _columns.ColumnFromDisplayIndex(displayIndex);
}

/// <summary>
///     A map of display index (key) to index in the column collection (value).  
///     Used by the CellsPanel to quickly find a child from a column display index.
/// </summary>

std::vector<int>& XUI::UI::DataGrid::DisplayIndexMap() { return _columns.DisplayIndexMap(); }

/// <summary>
///     Throws an ArgumentOutOfRangeException if the given displayIndex is invalid.
/// </summary>

void XUI::UI::DataGrid::ValidateDisplayIndex(DataGridColumn * column, int displayIndex)
{
    _columns.ValidateDisplayIndex(column, displayIndex);
}

/// <summary>
///     Returns the index of a column from the given DisplayIndex
/// </summary>

int XUI::UI::DataGrid::ColumnIndexFromDisplayIndex(int displayIndex)
{
    if (displayIndex >= 0 && displayIndex < DisplayIndexMap().size())
    {
        return DisplayIndexMap()[displayIndex];
    }

    return -1;
}

/// <summary>
///     Given the DisplayIndex of a column returns the DataGridColumnHeader for that column.
///     Used by DataGridColumnHeader to find its previous sibling.
/// </summary>
/// <param name="displayIndex"></param>
/// <returns></returns>

DataGridColumnHeader * XUI::UI::DataGrid::ColumnHeaderFromDisplayIndex(int displayIndex)
{
    int columnIndex = ColumnIndexFromDisplayIndex(displayIndex);

    if (columnIndex != -1)
    {
        if (_columnHeadersPresenter != nullptr && _columnHeadersPresenter->GetItemContainerGenerator() != nullptr)
        {
            return _columnHeadersPresenter->GetItemContainerGenerator()->ContainerFromIndex(columnIndex)->Static_As<DataGridColumnHeader>();
        }
    }

    return nullptr;
}


/// <summary>
///     Called by DataGridColumnCollection when columns' DisplayIndex changes
/// </summary>
/// <param name="e"></param>

void XUI::UI::DataGrid::UpdateColumnsOnVirtualizedCellInfoCollections(System::Collection::NotifyCollectionChangedAction action, int oldDisplayIndex, DataGridColumn * oldColumn, int newDisplayIndex)
{
	//ChangingSelectedCellsHelper g(this);
	//{
	//	_selectedCells.OnColumnsChanged(action, oldDisplayIndex, oldColumn, newDisplayIndex, SelectedItems);
	//}
}


/// <summary>
/// Helper method which return any one of the cells or column headers
/// </summary>
/// <returns></returns>


/// <summary>
///     Helper method which queue a request to dispatcher to
///     invalidate the cellspanel offset if not already queued
/// </summary>

void XUI::UI::DataGrid::QueueInvalidateCellsPanelHorizontalOffset()
{
	//if (!_cellsPanelHorizontalOffsetComputationPending)
	//{
	//	Core::UIDispatcher::OnRender += [&]() { InvalidateCellsPanelHorizontalOffset(); };
	//	_cellsPanelHorizontalOffsetComputationPending = true;
	//}
}

UI::Controls::Control * XUI::UI::DataGrid::GetAnyCellOrColumnHeader()
{
	// Find the best try at a visible cell from a visible row.
	if (GetItemContainerGenerator() != nullptr)
	{
        for (auto const& row : GetRows())
        {
            for (auto const& cell : row->GetCells())
            {
                if (cell->IsVisible)
                    return cell.getTarget();
            }
        }
	}

	// If the row that we found earlier is not a good choice try a column header.
	// If no good column header is found the fall back will be the cell.
	if (_columnHeadersPresenter != nullptr)
	{
        
        for (auto const& header : _columnHeadersPresenter->GetHeaders())
        {
            if (header->IsVisible)
                return header.getTarget();
        }
	}

	return nullptr;
}

void XUI::UI::DataGrid::SetFocusedCell(std::shared_ptr<DataGridCell> const & cell)
{
    if (_focusedCell != cell)
    {
        if (_focusedCell != nullptr)
            CurrentCell = nullptr;
        _focusedCell = cell;
        if (_focusedCell != nullptr)
            CurrentCell = _focusedCell;
    }
}


/// <summary>
/// Helper method which returns the width of the viewport which is available for the columns to render
/// </summary>
/// <returns></returns>

double XUI::UI::DataGrid::GetViewportWidthForColumns()
{
	if (_internalScrollHost == nullptr)
	{
		return 0.0;
	}

	double totalAvailableWidth = _internalScrollHost->Viewport.Width;
    totalAvailableWidth -= CellsPanelHorizontalOffset;
	return totalAvailableWidth;
}

void XUI::UI::DataGrid::OnMaterializedHeader(void * sender, Interfaces::ItemContainerEventArgs & e)
{
    auto items = static_cast<System::Collection::Vector<Interfaces::ItemContainerInfo>*>(e.Container);
    for (int i = 0; i < items->size(); ++i)
    {
        auto item = (*items)[e.StartingIndex + i];
        auto Row = item.ContainerControl->Static_As<UI::DataGridRow>();
        if (Row)
        {
            Row->_rowReference.link(Row, this);
            Row->PrepareRow(item.Item, this);
        }
    }
}

void XUI::UI::DataGrid::OnDematerializedHeader(void * sender, Interfaces::ItemContainerEventArgs & e)
{
    auto items = static_cast<System::Collection::Vector<Interfaces::ItemContainerInfo>*>(e.Container);
    for (int i = 0; i < items->size(); ++i)
    {
        auto item = (*items)[e.StartingIndex + i];
        auto Row = item.ContainerControl->Static_As<UI::DataGridRow>();
        if (Row)
        {
            Row->_rowReference.unlink();
        }
    }
}

void XUI::UI::DataGrid::SetColumnHeaderPresenterInternal(DataGridColumnHeadersPresenter* presenter)
{
    if (presenter)
        _columnHeadersPresenter = presenter->shared_from_base_static<DataGridColumnHeadersPresenter>();
    else
        _columnHeadersPresenter = nullptr;
}

void XUI::UI::DataGrid::HandleSelectionForCellInput(DataGridCell * cell, bool startDragging, bool allowsExtendSelect, bool allowsMinimalSelect)
{
    DataGridSelectionUnit selectionUnit = SelectionUnit;

    if (selectionUnit == DataGridSelectionUnit::FullRow)
    {
        MakeFullRowSelection(cell->RowDataItem(), allowsExtendSelect, allowsMinimalSelect);
        SetFocusedCell(cell->shared_from_base_static<DataGridCell>());
    }
    else
    {
        MakeCellSelection(cell, allowsExtendSelect, allowsMinimalSelect);
    }

    if (startDragging)
    {
        BeginDragging();
    }
}

void XUI::UI::DataGrid::MakeFullRowSelection(System::Reflection::Variant const & dataItem, bool allowsExtendSelect, bool allowsMinimalSelect)
{
    if (!allowsMinimalSelect)
    {
        UnSelectAll();
        for (auto const& cell : _selectedCells)
            cell->SyncIsSelected(false);
        _selectedCells.clear();
    }

    // ToDo
    //ChangingSelectedCellsHelper g(this);

    if (allowsExtendSelect && _selectionAnchor)
    {
        int startRowIndex = Items->IndexOf(_selectionAnchor->RowDataItem());
        int endRowIndex = Items->IndexOf(dataItem);

        if (startRowIndex == -1 || endRowIndex == -1)
            return;

        if (endRowIndex < startRowIndex)
            std::swap(startRowIndex, endRowIndex);

        auto generator = GetItemContainerGenerator().get();

        for (int r = startRowIndex; r <= endRowIndex; ++r)
        {
            for (int c = 0; c <= _columns.size(); ++c)
            {
                auto dataRowItem = Items->ElementAt(r);
                int rowControlIndex = generator->IndexFromItem(dataRowItem);
                if (rowControlIndex == -1)
                    continue;

                DataGridRow* rowControl = generator->ContainerFromIndex(rowControlIndex)->Static_As<DataGridRow>();
                if (!rowControl)
                    continue;

                DataGridCell* _cell = rowControl->GetCell(c);
                if (!_cell || _cell->IsSelected)
                    continue;

                CellIsSelectedChanged(_cell, true);
                _cell->SyncIsSelected(true);
                if (!GetSelectedItems().Contains(dataRowItem))
                    GetSelectedItems().Add(dataRowItem);
            }
        }
    }
    else
    {
        int rowIndex = GetItemContainerGenerator()->IndexFromItem(dataItem);
        if (rowIndex != -1)
        {
            DataGridCell* anchor = nullptr;
            DataGridRow* row = GetItemContainerGenerator()->ContainerFromIndex(rowIndex)->Static_As<DataGridRow>();
            if (row)
            {
                for (auto const& cell : row->GetCells())
                {
                    if (cell->Column && cell->Column->DisplayIndex == 0)
                        anchor = cell.getTarget();
                    CellIsSelectedChanged(cell.getTarget(), true);
                    cell->SyncIsSelected(true);
                }

                SetSelectedItem(dataItem);

                if (anchor)
                {
                    _selectionAnchor = anchor->shared_from_base_static<DataGridCell>();
                }
            }
        }
    }
}

void XUI::UI::DataGrid::MakeCellSelection(DataGridCell* cell, bool allowsExtendSelect, bool allowsMinimalSelect)
{
    if (!allowsMinimalSelect)
    {
        UnSelectAll();
        for (auto const& cell : _selectedCells)
            cell->SyncIsSelected(false);
        _selectedCells.clear();
    }

    if (!cell)
        return;

    // ToDo
    //ChangingSelectedCellsHelper g(this);

    if (allowsExtendSelect && _selectionAnchor)
    {
        int startColumn = _selectionAnchor->Column->DisplayIndex;
        int startRowIndex = Items->IndexOf(_selectionAnchor->RowDataItem());
        int endColumn = cell->Column->DisplayIndex;
        int endRowIndex = Items->IndexOf(cell->RowDataItem());

        if (startRowIndex == -1 || endRowIndex == -1)
            return;

        if (endColumn < startColumn)
            std::swap(startColumn, endColumn);

        if (endRowIndex < startRowIndex)
            std::swap(startRowIndex, endRowIndex);

        auto generator = GetItemContainerGenerator().get();

        for (int r = startRowIndex; r <= endRowIndex; ++r)
        {
            for (int c = 0; c <= _columns.size(); ++c)
            {
                if ((r == startRowIndex && c < startColumn) || (r == endRowIndex && c > endColumn))
                    continue;

                auto dataRowItem = Items->ElementAt(r);
                int rowControlIndex = generator->IndexFromItem(dataRowItem);
                if (rowControlIndex == -1)
                    continue;

                DataGridRow* rowControl = generator->ContainerFromIndex(rowControlIndex)->Static_As<DataGridRow>();
                if (!rowControl)
                    continue;

                DataGridCell* _cell = rowControl->GetCell(c);
                if (!_cell || _cell->IsSelected)
                    continue;

                CellIsSelectedChanged(_cell, true);
                _cell->SyncIsSelected(true);
                if (!GetSelectedItems().Contains(dataRowItem))
                    GetSelectedItems().Add(dataRowItem);
            }
        }
    }
    else if (!cell->IsSelected)
    {
        CellIsSelectedChanged(cell, true);
        cell->SyncIsSelected(true);
        SetSelectedItem(cell->RowDataItem());

        _selectionAnchor = cell->shared_from_base_static<DataGridCell>();
        SetFocusedCell(_selectionAnchor);
    }
}

void XUI::UI::DataGrid::SelectAll()
{
    int startRowIndex = 0;
    int endRowIndex = Items->size();

    auto generator = GetItemContainerGenerator().get();

    for (int r = startRowIndex; r < endRowIndex; ++r)
    {
        for (int c = 0; c <= _columns.size(); ++c)
        {
            auto dataRowItem = Items->ElementAt(r);
            int rowControlIndex = generator->IndexFromItem(dataRowItem);
            if (rowControlIndex == -1)
                continue;

            DataGridRow* rowControl = generator->ContainerFromIndex(rowControlIndex)->Static_As<DataGridRow>();
            if (!rowControl)
                continue;

            DataGridCell* _cell = rowControl->GetCell(c);
            if (!_cell || _cell->IsSelected)
                continue;

            CellIsSelectedChanged(_cell, true);
            _cell->SyncIsSelected(true);
            if (!GetSelectedItems().Contains(dataRowItem))
                GetSelectedItems().Add(dataRowItem);
        }
    }
}

/// <summary>
///     There was general input on a row header that indicated that
///     selection should occur on the given row.
/// </summary>
/// <param name="row">The target row.</param>
/// <param name="startDragging">Whether the input also indicated that dragging should start.</param>

void XUI::UI::DataGrid::HandleSelectionForRowHeaderAndDetailsInput(DataGridRow* row, bool startDragging, bool allowsMinimalSelect)
{
    auto rowItem = row->Item;

    // When not dragging, move focus to the first cell
    if (!_isDraggingSelection && (_columns.size() > 0))
    {
        if (!IsFocused)
        {
            // In order for CurrentCell to move focus, the
            // DataGrid needs to be focused.
            //Core::Input::MouseDevice* mouseDevice = Platform::UIService::Instance()->Get<Core::Input::MouseDevice>();
            //mouseDevice->Capture(this);
        }

        auto currentCell = CurrentCell;
        if (!currentCell || currentCell->RowDataItem() != rowItem)
        {
            // Change the CurrentCell if the row is different
            CurrentCell = row->GetCell(_columns.DisplayIndexMap()[0])->shared_from_base_static<DataGridCell>(); //row->GetCells().begin()->getTarget()->shared_from_base_static<DataGridCell>();
        }
        else
        {
            if (_currentCellContainer && _currentCellContainer->IsEditing)
            {
                // End the pending edit even for the same row
                EndEdit(CommitEditCommand.get(), _currentCellContainer.get(), DataGridEditingUnit::Cell, /* exitEditingMode = */ true);
            }
        }
    }

    // Select a row when the mode is not None and the unit allows selecting rows
    if (CanSelectRows())
    {
        MakeFullRowSelection(rowItem, /* allowsExtendSelect = */ _isDraggingSelection, /* allowsMinimalSelect = */ allowsMinimalSelect || _isDraggingSelection);
        CurrentItem = row->Item;

        if (startDragging)
        {
            BeginRowDragging();
        }
    }
}

void XUI::UI::DataGrid::SelectItem(System::Reflection::Variant const & item)
{
    SelectItem(item, true);
}

void XUI::UI::DataGrid::SelectItem(System::Reflection::Variant const & item, bool selectCells)
{
    {
        ChangingSelectedCellsHelper g(this);
        int itemIndex = GetItemContainerGenerator()->IndexFromItem(item);
        auto container = GetItemContainerGenerator()->ContainerFromIndex(itemIndex);
        if (container != nullptr)
        {
            DataGridRow* row = container->Static_As<DataGridRow>();
            for (auto cell : row->GetCells())
                _selectedCells.insert(cell.getTarget());
        }
    }


    _selectedItems.Add(item); //UpdateSelectedItems(item, /*Add = */ true);
}

std::shared_ptr<Generators::IItemContainerGenerator> XUI::UI::DataGrid::CreateItemContainerGenerator()
{
	auto itemGenerator = std::make_shared<Generators::ItemContainerGenerator<DataGridRow>>(this, DataGridRow::ItemProperty.get(), nullptr/*, DataGridRow::RowHeaderTemplateProperty.get()*/);

    itemGenerator->Materialized += System::make_eventhandler(&DataGrid::OnMaterializedHeader, this);
    itemGenerator->Dematerialized += System::make_eventhandler(&DataGrid::OnDematerializedHeader, this);

    return itemGenerator;
}

int XUI::UI::DataGrid::GetDataItemCount() const
{
    if (Items)
    {
        int count = Items->size();

        return count;
    }
    return 0;
}

void XUI::UI::DataGrid::DeleteAutoColumns()
{
	for (auto itr = _columns.begin(); itr != _columns.end(); ++itr)
	{
		if ((*itr)->_isAutoGenerated)
			_columns.GetNativeEnumerator().erase(itr);
	}
}

void XUI::UI::DataGrid::GenerateAutoColumns()
{
    if (Items != nullptr && !Items->empty())
    {
        System::Reflection::Type const* itemProperties = Items->GetValueType();
        if (itemProperties->Properties)
        {
            for (auto itr = itemProperties->Properties->begin(); itr != itemProperties->Properties->end(); ++itr)
            {
                if ((*itr)->GetMethod())
                    continue;

                std::shared_ptr<DataGridColumn> dataGridColumn = DataGridColumn::CreateDefaultColumn(*itr);
        
                DataGridAutoGeneratingColumnEventArgs e;
                e.Column = dataGridColumn.get();
                e.PropertyName = (*itr)->GetName();
                e.PropertyType = (*itr)->GetFieldType();
                e.PropertyDescriptor = (*itr);
        
                if (AutoGeneratingColumn)
                    AutoGeneratingColumn(this, e);
        
                if (!e.Cancel && e.Column != nullptr)
                {
                    e.Column->_isAutoGenerated = true;
                    _columns.Add(dataGridColumn);
                }
            }
        }
        else if (itemProperties->fields)
        {
            for (auto itr = itemProperties->fields->begin(); itr != itemProperties->fields->end(); ++itr)
            {
                std::shared_ptr<DataGridColumn> dataGridColumn = DataGridColumn::CreateDefaultColumn(*itr);

                DataGridAutoGeneratingColumnEventArgs e;
                e.Column = dataGridColumn.get();
                e.PropertyName = (*itr)->GetName();
                e.PropertyType = (*itr)->GetFieldType();
                e.PropertyDescriptor = (*itr);

                if (AutoGeneratingColumn)
                    AutoGeneratingColumn(this, e);

                if (!e.Cancel && e.Column != nullptr)
                {
                    e.Column->_isAutoGenerated = true;
                    _columns.Add(dataGridColumn);
                }
            }
        }
    }
}

/// <summary>
/// Validates that the minimum column width is an acceptable value
/// </summary>

bool XUI::UI::DataGrid::ValidateMinColumnWidth(System::Reflection::Variant const & v)
{
    double value = v.GetValue<double>();
    return !(value < 0.0 || std::isnan(value) || std::isinf(value));
}

/// <summary>
/// Validates that the maximum column width is an acceptable value
/// </summary>

bool XUI::UI::DataGrid::ValidateMaxColumnWidth(System::Reflection::Variant const & v)
{
    double value = v.GetValue<double>();
    return !(value < 0.0 || std::isnan(value));
}

/// <summary>
///     Called when the Columns collection changes.
/// </summary>

void XUI::UI::DataGrid::OnColumnsChanged(void * sender, System::Collection::NotifyCollectionChangedEventArgs & e)
{
    // Update the reference to this DataGrid on the affected column(s)
    // and update the SelectedCells collection.
    switch (e.Action)
    {
        case System::Collection::NotifyCollectionChangedAction::Add:
        UpdateDataGridReference(_columns[e.NewStartingIndex], /* clear = */ false);
        UpdateColumnSizeConstraints(_columns[e.NewStartingIndex]);
        break;

        case System::Collection::NotifyCollectionChangedAction::Remove:
        UpdateDataGridReference(_columns[e.OldStartingIndex], /* clear = */ true);
        break;

        case System::Collection::NotifyCollectionChangedAction::Replace:
        UpdateDataGridReference(_columns[e.OldStartingIndex], /* clear = */ true);
		UpdateDataGridReference(_columns[e.NewStartingIndex], /* clear = */ false);
		UpdateColumnSizeConstraints(_columns[e.NewStartingIndex]);
		break;

		case System::Collection::NotifyCollectionChangedAction::Reset:
        // We can't clear column references on Reset: _columns has 0 items and e.OldItems is empty.
        _selectedCells.clear();
        break;
    }

    // FrozenColumns rely on column DisplayIndex
    // Delay the coercion if necessary
    if (_columns._displayIndexMapInitialized)
    {
        CoerceValue(FrozenColumnCountProperty.get());
    }

    bool visibleColumnsChanged = _columns[e.OldStartingIndex] && _columns[e.OldStartingIndex]->IsVisible();
	visibleColumnsChanged |= _columns[e.NewStartingIndex] && _columns[e.NewStartingIndex]->IsVisible();
    visibleColumnsChanged |= (e.Action == System::Collection::NotifyCollectionChangedAction::Reset);

    //if (visibleColumnsChanged)
    //{
    //    _columns.InvalidateColumnRealization(true);
    //}
    
    for (auto const& row : GetRows())
        row->OnColumnsChanged(_columns, e);

    if (ColumnChanged)
        ColumnChanged(this, e);

    // Recompute the column width if required, but wait until the first load
    if (visibleColumnsChanged && e.Action != System::Collection::NotifyCollectionChangedAction::Move)
    {
        _dataGridColumnsInvalidated = true;
        InvalidateMeasure();
    }
}

/// <summary>
///     Updates the transferred size constraints from DataGrid on the columns.
/// </summary>
/// <param name="list">The list of affected columns.</param>

void XUI::UI::DataGrid::UpdateColumnSizeConstraints(std::shared_ptr<DataGridColumn>& column)
{
    column->SyncProperties();
}

void XUI::UI::DataGrid::UnselectItem(System::Reflection::Variant const & item)
{
    {
        ChangingSelectedCellsHelper g(this);
        int itemIndex = Items->IndexOf(item);
        int numColumns = _columns.size();
        if ((itemIndex >= 0) && (numColumns > 0))
        {
            for (auto iCell = _selectedCells.begin(); iCell != _selectedCells.end();)
            {
                if (item == (*iCell)->RowDataItem())
                {
                    _selectedCells.erase(iCell++);
                }
    
                ++iCell;
            }
        }
    }
    

    _selectedItems.Remove(item);
}

void XUI::UI::DataGrid::BeginRowDragging()
{
    BeginDragging();
    _isRowDragging = true;
}

void XUI::UI::DataGrid::BeginDragging()
{
    Core::Input::MouseDevice* mouseDevice = Platform::UIService::Instance()->Get<Core::Input::MouseDevice>();
    //if (mouseDevice->Captured == this)
    {
        _isDraggingSelection = true;
        _dragPoint = mouseDevice->GetRelativePosition(this);
    }
}

void XUI::UI::DataGrid::EndDragging()
{
    //StopAutoScroll();
    Core::Input::MouseDevice* mouseDevice = Platform::UIService::Instance()->Get<Core::Input::MouseDevice>();
    if (mouseDevice->Captured == this)
    {
        mouseDevice->Capture(nullptr);
    }

    _isDraggingSelection = false;
    _isRowDragging = false;
}

/// <summary>
/// Determines the sort direction and sort property name and adds a sort
/// description to the Items>SortDescriptions Collection. Clears all the 
/// existing sort descriptions.
/// </summary>
/// <param name="column"></param>
/// <param name="clearExistingSortDescriptions"></param>

void XUI::UI::DataGrid::DefaultSort(DataGridColumn * column, bool clearExistingSortDescriptions)
{
    ListSortDirection sortDirection = ListSortDirection::Ascending;
    ListSortDirection currentSortDirection = column->SortDirection;
    if (currentSortDirection != ListSortDirection::UnSorted &&
        currentSortDirection == ListSortDirection::Ascending)
    {
        sortDirection = ListSortDirection::Descending;
    }
    
    std::shared_ptr<XamlCPP::DataBind::SourceObserverNode> sourceObserver;
    std::shared_ptr<XamlCPP::Interfaces::IBinding> binding = column->Binding;
    if (binding && binding->Is<XamlCPP::DataBind::Binding>())
    {
        auto bindObject = static_cast<XamlCPP::DataBind::Binding*>(binding.get());
        sourceObserver = bindObject->GetSourceObserver();
    }

    if (sourceObserver && Presenter && Presenter->Panel)
    {
        Presenter->Panel->Children.GetNativeEnumerator().sort([this, sortDirection, observer = sourceObserver.get()](std::shared_ptr<Control> const& lhs, std::shared_ptr<Control> const& rhs)
        {
            DataGridRow* leftheader = lhs->Static_As<DataGridRow>();
            DataGridRow* rightheader = rhs->Static_As<DataGridRow>();

            if (leftheader == _placeHolderRow.get())
                return _newItemPlaceholderPosition != NewItemPlaceholderPosition::AtEnd;

            if (rightheader == _placeHolderRow.get())
                return _newItemPlaceholderPosition == NewItemPlaceholderPosition::AtEnd;

            if (leftheader && rightheader)
            {
                auto leftValue = observer->GetTargetValue(leftheader->Item.ToObject());
                auto rightValue = observer->GetTargetValue(rightheader->Item.ToObject());
                if (sortDirection == ListSortDirection::Ascending)
                    return System::TypeCompare::IsLesser(leftValue, rightValue);
                return System::TypeCompare::IsGreater(leftValue, rightValue);
            }
            return false;
        });

        Items->QuickSort([sortDirection, observer = sourceObserver.get()](System::Reflection::Variant const& lhs, System::Reflection::Variant const& rhs)
        {
            auto leftValue = observer->GetTargetValue(lhs.ToObject());
            auto rightValue = observer->GetTargetValue(rhs.ToObject());
            if (sortDirection == ListSortDirection::Ascending)
                return System::TypeCompare::IsLesser(leftValue, rightValue);
            return System::TypeCompare::IsGreater(leftValue, rightValue);
        });

        Presenter->Panel->InvalidateArrange();
    }

    column->SortDirection = sortDirection;
}

void XUI::UI::DataGrid::OnCurrentCellChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs & e)
{
    auto oldCell = e.OldValue.GetValue<std::shared_ptr<DataGridCell>>();
    auto newCell = e.NewValue.GetValue<std::shared_ptr<DataGridCell>>();

    if (newCell && CurrentItem != newCell->RowDataItem())
        CurrentItem = newCell->RowDataItem();

    if (newCell && CurrentColumn != newCell->Column)
        CurrentItem = newCell->Column;

    if (oldCell)
    {
        if (IsAddingNewItem() || IsEditingRowItem())
            EndEdit(CommitEditCommand.get(), oldCell.get(), DataGridEditingUnit::Row, true);

        if (oldCell->IsEditing)
            EndEdit(CommitEditCommand.get(), oldCell.get(), DataGridEditingUnit::Cell, true);
    }

    //if (newCell && IsPointerOver)
    //{
    //    newCell->Focus();
    //}

    if (CurrentCellChanged)
    {
        System::Events::EventArg e;
        CurrentCellChanged(this, e);
    }
}

void XUI::UI::DataGrid::UpdateRowEditing(DataGridCell * cell)
{
    auto rowDataItem = cell->RowDataItem();

    // If the row is not in edit/add mode, then clear its IsEditing flag.
    if (!IsAddingOrEditingRowItem(rowDataItem))
    {
        cell->RowOwner()->IsEditing = false;
        _editingRowItem = System::Reflection::Variant::Empty;
        _editingRowIndex = -1;
    }
}

DataGridCell * XUI::UI::DataGrid::GetCellNearMouse()
{
    auto itemsHost = Presenter->Panel.get();
    if (itemsHost)
    {
        Core::Media::Rect bounds(itemsHost->DesiredSize);
        RelativeMousePositions position = GetRelativeMousePosition();
        double closestDistance = std::numeric_limits<double>::max();
        DataGridCell* closestCell = nullptr;
        bool isMouseInCorner = (position != RelativeMousePositions::Over) &&
            (position != RelativeMousePositions::Above) &&
            (position != RelativeMousePositions::Below) &&
            (position != RelativeMousePositions::Left) &&
            (position != RelativeMousePositions::Right);

        // Iterate from the end to the beginning since it is more common
        // to drag toward the end.
        int count = itemsHost->Children.size();
        for (int i = count - 1; i >= 0; i--)
        {
            DataGridRow* row = itemsHost->Children.ElementAt(i).ToObject()->Static_As<DataGridRow>();
            if (row)
            {
                for (auto const& cell : row->GetCells())
                {
                    double cellDistance;
                    if (CalculateCellDistance(cell.getTarget(), row, itemsHost, bounds, isMouseInCorner, cellDistance))
                    {
                        if ((closestCell == nullptr) || (cellDistance < closestDistance))
                        {
                            // This cell's distance is less, so make it the closest cell
                            closestDistance = cellDistance;
                            closestCell = cell.getTarget();
                        }
                    }
                }


                // Check if the header is close
                DataGridRowHeader* rowHeader = row->RowHeader;
                if (rowHeader)
                {
                    double cellDistance;
                    if (CalculateCellDistance(rowHeader, row, itemsHost, bounds, isMouseInCorner, cellDistance))
                    {
                        if ((closestCell == nullptr) || (cellDistance < closestDistance))
                        {
                            // If the header is the closest, then use the first cell from the row
                            DataGridCell* cell = row->GetCell(_columns.DisplayIndexMap()[0]);
                            if (cell)
                            {
                                closestDistance = cellDistance;
                                closestCell = cell;
                            }
                        }
                    }
                }

            }
        }

        return closestCell;
    }

    return nullptr;
}

DataGridRow * XUI::UI::DataGrid::GetRowNearMouse()
{
    ASSERT(GetRelativeMousePosition() != RelativeMousePositions::Over && "The mouse is not supposed to be over the DataGrid.");

    auto itemsHost = Presenter->Panel.get();
    if (itemsHost)
    {
        Core::Input::MouseDevice* mouseDevice = Platform::UIService::Instance()->Get<Core::Input::MouseDevice>();

        // Iterate from the end to the beginning since it is more common
        // to drag toward the end.
        int count = itemsHost->Children.size();
        for (int i = count - 1; i >= 0; i--)
        {
            DataGridRow* row = itemsHost->Children.ElementAt(i).ToObject()->Static_As<DataGridRow>();
            if (row)
            {
                Core::Media::Point pt = mouseDevice->GetRelativePosition(row);
                Core::Media::Rect rowBounds(row->DesiredSize);
                if ((pt.y >= rowBounds.y0()) && (pt.y <= rowBounds.y1()))
                {
                    // The mouse cursor's Y position is within the Y bounds of the row
                    return row;
                }
            }
        }
    }

    return nullptr;
}

bool XUI::UI::DataGrid::CalculateCellDistance(UI::Controls::Visual * cell, DataGridRow * rowOwner, Panel * itemsHost, Core::Media::Rect const & itemsHostBounds, bool isMouseInCorner, double & distance)
{
    Core::Input::MouseDevice* mouseDevice = Platform::UIService::Instance()->Get<Core::Input::MouseDevice>();
    auto transform = cell->Bounds.GetPosition(); // ToDo: implement transformation cell.TransformToAncestor(itemsHost);
    Core::Media::Rect cellBounds(cell->DesiredSize);

    // Limit to only cells that are entirely visible
    if (cellBounds.contains(itemsHostBounds)) //itemsHostBounds.Contains(transform.TransformBounds(cellBounds)))
    {
        Core::Media::Point pt = mouseDevice->GetRelativePosition(cell);
        if (isMouseInCorner)
        {
            // When the mouse is in the corner, go by distance from center of the cell
            Core::Media::Vector v(pt.x - (cellBounds.Width * 0.5), pt.y - (cellBounds.Height * 0.5));
            distance = v.length();
            return true;
        }
        else
        {
            Core::Media::Point rowPt = mouseDevice->GetRelativePosition(rowOwner);
            Core::Media::Rect rowBounds(rowOwner->DesiredSize);

            // The mouse should overlap a row or column
            if ((pt.x >= cellBounds.x0()) && (pt.x <= cellBounds.x1()))
            {
                // The mouse is within a column
                if ((rowPt.y >= rowBounds.y0()) && (rowPt.y <= rowBounds.y1()))
                {
                    // Mouse is within the cell
                    distance = 0.0;
                }
                else
                {
                    // Mouse is outside but is within a columns horizontal bounds
                    distance = std::abs(pt.y - cellBounds.y0());
                }

                return true;
            }
            else if ((rowPt.y >= rowBounds.y0()) && (rowPt.y <= rowBounds.y0()))
            {
                // Mouse is outside but is within a row's vertical bounds
                distance = std::abs(pt.x - cellBounds.x0());
                return true;
            }
        }
    }

    distance = std::numeric_limits<double>::max();
    return false;
}

/// <summary>
///     Helper method which determines if the
///     given list has visible columns
/// </summary>

void XUI::UI::DataGrid::OnItemsSourceCollectionChanged(void* sender, System::Collection::NotifyCollectionChangedEventArgs& e)
{
    if (e.Action == System::Collection::NotifyCollectionChangedAction::Add)
    {
    }
    else if ((e.Action == System::Collection::NotifyCollectionChangedAction::Remove) || (e.Action == System::Collection::NotifyCollectionChangedAction::Replace))
    {
    }
    else if (e.Action == System::Collection::NotifyCollectionChangedAction::Reset)
    {
        //ResetRowHeaderActualWidth();
    }
}

void XUI::UI::DataGrid::UpdateNewItemPlaceholder(bool isAddingNewItem)
{
    if (!isAddingNewItem)
    {
        if (CanUserAddRows)
        {
            _newItemPlaceholderPosition = NewItemPlaceholderPosition::AtEnd;
            _placeholderVisibility = Visibility::Visible;
        }
        else if (!CanUserAddRows)
        {
            _newItemPlaceholderPosition = NewItemPlaceholderPosition::None;
            _placeholderVisibility = Visibility::Collapsed;
        }
    }
    else
    {
        // During a row add, hide the placeholder
        _placeholderVisibility = Visibility::Collapsed;
    }


    if (_placeholderVisibility == Visibility::Visible)
    {
        if (!_placeHolderRow)
        {
            _placeHolderRow = std::make_shared<DataGridRow>();
            _placeHolderRow->BeginInit();
            _placeHolderRow->EndInit();
            _placeHolderRow->_rowReference.link(_placeHolderRow.get(), this);
            _placeHolderRow->PrepareRow(_newItemPlaceholder, this);
        }

        if (_placeHolderRow && !_placeHolderRow->IsAttachedToVisualTree && Presenter && Presenter->Panel)
        {
            Presenter->Panel->Children.Add(_placeHolderRow);
            Presenter->Panel->InvalidateMeasure();
        }
        else if (_placeHolderRow && _placeHolderRow->IsAttachedToVisualTree && _newItemPlaceholderPosition != NewItemPlaceholderPosition::None)
        {
            if (_newItemPlaceholderPosition == NewItemPlaceholderPosition::AtEnd)
            {
                Presenter->Panel->Children.GetNativeEnumerator().splice(
                    Presenter->Panel->Children.GetNativeEnumerator().end(),
                    Presenter->Panel->Children.GetNativeEnumerator(),
                    std::find(Presenter->Panel->Children.GetNativeEnumerator().begin(), Presenter->Panel->Children.GetNativeEnumerator().end(), _placeHolderRow));
            }
            else
            {
                Presenter->Panel->Children.GetNativeEnumerator().splice(
                    Presenter->Panel->Children.GetNativeEnumerator().begin(),
                    Presenter->Panel->Children.GetNativeEnumerator(),
                    std::find(Presenter->Panel->Children.GetNativeEnumerator().begin(), Presenter->Panel->Children.GetNativeEnumerator().end(), _placeHolderRow));
            }

            Presenter->Panel->InvalidateArrange();
        }
    }
    else if (_placeHolderRow && _placeHolderRow->IsAttachedToVisualTree)
    {
        Presenter->Panel->Children.Remove(_placeHolderRow);
        if (_placeHolderRow->IsAttachedToLogicalTree)
            _placeHolderRow->Parent->RemoveLogicalChild(_placeHolderRow.get());
        if (_placeHolderRow->IsAttachedToVisualTree)
            _placeHolderRow->VisualParent->RemoveVisualChild(_placeHolderRow.get());
        _placeHolderRow->_rowReference.unlink();
    }
}
