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

#include "MenuItem.h"
#include "Menu.h"
#include "UI/StackPanel.h"
#include "Generators/MenuItemContainerGenerator.h"
#include "Core/Input/KeyboardDevice.h"
#include "Core/DispatcherTimer.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;
using namespace XUI::Interfaces;
using namespace XUI::UI;

STATIC_CLASS_INITIALIZER(MenuItem, MenuItem::StaticClassInitializer)

XUI::Core::Dependency::RefDependencyProperty MenuItem::CommandProperty;
XUI::Core::Dependency::RefDependencyProperty MenuItem::HotKeyProperty;
XUI::Core::Dependency::RefDependencyProperty MenuItem::CommandParameterProperty;
XUI::Core::Dependency::RefDependencyProperty MenuItem::IconProperty;
XUI::Core::Dependency::RefDependencyProperty MenuItem::IsSelectedProperty;
XUI::Core::Dependency::RefDependencyProperty MenuItem::IsSubMenuOpenProperty;
XUI::Core::Dependency::RefDependencyProperty MenuItem::CheckedProperty;
XUI::Core::Dependency::RefDependencyProperty MenuItem::IsCheckableProperty;
XUI::Core::Dependency::RefDependencyProperty MenuItem::RoleProperty;
std::shared_ptr<Core::Observer::RoutedEvent> MenuItem::ClickEvent;
std::shared_ptr<Core::Observer::RoutedEvent> MenuItem::SubmenuOpenedEvent;

std::shared_ptr<XamlCPP::Interfaces::ITemplate> MenuItem::DefaultPanel = std::make_shared<DataTemplates::FuncTemplate<std::shared_ptr<XUI::UI::Panel>>>([]()
{
    auto panel = std::make_shared<UI::StackPanel>();
    Core::Input::KeyboardDevice::SetDirectionalNavigation(panel.get(), Core::Input::KeyboardNavigationMode::Cycle);
    return panel;
});

void MenuItem::StaticClassInitializer()
{
    CommandProperty = (Core::Dependency::BindProperty< std::shared_ptr<Interfaces::ICommand> >::Register<MenuItem>(nameof(Command)));
    HotKeyProperty = (Core::Dependency::BindProperty< std::shared_ptr<Core::Input::KeyGesture> >::Register<MenuItem>(nameof(HotKey)));
    CommandParameterProperty = (Core::Dependency::BindProperty< System::Reflection::Variant >::Register<MenuItem>(nameof(CommandParameter)));
    IconProperty = (Core::Dependency::BindProperty< System::Reflection::RefObject >::Register<MenuItem>(nameof(Icon)));
    IsSelectedProperty = (Core::Dependency::BindProperty< bool >::Register<MenuItem>(nameof(IsSelected)));
    IsSubMenuOpenProperty = (Core::Dependency::BindProperty< bool >::Register<MenuItem>(nameof(IsSubMenuOpen)));
    CheckedProperty = (Core::Dependency::BindProperty< bool >::Register<MenuItem>(nameof(Checked)));
    IsCheckableProperty = (Core::Dependency::BindProperty< bool >::Register<MenuItem>(nameof(IsCheckable)));
    RoleProperty = (Core::Dependency::BindProperty< MenuItemRole >::Register<MenuItem>(nameof(Role), MenuItemRole::SubmenuItem));

    ClickEvent = (Core::Observer::RoutedEvent::Register<XUI::UI::MenuItem, Core::Observer::RoutedEventArgs>(nameof(Click), Core::Observer::RoutingStrategies(Core::Observer::RoutingStrategies::Bubble)));
    SubmenuOpenedEvent = (Core::Observer::RoutedEvent::Register<XUI::UI::MenuItem, Core::Observer::RoutedEventArgs>(nameof(SubmenuOpened), Core::Observer::RoutingStrategies(Core::Observer::RoutingStrategies::Bubble)));

    //SelectableMixin::Attach<MenuItem*>(IsSelectedProperty);
    ItemsPanelProperty->OverrideDefaultValue<MenuItem>(DefaultPanel);
    FocusableProperty->OverrideDefaultValue<MenuItem>(true);
    IconProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const & e)
    {
        MenuItem* control = e.Sender->Static_As<MenuItem>();
        if (control)
            control->IconChanged((XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs&)e);
    });
    IsSubMenuOpenProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const & e)
    {
        MenuItem* control = e.Sender->Static_As<MenuItem>();
        if (control)
            control->SubMenuOpenChanged((XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs&)e);
    });

    ClickEvent->AddClassSpecHandler<Core::Observer::RoutedEventArgs, MenuItem>(System::make_eventhandler([](void* x, Core::Observer::RoutedEventArgs& e)
    {
        MenuItem* item = e.Sender->Static_As<MenuItem>();
        if (item)
            item->OnClick(e);
    }));

    SubmenuOpenedEvent->AddClassSpecHandler<Core::Observer::RoutedEventArgs, MenuItem>(System::make_eventhandler([](void* x, Core::Observer::RoutedEventArgs& e)
    {
        MenuItem* item = e.Sender->Static_As<MenuItem>();
        if (item)
            item->OnSubmenuOpened(e);
    }));

    //AccessKeyHandler::AccessKeyPressedEvent->AddClassSpecHandler<Core::Observer::RoutedEventArgs, Menu>(System::make_eventhandler([](void* x, Core::Observer::RoutedEventArgs& e)
    //{
    //    static_cast<MenuItem*>((System::Reflection::Object*)x)->AccessKeyPressed(e);
    //}));
}

MenuItem::MenuItem()
    : Click(this)
    , SubmenuOpened(this)
{

}

std::shared_ptr<Interfaces::ICommand> MenuItem::GetCommand() const
{
    return GetValue<std::shared_ptr<Interfaces::ICommand>>(CommandProperty);
}

void MenuItem::SetCommand(std::shared_ptr<Interfaces::ICommand> const& value)
{
    SetValue(CommandProperty, value);
}

std::shared_ptr<Core::Input::KeyGesture> MenuItem::GetHotKey() const
{
    return GetValue<std::shared_ptr<Core::Input::KeyGesture>>(HotKeyProperty);
}

void MenuItem::SetHotKey(std::shared_ptr<Core::Input::KeyGesture> const& value)
{
    SetValue(HotKeyProperty, value);
}

System::Reflection::Variant MenuItem::GetCommandParameter() const
{
    return GetValue(CommandParameterProperty.get());
}

void MenuItem::SetCommandParameter(System::Reflection::Variant value)
{
    SetValue(CommandParameterProperty.get(), value);
}

System::Reflection::RefObject MenuItem::GetIcon() const
{
    return GetValue<System::Reflection::RefObject>(IconProperty);
}

void MenuItem::SetIcon(System::Reflection::RefObject const& value)
{
    SetValue(IconProperty, value);
}

bool MenuItem::GetIsSelected() const
{
    return GetValue<bool>(IsSelectedProperty);
}

void MenuItem::SetIsSelected(const bool value)
{
    SetValue(IsSelectedProperty, value);
}

bool XUI::UI::MenuItem::GetChecked() const
{
    return GetValue<bool>(CheckedProperty);
}

void XUI::UI::MenuItem::SetChecked(const bool & value)
{
    SetValue(CheckedProperty, value);
}

bool XUI::UI::MenuItem::GetIsCheckable() const
{
    return GetValue<bool>(IsCheckableProperty);
}

void XUI::UI::MenuItem::SetIsCheckable(const bool & value)
{
    SetValue(IsCheckableProperty, value);
}

MenuItemRole XUI::UI::MenuItem::GetRole() const
{
    return GetValue<MenuItemRole>(RoleProperty);
}

void XUI::UI::MenuItem::SetRole(MenuItemRole const & value)
{
    SetValue(RoleProperty, value);
}

bool MenuItem::GetIsSubMenuOpen() const
{
    return GetValue<bool>(IsSubMenuOpenProperty);
}

void MenuItem::SetIsSubMenuOpen(const bool &value)
{
    SetValue(IsSubMenuOpenProperty, value);
}

bool MenuItem::GetHasSubMenu() const
{
    return Items && !Items->empty();
}

bool MenuItem::GetIsTopLevel() const
{
    return Parent && Parent->Static_As<Menu>() != nullptr;
}

void MenuItem::OnClick(Core::Observer::RoutedEventArgs& e)
{
    if (IsCheckable)
    {
        Checked = !Checked;
        //e.Handled = true;
    }

    if (Command != nullptr)
    {
        Command->Execute(CommandParameter, this);
        //e.Handled = true;
    }
}

void MenuItem::OnGotFocus(Interfaces::GotFocusEventArgs& e)
{
    HeaderedSelectingItemsControl::OnGotFocus(e);
    IsSelected = true;
}

std::shared_ptr<Generators::IItemContainerGenerator> MenuItem::CreateItemContainerGenerator()
{
    return std::make_shared<Generators::MenuItemContainerGenerator>(this);
}

void MenuItem::OnKeyDown(Interfaces::KeyEventArgs& e)
{
    // Some keypresses we want to pass straight to the parent MenuItem/Menu without giving
    // this MenuItem the chance to handle them. This is usually e.g. when the submenu is
    // closed so passing them to the base would try to move the selection in a hidden
    // submenu.
    auto passStraightToParent = true;

    switch (e.Key)
    {
        case Key::Left:
            if (!IsTopLevel && IsSubMenuOpen)
            {
                IsSubMenuOpen = false;
                e.Handled = true;
            }

            passStraightToParent = IsTopLevel || !IsSubMenuOpen;
            break;

        case Key::Right:
CaseLabel1:
            if (!IsTopLevel && HasSubMenu && !IsSubMenuOpen)
            {
                SelectedIndex = 0;
                IsSubMenuOpen = true;
                e.Handled = true;
            }

            passStraightToParent = IsTopLevel || !IsSubMenuOpen;
            break;

        case Key::Enter:
            if (HasSubMenu)
            {
                goto CaseLabel1;
            }
            else
            {
                Core::Observer::RoutedEventArgs e(ClickEvent.get());
                RaiseEvent(e);
                e.Handled = true;
            }

            break;

        case Key::Escape:
            if (IsSubMenuOpen)
            {
                IsSubMenuOpen = false;
                e.Handled = true;
            }

            break;
    }

    if (!passStraightToParent)
    {
        HeaderedSelectingItemsControl::OnKeyDown(e);
    }
}

void MenuItem::OnPointerEnter(Interfaces::PointerEventArgs& e)
{
    HeaderedSelectingItemsControl::OnPointerEnter(e);

    auto menu = Parent->Static_As<Menu>();

    if (menu)
    {
        if (menu->IsOpen)
        {
            IsSubMenuOpen = true;
        }
    }
    else if (HasSubMenu && !IsSubMenuOpen)
    {
        _submenuTimer = Core::DispatcherTimer::Run([&]()
        {
            IsSubMenuOpen = true;
        }, std::chrono::milliseconds(400).count());
    }
    else
    {
        auto parentItem = Parent->Static_As<MenuItem>();
        if (parentItem != nullptr)
        {
            for (auto enumerator = parentItem->Items->GetGenericEnumerator(); !enumerator->IsFinished(); ++*enumerator)
            {
                auto current = enumerator->Current();
                if (current.GetType()->IsAssignableFrom<MenuItem>())
                {
                    auto sibling = current.TryGetValue<std::shared_ptr<MenuItem>>();
                    if (sibling.get() != this && sibling->IsSubMenuOpen)
                    {
                        sibling->CloseSubmenus();
                        sibling->IsSubMenuOpen = false;
                        sibling->IsSelected = false;
                    }
                }
            }
        }
    }
}

void MenuItem::OnPointerLeave(Interfaces::PointerEventArgs& e)
{
    HeaderedSelectingItemsControl::OnPointerLeave(e);

    if (_submenuTimer.is_subscribed())
        _submenuTimer.unsubscribe();
}

void MenuItem::OnPointerPressed(Interfaces::PointerPressedEventArgs& e)
{
    HeaderedSelectingItemsControl::OnPointerPressed(e);

    if (!HasSubMenu)
    {
        Core::Observer::RoutedEventArgs e(ClickEvent.get());
        RaiseEvent(e);
    }
    else if (IsTopLevel)
    {
        IsSubMenuOpen = !IsSubMenuOpen;
    }
    else
    {
        IsSubMenuOpen = true;
    }

    e.Handled = true;
}

void MenuItem::OnSubmenuOpened(Core::Observer::RoutedEventArgs& e)
{
    auto menuItem = e.Source->Static_As<MenuItem>();

    if (menuItem && menuItem->Parent == this)
    {
        for (auto enumerator = Items->GetGenericEnumerator(); !enumerator->IsFinished(); ++*enumerator)
        {
            auto current = enumerator->Current();
            if (current.GetType()->IsAssignableFrom<MenuItem>())
            {
                auto sibling = current.TryGetValue<std::shared_ptr<MenuItem>>();
                if (sibling.get() != menuItem && sibling->IsSubMenuOpen)
                {
                    sibling->IsSubMenuOpen = false;
                }
            }
        }
    }
}

void MenuItem::OnTemplateApplied(Controls::TemplateAppliedEventArgs const& e)
{
    HeaderedSelectingItemsControl::OnTemplateApplied(e);

    auto namedObj = Template->FindName("PART_Popup", this);

    if (namedObj)
    {
        auto popup = namedObj->Dynamic_As<Popup>();
        _popup = popup->shared_from_base_dynamic<Popup>();
        
        //_popup->DependencyResolver = DependencyResolver::Instance;
        _popup->Opened += System::make_eventhandler(&MenuItem::PopupOpened, this);
        _popup->Closed += System::make_eventhandler(&MenuItem::PopupClosed, this);
    }
}

void MenuItem::AccessKeyPressed(Core::Observer::RoutedEventArgs& e)
{
    if (HasSubMenu)
    {
        SelectedIndex = 0;
        IsSubMenuOpen = true;
    }
    else
    {
        Core::Observer::RoutedEventArgs e(ClickEvent.get());
        RaiseEvent(e);
    }

    e.Handled = true;
}

void MenuItem::CloseSubmenus()
{
    for (auto enumerator = Items->GetGenericEnumerator(); !enumerator->IsFinished(); ++*enumerator)
    {
        auto current = enumerator->Current();
        if (current.GetType()->IsAssignableFrom<MenuItem>())
        {
            auto sibling = current.TryGetValue<std::shared_ptr<MenuItem>>();
            if (sibling->IsSubMenuOpen)
            {
                sibling->IsSubMenuOpen = false;
            }
        }
    }
}

void MenuItem::IconChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
    auto oldValue = e.OldValue.TryGetValue<std::shared_ptr<Controls::Control>>();
    auto newValue = e.NewValue.TryGetValue<std::shared_ptr<Controls::Control>>();

    if (oldValue != nullptr)
    {
        RemoveLogicalChild(oldValue.get());
    }

    if (newValue != nullptr)
    {
        AddLogicalChild(newValue.get());
    }
}

void MenuItem::SubMenuOpenChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
    bool value = e.NewValue.GetValue<bool>();

    if (value)
    {
        Core::Observer::RoutedEventArgs e(SubmenuOpenedEvent.get());
        RaiseEvent(e);
        IsSelected = true;
    }
    else
    {
        CloseSubmenus();
        SelectedIndex = -1;
    }
}

void MenuItem::PopupOpened(void* sender, System::Events::EventArg& e)
{
    auto selected = SelectedIndex;

    if (selected != -1)
    {
        auto container = GetItemContainerGenerator()->ContainerFromIndex(selected);
        container == nullptr ? nullptr : container->Focus();
    }
}

void MenuItem::PopupClosed(void* sender, System::Events::EventArg& e)
{
    SelectedItem = nullptr;
}

void MenuItem::ItemsCollectionChanged(void *sender, System::Collection::NotifyCollectionChangedEventArgs& e)
{
    SelectingItemsControl::ItemsCollectionChanged(sender, e);

    for (auto enumerator = Items->GetGenericEnumerator(); !enumerator->IsFinished(); ++*enumerator)
    {
        auto current = enumerator->Current();
        if (current.GetType()->IsAssignableFrom<MenuItem>())
        {
            auto sibling = current.TryGetValue<std::shared_ptr<MenuItem>>();
            if (sibling->Role != MenuItemRole::SubmenuItem)
                sibling->Role = MenuItemRole::SubmenuItem;
        }
    }
}
