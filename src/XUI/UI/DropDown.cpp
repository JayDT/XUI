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

#include "DropDown.h"
#include "DropDownItem.h"
#include "UI/Detail/PopupRoot.h"
#include "UI/Generators/ItemContainerGenerator.h"
#include "UI/Shapes/Rectangle.h"
#include "Core/Media/VisualBrush.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;
using namespace XUI::UI;

STATIC_CLASS_POST_INITIALIZER(DropDown, DropDown::StaticClassInitializer)

XUI::Core::Dependency::RefDependencyProperty DropDown::IsDropDownOpenProperty;
XUI::Core::Dependency::RefDependencyProperty DropDown::MaxDropDownHeightProperty;
XUI::Core::Dependency::RefDependencyProperty DropDown::SelectionBoxItemProperty;

void DropDown::StaticClassInitializer()
{
    IsDropDownOpenProperty = (Core::Dependency::DirectProperty<DropDown, bool>::RegisterDirect(nameof(IsDropDownOpen),
        [](DropDown* x) { return  x->IsDropDownOpen; },
        [](DropDown* x, bool v) { x->IsDropDownOpen = v; })
        );
    MaxDropDownHeightProperty = (Core::Dependency::BindProperty<double>::Register<DropDown>(nameof(MaxDropDownHeight), 200.0));
    SelectionBoxItemProperty = (Core::Dependency::DirectProperty<DropDown, System::Reflection::Variant>::RegisterDirect(nameof(SelectionBoxItem),
        [](DropDown* x) -> System::Reflection::Variant const& { return  x->SelectionBoxItem; },
        [](DropDown* x, System::Reflection::Variant const& v) { x->SelectionBoxItem = v; })
        );

    FocusableProperty->OverrideDefaultValue<DropDown>(true);

    SelectedItemProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        DropDown* control = e.Sender->Static_As<DropDown>();
        if (control)
            control->SelectedItemChanged(e);
    });
}

bool DropDown::GetIsDropDownOpen() const
{
    return _isDropDownOpen;
}

void DropDown::SetIsDropDownOpen(const bool &value)
{
    SetAndRaise(IsDropDownOpenProperty, _isDropDownOpen, value);
}

double DropDown::GetMaxDropDownHeight() const
{
    return GetValue<double>(MaxDropDownHeightProperty);
}

void DropDown::SetMaxDropDownHeight(const double &value)
{
    SetValue(MaxDropDownHeightProperty, value);
}

System::Reflection::Variant const& DropDown::GetSelectionBoxItem() const
{
    return _selectionBoxItem;
}

void DropDown::SetSelectionBoxItem(System::Reflection::Variant const& value)
{
    SetAndRaise(SelectionBoxItemProperty.get(), _selectionBoxItem, value);
}

std::shared_ptr<Generators::IItemContainerGenerator> DropDown::CreateItemContainerGenerator()
{
    return std::make_shared<Generators::ItemContainerGenerator<DropDownItem>>(this, DropDownItem::ContentProperty.get(), DropDownItem::ContentTemplateProperty.get());
}

void DropDown::OnAttachedToLogicalTree(Controls::LogicalTreeAttachmentEventArgs& e)
{
    SelectingItemsControl::OnAttachedToLogicalTree(e);
    UpdateSelectionBoxItem(SelectedItem);
}

void DropDown::OnGotFocus(Interfaces::GotFocusEventArgs& e)
{
    SelectingItemsControl::OnGotFocus(e);

    if (!e.Handled && e.NavigationMethod == Interfaces::NavigationMethod::Directional)
    {
        e.Handled = UpdateSelectionFromEventSource(e.Source);
    }
}

void DropDown::OnKeyDown(Interfaces::KeyEventArgs& e)
{
    SelectingItemsControl::OnKeyDown(e);

    if (!e.Handled)
    {
        if (e.Key == Interfaces::Key::F4 || ((e.Key == Interfaces::Key::Down || e.Key == Interfaces::Key::Up) && ((e.Modifiers & Interfaces::InputModifiers::Alt) != 0)))
        {
            IsDropDownOpen = !IsDropDownOpen;
            e.Handled = true;
        }
        else if (IsDropDownOpen && (e.Key == Interfaces::Key::Escape || e.Key == Interfaces::Key::Enter))
        {
            IsDropDownOpen = false;
            e.Handled = true;
        }

        if (!IsDropDownOpen)
        {
            if (e.Key == Interfaces::Key::Down)
            {
                if (SelectedIndex == -1)
                {
                    SelectedIndex = 0;
                }

                if ((_selectedIndex + 1) >= ItemCount)
                {
                    SelectedIndex = 0;
                }
                else
                    SelectedIndex = (_selectedIndex + 1);

                e.Handled = true;
            }
            else if (e.Key == Interfaces::Key::Up)
            {
                if ((_selectedIndex - 1) < 0)
                {
                    SelectedIndex = ItemCount - 1;
                }
                else
                    SelectedIndex = (_selectedIndex - 1);

                e.Handled = true;
            }
        }
    }
}

void DropDown::OnPointerPressed(Interfaces::PointerPressedEventArgs& e)
{
    if (!e.Handled)
    {
        if (e.Source->GetVisualRoot() && e.Source->GetVisualRoot()->ToVisual()->Is<PopupRoot>())
        {
            if (UpdateSelectionFromEventSource(e.Source))
            {
                if (_popup)
                    _popup->Close();
                e.Handled = true;
            }
        }
        else
        {
            IsDropDownOpen = !IsDropDownOpen;
        }
    }
    SelectingItemsControl::OnPointerPressed(e);
}

void DropDown::OnTemplateApplied(UI::Controls::TemplateAppliedEventArgs const& e)
{
    if (_popup)
    {
        _popup->Opened -= System::make_eventhandler(&DropDown::PopupOpened, this);
    }

    auto popup = Template->FindName("PART_Popup", this);
    if (popup)
    {
        auto __popup = popup->Static_As<Popup>();
        if (__popup)
        {
            _popup = __popup->shared_from_base_static<XUI::UI::Popup>();
            _popup->Opened += System::make_eventhandler(&DropDown::PopupOpened, this);
        }
    }
}

void DropDown::PopupOpened(void* sender, System::Events::EventArg& e)
{
    auto selectedIndex = SelectedIndex;
    if (selectedIndex == -1)
    {
        if (!AlwaysSelected)
            return;

        selectedIndex = 0;
    }

    if (selectedIndex != -1)
    {
        auto container = GetItemContainerGenerator()->ContainerFromIndex(selectedIndex);
        if (container) container->Focus();
    }
}

void DropDown::SelectedItemChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
    UpdateSelectionBoxItem(e.NewValue);
}

void DropDown::UpdateSelectionBoxItem(System::Reflection::Variant const& item)
{
    if (!item.Valid())
    {
        //// WorkAround for binding SelectedItem but itemgenerator after initialize
        //if (!SelectedItems.empty())
        //{
        //    SelectedItemsAdded(SelectedItems);
        //}
        //
        //if (!item.Valid())
        {
            SelectionBoxItem = System::Reflection::Variant::Empty;
            return;
        }
    }


    System::Reflection::Object* object = item.ToObject();

    if (object && object->Is<Controls::ContentControl>())
        object = object->Static_As<Controls::ContentControl>()->Content.ToObject();

    if (object && object->Is<Controls::Control>())
    {
        Controls::Control* control = static_cast<Controls::Control*>(object);
        control->InvalidateMeasure(); //Measure(Core::Media::Size::Infinity);

        auto rect = std::make_shared<UI::Shapes::Rectangle>();
        
        rect->Width = std::min(DesiredSize.Width, control->DesiredSize.Width);
        rect->Height = std::min(DesiredSize.Height, control->DesiredSize.Height);
        auto brush = std::make_shared<Core::Media::VisualBrush>();
        brush->Visual = control->shared_from_base_static<Controls::Visual>();
        brush->Stretch = Core::Media::Stretch::None;
        brush->AlignmentX = Core::Media::AlignmentX::Left;
        rect->Fill = brush;

        SelectionBoxItem = rect;
    }
    else
        SelectionBoxItem = item;
}
