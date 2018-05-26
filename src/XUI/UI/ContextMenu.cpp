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

#include "ContextMenu.h"
#include "UI/MenuItem.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;
using namespace XUI::Interfaces;
using namespace XUI::UI;

STATIC_CLASS_POST_INITIALIZER(ContextMenu, ContextMenu::StaticClassInitializer)

std::shared_ptr<Core::Observer::RoutedEvent> ContextMenu::OpenedEvent;
std::shared_ptr<Core::Observer::RoutedEvent> ContextMenu::ClosedEvent;

void ContextMenu::StaticClassInitializer()
{
    OpenedEvent = (Core::Observer::RoutedEvent::Register<ContextMenu, Core::Observer::RoutedEventArgs>(nameof(Opened), Core::Observer::RoutingStrategies::Bubble));
    ClosedEvent = (Core::Observer::RoutedEvent::Register<ContextMenu, Core::Observer::RoutedEventArgs>(nameof(Closed), Core::Observer::RoutingStrategies::Bubble));

    ContextMenuProperty->Changed.get_observable().subscribe(ContextMenuChanged);

    MenuItem::ClickEvent->AddClassHandler(&typeof(ContextMenu), System::make_eventhandler([](void* sender, Core::Observer::RoutedEventArgs& e)
    {
        ContextMenu* menu = static_cast<ContextMenu*>(e.Sender);
        if (menu)
            menu->OnContextMenuClick(e);
    }), Core::Observer::RoutingStrategies(Core::Observer::RoutingStrategies::Direct | Core::Observer::RoutingStrategies::Bubble), true);
}

XUI::UI::ContextMenu::ContextMenu()
    : Opened(this)
    , Closed(this)
{
}

void ContextMenu::ContextMenuChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
    auto control = static_cast<Control*>(e.Sender);

    if (e.OldValue != nullptr)
    {
        control->PointerReleased -= System::make_eventhandler(System::make_function(ContextMenu::ControlPointerReleased));
    }

    if (e.NewValue != nullptr)
    {
        control->PointerReleased += System::make_eventhandler(System::make_function(ContextMenu::ControlPointerReleased));
    }
}

void ContextMenu::OnContextMenuClick(Core::Observer::RoutedEventArgs& e)
{
    Hide();
    //FocusManager::Instance->Focus(nullptr);
    e.Handled = true;
}

void ContextMenu::Hide()
{
    if (_popup != nullptr && _popup->IsVisible)
    {
        _popup->Close();
    }

    SelectedIndex = -1;

    _isOpen = false;
}

void ContextMenu::Show(Control *control)
{
    if (control != nullptr)
    {
        if (_popup == nullptr)
        {
            _popup = std::make_shared<Popup>();
            _popup->PlacementMode = PlacementMode::Pointer;
            _popup->PlacementTarget = control->shared_from_base_static<Controls::Control>();
            _popup->StaysOpen = false;
            _popup->IsEmbed = true;

            _popup->Closed += PopupClosed;
        }

        control->AddVisualChild(_popup.get()); // for inheritance
        control->AddLogicalChild(_popup.get());

        _popup->Child = control->ContextMenu->shared_from_base_static<Controls::Control>();

        _popup->Open();

        control->ContextMenu->_isOpen = true;

        Core::Observer::RoutedEventArgs e_opened(OpenedEvent.get());
        RaiseEvent(e_opened);
    }
}

void ContextMenu::PopupClosed(void* sender, System::Events::EventArg& e)
{
    auto contextMenu = static_cast<Popup*>(sender)->Child ? static_cast<Popup*>(sender)->Child->Static_As<ContextMenu>() : nullptr;

    if (contextMenu != nullptr)
    {
        for (auto i : contextMenu->GetLogicalChildren())
        {
            MenuItem* item = i->Static_As<MenuItem>();
            if (item)
                item->IsSubMenuOpen = false;
        }

        contextMenu->_isOpen = false;
        contextMenu->SelectedIndex = -1;

        Core::Observer::RoutedEventArgs e_closed(ClosedEvent.get());
        contextMenu->RaiseEvent(e_closed);
    }
}

void ContextMenu::ControlPointerReleased(void* sender, Interfaces::PointerReleasedEventArgs& e)
{
    auto control = static_cast<Controls::Control*>(e.Sender);
    auto contextMenu = control->ContextMenu;

    if (e.MouseButton == MouseButton::Right)
    {
        if (control->ContextMenu->_isOpen)
        {
            control->ContextMenu->Hide();
        }

        contextMenu->Show(control);
        e.Handled = true;
    }
    else if (contextMenu->_isOpen && contextMenu->_popup && !contextMenu->_popup->StaysOpen)
    {
        control->ContextMenu->Hide();
        e.Handled = true;
    }
}