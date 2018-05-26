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
#include "DataGridCheckBoxColumn.h"
#include "DataGridCell.h"
#include "DataGridControl.h"
#include "UI/Checkbox.h"
#include "XamlCPP/DataBind/Binding.h"
#include "XamlCPP/DataBind/SourceObserver.h"
#include "Input/KeyboardDevice.h"
#include "Input/MouseDevice.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;
using namespace XUI::UI;

Core::Dependency::RefDependencyProperty DataGridCheckBoxColumn::IsThreeStateProperty;

STATIC_CLASS_INITIALIZER(DataGridCheckBoxColumn, XUI::UI::DataGridCheckBoxColumn::StaticClassInitializer);

std::shared_ptr<XUI::UI::Controls::Control> XUI::UI::DataGridCheckBoxColumn::GenerateElement(DataGridCell * cell, System::Reflection::Variant const& dataItem)
{
    std::shared_ptr<UI::CheckBox> control = std::make_shared<UI::CheckBox>();
    control->BeginInit();
    control->IsThreeState = IsThreeState;
    control->IsHitTestVisible = false;

    if (cell->RowDataItem() != DataGrid::NewPlaceHolder())
    {
        if (Binding)
            control->Bind(UI::CheckBox::IsCheckedProperty.get(), Binding.get());
        else
            control->IsChecked = dataItem.ToBool() ? ToggleButtonState::TBS_CHECKED : ToggleButtonState::TBS_NONE;
    }

    control->EndInit();
    return control;
}

std::shared_ptr<XUI::UI::Controls::Control> XUI::UI::DataGridCheckBoxColumn::GenerateEditingElement(DataGridCell * cell, System::Reflection::Variant const& dataItem)
{
    std::shared_ptr<UI::CheckBox> control = std::make_shared<UI::CheckBox>();
    control->BeginInit();
    control->IsThreeState = IsThreeState;

    if (cell->RowDataItem() != DataGrid::NewPlaceHolder())
    {
        if (Binding)
            control->Bind(UI::CheckBox::IsCheckedProperty.get(), Binding.get());
        else
            control->IsChecked = dataItem.ToBool() ? ToggleButtonState::TBS_CHECKED : ToggleButtonState::TBS_NONE;
    }

    control->EndInit();
    return control;
}

void XUI::UI::DataGridCheckBoxColumn::StaticClassInitializer()
{
    IsThreeStateProperty = (Core::Dependency::BindProperty<bool>::Register<DataGridCheckBoxColumn>(nameof(IsThreeState)));
}

XUI::UI::DataGridCheckBoxColumn::DataGridCheckBoxColumn()
{
}

XUI::UI::DataGridCheckBoxColumn::~DataGridCheckBoxColumn()
{
}

void XUI::UI::DataGridCheckBoxColumn::OnInput(Core::Observer::RoutedEventArgs const & e)
{
    // Space key down will begin edit mode
    if (Interfaces::KeyEventArgs* keyEventArg = const_cast<Core::Observer::RoutedEventArgs&>(e).Static_As<Interfaces::KeyEventArgs>())
    {
        if (keyEventArg->Key == Interfaces::Key::Space)
        {
            BeginEdit(e);
        }
    }
    else if (Interfaces::PointerReleasedEventArgs* pointerEventArg = const_cast<Core::Observer::RoutedEventArgs&>(e).Static_As<Interfaces::PointerReleasedEventArgs>())
    {
        if (pointerEventArg->MouseButton == Interfaces::MouseButton::Left && (Core::Input::KeyboardDevice::Modifiers & (Interfaces::InputModifiers::Control | Interfaces::InputModifiers::Shift)) == 0)
        {
            BeginEdit(e);
        }
    }
}

bool XUI::UI::DataGridCheckBoxColumn::CommitCellEdit(XUI::UI::Controls::Control * editingElement)
{
    UI::CheckBox* control = editingElement->Static_As<UI::CheckBox>();
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
                    value = converter->ConvertBack(control->IsChecked, sourceObserver->GetPropertyType(), bindObject->ConverterParameter, 0);
                else
                    value = control->IsChecked;

                if (!value.GetBase())
                    value = sourceObserver->GetPropertyType()->TypeDescriptor->DefaultValue();
            }
        }
        else
            return false;

        if (grid->GetEditingRowItem().GetBase())
            return sourceObserver->SetTargetValue(grid->GetEditingRowItem().ToObject(), value, XamlCPP::BindingLevel::LocalValue);
        return false; // !Validation.GetHasError(textBox);
    }

    return true;
}

void XUI::UI::DataGridCheckBoxColumn::CancelCellEdit(XUI::UI::Controls::Control * editingElement, System::Reflection::Variant & uneditedValue)
{
    UI::CheckBox* textBox = editingElement->Static_As<UI::CheckBox>();
    if (textBox != nullptr)
    {
        auto bindings = textBox->GetBindingExpressions(UI::CheckBox::IsCheckedProperty.get());
        for (auto bind : *bindings)
            bind->UpdateTarget();
    }
}

System::Reflection::Variant XUI::UI::DataGridCheckBoxColumn::PrepareCellForEdit(XUI::UI::Controls::Control * editingElement, Core::Observer::RoutedEventArgs & editingEventArgs)
{
    UI::CheckBox* control = editingElement->Static_As<UI::CheckBox>();
    if (control != nullptr)
    {
        DataGridCell* cell = static_cast<DataGridCell*>(control->VisitLogicalAncestors([](Controls::Control* x) { return x->Is<DataGridCell>(); }));
        if (cell)
        {
            cell->Focus();

            if (control->VisualParent)
                control->Measure(control->VisualParent->Bounds.GetSize());

            auto originalValue = control->IsChecked == ToggleButtonState::TBS_CHECKED;

            Interfaces::KeyEventArgs* keyEventArg = editingEventArgs.Static_As<Interfaces::KeyEventArgs>();
            Interfaces::PointerReleasedEventArgs* pointerEventArg = editingEventArgs.Static_As<Interfaces::PointerReleasedEventArgs>();
            bool pointerSourceIsCheckBox = pointerEventArg; // ToDo

            if ((keyEventArg && keyEventArg->Key == Interfaces::Key::Space) || (pointerSourceIsCheckBox && pointerEventArg->MouseButton == Interfaces::MouseButton::Left))
            {
                control->IsChecked = originalValue != true ? ToggleButtonState::TBS_CHECKED : ToggleButtonState::TBS_NONE;
            }
        }
    }

    return System::Reflection::Variant::Empty;
}

void XUI::UI::DataGridCheckBoxColumn::CheckBoxPointerPress(void * x, Interfaces::PointerPressedEventArgs & e)
{
    //if (e.InputModifiers & Interfaces::InputModifiers::LeftMouseButton && (Core::Input::KeyboardDevice::Modifiers & (Interfaces::InputModifiers::Control | Interfaces::InputModifiers::Shift)) == 0)
    //{
    //    UI::CheckBox* checkBox = GENERIC_SAFE_AS(UI::CheckBox, x);
    //    if (checkBox)
    //    {
    //        DataGridCell* cell = static_cast<DataGridCell*>(checkBox->VisitLogicalAncestors([](Controls::Control* x) { return x->Is<DataGridCell>(); }));
    //        if (cell)
    //        {
    //            e.Handled = true;
    //            cell->OnAnyMouseLeftButtonDown(e);
    //            if (!cell->IsEditing)
    //                BeginEdit(e);
    //        }
    //    }
    //}
}
