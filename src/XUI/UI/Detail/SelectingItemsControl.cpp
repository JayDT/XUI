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

#include "SelectingItemsControl.h"
#include "UI/Detail/HeaderedContentControl.h"
#include "UI/Detail/HeaderedSelectingControl.h"
#include "Core/GlobalInitializerScheduler.h"

#include <cpplinq/linq.hpp>

using namespace XUI;
using namespace XUI::UI;

STATIC_CLASS_INITIALIZER(SelectingItemsControl, XUI::UI::SelectingItemsControl::StaticClassInitializer)

XUI::Core::Dependency::RefDependencyProperty SelectingItemsControl::AutoScrollToSelectedItemProperty;
XUI::Core::Dependency::RefDependencyProperty SelectingItemsControl::SelectedIndexProperty;
XUI::Core::Dependency::RefDependencyProperty SelectingItemsControl::SelectedItemProperty;
XUI::Core::Dependency::RefDependencyProperty SelectingItemsControl::SelectedItemsProperty;
XUI::Core::Dependency::RefDependencyProperty SelectingItemsControl::SelectionModeProperty;
std::shared_ptr<Core::Observer::RoutedEvent> SelectingItemsControl::IsSelectedChangedEvent;
std::shared_ptr<Core::Observer::RoutedEvent> SelectingItemsControl::SelectionChangedEvent;

void SelectingItemsControl::StaticClassInitializer()
{
    AutoScrollToSelectedItemProperty = (Core::Dependency::BindProperty<bool>::Register<SelectingItemsControl>(nameof(AutoScrollToSelectedItem)));
    SelectedIndexProperty = (Core::Dependency::DirectProperty<SelectingItemsControl, int>::RegisterDirect(nameof(SelectedIndex),
        [](SelectingItemsControl* x) { return x->SelectedIndex; },
        [](SelectingItemsControl* x, int const& v) { x->SelectedIndex = v; })
        );
    SelectedItemProperty = (Core::Dependency::DirectProperty<SelectingItemsControl, System::Reflection::Variant>::RegisterDirect(nameof(SelectedItem),
        [](SelectingItemsControl* x) { return x->SelectedItem; },
        [](SelectingItemsControl* x, System::Reflection::Variant const& v) { x->SelectedItem = v; })
        );
    SelectedItemsProperty = (Core::Dependency::DirectProperty<SelectingItemsControl, System::Collection::DynamicList>::RegisterDirect(nameof(SelectedItems),
        [](SelectingItemsControl* x) { return x->SelectedItems; },
        [](SelectingItemsControl* x, System::Collection::DynamicList const& v) { x->SelectedItems = v; })
        );
    SelectionModeProperty = (Core::Dependency::BindProperty<UI::SelectionMode>::Register<SelectingItemsControl>(nameof(SelectionModeItem)));
    IsSelectedChangedEvent = (Core::Observer::RoutedEvent::Register<XUI::UI::SelectingItemsControl, Core::Observer::RoutedEventArgs>(nameof(IsSelectedChanged), Core::Observer::RoutingStrategies::Bubble));
    SelectionChangedEvent = (Core::Observer::RoutedEvent::Register<XUI::UI::SelectingItemsControl, Interfaces::SelectionChangedEventArgs>(nameof(SelectionChanged), Core::Observer::RoutingStrategies::Bubble));

    IsSelectedChangedEvent->AddClassSpecHandler<Core::Observer::RoutedEventArgs, SelectingItemsControl>(System::make_eventhandler([](void* x, Core::Observer::RoutedEventArgs& e)
    {
        static_cast<SelectingItemsControl*>((System::Reflection::Object*)x)->ContainerSelectionChanged(e);
    }));
}

SelectingItemsControl::SelectingItemsControl()
    : IsSelectedChanged(this)
    , SelectionChanged(this)
{
    _selectedItems.CollectionChanged += System::make_eventhandler(&SelectingItemsControl::SelectedItemsCollectionChanged, this);
}

bool SelectingItemsControl::GetAutoScrollToSelectedItem()
{
    return GetValue<bool>(AutoScrollToSelectedItemProperty);
}

void SelectingItemsControl::SetAutoScrollToSelectedItem(bool value)
{
    SetValue(AutoScrollToSelectedItemProperty, value);
}

int SelectingItemsControl::GetSelectedIndex() const
{
    return _selectedIndex;
}

void SelectingItemsControl::SetSelectedIndex(int value)
{
    if (_updateCount == 0)
    {
        auto old = SelectedIndex;
        auto effective = (Items && value >= 0 && value < Items->size()) ? value : -1;

        if (old != effective)
        {
            _selectedIndex = effective;
            RaisePropertyChanged(SelectedIndexProperty.get(), old, effective, Core::Dependency::BindingLevel::LocalValue);
            SelectedItem = ElementAt(Items.get(), effective);
        }

        //SetAndRaise(SelectedIndexProperty, _selectedIndex, [&] (int val, int &backing, std::function<void(std::function<void()>)> notifyWrapper) -> int
        //{
        //    auto old = backing;
        //    auto effective = (val >= 0 && val < Items == nullptr ? nullptr : Items->Cast<void*>()->Count()) ? val : -1;
        //
        //    if (old != effective)
        //    {
        //        backing = effective;
        //        notifyWrapper([&] ()
        //        {
        //            RaisePropertyChanged(SelectedIndexProperty, old, effective, BindingPriority::LocalValue);
        //        });
        //        SelectedItem = ElementAt(Items, effective);
        //    }
        //}, value);
    }
    else
    {
        _updateSelectedIndex = value;
        _updateSelectedItems.Clear();
    }
}

System::Reflection::Variant SelectingItemsControl::GetSelectedItem()
{
    return _selectedItem;
}

void SelectingItemsControl::SetSelectedItem(System::Reflection::Variant const& value)
{
    if (_updateCount == 0)
    {
        auto old = SelectedItem;
        auto index = value && value.toPointer() ? IndexOf(Items.get(), value) : -1;
        auto effective = index != -1 ? value : System::Reflection::Variant::Empty;

        if (effective != old)
        {
            _selectedItem = effective;
            RaisePropertyChanged(SelectedItemProperty.get(), old, effective, Core::Dependency::BindingLevel::LocalValue);
            SelectedIndex = index;

            if (effective.GetBase())
            {
                if (SelectedItems.size() != 1 || SelectedItems.ElementAt(0) != effective)
                {
                    _syncingSelectedItems = true;
                    _selectedItems.Clear();
                    _selectedItems.Add(effective);
                    _syncingSelectedItems = false;
                }
            }
            else if (SelectedItems.size() > 0)
            {
                _selectedItems.Clear();
            }
        }

        //SetAndRaise(SelectedItemProperty, _selectedItem, [&] (void *val, void *&backing, std::function<void(std::function<void()>)> notifyWrapper)
        //{
        //    auto old = backing;
        //    auto index = IndexOf(Items, val);
        //    auto effective = index != -1 ? val : nullptr;
        //
        //    if (!Object::Equals(effective, old))
        //    {
        //        backing = effective;
        //
        //        notifyWrapper([&] ()
        //        {
        //            RaisePropertyChanged(SelectedItemProperty, old, effective, BindingPriority::LocalValue);
        //        });
        //
        //        SelectedIndex = index;
        //
        //        if (effective != nullptr)
        //        {
        //            if (SelectedItems->Count != 1 || SelectedItems[0] != effective)
        //            {
        //                _syncingSelectedItems = true;
        //                SelectedItems->Clear();
        //                SelectedItems->Add(effective);
        //                _syncingSelectedItems = false;
        //            }
        //        }
        //        else if (SelectedItems->Count > 0)
        //        {
        //            SelectedItems->Clear();
        //        }
        //    }
        //}, value);
    }
    else
    {
        _updateSelectedItems.Clear();
        _updateSelectedItems.Add(value);
        _updateSelectedIndex = std::numeric_limits<int>::min();
    }
}

void XUI::UI::SelectingItemsControl::UnSelectAll()
{
    auto old = SelectedItem;

    if (System::Reflection::Variant::Empty != old)
    {
        _selectedItem = System::Reflection::Variant::Empty;
        RaisePropertyChanged(SelectedItemProperty.get(), old, System::Reflection::Variant::Empty, Core::Dependency::BindingLevel::LocalValue);
        SelectedIndex = -1;

        if (SelectedItems.size() > 0)
        {
            _selectedItems.Clear();
        }
    }
}

System::Collection::DynamicList& SelectingItemsControl::GetSelectedItems()
{
    return _selectedItems;
}

System::Collection::DynamicList const& SelectingItemsControl::GetSelectedItems() const
{
    return _selectedItems;
}

void SelectingItemsControl::SetSelectedItems(System::Collection::DynamicList const& value)
{
    //if ((value && value->IsFixedSize == true) || (value && value->IsReadOnly == true))
    //{
    //    throw System::NotSupportedException("Cannot use a fixed size or read-only collection as SelectedItems.");
    //}

    UnsubscribeFromSelectedItems();
    _selectedItems.Clear();
    if (!value.empty())
    {
        for (auto const& item : value.GetNativeEnumerator())
        {
            _selectedItems.Add(item);
        }
    }
    SubscribeToSelectedItems();
}

UI::SelectionMode SelectingItemsControl::GetSelectionMode()
{
    return GetValue<UI::SelectionMode>(SelectionModeProperty);
}

void SelectingItemsControl::SetSelectionMode(UI::SelectionMode value)
{
    SetValue(SelectionModeProperty, value);
}

bool SelectingItemsControl::GetAlwaysSelected()
{
    return (uint(SelectionMode) & uint(SelectionMode::AlwaysSelected)) != 0;
}

void SelectingItemsControl::BeginInit()
{
    ItemsControl::BeginInit();
    ++_updateCount;
    _updateSelectedIndex = std::numeric_limits<int>::min();
}

void SelectingItemsControl::EndInit()
{
    ItemsControl::EndInit();

    if (--_updateCount == 0)
    {
        UpdateFinished();
    }
}

void SelectingItemsControl::ScrollIntoView(System::Reflection::Variant const& item)
{
    if (Presenter)
        Presenter->ScrollIntoView(item);
}

Controls::Control *SelectingItemsControl::GetContainerFromEventSource(Controls::InputElement *eventSource)
{
    Controls::Control * item = nullptr;

    if (eventSource->Is<UI::Presenters::ContentPresenter>())
    {
        auto child = static_cast<UI::Presenters::ContentPresenter*>(eventSource)->Child;
        if (child && child->GetParent() == this)
            item = child.get();
    }

    if (!item)
    {
        Core::Dependency::DependencyObject* parent = eventSource;
        while (parent)
        {
            // Here not intersting invalod cast or not
            if (GetItemContainerGenerator()->IndexFromContainer(static_cast<Controls::Control*>(parent)) != -1)
            {
                item = parent->Static_As<Controls::Control>();
                break;
            }

            parent = parent->GetInheritanceParent();
        }
    }

    return item;
}

void SelectingItemsControl::ItemsChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
    ItemsControl::ItemsChanged(e);

    static const System::Reflection::Type* containerType = &typeof(System::Collection::Generic::IContainer);

    if (_updateCount == 0)
    {
        System::Collection::Generic::IContainer* newNotifyContainer = containerType->IsAssignableFrom(*e.NewValue.GetType()) ? e.NewValue.GetValue<System::Collection::Generic::IContainer*>() : nullptr;

        if (SelectedIndex != -1 && newNotifyContainer)
        {
            SelectedIndex = IndexOf(newNotifyContainer, SelectedItem);
        }
        else if (AlwaysSelected)
        {
            SelectedIndex = 0;
        }
    }
}

void SelectingItemsControl::ItemsCollectionChanged(void *sender, System::Collection::NotifyCollectionChangedEventArgs& e)
{
    ItemsControl::ItemsCollectionChanged(sender, e);

    System::Collection::Generic::IContainer& list = *(System::Collection::Generic::IContainer*)sender;
    switch (e.Action)
    {
        case System::Collection::NotifyCollectionChangedAction::Add:
        {
            if (AlwaysSelected && SelectedIndex == -1)
            {
                SelectedIndex = 0;
            }

            break;
        }
        case System::Collection::NotifyCollectionChangedAction::AfterRemove:
        case System::Collection::NotifyCollectionChangedAction::Replace:
        {
            auto selectedIndex = SelectedIndex;

            if (selectedIndex >= list.size())
            {
                SelectedIndex = Items->size() - 1;
                selectedIndex = SelectedIndex;
            }

            if (selectedIndex >= e.OldStartingIndex && selectedIndex <= e.OldStartingIndex /*+ e.OldItems->Count*/)
            {
                if (!AlwaysSelected)
                {
                    SelectedIndex = -1;
                }
                else
                {
                    LostSelection();
                }
            }

            break;

        }
        case System::Collection::NotifyCollectionChangedAction::Reset:
            SelectedIndex = IndexOf(&list, SelectedItem);
            break;
    }
}

void SelectingItemsControl::OnContainersMaterialized(Interfaces::ItemContainerEventArgs& e)
{
    ItemsControl::OnContainersMaterialized(e);

    bool unselect = false;
    auto selectedIndex = SelectedIndex;
    Interfaces::ItemContainerInfo selectedContainer;

    for (auto itr = e.Container->GetGenericEnumerator(); !itr->IsFinished(); itr->MoveNext())
    {
        Interfaces::ItemContainerInfo info = itr->Current().GetValue<Interfaces::ItemContainerInfo>();
        if (!info.ContainerControl)
            continue;

        if (selectedIndex >= info.Index && selectedIndex < info.Index + e.Container->size())
        {
            unselect = true;
            selectedContainer = info;
            MarkContainerSelected(info.ContainerControl.get(), true);
        }
        else if (!_selectedItem.IsValid() && !_selectedItems.empty())
        {
            Interfaces::UIControls::ISelectable* hcontrol = info.ContainerControl->Dynamic_As<Interfaces::UIControls::ISelectable>();
            if (hcontrol && hcontrol->GetIsSelected())
            {
                unselect = true;
                selectedContainer = info;
            }
            else if (_selectedItems.Contains(info.Item))
            {
                unselect = true;
                selectedContainer = info;
            }
        }

    }

    // ToDo: multi item select
    if (selectedContainer.Index != -1 && _selectedItems.size() <= 1 && Items->size() > selectedContainer.Index)
    {
        SelectedIndex = -1; // Force update
        LostSelection();
    }

    if (selectedContainer.ContainerControl)
    {
        SelectedIndex = selectedContainer.Index;
    }
}

void SelectingItemsControl::OnContainersDematerialized(Interfaces::ItemContainerEventArgs& e)
{
    ItemsControl::OnContainersDematerialized(e);

    auto panel = Presenter->Panel.get();

    auto selectedIndex = SelectedIndex;
    for (auto itr = e.Container->GetGenericEnumerator(); !itr->IsFinished(); itr->MoveNext())
    {
        Interfaces::ItemContainerInfo info = itr->Current().GetValue<Interfaces::ItemContainerInfo>();
        if (!info.ContainerControl)
            continue;

        if (selectedIndex >= info.Index && selectedIndex < info.Index + e.Container->size())
        {
            MarkItemSelected(info.Index, true);
        }
    }

    //for (auto container : *e.Container)
    //{
    //    if (KeyboardNavigation::GetTabOnceActiveElement(panel) == container->ContainerControl)
    //    {
    //        KeyboardNavigation::SetTabOnceActiveElement(panel, nullptr);
    //        break;
    //    }
    //}
}

void SelectingItemsControl::OnContainersRecycled(Interfaces::ItemContainerEventArgs& e)
{
    for (auto itr = e.Container->GetGenericEnumerator(); !itr->IsFinished(); itr->MoveNext())
    {
        Interfaces::ItemContainerInfo containerInfo = itr->Current().GetValue<Interfaces::ItemContainerInfo>();

        if (containerInfo.ContainerControl != nullptr && containerInfo.Item != nullptr)
        {
            MarkContainerSelected(containerInfo.ContainerControl.get(), SelectedItems.Contains(containerInfo.Item));
        }
    }
}

void SelectingItemsControl::OnDataContextChanging()
{
    ItemsControl::OnDataContextChanging();
    ++_updateCount;
}

void SelectingItemsControl::OnDataContextChanged()
{
    ItemsControl::OnDataContextChanged();

    if (--_updateCount == 0)
    {
        UpdateFinished();
    }
}

void SelectingItemsControl::UpdateSelection(int index, bool select, bool rangeModifier, bool toggleModifier)
{
    if (index != -1)
    {
        if (select)
        {
            auto mode = SelectionMode;
            auto toggle = toggleModifier || (uint(mode) & uint(UI::SelectionMode::Toggle)) != 0;
            auto multi = (uint(mode) & uint(UI::SelectionMode::Multiple)) != 0;
            auto range = multi && SelectedIndex != -1 && rangeModifier;

            if (!toggle && !range)
            {
                SelectedIndex = index;
            }
            else if (multi && range)
            {
                SynchronizeItems(&_selectedItems, GetRange(Items.get(), SelectedIndex, index));
            }
            else
            {
                auto item = ElementAt(Items.get(), index);
                auto i = _selectedItems.IndexOf(item);

                if (i != -1 && (!AlwaysSelected || _selectedItems.size() > 1))
                {
                    _selectedItems.Remove(item);
                }
                else
                {
                    if (multi)
                    {
                        _selectedItems.Add(item);
                    }
                    else
                    {
                        SelectedIndex = index;
                    }
                }
            }

            //if (Presenter && Presenter->Panel != nullptr)
            //{
            //    auto container = ItemContainerGenerator::ContainerFromIndex(index);
            //    KeyboardNavigation::SetTabOnceActiveElement(static_cast<InputElement*>(Presenter::Panel), container);
            //}
        }
        else
        {
            LostSelection();
        }
    }
}

void SelectingItemsControl::UpdateSelection(Controls::Control *container, bool select, bool rangeModifier, bool toggleModifier)
{
    if (GetItemContainerGenerator())
    {
        int index = GetItemContainerGenerator()->IndexFromContainer(container);
        if (index == -1)
            index = GetItemContainerGenerator()->IndexFromItem(container);

        if (index != -1)
        {
            UpdateSelection(index, select, rangeModifier, toggleModifier);
        }
    }
}

bool SelectingItemsControl::UpdateSelectionFromEventSource(Controls::InputElement *eventSource, bool select, bool rangeModifier, bool toggleModifier)
{
    auto container = GetContainerFromEventSource(eventSource);

    if (container != nullptr)
    {
        UpdateSelection(container, select, rangeModifier, toggleModifier);
        return true;
    }

    return false;
}

std::vector<System::Reflection::Variant> SelectingItemsControl::GetRange(System::Collection::Generic::IContainer* items, int first, int last)
{
    std::vector<System::Reflection::Variant> ret;

    int step = first > last ? -1 : 1;

    for (int i = first; i != last; i += step)
    {
        ret.push_back(items->ElementAt(i));
    }

    return ret;
}

void SelectingItemsControl::SynchronizeItems(System::Collection::Generic::IContainer* items, std::vector<System::Reflection::Variant> &desired)
{
    int index = 0;

    for (System::Reflection::Variant const& i : desired)
    {
        if (index < items->size())
        {
            System::Reflection::Variant element = items->ElementAt(index);
            if (element != i)
            {
                items->UpdateAt(index, element);
            }
        }
        else
        {
            items->Add(i);
        }

        ++index;
    }

    while (index < items->size())
    {
        items->RemoveAt(items->size() - 1);
    }
}

void SelectingItemsControl::ContainerSelectionChanged(Core::Observer::RoutedEventArgs& e)
{
    if (!_ignoreContainerSelectionChanged)
    {
        auto control = e.Source->Static_As<Controls::Control>();
        auto selectable = e.Source->Dynamic_As<Interfaces::UIControls::ISelectable>();

        if (control && selectable && control->Parent == this && GetItemContainerGenerator() && GetItemContainerGenerator()->IndexFromContainer(control) != -1)
        {
            UpdateSelection(control, selectable->GetIsSelected());
        }
    }

    if (e.Source != this)
        e.Handled = true;
}

void SelectingItemsControl::LostSelection()
{
    if (Items != nullptr && AlwaysSelected)
    {
        auto index = std::min(SelectedIndex, int(Items->size() - 1));

        if (index > -1)
        {
            SelectedItem = Items->ElementAt(index);
            return;
        }
    }

    SelectedIndex = -1;
}

bool SelectingItemsControl::MarkContainerSelected(Controls::Control *container, bool selected)
{
    auto selectable = container->Dynamic_As<Interfaces::UIControls::ISelectable>();
    bool result = false;

    _ignoreContainerSelectionChanged = true;
    System::ScopeGuard __scope([&]()
    {
        _ignoreContainerSelectionChanged = false;
    });

    if (selectable != nullptr)
    {
        result = selectable->GetIsSelected();
        selectable->SetIsSelected(selected);
    }
    //else
    //{
    //    result = container->Classes.find(":selected") != container->Classes.end();
    //    if (selected)
    //        container->PseudoClasses.insert(":selected");
    //    else
    //        container->PseudoClasses.erase(":selected");
    //}

    Core::Observer::RoutedEventArgs e(IsSelectedChangedEvent.get());
    RaiseEvent(e);

    return result;
}

void SelectingItemsControl::MarkItemSelected(int index, bool selected)
{
    auto container = GetItemContainerGenerator() ? GetItemContainerGenerator()->ContainerFromIndex(index) : nullptr;
    
    if (container != nullptr)
    {
        MarkContainerSelected(container, selected);
    }
}

void SelectingItemsControl::MarkItemSelected(System::Reflection::Variant const& item, bool selected)
{
    auto index = IndexOf(Items.get(), item);

    if (index != -1)
    {
        MarkItemSelected(index, selected);
    }
}

void SelectingItemsControl::SelectedItemsCollectionChanged(void *sender, System::Collection::NotifyCollectionChangedEventArgs& e)
{
    auto generator = GetItemContainerGenerator();
    Interfaces::SelectionChangedEventArgs eventArg(SelectionChangedEvent.get());
    std::vector<System::Reflection::Variant>& added = eventArg.AddedItems;
    std::vector<System::Reflection::Variant>& removed = eventArg.RemovedItems;

    System::Collection::Generic::IContainer& list = *(System::Collection::Generic::IContainer*)sender;

    switch (e.Action)
    {
        case System::Collection::NotifyCollectionChangedAction::Add:
        {
            System::Reflection::Variant item = list.ElementAt(e.NewStartingIndex);
            added.push_back(item);
            SelectedItemsAdded(added);

            if (AutoScrollToSelectedItem)
            {
                ScrollIntoView(added[0]);
            }

            break;
        }
        case System::Collection::NotifyCollectionChangedAction::Remove:
            removed.push_back(list.ElementAt(e.OldStartingIndex));

            if (SelectedItems.size() == 0)
            {
                if (!_syncingSelectedItems)
                {
                    SelectedIndex = -1;
                }
            }
            else
            {
                for (auto item : removed)
                {
                    MarkItemSelected(item, false);
                }
            }

            break;

        case System::Collection::NotifyCollectionChangedAction::Reset:
            //if (generator != nullptr) // implement selectedControls pattern DataGrid::selectedCells
            //{
            //    for (auto item : generator->Containers)
            //    {
            //        if (item.second.ContainerControl != nullptr)
            //        {
            //            if (MarkContainerSelected(item.second.ContainerControl.get(), false))
            //            {
            //                removed.push_back(item.second.Item);
            //            }
            //        }
            //    }
            //}

            for (auto itr = list.GetGenericEnumerator(); !itr->IsFinished(); ++*itr)
            {
                int index = generator->IndexFromItem(itr->Current());
                if (index == -1)
                    continue;

                auto control = generator->ContainerFromIndex(index);
                if (control && MarkContainerSelected(control, false))
                {
                    removed.push_back(itr->Current());
                }
            }

            /*if (SelectedItems.size() > 0)
            {
                _selectedItem = nullptr;
                for (auto const& i : SelectedItems.GetNativeEnumerator())
                    added.push_back(i);
                SelectedItemsAdded(added);
            }
            else*/ if (!_syncingSelectedItems && !AlwaysSelected)
            {
                SelectedIndex = -1;
            }

            break;

        case System::Collection::NotifyCollectionChangedAction::Replace:
            MarkItemSelected(list.ElementAt(e.OldStartingIndex), false);
            MarkItemSelected(list.ElementAt(e.NewStartingIndex), true);

            if (SelectedItem != SelectedItems[0] && !_syncingSelectedItems)
            {
                auto oldItem = SelectedItem;
                auto oldIndex = SelectedIndex;
                auto item = SelectedItems[0];
                auto index = IndexOf(Items.get(), item);
                _selectedIndex = index;
                _selectedItem = item;
                RaisePropertyChanged(SelectedIndexProperty.get(), oldIndex, index, Core::Dependency::BindingLevel::LocalValue);
                RaisePropertyChanged(SelectedItemProperty.get(), oldItem, item, Core::Dependency::BindingLevel::LocalValue);
            }

            added.push_back(list.ElementAt(e.NewStartingIndex));
            removed.push_back(list.ElementAt(e.OldStartingIndex));
            break;
    }

    if (!added.empty() || !removed.empty())
        RaiseEvent(eventArg);
}

void SelectingItemsControl::SelectedItemsAdded(std::vector<System::Reflection::Variant> const& items)
{
    if (items.size() > 0)
    {
        for (auto const& i : items)
            MarkItemSelected(i, true);

        if (SelectedItem == nullptr && !_syncingSelectedItems)
        {
            auto index = IndexOf(Items.get(), items[0]);

            if (index != -1)
            {
                _selectedItem = items[0];
                _selectedIndex = index;
                RaisePropertyChanged(SelectedIndexProperty.get(), -1, index, Core::Dependency::BindingLevel::LocalValue);
                RaisePropertyChanged(SelectedItemProperty.get(), nullptr, items[0], Core::Dependency::BindingLevel::LocalValue);
            }
        }
    }
}

void SelectingItemsControl::SubscribeToSelectedItems()
{
    System::Collection::NotifyCollectionChangedEventArgs e;
    e.Action = System::Collection::NotifyCollectionChangedAction::Reset;
    SelectedItemsCollectionChanged(&_selectedItems, e);
}

void SelectingItemsControl::UnsubscribeFromSelectedItems()
{
}

void SelectingItemsControl::UpdateFinished()
{
    if (_updateSelectedIndex != std::numeric_limits<int>::min())
    {
        SelectedIndex = _updateSelectedIndex;
    }
    else
    {
        SelectedItems.Clear();
        for (auto const& item : _updateSelectedItems)
            SelectedItems.Add(item);
    }
}
