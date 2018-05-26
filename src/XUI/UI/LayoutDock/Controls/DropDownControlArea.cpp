#include "DropDownControlArea.h"
#include "UI/ContextMenu.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;
using namespace XUI::UI;

STATIC_CLASS_INITIALIZER(DropDownControlArea, XUI::UI::DropDownControlArea::StaticClassInitializer);

XUI::Core::Dependency::RefDependencyProperty DropDownControlArea::DropDownContextMenuProperty;
XUI::Core::Dependency::RefDependencyProperty DropDownControlArea::DropDownContextMenuDataContextProperty;

void DropDownControlArea::StaticClassInitializer()
{
    DropDownContextMenuProperty = (Core::Dependency::BindProperty<std::shared_ptr<UI::ContextMenu>>::Register<DropDownControlArea>(nameof(DropDownContext)));
    DropDownContextMenuDataContextProperty = (Core::Dependency::BindProperty<System::Reflection::Variant>::Register<DropDownControlArea>(nameof(DropDownContextMenuDataContext)));
}

DropDownControlArea::DropDownControlArea()
{

}

System::Reflection::Variant DropDownControlArea::GetDropDownContextMenuDataContext() const
{
    return GetValue(DropDownContextMenuDataContextProperty.get());
}

void DropDownControlArea::SetDropDownContextMenuDataContext(System::Reflection::Variant value)
{
    SetValue(DropDownContextMenuDataContextProperty, value);
}

void DropDownControlArea::OnPointerPressed(Interfaces::PointerPressedEventArgs& e)
{
    UserControl::OnPointerPressed(e);
}

void DropDownControlArea::OnPreviewPointerReleased(Interfaces::PointerReleasedEventArgs& e)
{
    UserControl::OnPreviewPointerReleased(e);

    if (!e.Handled)
    {
        if (DropDownContextMenu != nullptr)
        {
            //DropDownContextMenu->PlacementTarget = nullptr;
            //DropDownContextMenu->Placement = PlacementMode::MousePoint;
            //DropDownContextMenu->DataContext = DropDownContextMenuDataContext;
            //DropDownContextMenu->IsOpen = true;
            //// e.Handled = true;
        }
    }
}
