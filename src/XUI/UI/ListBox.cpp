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

#include "ListBox.h"
#include "ListBoxItem.h"
#include "UI/ScrollViewer.h"
#include "UI/VirtualizingStackPanel.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XUI::UI;
using namespace XUI::UI::Controls;

STATIC_CLASS_INITIALIZER(ListBox, XUI::UI::ListBox::StaticClassInitializer)

XUI::Core::Dependency::RefDependencyProperty ListBox::ScrollProperty;
XUI::Core::Dependency::RefDependencyProperty ListBox::SelectedItemsProperty;
XUI::Core::Dependency::RefDependencyProperty ListBox::SelectionModeProperty;
XUI::Core::Dependency::RefDependencyProperty ListBox::VirtualizationModeProperty;

void ListBox::StaticClassInitializer()
{
    ScrollProperty = (Core::Dependency::DirectProperty<ListBox, UI::ScrollViewer*>::RegisterDirect(nameof(Scroll),
        [](ListBox* x) { return x->Scroll; },
        [](ListBox* x, UI::ScrollViewer * v) { x->Scroll = v; })
        );
    SelectedItemsProperty = (Core::Dependency::BindProperty<System::Collection::Generic::IContainer *>::Register<ListBox>(nameof(SelectedItems)));
    SelectionModeProperty = (Core::Dependency::BindProperty<UI::SelectionMode>::Register<ListBox>(nameof(SelectionMode)));
    VirtualizationModeProperty = (Core::Dependency::BindProperty<UI::Presenters::ItemVirtualizationMode>::Register<ListBox>(nameof(VirtualizationMode), UI::Presenters::ItemVirtualizationMode::Simple));

    MemberSelectorProperty->OverrideDefaultValue<ListBox>(System::Delegate<System::Reflection::Variant(System::Reflection::Variant const&)>([](System::Reflection::Variant const& o) ->System::Reflection::Variant
    {
        if (o.GetBase() && o.IsObject() && o.GetBase()->IsSharedPointer()) // can be datacontext
            return o;
        if (o.IsObject() && o.ToObject())
            return std::make_shared<System::String>(o.ToObject()->ToString());
        return std::make_shared<System::String>(o.ToString());
    }));

    ItemsPanelProperty->OverrideDefaultValue<ListBox>(std::make_shared<DataTemplates::FuncTemplate<std::shared_ptr<XUI::UI::Panel>>>([]()
    {
        return std::make_shared<VirtualizingStackPanel>();
    }));
}

XUI::UI::ListBox::ListBox()
{
    Items = std::make_shared<System::Collection::DynamicList>();
}

XUI::UI::ListBox::~ListBox()
{
}

XUI::UI::ScrollViewer *ListBox::GetScroll()
{
    return _scroll.get();
}

void ListBox::SetScroll(XUI::UI::ScrollViewer *value)
{
    SetAndRaise(ScrollProperty, _scroll, value->shared_from_base_static<XUI::UI::ScrollViewer>());
}

System::Collection::Generic::IContainer* ListBox::GetSelectedItems()
{
    return &_selectedItems;
}

XUI::UI::SelectionMode ListBox::GetSelectionMode()
{
    return SelectingItemsControl::SelectionMode;
}

void ListBox::SetSelectionMode(XUI::UI::SelectionMode value)
{
    SelectingItemsControl::SelectionMode = value;
}

XUI::UI::Presenters::ItemVirtualizationMode ListBox::GetVirtualizationMode()
{
    return GetValue<XUI::UI::Presenters::ItemVirtualizationMode>(VirtualizationModeProperty);
}

void ListBox::SetVirtualizationMode(XUI::UI::Presenters::ItemVirtualizationMode value)
{
    SetValue(VirtualizationModeProperty, value);
}

std::shared_ptr<Generators::IItemContainerGenerator> XUI::UI::ListBox::CreateItemContainerGenerator()
{
    return std::make_shared<Generators::ItemContainerGenerator<ListBoxItem>>(this, Controls::ContentControl::ContentProperty.get(), Controls::ContentControl::ContentTemplateProperty.get());
}

void ListBox::OnGotFocus(XUI::Interfaces::GotFocusEventArgs& e)
{
    SelectingItemsControl::OnGotFocus(e);

    if (e.NavigationMethod == Interfaces::NavigationMethod::Directional)
    {
        e.Handled = UpdateSelectionFromEventSource(e.Source, true, (e.InputModifiers & Interfaces::InputModifiers::Shift) != 0);
    }
}

void ListBox::OnPointerPressed(XUI::Interfaces::PointerPressedEventArgs& e)
{
    SelectingItemsControl::OnPointerPressed(e);

    if (e.MouseButton == Interfaces::MouseButton::Left || e.MouseButton == Interfaces::MouseButton::Right)
    {
        e.Handled = UpdateSelectionFromEventSource(e.Source, true, (e.InputModifiers & Interfaces::InputModifiers::Shift) != 0, (e.InputModifiers & Interfaces::InputModifiers::Control) != 0);
    }
}

void ListBox::OnTemplateApplied(TemplateAppliedEventArgs const& e)
{
    SelectingItemsControl::OnTemplateApplied(e);

    auto _scroll = Template->FindName("PART_ScrollViewer", this);
    if (_scroll)
        Scroll = _scroll->Dynamic_As<ScrollViewer>();
}
