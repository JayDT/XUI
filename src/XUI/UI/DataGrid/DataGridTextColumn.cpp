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

#include "DataGridTextColumn.h"
#include "DataGridCell.h"
#include "DataGridColumn.h"
#include "DataGridControl.h"
#include "UI/TextBox.h"
#include "UI/TextBlock.h"
#include "Core/Media/Colors.h"
#include "XamlCPP/DataBind/Binding.h"
#include "XamlCPP/DataBind/SourceObserver.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;
using namespace XUI::UI;

Core::Dependency::RefDependencyProperty DataGridTextColumn::FontFamilyProperty;
Core::Dependency::RefDependencyProperty DataGridTextColumn::FontSizeProperty;
Core::Dependency::RefDependencyProperty DataGridTextColumn::FontStyleProperty;
Core::Dependency::RefDependencyProperty DataGridTextColumn::FontWeightProperty;
Core::Dependency::RefDependencyProperty DataGridTextColumn::ForegroundProperty;

STATIC_CLASS_INITIALIZER(DataGridTextColumn, XUI::UI::DataGridTextColumn::StaticClassInitializer)

void XUI::UI::DataGridTextColumn::StaticClassInitializer()
{
    FontFamilyProperty = (Core::Dependency::BindProperty<std::string>::Register<DataGridTextColumn>(nameof(FontFamily), "mono_dejavu_sans_", true));
    FontSizeProperty = (Core::Dependency::BindProperty<double>::Register<DataGridTextColumn>(nameof(FontSize), 10, true));
    FontStyleProperty = (Core::Dependency::BindProperty<Core::Media::FontStyle>::Register<DataGridTextColumn>(nameof(FontStyle), Core::Media::FontStyle::Normal, true));
    FontWeightProperty = (Core::Dependency::BindProperty<Core::Media::FontWeight>::Register<DataGridTextColumn>(nameof(FontWeight), Core::Media::FontWeight::Normal, true));
    ForegroundProperty = (Core::Dependency::BindProperty<std::shared_ptr<Interfaces::IBrush>>::Register<DataGridTextColumn>(nameof(Foreground), std::make_shared<Core::Media::SolidColorBrush>(Core::Media::Colors::GetWhiteSmoke()), true));
}

XUI::UI::DataGridTextColumn::DataGridTextColumn()
{
}

XUI::UI::DataGridTextColumn::~DataGridTextColumn()
{
}

std::shared_ptr<XUI::UI::Controls::Control> XUI::UI::DataGridTextColumn::GenerateElement(DataGridCell * cell, System::Reflection::Variant const& dataItem)
{
    std::shared_ptr<UI::TextBlock> textBlock = std::make_shared<UI::TextBlock>();
    textBlock->BeginInit();
    SyncProperties(textBlock.get());

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

std::shared_ptr<XUI::UI::Controls::Control> XUI::UI::DataGridTextColumn::GenerateEditingElement(DataGridCell * cell, System::Reflection::Variant const& dataItem)
{
    std::shared_ptr<UI::TextBox> textBox = std::make_shared<UI::TextBox>();
    textBox->BeginInit();
    SyncProperties(textBox.get());

    auto dataContext = cell->DataContext;
    if (cell->RowDataItem() != DataGrid::NewPlaceHolder())
    {
        if (Binding)
            textBox->Bind(TextBox::TextProperty.get(), Binding.get());
        else
            textBox->Text = System::String::ToWString(dataItem.ToString());
    }

    textBox->EndInit();
    return textBox;
}

std::string XUI::UI::DataGridTextColumn::GetFontFamily() const
{
    return GetValue<std::string>(FontFamilyProperty.get());
}

void XUI::UI::DataGridTextColumn::SetFontFamily(std::string value)
{
    SetValue(FontFamilyProperty.get(), value);
}

double XUI::UI::DataGridTextColumn::GetFontSize() const
{
    return GetValue<double>(FontSizeProperty.get());
}

void XUI::UI::DataGridTextColumn::SetFontSize(double value)
{
    SetValue(FontSizeProperty.get(), value);
}

Core::Media::FontStyle XUI::UI::DataGridTextColumn::GetFontStyle() const
{
    return GetValue<Core::Media::FontStyle>(FontStyleProperty.get());
}

void XUI::UI::DataGridTextColumn::SetFontStyle(Core::Media::FontStyle value)
{
    SetValue(FontStyleProperty.get(), value);
}

Core::Media::FontWeight XUI::UI::DataGridTextColumn::GetFontWeight() const
{
    return GetValue<Core::Media::FontWeight>(FontWeightProperty.get());
}

void XUI::UI::DataGridTextColumn::SetFontWeight(Core::Media::FontWeight value)
{
    SetValue(FontWeightProperty.get(), value);
}

std::shared_ptr<XUI::Interfaces::IBrush> XUI::UI::DataGridTextColumn::GetForeground() const
{
    return GetValue<std::shared_ptr<Interfaces::IBrush>>(ForegroundProperty.get());
}

void XUI::UI::DataGridTextColumn::SetForeground(std::shared_ptr<XUI::Interfaces::IBrush> value)
{
    SetValue(ForegroundProperty.get(), value->shared_from_base_static<Interfaces::IBrush>());
}

void XUI::UI::DataGridTextColumn::OnInput(Core::Observer::RoutedEventArgs const & e)
{
}

bool XUI::UI::DataGridTextColumn::CommitCellEdit(XUI::UI::Controls::Control * editingElement)
{
    TextBox* textBox = editingElement->Static_As<TextBox>();
    if (textBox != nullptr)
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
                    value = converter->ConvertBack(textBox->Text, sourceObserver->GetPropertyType(), bindObject->ConverterParameter, 0);
                else
                    value = textBox->Text;

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

void XUI::UI::DataGridTextColumn::CancelCellEdit(XUI::UI::Controls::Control * editingElement, System::Reflection::Variant & uneditedValue)
{
    TextBox* textBox = editingElement->Static_As<TextBox>();
    if (textBox != nullptr)
    {
        auto bindings = textBox->GetBindingExpressions(TextBox::TextProperty.get());
        for (auto bind : *bindings)
            bind->UpdateTarget();
    }
}

bool PlaceCaretOnTextBox(TextBox* textBox, Core::Media::Point position)
{
    int characterIndex = textBox->GetCharacterIndexFromPoint(position, /* snapToText = */ false);
    if (characterIndex >= 0)
    {
        textBox->Select(characterIndex, 0);
        return true;
    }

    return false;
}

System::Reflection::Variant XUI::UI::DataGridTextColumn::PrepareCellForEdit(XUI::UI::Controls::Control * editingElement, Core::Observer::RoutedEventArgs & editingEventArgs)
{
    UI::TextBox* textBox = editingElement->Static_As<UI::TextBox>();
    if (textBox != nullptr)
    {
        textBox->Focus();

        auto originalValue = textBox->Text;

        Interfaces::TextInputEventArgs* textArgs = editingEventArgs.Static_As<Interfaces::TextInputEventArgs>();
        if (textArgs != nullptr)
        {
            // If text input started the edit, then replace the text with what was typed.
            auto inputText = textArgs->Text;
            textBox->Text = inputText;

            // Place the caret after the end of the text.
            textBox->Select(inputText.size(), 0);
        }
        else
        {
            if (textBox->VisualParent)
                textBox->Measure(textBox->VisualParent->Bounds.GetSize());

            // If a mouse click started the edit, then place the caret under the mouse.
            Interfaces::PointerEventArgs* mouseArgs = editingEventArgs.Static_As<Interfaces::PointerEventArgs>();
            if ((mouseArgs == nullptr) || !PlaceCaretOnTextBox(textBox, mouseArgs->GetPosition(textBox)))
            {
                // If the mouse isn't over the textbox or something else started the edit, then select the text.
                textBox->SelectAll();
            }
        }
    }

    return System::Reflection::Variant::Empty;
}

void XUI::UI::DataGridTextColumn::SyncProperties(XUI::UI::Controls::Control * e)
{
    e->FontFamily = this->FontFamily;
    e->FontSize = this->FontSize;
    e->FontStyle = this->FontStyle;
    e->FontWeight = this->FontWeight;
    e->Foreground = this->Foreground;
    e->BorderThickness = 0.0;
}
