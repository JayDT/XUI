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

#include "ItemsControl.h"
#include "UI/StackPanel.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;
using namespace XUI::UI;
using namespace XUI::UI::Controls;

STATIC_CLASS_INITIALIZER(ItemsControl, XUI::UI::Controls::ItemsControl::StaticClassInitializer)

XUI::Core::Dependency::RefDependencyProperty ItemsControl::ItemsProperty;
XUI::Core::Dependency::RefDependencyProperty ItemsControl::ItemsPanelProperty;
XUI::Core::Dependency::RefDependencyProperty ItemsControl::ItemTemplateProperty;
XUI::Core::Dependency::RefDependencyProperty ItemsControl::MemberSelectorProperty;
XUI::Core::Dependency::RefDependencyProperty ItemsControl::AlternationCountProperty;
XUI::Core::Dependency::RefDependencyProperty ItemsControl::AlternationIndexProperty;

XUI::UI::DataTemplates::FuncTemplate<std::shared_ptr<XUI::UI::Panel>>* ItemsControl::__0; // this hack removed after implemented correct template handle in reflection

void ItemsControl::StaticClassInitializer()
{
    ItemsProperty = (Core::Dependency::DirectProperty<ItemsControl, std::shared_ptr<System::Collection::Generic::IContainer>>::RegisterDirect(nameof(Items),
        [](ItemsControl* x) { return x->Items; },
        [](ItemsControl* x, std::shared_ptr<System::Collection::Generic::IContainer> const& v) { x->Items = v; })
        );
    ItemsPanelProperty = (Core::Dependency::BindProperty<std::shared_ptr<XamlCPP::Interfaces::ITemplate>>::Register<ItemsControl>(nameof(ItemsPanel), std::make_shared<DataTemplates::FuncTemplate<std::shared_ptr<XUI::UI::Panel>>>([]()
    {
        auto panel = std::make_shared<StackPanel>();
        return panel;
    })));
    ItemTemplateProperty = (Core::Dependency::BindProperty<std::shared_ptr<UI::DataTemplates::DataTemplate>>::Register<ItemsControl>(nameof(ItemTemplate)));
    MemberSelectorProperty = (Core::Dependency::BindProperty<System::Delegate<System::Reflection::Variant(System::Reflection::Variant const&)>>::Register<ItemsControl>(nameof(MemberSelector)));
    AlternationCountProperty = (Core::Dependency::BindProperty<int>::Register<ItemsControl>(nameof(AlternationCount)));
    AlternationIndexProperty = (Core::Dependency::BindProperty<int>::Register<ItemsControl>(nameof(AlternationIndex)));

    ItemTemplateProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const & e)
    {
        ItemsControl* control = e.Sender->Dynamic_As<ItemsControl>();
        if (control)
            control->ItemTemplateChanged((XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs&)e);
    });

    ItemsProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const & e)
    {
        ItemsControl* control = e.Sender->Dynamic_As<ItemsControl>();
        if (control)
            control->ItemsChanged((XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs&)e);
    });
}

ItemsControl::ItemsControl()
{
    Items = std::make_shared<System::Collection::List<System::Reflection::RefObject>>();

    //PseudoClasses.insert(":empty");
    SubscribeToItems(_items.get());
}

std::shared_ptr<System::Collection::Generic::IContainer> const& ItemsControl::GetItems() const
{
    return _items;
}

void ItemsControl::SetItems(std::shared_ptr<System::Collection::Generic::IContainer> const& value)
{
    SetAndRaise(ItemsProperty, _items, value);
}

int ItemsControl::GetItemCount()
{
    return m_ItemCount;
}

void ItemsControl::SetItemCount(int value)
{
    m_ItemCount = value;
}

std::shared_ptr<XamlCPP::Interfaces::ITemplate> ItemsControl::GetItemsPanel()
{
    return GetValue<std::shared_ptr<XamlCPP::Interfaces::ITemplate>>(ItemsPanelProperty);
}

void ItemsControl::SetItemsPanel(std::shared_ptr<XamlCPP::Interfaces::ITemplate> const& value)
{
    SetValue(ItemsPanelProperty, value);
}

System::Delegate<System::Reflection::Variant(System::Reflection::Variant const&)> ItemsControl::GetMemberSelector() const
{
    return GetValue<System::Delegate<System::Reflection::Variant(System::Reflection::Variant const&)>>(MemberSelectorProperty);
}

void ItemsControl::SetMemberSelector(System::Delegate<System::Reflection::Variant(System::Reflection::Variant const&)> const& value)
{
    SetValue(MemberSelectorProperty, value);
}

std::shared_ptr<XUI::UI::DataTemplates::DataTemplate> ItemsControl::GetItemTemplate()
{
    return GetValue<std::shared_ptr<UI::DataTemplates::DataTemplate>>(ItemTemplateProperty);
}

void ItemsControl::SetItemTemplate(std::shared_ptr<XUI::UI::DataTemplates::DataTemplate> const& value)
{
    SetValue(ItemTemplateProperty, value);
}

void ItemsControl::RegisterItemsPresenter(Presenters::ItemsPresenterBase *presenter)
{
    Presenter = presenter ? presenter->shared_from_base_static<Presenters::ItemsPresenterBase>() : nullptr;
}

std::shared_ptr<Generators::IItemContainerGenerator> XUI::UI::Controls::ItemsControl::GetItemContainerGenerator()
{
    if (!_itemContainerGenerator)
    {
        _itemContainerGenerator = CreateItemContainerGenerator();

        if (_itemContainerGenerator)
        {
            _itemContainerGenerator->ItemTemplate = ItemTemplate;
            _itemContainerGenerator->Materialized += [&](void* sender, Interfaces::ItemContainerEventArgs& e) { OnContainersMaterialized(e); };
            _itemContainerGenerator->Dematerialized += [&](void* sender, Interfaces::ItemContainerEventArgs& e) { OnContainersDematerialized(e); };
            _itemContainerGenerator->Recycled += [&](void* sender, Interfaces::ItemContainerEventArgs& e) { OnContainersRecycled(e); };
        }
    }

    return _itemContainerGenerator;
}


/// <summary>
/// Creates the <see cref="ItemContainerGenerator"/> for the control.
/// </summary>
/// <returns>
/// An <see cref="IItemContainerGenerator"/> or null.
/// </returns>
/// <remarks>
/// Certain controls such as <see cref="TabControl"/> don't actually create item 
/// containers; however they want it to be ItemsControls so that they have an Items 
/// property etc. In this case, a derived class can override this method to return null
/// in order to disable the creation of item containers.
/// </remarks>

std::shared_ptr<Generators::IItemContainerGenerator> XUI::UI::Controls::ItemsControl::CreateItemContainerGenerator()
{
    return std::make_shared<Generators::ItemContainerGeneratorGeneric>(this);
}

System::Reflection::Variant ItemsControl::ElementAt(System::Collection::Generic::IContainer *items, int index)
{
    if (index != -1 && index < items->size())
        return items->ElementAt(index);

    return System::Reflection::Variant::Empty;
}

int ItemsControl::IndexOf(System::Collection::Generic::IContainer *items, System::Reflection::Variant const& item)
{
    if (items != nullptr && item)
        return int(items->IndexOf(item));

    return -1;
}

void ItemsControl::OnContainersMaterialized(XUI::Interfaces::ItemContainerEventArgs& e)
{
    for (auto itr = e.Container->GetGenericEnumerator(); !itr->IsFinished(); itr->MoveNext())
    {
        Interfaces::ItemContainerInfo containerInfo = itr->Current().GetValue<Interfaces::ItemContainerInfo>();
        if (containerInfo.ContainerControl != nullptr && static_cast<System::Reflection::Object*>(containerInfo.ContainerControl.get()) != containerInfo.Item.ToObject())
        {
            Presenters::ContentPresenter* presenter = containerInfo.ContainerControl->Static_As<Presenters::ContentPresenter>();
            if (presenter)
            {
                presenter->SetParent(this);
                //presenter->SetTemplatedParent(nullptr);
    
                presenter->UpdateChild();
    
                if (presenter->Child && presenter->Child->Parent != this)
                    LogicalChildren.Insert(0, presenter->Child); //AddLogicalChild(presenter->Child.get());
            }
            else if (containerInfo.ContainerControl->Parent != this)
            {
                LogicalChildren.Insert(0, containerInfo.ContainerControl); //AddLogicalChild(containerInfo.ContainerControl.get());
            }
        }
    }
}

void ItemsControl::OnContainersDematerialized(XUI::Interfaces::ItemContainerEventArgs& e)
{
    for (auto itr = e.Container->GetGenericEnumerator(); !itr->IsFinished(); itr->MoveNext())
    {
        Interfaces::ItemContainerInfo containerInfo = itr->Current().GetValue<Interfaces::ItemContainerInfo>();
        if (containerInfo.ContainerControl != nullptr && static_cast<System::Reflection::Object*>(containerInfo.ContainerControl.get()) != containerInfo.Item.ToObject())
        {
            Presenters::ContentPresenter* presenter = containerInfo.ContainerControl->Static_As<Presenters::ContentPresenter>();
            if (presenter)
            {
                if (presenter->Child && presenter->Child->Parent == this)
                    RemoveLogicalChild(presenter->Child.get());
                else
                    continue; // debug
            }
            else if (containerInfo.ContainerControl->Parent == this)
            {
                RemoveLogicalChild(containerInfo.ContainerControl.get());
            }
            else
            {
                continue; // debug
            }
        }
    }
}

void ItemsControl::OnContainersRecycled(XUI::Interfaces::ItemContainerEventArgs& e)
{
    for (auto itr = e.Container->GetGenericEnumerator(); !itr->IsFinished(); itr->MoveNext())
    {
        Interfaces::ItemContainerInfo containerInfo = itr->Current().GetValue<Interfaces::ItemContainerInfo>();

        // If the item is its own container, then it will be removed from the logical tree
        // when it is removed from the Items collection.
        if (containerInfo.ContainerControl != nullptr && static_cast<System::Reflection::Object*>(containerInfo.ContainerControl.get()) != containerInfo.Item.ToObject())
        {
            if (containerInfo.ContainerControl->Parent == this)
                RemoveLogicalChild(containerInfo.ContainerControl.get());
        }
    }
}

void ItemsControl::ItemsChanged(Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
    static const System::Reflection::Type* containerType = &typeof(System::Collection::Generic::IContainer);

    System::Collection::Generic::IContainer* oldNotifyContainer = containerType->IsAssignableFrom(*e.OldValue.GetType()) ? e.OldValue.GetValue<System::Collection::Generic::IContainer*>() : nullptr;

    if (oldNotifyContainer != nullptr)
        oldNotifyContainer->CollectionChanged -= System::make_eventhandler(&ItemsControl::ItemsCollectionChanged, this);

    auto newValue = containerType->IsAssignableFrom(*e.OldValue.GetType()) ? e.NewValue.GetValue<System::Collection::Generic::IContainer*>() : nullptr;

    RemoveControlItemsFromLogicalChildren(oldNotifyContainer);
    AddControlItemsToLogicalChildren(newValue);
    SubscribeToItems(newValue);
}

void ItemsControl::ItemsCollectionChanged(void *sender, System::Collection::NotifyCollectionChangedEventArgs& e)
{
    static const System::Reflection::Type* containerType = &typeof(System::Collection::Generic::IContainer);

    System::Collection::Generic::IContainer& list = *(System::Collection::Generic::IContainer*)sender;
    switch (e.Action)
    {
        case System::Collection::NotifyCollectionChangedAction::Add:
        {
            auto element = e.NewValue; //list.ElementAt(e.NewStartingIndex);
            auto newValue = containerType->IsAssignableFrom(*element.GetType()) ? element.GetValue<System::Collection::Generic::IContainer*>() : nullptr;
            if (newValue)
                AddControlItemsToLogicalChildren(newValue);
            else if (element.GetBase() && element.GetBase()->IsSharedPointer() && element.GetType()->IsAssignableFrom<Controls::Control>())
            {
                Controls::Control* control = element.ToSharedObject()->Static_As<Controls::Control>();
                if (control != nullptr)
                {
                    if (Presenter)
                    {
                        if (control->Parent != Presenter.get())
                            Presenter->LogicalChildren.Insert(0, control->shared_from_base_static<Controls::Control>()); //AddLogicalChild(control);
                    }
                    else if (control->Parent != this)
                        LogicalChildren.Insert(0, control->shared_from_base_static<Controls::Control>()); //AddLogicalChild(control);
                }
            }
            break;
        }
        case System::Collection::NotifyCollectionChangedAction::Remove:
        {
            auto element = list.ElementAt(e.OldStartingIndex);
            auto oldValue = containerType->IsAssignableFrom(*element.GetType()) ? element.GetValue<System::Collection::Generic::IContainer*>() : nullptr;
            if (oldValue)
                RemoveControlItemsFromLogicalChildren(oldValue);
            else if (element.GetBase() && element.GetBase()->IsSharedPointer() && element.GetType()->IsAssignableFrom<Controls::Control>())
            {
                Controls::Control* control = element.ToSharedObject()->Static_As<Controls::Control>();
                if (control != nullptr)
                {
                    if (Presenter && control->Parent == Presenter.get())
                        Presenter->RemoveLogicalChild(control);
                    else if (control->Parent == this)
                        RemoveLogicalChild(control);
                }
            }
            break;
        }
    }

    
    int count = Items ? Items->size() : 0;
    if (Items)
        ItemCount = count;

    //if (list.empty())
    //    PseudoClasses.insert(":empty");
    //else
    //    PseudoClasses.erase(":empty");
}

void ItemsControl::AddControlItemsToLogicalChildren(System::Collection::Generic::IContainer *items)
{
    static const System::Reflection::Type* controlType = &typeof(UI::Controls::Control);

    if (items != nullptr)
    {
        for (auto enumerator = items->GetGenericEnumerator(); !enumerator->IsFinished(); ++(*enumerator))
        {
            auto control = enumerator->Current().TryGetValue<std::shared_ptr<UI::Controls::Control>>();
            if (control != nullptr && !LogicalChildren.Contains(enumerator->Current()))
                AddLogicalChild(control.get());
        }
    }
}

void ItemsControl::RemoveControlItemsFromLogicalChildren(System::Collection::Generic::IContainer *items)
{
    static const System::Reflection::Type* controlType = &typeof(UI::Controls::Control);

    if (items != nullptr)
    {
        for (auto enumerator = items->GetGenericEnumerator(); !enumerator->IsFinished(); ++(*enumerator))
        {
            auto control = enumerator->Current().TryGetValue<std::shared_ptr<UI::Controls::Control>>();
            if (control != nullptr && !LogicalChildren.Contains(enumerator->Current()))
                RemoveLogicalChild(control.get());
        }
    }
}

void ItemsControl::SubscribeToItems(System::Collection::Generic::IContainer *items)
{
    //if (!items || items->empty())
    //    PseudoClasses.insert(":empty");
    //else
    //    PseudoClasses.erase(":empty");

    if (items)
        items->CollectionChanged += System::make_eventhandler(&ItemsControl::ItemsCollectionChanged, this);
}

void ItemsControl::ItemTemplateChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
}
