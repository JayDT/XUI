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

#include "TreeView.h"
#include "TreeViewItem.h"
#include "UI/Detail/HeaderedContentControl.h"
#include "UI/Generators/TreeContainerIndex.h"
#include "UI/Generators/TreeItemContainerGenerator.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;
using namespace XUI::Interfaces;
using namespace XUI::UI;

STATIC_CLASS_INITIALIZER(TreeView, TreeView::StaticClassInitializer)

XUI::Core::Dependency::RefDependencyProperty TreeView::AutoScrollToSelectedItemProperty;
XUI::Core::Dependency::RefDependencyProperty TreeView::SelectedItemProperty;
std::shared_ptr<Core::Observer::RoutedEvent> TreeView::SelectedItemChangedEvent;
Core::Observer::RoutedPropertyChangedEventArgs<System::Reflection::RefObject>* TreeView::__0;

void TreeView::StaticClassInitializer()
{
    SelectedItemChangedEvent = (Core::Observer::RoutedEvent::Register<TreeView, Core::Observer::RoutedPropertyChangedEventArgs<System::Reflection::RefObject>>(nameof(SelectedItemChanged), Core::Observer::RoutingStrategies(Core::Observer::RoutingStrategies::Bubble | Core::Observer::RoutingStrategies::Direct)));

    AutoScrollToSelectedItemProperty = (Core::Dependency::BindProperty<bool>::Register<TreeView>(nameof(AutoScrollToSelectedItem)));
    SelectedItemProperty = (Core::Dependency::BindProperty<System::Reflection::RefObject>::Register<TreeView>(nameof(SelectedItem)));
}

XUI::UI::TreeView::TreeView()
    : SelectedItemChanged(this)
{
    Items = std::make_shared<System::Collection::List<System::Reflection::RefObject>>();
}

XUI::UI::TreeView::~TreeView()
{
}

bool TreeView::GetAutoScrollToSelectedItem() const
{
    return GetValue<bool>(AutoScrollToSelectedItemProperty);
}

void TreeView::SetAutoScrollToSelectedItem(const bool &value)
{
    SetValue(AutoScrollToSelectedItemProperty, value);
}

System::Reflection::RefObject const& TreeView::GetSelectedItem() const
{
    return _selectedItem;
}

void TreeView::SetSelectedItem(System::Reflection::RefObject const& value)
{
    if (_selectedItem != nullptr)
    {
        auto container = static_cast<Generators::TreeItemContainerGenerator<TreeViewItem>*>(GetItemContainerGenerator().get())->Index->ContainerFromItem(_selectedItem);
        MarkContainerSelected(container, false);
    }


    auto oldValue = _selectedItem;
    if (SetAndRaise(SelectedItemProperty, _selectedItem, value))
    {
        Core::Observer::RoutedPropertyChangedEventArgs<System::Reflection::RefObject> e(TreeView::SelectedItemChangedEvent.get());
        e.Source = this;
        e.Sender = this;
        e.OldValue = oldValue;
        e.NewValue = _selectedItem;
        RaiseEvent(e);
    }

    if (_selectedItem != nullptr)
    {
        auto container = static_cast<Generators::TreeItemContainerGenerator<TreeViewItem>*>(GetItemContainerGenerator().get())->Index->ContainerFromItem(_selectedItem);
        MarkContainerSelected(container, true);
    
        if (AutoScrollToSelectedItem && container != nullptr)
        {
            container->BringIntoView(Core::Media::Rect::xywh(0, 0, container->Bounds.width(), container->Bounds.height()));
        }
    }
}

std::shared_ptr<Generators::IItemContainerGenerator> XUI::UI::TreeView::CreateItemContainerGenerator()
{
    auto treeindex = std::make_shared<Generators::TreeContainerIndex>();
    auto result = std::make_shared<Generators::TreeItemContainerGenerator<TreeViewItem>>(
        this,
        TreeViewItem::HeaderProperty.get(),
        TreeViewItem::ItemTemplateProperty.get(),
        TreeViewItem::ItemsProperty.get(),
        TreeViewItem::IsExpandedProperty.get(),
        treeindex);
    result->Index->Materialized += System::make_eventhandler(&TreeView::ContainerMaterialized, this);
    return result;
}

void TreeView::OnGotFocus(XUI::Interfaces::GotFocusEventArgs& e)
{
    if (e.NavigationMethod == NavigationMethod::Directional)
    {
        e.Handled = UpdateSelectionFromEventSource(e.Source, true, (e.InputModifiers & InputModifiers::Shift) != 0);
    }
}

void TreeView::OnPointerPressed(XUI::Interfaces::PointerPressedEventArgs& e)
{
    ItemsControl::OnPointerPressed(e);

    if (e.MouseButton == MouseButton::Left || e.MouseButton == MouseButton::Right)
    {
        e.Handled = UpdateSelectionFromEventSource(e.Source, true, (e.InputModifiers & InputModifiers::Shift) != 0, (e.InputModifiers & InputModifiers::Control) != 0);
    }
}

void TreeView::UpdateSelectionFromContainer(Controls::Control *container, bool select, bool rangeModifier, bool toggleModifier)
{
    auto item = static_cast<Generators::TreeItemContainerGenerator<TreeViewItem>*>(GetItemContainerGenerator().get())->Index->ItemFromContainer(container);
    
    if (item != nullptr)
    {
        if (SelectedItem != nullptr)
        {
            auto old = static_cast<Generators::TreeItemContainerGenerator<TreeViewItem>*>(GetItemContainerGenerator().get())->Index->ContainerFromItem(SelectedItem);
            MarkContainerSelected(old, false);
        }
    
        SelectedItem = item;
    
        MarkContainerSelected(container, true);
    }
}

bool TreeView::UpdateSelectionFromEventSource(Controls::InputElement *eventSource, bool select, bool rangeModifier, bool toggleModifier)
{
    auto container = GetContainerFromEventSource(eventSource);

    if (container != nullptr)
    {
        UpdateSelectionFromContainer(container, select, rangeModifier, toggleModifier);
        return true;
    }

    return false;
}

Controls::Control *TreeView::GetContainerFromEventSource(Controls::InputElement *eventSource)
{
    std::list<Controls::Visual*> visuals;
    eventSource->GetVisualAncestors(visuals, [](Controls::Visual* x) -> bool
    {
        return x && x->Is<TreeViewItem>();
    });

    auto item = !visuals.empty() ? (*visuals.begin())->Static_As<TreeViewItem>() : nullptr;
    
    if (item != nullptr)
    {
        if (static_cast<Generators::TreeItemContainerGenerator<TreeViewItem>*>(item->GetItemContainerGenerator().get())->Index == static_cast<Generators::TreeItemContainerGenerator<TreeViewItem>*>(GetItemContainerGenerator().get())->Index)
        {
            return item;
        }
    }

    return nullptr;
}

void TreeView::ContainerMaterialized(void* sender, XUI::Interfaces::ItemContainerEventArgs& e)
{
    auto selectedItem = SelectedItem;
    
    if (selectedItem != nullptr)
    {

        for (auto itr = e.Container->GetGenericEnumerator(); !itr->IsFinished(); itr->MoveNext())
        {
            Interfaces::ItemContainerInfo info = itr->Current().GetValue<Interfaces::ItemContainerInfo>();
            if (!info.ContainerControl)
                continue;

            if (info.Item == selectedItem)
            {
                (info.ContainerControl->Static_As<TreeViewItem>())->IsSelected = true;
    
                if (AutoScrollToSelectedItem)
                {
                    XUI::Core::UIDispatcher::OnRender += [info]()
                    {
                        info.ContainerControl->BringIntoView(Core::Media::Rect::xywh(0, 0, info.ContainerControl->Bounds.width(), info.ContainerControl->Bounds.height()));
                    };
                }
                break;
            }
        }
    }
}

void TreeView::MarkContainerSelected(Controls::Control *container, bool selected)
{
    if (container != nullptr)
    {
        auto selectable = container->Static_As<UI::HeaderedContentControl>();

        if (selectable != nullptr)
        {
            selectable->IsSelected = selected;
        }
        else
        {
            //if (selected)
            //    container->PseudoClasses.insert(":selected");
            //else
            //    container->PseudoClasses.erase(":selected");
        }
    }
}