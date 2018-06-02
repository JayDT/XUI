/*
* Copyright (C) 2017-2018 Tauri JayD <https://www.>
* Copyright (c) 2014 Steven Kirk <The Avalonia Project>
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
#include "System/BaseTypes.h"
#include "DataGridColumn.h"
#include "DependencyPropertyMgr.h"
#include "UI/DataGrid/DataGridControl.h"
#include "UI/DataGrid/DataGridCheckBoxColumn.h"
#include "UI/DataGrid/DataGridComboBoxColumn.h"
#include "UI/DataGrid/DataGridTemplateColumn.h"
#include "UI/DataGrid/DataGridTextColumn.h"
#include "UI/DataGrid/DataGridExtensions.h"
#include "XamlCPP/DataBind/Binding.h"
#include "System/TypeComparer.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;
using namespace XUI::UI;

Core::Dependency::RefDependencyProperty DataGridColumn::HeaderProperty;
Core::Dependency::RefDependencyProperty DataGridColumn::HeaderStyleProperty;
Core::Dependency::RefDependencyProperty DataGridColumn::HeaderStringFormatProperty;
Core::Dependency::RefDependencyProperty DataGridColumn::HeaderTemplateProperty;
Core::Dependency::RefDependencyProperty DataGridColumn::CellStyleProperty;
Core::Dependency::RefDependencyProperty DataGridColumn::CellTemplateProperty;
Core::Dependency::RefDependencyProperty DataGridColumn::IsReadOnlyProperty;
Core::Dependency::RefDependencyProperty DataGridColumn::WidthProperty;
Core::Dependency::RefDependencyProperty DataGridColumn::MinWidthProperty;
Core::Dependency::RefDependencyProperty DataGridColumn::MaxWidthProperty;
Core::Dependency::RefDependencyProperty DataGridColumn::OriginalValueProperty; // Attach
Core::Dependency::RefDependencyProperty DataGridColumn::DisplayIndexProperty;
Core::Dependency::RefDependencyProperty DataGridColumn::CanUserSortProperty;
Core::Dependency::RefDependencyProperty DataGridColumn::CanUserReorderProperty;
Core::Dependency::RefDependencyProperty DataGridColumn::CanUserResizeProperty;
Core::Dependency::RefDependencyProperty DataGridColumn::SortDirectionProperty;
Core::Dependency::RefDependencyProperty DataGridColumn::IsFrozenProperty;
Core::Dependency::RefDependencyProperty DataGridColumn::CellValueProperty; // Attach
Core::Dependency::RefDependencyProperty DataGridColumn::VisibilityProperty;
Core::Dependency::RefDependencyProperty DataGridColumn::ElementStyleProperty;
Core::Dependency::RefDependencyProperty DataGridColumn::EditingElementStyleProperty;
Core::Dependency::RefDependencyProperty DataGridColumn::ActualWidthProperty;
Core::Dependency::RefDependencyProperty DataGridColumn::BindingProperty;

STATIC_CLASS_INITIALIZER(DataGridColumn, XUI::UI::DataGridColumn::StaticClassInitializer);

void XUI::UI::DataGridColumn::StaticClassInitializer()
{
    HeaderProperty = (Core::Dependency::BindProperty<System::Reflection::RefObject>::Register<DataGridColumn>(nameof(Header), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<DataGridColumn*>(d);
        control->NotifyPropertyChanged(control, e, NotificationTarget(uint32(NotificationTarget::Columns) | uint32(NotificationTarget::ColumnHeaders)));
    }, [](Core::Dependency::DependencyObject* d, System::Reflection::Variant const& baseValue)
    {
        auto column = static_cast<DataGridColumn*>(d);
        return baseValue; // DataGridHelper::GetCoercedTransferPropertyValue(d, baseValue, HeaderProperty.get(), column->DataGridOwner(), DataGrid::HeaderProperty.get());
    }));
    HeaderStyleProperty = (Core::Dependency::BindProperty<std::shared_ptr<XUI::Styling::Style>>::Register<DataGridColumn>(nameof(HeaderStyle), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<DataGridColumn*>(d);
        control->NotifyPropertyChanged(control, e, NotificationTarget(uint32(NotificationTarget::Columns) | uint32(NotificationTarget::ColumnHeaders)));
    }, [](Core::Dependency::DependencyObject* d, System::Reflection::Variant const& baseValue)
    {
        auto column = static_cast<DataGridColumn*>(d);
        return DataGridHelper::GetCoercedTransferPropertyValue(d, baseValue, HeaderStyleProperty.get(), column->DataGridOwner(), DataGrid::ColumnHeaderStyleProperty.get());
    }));
    HeaderStringFormatProperty = (Core::Dependency::BindProperty<std::string>::Register<DataGridColumn>(nameof(HeaderStringFormat), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<DataGridColumn*>(d);
        control->NotifyPropertyChanged(control, e, NotificationTarget(uint32(NotificationTarget::Columns) | uint32(NotificationTarget::ColumnHeaders)));
    }, nullptr));
    HeaderTemplateProperty = (Core::Dependency::BindProperty<std::shared_ptr<XUI::UI::DataTemplates::ContentTemplate>>::Register<DataGridColumn>(nameof(HeaderTemplate), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<DataGridColumn*>(d);
        control->NotifyPropertyChanged(control, e, NotificationTarget(uint32(NotificationTarget::Columns) | uint32(NotificationTarget::ColumnHeaders)));
    }, [](Core::Dependency::DependencyObject* d, System::Reflection::Variant const& baseValue)
    {
        auto column = static_cast<DataGridColumn*>(d);
        return baseValue; // DataGridHelper::GetCoercedTransferPropertyValue(d, baseValue, HeaderTemplateProperty.get(), column->DataGridOwner(), DataGrid::HeaderTemplateProperty.get());
    }));
    CellStyleProperty = (Core::Dependency::BindProperty<std::shared_ptr<XUI::Styling::Style>>::Register<DataGridColumn>(nameof(CellStyle), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<DataGridColumn*>(d);
        control->NotifyPropertyChanged(control, e, NotificationTarget(uint32(NotificationTarget::Columns) | uint32(NotificationTarget::Cells)));
    }, [](Core::Dependency::DependencyObject* d, System::Reflection::Variant const& baseValue)
    {
        auto column = static_cast<DataGridColumn*>(d);
        return DataGridHelper::GetCoercedTransferPropertyValue(d, baseValue, CellStyleProperty.get(), column->DataGridOwner(), DataGrid::CellStyleProperty.get());
    }));
    CellTemplateProperty = (Core::Dependency::BindProperty<std::shared_ptr<XUI::UI::DataTemplates::ContentTemplate>>::Register<DataGridColumn>(nameof(CellTemplate), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<DataGridColumn*>(d);
        control->NotifyPropertyChanged(control, e, NotificationTarget(uint32(NotificationTarget::Columns) | uint32(NotificationTarget::Cells)));
    }, nullptr));
    IsReadOnlyProperty = (Core::Dependency::BindProperty<bool>::Register<DataGridColumn>(nameof(IsReadOnly), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<DataGridColumn*>(d);
        control->NotifyPropertyChanged(control, e, NotificationTarget(uint32(NotificationTarget::Columns) | uint32(NotificationTarget::Cells)));
    }, [](Core::Dependency::DependencyObject* d, System::Reflection::Variant const& baseValue)
    {
        auto column = static_cast<DataGridColumn*>(d);
        return DataGridHelper::GetCoercedTransferPropertyValue(d, baseValue, IsReadOnlyProperty.get(), column->DataGridOwner(), DataGrid::IsReadOnlyProperty.get());
    }));
    WidthProperty = (Core::Dependency::BindProperty<GridLength>::Register<DataGridColumn>(nameof(Width), DataGridColumn::OnWidthPropertyChanged, [](Core::Dependency::DependencyObject* d, System::Reflection::Variant const& baseValue)
    {
        auto column = static_cast<DataGridColumn*>(d);
        return DataGridHelper::GetCoercedTransferPropertyValue(d, baseValue, WidthProperty.get(), column->DataGridOwner(), DataGrid::ColumnWidthProperty.get());
    }));
    MinWidthProperty = (Core::Dependency::BindProperty<double>::Register<DataGridColumn>(nameof(MinWidth), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<DataGridColumn*>(d);
        DataGrid* dataGrid = control->DataGridOwner();

        control->NotifyPropertyChanged(control, e, NotificationTarget::Columns);

        if (dataGrid != nullptr && control->IsVisible())
        {
            dataGrid->GetColumns().RedistributeColumnWidthsOnMinWidthChangeOfColumn(control, e.OldValue.GetValue<double>());
        }
    }, [](Core::Dependency::DependencyObject* d, System::Reflection::Variant const& baseValue)
    {
        auto column = static_cast<DataGridColumn*>(d);
        return DataGridHelper::GetCoercedTransferPropertyValue(d, baseValue, MinWidthProperty.get(), column->DataGridOwner(), DataGrid::MinColumnWidthProperty.get());
    }));
    MaxWidthProperty = (Core::Dependency::BindProperty<double>::Register<DataGridColumn>(nameof(MaxWidth), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<DataGridColumn*>(d);
        DataGrid* dataGrid = control->DataGridOwner();

        control->NotifyPropertyChanged(control, e, NotificationTarget::Columns);

        if (dataGrid != nullptr && control->IsVisible())
        {
            dataGrid->GetColumns().RedistributeColumnWidthsOnMaxWidthChangeOfColumn(control, e.OldValue.GetValue<double>());
        }
    }, [](Core::Dependency::DependencyObject* d, System::Reflection::Variant const& baseValue)
    {
        auto column = static_cast<DataGridColumn*>(d);
        return DataGridHelper::GetCoercedTransferPropertyValue(d, baseValue, MaxWidthProperty.get(), column->DataGridOwner(), DataGrid::MaxColumnWidthProperty.get());
    }, std::numeric_limits<double>::max()));
    OriginalValueProperty = (Core::Dependency::BindProperty<System::Reflection::Variant>::Register<DataGridColumn>(nameof(OriginalValue))); // Attach
    DisplayIndexProperty = (Core::Dependency::BindProperty<int>::Register<DataGridColumn>(nameof(DisplayIndex), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<DataGridColumn*>(d);
        control->NotifyPropertyChanged(control, e, NotificationTarget::Columns |
            NotificationTarget::ColumnCollection |
            NotificationTarget::Cells |
            NotificationTarget::ColumnHeaders |
            NotificationTarget::CellsPresenter |
            NotificationTarget::ColumnHeadersPresenter);
    }, [](Core::Dependency::DependencyObject* d, System::Reflection::Variant const& baseValue)
    {
        auto column = static_cast<DataGridColumn*>(d);

        if (column->DataGridOwner() != nullptr)
            column->DataGridOwner()->ValidateDisplayIndex(column, baseValue.GetValue<int>());

        return baseValue;
    }));
    CanUserSortProperty = (Core::Dependency::BindProperty<bool>::Register<DataGridColumn>(nameof(CanUserSort), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<DataGridColumn*>(d);
        control->CoerceValue(CanUserSortProperty.get());
    }, [](Core::Dependency::DependencyObject* d, System::Reflection::Variant const& baseValue)
    {
        auto column = static_cast<DataGridColumn*>(d);
        //ValueSource propertySource = DependencyPropertyHelper.GetValueSource(column, CanUserSortProperty);
        //bool basePropertyHasModifiers = propertySource.IsAnimated || propertySource.IsCoerced || propertySource.IsExpression;
        //
        //if (column.DataGridOwner != null)
        //{
        //    ValueSource parentValueSource = DependencyPropertyHelper.GetValueSource(column.DataGridOwner, DataGrid::CanUserSortColumnsProperty.get());
        //    bool parentPropertyHasModifiers = parentValueSource.IsAnimated || parentValueSource.IsCoerced || parentValueSource.IsExpression; ;
        //
        //    if (parentValueSource.BaseValueSource == propertySource.BaseValueSource && !basePropertyHasModifiers && parentPropertyHasModifiers)
        //    {
        //        return column.DataGridOwner.GetValue(DataGrid::CanUserSortColumnsProperty.get());
        //    }
        //}
        return DataGridHelper::GetCoercedTransferPropertyValue(d, baseValue, CanUserSortProperty.get(), column->DataGridOwner(), DataGrid::CanUserSortColumnsProperty.get());
    }));
    CanUserReorderProperty = (Core::Dependency::BindProperty<bool>::Register<DataGridColumn>(nameof(CanUserReorder), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<DataGridColumn*>(d);
        control->NotifyPropertyChanged(control, e, NotificationTarget::Columns);
    }, [](Core::Dependency::DependencyObject* d, System::Reflection::Variant const& baseValue)
    {
        auto column = static_cast<DataGridColumn*>(d);
        return DataGridHelper::GetCoercedTransferPropertyValue(d, baseValue, CanUserReorderProperty.get(), column->DataGridOwner(), DataGrid::CanUserReorderColumnsProperty.get());
    }));
    CanUserResizeProperty = (Core::Dependency::BindProperty<bool>::Register<DataGridColumn>(nameof(CanUserResize), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<DataGridColumn*>(d);
        control->NotifyPropertyChanged(control, e, NotificationTarget(uint32(NotificationTarget::Columns) | uint32(NotificationTarget::ColumnHeaders)));
    }, [](Core::Dependency::DependencyObject* d, System::Reflection::Variant const& baseValue)
    {
        auto column = static_cast<DataGridColumn*>(d);
        return DataGridHelper::GetCoercedTransferPropertyValue(d, baseValue, CanUserResizeProperty.get(), column->DataGridOwner(), DataGrid::CanUserResizeColumnsProperty.get());
    }));
    SortDirectionProperty = (Core::Dependency::BindProperty<ListSortDirection>::Register<DataGridColumn>(nameof(SortDirection), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<DataGridColumn*>(d);
        control->NotifyPropertyChanged(control, e, NotificationTarget::ColumnHeaders);
    }, nullptr));
    IsFrozenProperty = (Core::Dependency::BindProperty<bool>::Register<DataGridColumn>(nameof(IsFrozen), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<DataGridColumn*>(d);
        control->NotifyPropertyChanged(control, e, NotificationTarget::ColumnHeaders);
    }, [](Core::Dependency::DependencyObject* d, System::Reflection::Variant const& baseValue) -> System::Reflection::Variant
    {
        auto column = static_cast<DataGridColumn*>(d);
        DataGrid* dataGrid = column->DataGridOwner();
        if (dataGrid != nullptr)
        {
            if (column->DisplayIndex < dataGrid->FrozenColumnCount)
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        return baseValue;
    }));
    CellValueProperty = (Core::Dependency::BindProperty<System::Reflection::Variant>::Register<DataGridColumn>(nameof(CellValue))); // Attach
    VisibilityProperty = (Core::Dependency::BindProperty<UI::Visibility>::Register<DataGridColumn>(nameof(Visibility), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<DataGridColumn*>(d);
        control->NotifyPropertyChanged(control, e, NotificationTarget::CellsPresenter | NotificationTarget::ColumnHeadersPresenter | NotificationTarget::ColumnCollection | NotificationTarget::ColumnHeaders);
    }, nullptr));
    ElementStyleProperty = (Core::Dependency::BindProperty<std::shared_ptr<XUI::Styling::Style>>::Register<DataGridColumn>(nameof(ElementStyle)));
    EditingElementStyleProperty = (Core::Dependency::BindProperty<std::shared_ptr<XUI::Styling::Style>>::Register<DataGridColumn>(nameof(EditingElementStyle)));
    ActualWidthProperty = (Core::Dependency::DirectProperty<DataGridColumn, double>::RegisterDirect(nameof(ActualWidth),
        [](DataGridColumn* x) { return x->ActualWidth; },
        [](DataGridColumn* x, double v) { x->ActualWidth = v; },
        0.0,
        XamlCPP::BindingMode::OneWay,
        false)
    );
    BindingProperty = (Core::Dependency::DirectProperty<DataGridColumn, std::shared_ptr<XamlCPP::Interfaces::IBinding>>::RegisterDirect(nameof(Binding),
        [](DataGridColumn* x) { return x->Binding; },
        [](DataGridColumn* x, std::shared_ptr<XamlCPP::Interfaces::IBinding> const& v) { x->Binding = v; })
    );

    BindingProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const & e)
    {
        auto control = static_cast<DataGridColumn*>(e.Sender);
        control->NotifyPropertyChanged(control, (XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs&)e, NotificationTarget::RefreshCellContent);
    });

    ActualWidthProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const & e)
    {
        auto control = static_cast<DataGridColumn*>(e.Sender);
        control->OnActualWidthPropertyChanged(control, (XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs&)e);
    });
}

XUI::UI::DataGridColumn::DataGridColumn()
{
}

XUI::UI::DataGridColumn::~DataGridColumn()
{
}

std::shared_ptr<XUI::UI::Controls::Control> XUI::UI::DataGridColumn::BuildVisualTree(bool isEditing, System::Reflection::Variant const& dataItem, XUI::UI::DataGridCell * cell)
{
    if (isEditing)
    {
        return GenerateEditingElement(cell, dataItem);
    }
    else
    {
        return GenerateElement(cell, dataItem);
    }
}

void XUI::UI::DataGridColumn::NotifyPropertyChanged(Core::Dependency::DependencyObject * d, Core::Dependency::DependencyPropertyChangedEventArgs & e, NotificationTarget target)
{
    if (target & NotificationTarget::Columns)
    {
        // Remove columns target since we're handling it.  If we're targeting multiple targets it may also need to get
        // sent to the DataGrid.
        target = NotificationTarget(uint32(target) & ~uint32(NotificationTarget::Columns));

        if (e.Property == DataGrid::MaxColumnWidthProperty.get() || e.Property == MaxWidthProperty.get())
        {
            CoerceValue(MaxWidthProperty.get());
        }
        else if (e.Property == DataGrid::MinColumnWidthProperty.get() || e.Property == MinWidthProperty.get())
        {
            CoerceValue(MinWidthProperty.get());
        }
        else if (e.Property == DataGrid::ColumnWidthProperty.get() || e.Property == WidthProperty.get())
        {
            CoerceValue( WidthProperty.get());
        }
        else if (e.Property == DataGrid::ColumnHeaderStyleProperty.get() || e.Property == HeaderStyleProperty.get())
        {
            CoerceValue( HeaderStyleProperty.get());
        }
        else if (e.Property == DataGrid::CellStyleProperty.get() || e.Property == CellStyleProperty.get())
        {
            CoerceValue( CellStyleProperty.get());
        }
        else if (e.Property == DataGrid::IsReadOnlyProperty.get() || e.Property == IsReadOnlyProperty.get())
        {
            CoerceValue( IsReadOnlyProperty.get());
        }
        /*else if (e.Property == DataGrid::DragIndicatorStyleProperty.get() || e.Property == DragIndicatorStyleProperty.get())
        {
            CoerceValue( DragIndicatorStyleProperty.get());
        }*/
        else if (e.Property == DisplayIndexProperty.get())
        {
            CoerceValue(IsFrozenProperty.get());
        }
        else if (e.Property == DataGrid::CanUserSortColumnsProperty.get())
        {
            CoerceValue( CanUserSortProperty.get());
        }
        else if (e.Property == DataGrid::CanUserResizeColumnsProperty.get() || e.Property == CanUserResizeProperty.get())
        {
            CoerceValue( CanUserResizeProperty.get());
        }
        else if (e.Property == DataGrid::CanUserReorderColumnsProperty.get() || e.Property == CanUserReorderProperty.get())
        {
            CoerceValue( CanUserReorderProperty.get());
        }

        if (e.Property == WidthProperty.get() || e.Property == MinWidthProperty.get() || e.Property == MaxWidthProperty.get())
        {
            CoerceValue(ActualWidthProperty.get());
        }
    }

    if (target != NotificationTarget::None)
    {
        // Everything else gets sent to the DataGrid so it can propogate back down 
        // to the targets that need notification.
        DataGridColumn* column = static_cast<DataGridColumn*>(d);
        DataGrid* dataGridOwner = column->DataGridOwner();
        if (dataGridOwner != nullptr)
        {
            dataGridOwner->NotifyPropertyChanged(d, e, target);
        }
    }
}

void XUI::UI::DataGridColumn::OnWidthPropertyChanged(XamlCPP::Core::Dependency::IDependencyObject * d, Core::Dependency::DependencyPropertyChangedEventArgs & e)
{
    DataGridColumn* column = static_cast<DataGridColumn*>(d);
    GridLength oldWidth = e.OldValue.GetValue<GridLength>();
    GridLength newWidth = e.NewValue.GetValue<GridLength>();
    DataGrid* dataGrid = column->DataGridOwner();

    if (column->_processingWidthChange)
    {
        column->CoerceValue(ActualWidthProperty.get());
        return;
    }

    column->_processingWidthChange = true;
    if (oldWidth.IsStar != newWidth.IsStar)
    {
        column->CoerceValue(MaxWidthProperty.get());
    }

    System::ScopeGuard g([&]()
    {
        column->_processingWidthChange = false;
    });


    if (dataGrid != nullptr && (newWidth.IsStar ^ oldWidth.IsStar))
    {
        dataGrid->GetColumns().InvalidateHasVisibleStarColumns();
        column->_invalidatedStarWidth = true;
    }

    column->NotifyPropertyChanged(
        static_cast<Core::Dependency::DependencyObject*>(d),
        e,
        NotificationTarget::ColumnCollection |
        NotificationTarget::Columns |
        NotificationTarget::Cells |
        NotificationTarget::ColumnHeaders |
        NotificationTarget::CellsPresenter |
        NotificationTarget::ColumnHeadersPresenter);

    if (dataGrid != nullptr)
    {
        if (!column->_ignoreRedistributionOnWidthChange && column->IsVisible())
        {
            if (!column->Width.IsStar && !column->Width.IsAbsolute)
            {
                column->CoerceValue(ActualWidthProperty.get());
            }
        }
    }
}

void XUI::UI::DataGridColumn::OnActualWidthPropertyChanged(XamlCPP::Core::Dependency::IDependencyObject * d, Core::Dependency::DependencyPropertyChangedEventArgs & e)
{
    DataGridColumn* column = static_cast<DataGridColumn*>(d);
    double oldWidth = e.OldValue.GetValue<double>();
    double newWidth = e.NewValue.GetValue<double>();
    DataGrid* dataGrid = column->DataGridOwner();

    column->NotifyPropertyChanged(
        static_cast<Core::Dependency::DependencyObject*>(d),
        e,
        NotificationTarget::ColumnCollection |
        NotificationTarget::Columns |
        NotificationTarget::Cells |
        NotificationTarget::ColumnHeaders |
        NotificationTarget::CellsPresenter |
        NotificationTarget::ColumnHeadersPresenter);

    if (dataGrid != nullptr)
    {
        //dataGrid->GetColumns().InvalidateAverageColumnWidth();

        if (!column->_ignoreRedistributionOnWidthChange && column->IsVisible())
        {
            if (!column->Width.IsStar && !column->Width.IsAbsolute)
            {
                column->CoerceValue(ActualWidthProperty.get());
            }

            dataGrid->GetColumns().RedistributeColumnWidthsOnWidthChangeOfColumn(column, oldWidth);
        }
    }

    column->_invalidatedStarWidth = false;
}

/// <summary>
///     Retrieve the proper measure constraint for cells.
/// </summary>
/// <param name="isHeader">Whether a header constraint or a normal cell constraint is requested.</param>
/// <returns>The value to use as the width when creating a measure constraint.</returns>

double XUI::UI::DataGridColumn::GetConstraintWidth(bool isHeader)
{
    GridLength width = Width;
    if (!std::isnan(ActualWidth) && ActualWidth > 0.0)
    {
        return ActualWidth;
    }

    if (width.IsAbsolute ||
        width.IsStar ||
        (width.IsSizeToCells && isHeader) ||
        (width.IsSizeToHeader && !isHeader))
    {
        // In these cases, the cell's desired size does not matter.
        // Use the column's current width as the constraint.
        return ActualWidth;
    }
    else
    {
        // The element gets to size to content.
        return std::numeric_limits<double>::max();
    }
}

/// <summary>
/// Helper Method which creates a default DataGridColumn object for the specified property type.
/// </summary>
/// <param name="itemProperty"></param>
/// <returns></returns>
std::shared_ptr<DataGridColumn> XUI::UI::DataGridColumn::CreateDefaultColumn(System::Reflection::Field const * itemProperty)
{
    ASSERT(itemProperty != nullptr && itemProperty->GetFieldType()->IsValid() && "itemProperty and/or its PropertyType member cannot be null");

    std::shared_ptr<DataGridColumn> dataGridColumn;
    std::shared_ptr<DataGridComboBoxColumn> comboBoxColumn;
    const System::Reflection::Type* propertyType = itemProperty->GetFieldType();
    
    // determine the type of column to be created and create one
    if (propertyType->IsEnum())
    {
        comboBoxColumn = std::make_shared<DataGridComboBoxColumn>();
        auto items = std::make_shared<System::Collection::DynamicList>();
        for (auto const& value : System::Reflection::Enum::GetValues(*propertyType))
            items->Add(value);
        comboBoxColumn->ItemsSource = items;
        dataGridColumn = comboBoxColumn;
    }
    else if (typeof(System::String).IsAssignableFrom(*propertyType) || *propertyType == typeid(std::string) || *propertyType == typeid(std::wstring))
    {
        dataGridColumn = std::make_shared<DataGridTextColumn>();
    }
    else if (typeof(System::Bool).IsAssignableFrom(*propertyType) || *propertyType == typeid(bool))
    {
        dataGridColumn = std::make_shared<DataGridCheckBoxColumn>();
    }
    //else if (typeof(Uri).IsAssignableFrom(propertyType))
    //{
    //    dataGridColumn = new DataGridHyperlinkColumn();
    //}
    else
    {
        dataGridColumn = std::make_shared<DataGridTextColumn>();
    }

    // determine if the datagrid can sort on the column or not
    if (!propertyType->IsPrimitive() && !propertyType->IsEnum() && !typeof(System::IComparable).IsAssignableFrom(*propertyType))
        dataGridColumn->CanUserSort = false;
    
    dataGridColumn->Header = std::make_shared<System::String>(itemProperty->GetName());

    //// Set the data field binding for such created columns and 
    //// choose the BindingMode based on editability of the property.
    dataGridColumn->Binding = std::make_shared<XamlCPP::DataBind::Binding>(itemProperty->GetName(), XUI::Core::Dependency::BindingMode::OneWay);
    dataGridColumn->IsReadOnly = itemProperty->SetMethod() || !itemProperty->IsReadOnly();

    return dataGridColumn;
}

/// <summary>
/// This method is called for each selected cell in each selected cell to retrieve the default cell content.
/// Default cell content is calculated using ClipboardContentBinding.
/// </summary>
/// <param name="item"></param>
/// <returns></returns>
System::Reflection::Variant XUI::UI::DataGridColumn::OnCopyingCellClipboardContent(System::Reflection::Variant const & item)
{
    System::Reflection::Variant cellValue;
    //BindingBase binding = ClipboardContentBinding;
    //if (binding != null)
    //{
    //    FrameworkElement fe = new FrameworkElement();
    //    fe.DataContext = item;
    //    fe.SetBinding(CellValueProperty, binding);
    //    cellValue = fe.GetValue(CellValueProperty);
    //}
    //
    //// Raise the event to give a chance for external listeners to modify the cell content
    //if (CopyingCellClipboardContent != null)
    //{
    //    DataGridCellClipboardEventArgs args = new DataGridCellClipboardEventArgs(item, this, cellValue);
    //    CopyingCellClipboardContent(this, args);
    //    cellValue = args.Content;
    //}

    return cellValue;
}

/// We don't provide default Paste but this public method is exposed to help custom implementation of Paste
/// <summary>
/// This method stores the cellContent into the item object using ClipboardContentBinding.
/// </summary>
/// <param name="item"></param>
/// <param name="cellContent"></param>
void XUI::UI::DataGridColumn::OnPastingCellClipboardContent(System::Reflection::Variant const & item, System::Reflection::Variant const & cellContent)
{
    //BindingBase binding = ClipboardContentBinding;
    //if (binding != null)
    //{
    //    // Raise the event to give a chance for external listeners to modify the cell content
    //    // before it gets stored into the cell
    //    if (PastingCellClipboardContent != null)
    //    {
    //        DataGridCellClipboardEventArgs args = new DataGridCellClipboardEventArgs(item, this, cellContent);
    //        PastingCellClipboardContent(this, args);
    //        cellContent = args.Content;
    //    }
    //
    //    // Event handlers can cancel Paste of a cell by setting its content to null
    //    if (cellContent != null)
    //    {
    //        FrameworkElement fe = new FrameworkElement();
    //        fe.DataContext = item;
    //        fe.SetBinding(CellValueProperty, binding);
    //        fe.SetValue(CellValueProperty, cellContent);
    //        BindingExpression be = fe.GetBindingExpression(CellValueProperty);
    //        be.UpdateSource();
    //    }
    //}
}

void XUI::UI::DataGridColumn::OnInput(Core::Observer::RoutedEventArgs const & e)
{
}

void XUI::UI::DataGridColumn::BeginEdit(Core::Observer::RoutedEventArgs const & e)
{
    auto owner = DataGridOwner();
    if (owner)
    {
        if (owner->BeginEdit((Core::Observer::RoutedEventArgs*)&e))
        {
            const_cast<Core::Observer::RoutedEventArgs&>(e).Handled = true;
        }
    }
}

/// <summary>
/// Override which rebuilds the cell's visual tree for Binding change
/// </summary>
/// <param name="element"></param>
/// <param name="propertyName"></param>

void XUI::UI::DataGridColumn::SyncProperties()
{
    CoerceValue(WidthProperty.get());
    CoerceValue(MinWidthProperty.get());
    CoerceValue(MaxWidthProperty.get());
    CoerceValue(HeaderStyleProperty.get());
    CoerceValue(HeaderStyleProperty.get());
    CoerceValue(CellStyleProperty.get());
    CoerceValue(IsReadOnlyProperty.get());
    CoerceValue(CanUserSortProperty.get());
    CoerceValue(CanUserReorderProperty.get());
    CoerceValue(CanUserResizeProperty.get());
}

void XUI::UI::DataGridColumn::RefreshCellContent(XUI::UI::Controls::Control * element, std::string propertyName)
{
}
