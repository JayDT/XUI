#include "DropDownButton.h"
#include "UI/ContextMenu.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;
using namespace XUI::UI;

STATIC_CLASS_INITIALIZER(DropDownButton, XUI::UI::DropDownButton::StaticClassInitializer);

XUI::Core::Dependency::RefDependencyProperty DropDownButton::DropDownContextMenuProperty;
XUI::Core::Dependency::RefDependencyProperty DropDownButton::DropDownContextMenuDataContextProperty;

void XUI::UI::DropDownButton::StaticClassInitializer()
{
    DropDownContextMenuProperty = (Core::Dependency::BindProperty<std::shared_ptr<UI::ContextMenu>>::Register<DropDownButton>(nameof(LayoutUpdateStrategy)));
    DropDownContextMenuDataContextProperty = (Core::Dependency::BindProperty<System::Reflection::Variant>::Register<DropDownButton>(nameof(LayoutUpdateStrategy)));
}

DropDownButton::DropDownButton()
{
    this->DetachedFromVisualTree += System::make_eventhandler(&DropDownButton::DropDownButton_Unloaded, this);
}

std::shared_ptr<UI::ContextMenu> DropDownButton::GetDropDownContextMenu() const
{
    return GetValue<std::shared_ptr<UI::ContextMenu>>(DropDownContextMenuProperty);
}

void DropDownButton::SetDropDownContextMenu(std::shared_ptr<UI::ContextMenu> const& value)
{
    SetValue(DropDownContextMenuProperty, value);
}

void DropDownButton::OnDropDownContextMenuChanged(DependencyObject *d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
    (static_cast<DropDownButton*>(d))->OnDropDownContextMenuChanged(e);
}

void DropDownButton::OnDropDownContextMenuChanged(Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
    auto oldContextMenu = e.OldValue.GetValue<std::shared_ptr<UI::ContextMenu>>();
    if (oldContextMenu && IsChecked)
    {
        oldContextMenu->Closed -= System::make_eventhandler(&DropDownButton::OnContextMenuClosed, this);
    }
}

System::Reflection::Variant DropDownButton::GetDropDownContextMenuDataContext() const
{
    return GetValue<System::Reflection::Variant>(DropDownContextMenuDataContextProperty);
}

void DropDownButton::SetDropDownContextMenuDataContext(System::Reflection::Variant const& value)
{
    SetValue(DropDownContextMenuDataContextProperty, value);
}

void DropDownButton::OnClick(Core::Observer::RoutedEventArgs & e)
{
    if (DropDownContextMenu != nullptr)
    {
        //IsChecked = true;
        //DropDownContextMenu->PlacementTarget = this;
        //DropDownContextMenu->Placement = PlacementMode::Bottom;
        //DropDownContextMenu->DataContext = DropDownContextMenuDataContext;
        //DropDownContextMenu->Closed += System::make_eventhandler(&DropDownButton::OnContextMenuClosed, this);
        //DropDownContextMenu->IsOpen = true;
    }

    ToggleButton::OnClick(e);
}

void DropDownButton::OnContextMenuClosed(void* sender, System::Events::EventArg& e)
{
    //auto ctxMenu = GENERIC_SAFE_CAST(ContextMenu, sender);
    //ctxMenu->Closed -= System::make_eventhandler(&DropDownButton::OnContextMenuClosed, this);
    //IsChecked = ToggleButtonState::TBS_NONE;
}

void DropDownButton::DropDownButton_Unloaded(void* sender, Controls::VisualTreeAttachmentEventArgs& e)
{
    // When changing theme, Unloaded event is called, erasing the DropDownContextMenu.
    // Prevent this on theme changes.
    //if (this->IsLoaded)
    //{
    //    DropDownContextMenu = nullptr;
    //}
}

