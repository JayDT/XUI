#pragma once

#include "XUI/UI/Window.h"

namespace WorldClient
{
    class DebugWindow : public XUI::UI::Window, public XUI::Interfaces::IComponentConnector
    {
    public:
        void InitializeComponent() override;
        virtual void Connect(int connectionId, System::Reflection::meta::Object *target) override;

        void SetDebuggedWindow(XUI::UI::Window* wnd);

        void PART_TreeviewSelectedItemChanged(void* sender, XUI::Core::Observer::RoutedPropertyChangedEventArgs<System::Reflection::RefObject> const& e);
        void PART_ContextMenuOpened(void* sender, XUI::Core::Observer::RoutedEventArgs& e);
        void PART_ContextMenuOpenedVisual(void* sender, XUI::Core::Observer::RoutedEventArgs& e);
    };
}