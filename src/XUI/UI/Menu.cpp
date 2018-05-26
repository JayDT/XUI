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

#include "Menu.h"
#include "UI/MenuItem.h"
#include "UI/Window.h"
#include "UI/StackPanel.h"
#include "UI/Generators/MenuItemContainerGenerator.h"
#include "UI/Presenters/ItemsPresenter.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;
using namespace XUI::Interfaces;
using namespace XUI::UI;

STATIC_CLASS_POST_INITIALIZER(Menu, Menu::StaticClassInitializer)

XUI::Core::Dependency::RefDependencyProperty Menu::IsOpenProperty;

std::shared_ptr<XamlCPP::Interfaces::ITemplate> Menu::DefaultPanel = std::make_shared<DataTemplates::FuncTemplate<std::shared_ptr<XUI::UI::Panel>>>([]()
{
    auto panel = std::make_shared<UI::StackPanel>();
    panel->Orientation = Orientation::Horizontal;
    return panel;
});

void Menu::StaticClassInitializer()
{
    IsOpenProperty = (Core::Dependency::DirectProperty<Menu, bool>::RegisterDirect(nameof(IsOpen),
                      [](Menu* x) { return x->IsOpen; },
                      [](Menu* x, bool v) { x->IsOpen = v; })
                      );

    ItemsPanelProperty->OverrideDefaultValue<Menu>(DefaultPanel);
    MenuItem::ClickEvent->AddClassSpecHandler<Core::Observer::RoutedEventArgs, Menu>(System::make_eventhandler([](void* x, Core::Observer::RoutedEventArgs& e)
    {
        static_cast<Menu*>((System::Reflection::Object*)x)->OnMenuClick(e);
    }), XUI::Core::Observer::RoutingStrategies(XUI::Core::Observer::RoutingStrategies::Direct | XUI::Core::Observer::RoutingStrategies::Bubble), true);

    MenuItem::SubmenuOpenedEvent->AddClassSpecHandler<Core::Observer::RoutedEventArgs, Menu>(System::make_eventhandler([](void* x, Core::Observer::RoutedEventArgs& e)
    {
        static_cast<Menu*>((System::Reflection::Object*)x)->OnSubmenuOpened(e);
    }));


    GotFocusEvent->AddClassSpecHandler<GotFocusEventArgs, Menu>(System::make_eventhandler([](void* x, GotFocusEventArgs& e)
    {
        if (e.Handled)
            static_cast<Menu*>((System::Reflection::Object*)x)->OnGotFocus(e);
    }), Core::Observer::RoutingStrategies(Core::Observer::RoutingStrategies::Direct | Core::Observer::RoutingStrategies::Bubble), true);

    LostFocusEvent->AddClassSpecHandler<GotFocusEventArgs, Menu>(System::make_eventhandler([](void* x, GotFocusEventArgs& e)
    {
        if (e.Handled)
            static_cast<Menu*>((System::Reflection::Object*)x)->OnLostFocus(e);
    }), Core::Observer::RoutingStrategies(Core::Observer::RoutingStrategies::Direct | Core::Observer::RoutingStrategies::Bubble), true);
}

bool Menu::GetIsOpen() const
{
    return _isOpen;
}

void Menu::SetIsOpen(const bool &value)
{
    SetAndRaise(IsOpenProperty, _isOpen, value);
}

MenuItem *Menu::GetSelectedMenuItem() const
{
    int index = SelectedIndex;
    return (index != -1) ? const_cast<Menu*>(this)->GetItemContainerGenerator()->ContainerFromIndex(index)->Static_As<MenuItem>() : nullptr;
}

void Menu::Close()
{
    for (auto it = Items->GetGenericEnumerator(); !it->IsFinished(); ++*it)
    {
        if (!it->Current().IsObject())
            continue;

        MenuItem* item = it->Current().ToObject()->Static_As<MenuItem>();
        if (item)
            item->IsSubMenuOpen = false;
    }

    IsOpen = false;
    SelectedIndex = -1;
}

void Menu::Open()
{
    SelectedIndex = 0;
    SelectedMenuItem->Focus();
    IsOpen = true;
}

void Menu::OnAttachedToVisualTree(Controls::VisualTreeAttachmentEventArgs const& e)
{
    SelectingItemsControl::OnAttachedToVisualTree(e);

    XUI::UI::TopLevel* topLevel = e.Root->ToVisual()->Static_As<TopLevel>();
    //XUI::UI::Window* window = e.Root->ToVisual()->Static_As<Window>();

    if (topLevel)
        topLevel->Deactivated += System::make_eventhandler(&Menu::Deactivated, this);

    rxcpp::subscription pointerPress = topLevel->AddHandler(PointerPressedEvent.get(), System::make_eventhandler(&Menu::TopLevelPreviewPointerPress, this), Core::Observer::RoutingStrategies::Tunnel);
    
    rxcpp::composite_subscription subscriptions;
    subscriptions.add(pointerPress);

    std::weak_ptr<TopLevel> weakWindow = topLevel->weak_from_base_static<TopLevel>();
    rxcpp::subscription deactivated = rxcpp::make_subscription([this, weakWindow]()
    {
        if (!weakWindow.expired())
            weakWindow.lock()->Deactivated -= System::make_eventhandler(&Menu::Deactivated, this);
    });

    subscriptions.add(deactivated);

    subscriptions.add(XUI::Platform::UIService::Instance()->GetCache<XUI::Interfaces::IInputMgr>()->Process.get_observable().subscribe([this](Interfaces::RawInputEventArgs & e)
    {
        ListenForNonClientClick(e);
    }));

    _subscription = subscriptions;

    //auto inputRoot = dynamic_cast<IInputRoot*>(e->Root);
    //
    //if (inputRoot == nullptr ? nullptr : inputRoot->AccessKeyHandler != nullptr)
    //{
    //    inputRoot->AccessKeyHandler->MainMenu = this;
    //}
}

void Menu::OnDetachedFromVisualTree(Controls::VisualTreeAttachmentEventArgs const& e)
{
    SelectingItemsControl::OnDetachedFromVisualTree(e);
    if (_subscription.is_subscribed())
        _subscription.unsubscribe();
}

std::shared_ptr<Generators::IItemContainerGenerator> Menu::CreateItemContainerGenerator()
{
    return std::make_shared<Generators::ItemContainerGenerator<MenuItem>>(this, MenuItem::HeaderProperty.get(), nullptr);
}

void Menu::OnKeyDown(Interfaces::KeyEventArgs& e)
{
    bool menuWasOpen = SelectedMenuItem ? SelectedMenuItem->IsSubMenuOpen : false;

    SelectingItemsControl::OnKeyDown(e);

    if (menuWasOpen)
    {
        // If a menu item was open and we navigate to a new one with the arrow keys, open
        // that menu and select the first item.
        UI::MenuItem* selection = SelectedMenuItem;

        if (selection != nullptr && !selection->IsSubMenuOpen)
        {
            selection->IsSubMenuOpen = true;
            selection->SelectedIndex = 0;
        }
    }
}

void Menu::OnLostFocus(Core::Observer::RoutedEventArgs& e)
{
    SelectingItemsControl::OnLostFocus(e);
    SelectedItem = nullptr;
    _stayOpen = false;
}

void Menu::OnSubmenuOpened(Core::Observer::RoutedEventArgs& e)
{
    auto menuItem = e.Source->Dynamic_As<MenuItem>();

    if (menuItem != nullptr && menuItem->Parent == this)
    {
        for (auto child : GetLogicalChildren().GetNativeEnumerator())
        {
            MenuItem* item = child->Static_As<MenuItem>();

            if (item && item != menuItem && item->IsSubMenuOpen)
                item->IsSubMenuOpen = false;
        }
    }

    IsOpen = true;
}

void XUI::UI::Menu::OnGotFocus(Interfaces::GotFocusEventArgs & e)
{
    _stayOpen = true;
    //if (VisualRoot->ToVisual() && VisualRoot->ToVisual()->Is<TopLevel>() && static_cast<TopLevel*>(VisualRoot->ToVisual())->GetWindowImpl())
    //{
    //    static_cast<TopLevel*>(VisualRoot->ToVisual())->GetWindowImpl()->Capture();
    //}
}

void Menu::Deactivated(void* sender, System::Events::EventArg& e)
{
    auto mouseDevice = XUI::Platform::UIService::Instance()->Get<Interfaces::IMouseDevice>();
    Interfaces::IRenderRoot* root = VisualRoot;
    
    if (root && !root->HitTestVisualTree(mouseDevice->GetRelativePosition(root->ToVisual()), root->ToVisual(), nullptr))
        _stayOpen = false;

    if (!_stayOpen)
        Close();
}

void Menu::ListenForNonClientClick(Interfaces::RawInputEventArgs& e)
{
    auto mouse = dynamic_cast<RawMouseEventArgs*>(&e);

    if (mouse)
    {
        if (mouse->Type == RawMouseEventType::NonClientLeftButtonDown)
        {
            //auto mouseDevice = XUI::Platform::UIService::Instance()->Get<Interfaces::IMouseDevice>();
            //Interfaces::IRenderRoot* root = mouse->Root->ToVisualBase()->Dynamic_As<Interfaces::IRenderRoot>();
            //
            //if (root && !root->HitTestVisualTree(mouseDevice->GetRelativePosition(mouse->Root->ToVisualBase()), mouse->Root->ToVisualBase(), nullptr))
            //    _stayOpen = false;

            if (!_stayOpen)
                Close();
        }
    }
}

void Menu::OnMenuClick(Core::Observer::RoutedEventArgs& e)
{
    Close();
    //FocusManager::Instance->Focus(nullptr);
    e.Handled = true;
}

void Menu::TopLevelPreviewPointerPress(void* sender, Interfaces::PointerPressedEventArgs& e)
{
    if (IsOpen)
    {
        auto control = e.Source->Static_As<Controls::Control>();

        std::list<Control*> controls;
        control->GetLogicalAncestors(controls, [&](Controls::Control* c) -> bool
        {
            return this == c;
        });

        if (controls.empty())
        {
            Close();
        }
    }
}

void Menu::ItemsCollectionChanged(void *sender, System::Collection::NotifyCollectionChangedEventArgs& e)
{
    SelectingItemsControl::ItemsCollectionChanged(sender, e);

    for (auto enumerator = Items->GetGenericEnumerator(); !enumerator->IsFinished(); ++*enumerator)
    {
        auto current = enumerator->Current();
        if (current.GetType()->IsAssignableFrom<MenuItem>())
        {
            auto sibling = current.TryGetValue<std::shared_ptr<MenuItem>>();
            if (sibling->Role != MenuItemRole::TopLevelItem)
                sibling->Role = MenuItemRole::TopLevelItem;
        }
    }
}