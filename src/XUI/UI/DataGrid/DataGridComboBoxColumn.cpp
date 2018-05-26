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
#include "DataGridComboBoxColumn.h"
#include "DataGridCell.h"
#include "DataGridColumn.h"
#include "DataGridControl.h"
#include "UI/DropDown.h"
#include "UI/TextBlock.h"
#include "Core/Media/Colors.h"
#include "XamlCPP/DataBind/Binding.h"
#include "XamlCPP/DataBind/SourceObserver.h"
#include "DataGridComboBoxColumn.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;
using namespace XUI::UI;

Core::Dependency::RefDependencyProperty DataGridComboBoxColumn::ItemsSourceProperty;

STATIC_CLASS_INITIALIZER(DataGridComboBoxColumn, XUI::UI::DataGridComboBoxColumn::StaticClassInitializer);

void XUI::UI::DataGridComboBoxColumn::StaticClassInitializer()
{
    ItemsSourceProperty = (Core::Dependency::BindProperty<std::shared_ptr<System::Collection::Generic::IContainer>>::Register<DataGridComboBoxColumn>(nameof(ItemsSource)));
}

std::shared_ptr<XUI::UI::Controls::Control> XUI::UI::DataGridComboBoxColumn::GenerateElement(DataGridCell * cell, System::Reflection::Variant const& dataItem)
{
    std::shared_ptr<UI::TextBlock> textBlock = std::make_shared<UI::TextBlock>();
    textBlock->BeginInit();
    SyncProperties(textBlock.get());
    textBlock->IsHitTestVisible = false;
    textBlock->BorderThickness = 0.0;

    if (cell->RowDataItem() != DataGrid::NewPlaceHolder())
    {
        if (Binding)
            textBlock->Bind(TextBlock::TextProperty.get(), Binding.get());
        else
            textBlock->Text = System::String::ToWString(dataItem.ToString());
    }

    textBlock->EndInit();
    return textBlock;
}

std::shared_ptr<XUI::UI::Controls::Control> XUI::UI::DataGridComboBoxColumn::GenerateEditingElement(DataGridCell * cell, System::Reflection::Variant const& dataItem)
{
    std::shared_ptr<UI::DropDown> control = std::make_shared<UI::DropDown>();
    control->BeginInit();
    SyncProperties(control.get());
    control->HorizontalAlignment = UI::Controls::HorizontalAlignment::Stretch;
    control->VerticalAlignment = UI::Controls::VerticalAlignment::Stretch;

    if (cell->RowDataItem() != DataGrid::NewPlaceHolder())
    {
        control->Items = ItemsSource;
        if (Binding)
            control->Bind(SelectingItemsControl::SelectedItemProperty.get(), Binding.get());
    }

    control->EndInit();
    return control;
}

XUI::UI::DataGridComboBoxColumn::DataGridComboBoxColumn()
{
}

XUI::UI::DataGridComboBoxColumn::~DataGridComboBoxColumn()
{
}

void XUI::UI::DataGridComboBoxColumn::OnInput(Core::Observer::RoutedEventArgs const & e)
{
}

bool XUI::UI::DataGridComboBoxColumn::CommitCellEdit(XUI::UI::Controls::Control * editingElement)
{
    DropDown* control = editingElement->Static_As<DropDown>();
    if (control != nullptr)
    {
        DataGrid* grid = DataGridOwner();
        if (!grid)
            return false;

        std::shared_ptr<XamlCPP::DataBind::SourceObserverNode> sourceObserver;
        System::Reflection::Variant value;
        std::shared_ptr<XamlCPP::Interfaces::IBinding> binding = Binding;
        if (binding && binding->Is<XamlCPP::DataBind::Binding>())
        {
            auto bindObject = static_cast<XamlCPP::DataBind::Binding*>(binding.get());
            auto converter = bindObject->GetConverterOrDefault();
            sourceObserver = bindObject->GetSourceObserver();
            sourceObserver->InitializeFrom(grid->GetEditingRowItem().ToObject());

            if (sourceObserver->GetPropertyType())
            {
                if (converter)
                    value = converter->ConvertBack(control->SelectedItem, sourceObserver->GetPropertyType(), bindObject->ConverterParameter, 0);
                else
                    value = control->SelectedItem;
            
                if (!value.GetBase())
                    value = sourceObserver->GetPropertyType()->TypeDescriptor->DefaultValue();
            }
        }
        else
            return false;

        if (grid->GetEditingRowItem().GetBase() && value.IsValid())
            return sourceObserver->SetTargetValue(grid->GetEditingRowItem().ToObject(), value, XamlCPP::BindingLevel::LocalValue);
        return false; // !Validation.GetHasError(textBox);
    }

    return true;
}

void XUI::UI::DataGridComboBoxColumn::CancelCellEdit(XUI::UI::Controls::Control * editingElement, System::Reflection::Variant & uneditedValue)
{
    //DropDown* control = editingElement->Static_As<DropDown>();
    //if (control != nullptr)
    //{
    //    auto bindings = control->GetBindingExpressions(TextBox::TextProperty.get());
    //    for (auto bind : *bindings)
    //        bind->UpdateTarget();
    //}
}

System::Reflection::Variant XUI::UI::DataGridComboBoxColumn::PrepareCellForEdit(XUI::UI::Controls::Control * editingElement, Core::Observer::RoutedEventArgs & editingEventArgs)
{
    UI::DropDown* control = editingElement->Static_As<UI::DropDown>();
    if (control != nullptr)
    {
        control->Focus();

    }

    return System::Reflection::Variant::Empty;
}

void XUI::UI::DataGridComboBoxColumn::SyncProperties(XUI::UI::Controls::Control * e)
{
}
