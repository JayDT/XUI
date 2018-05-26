#include "DebugUI.h"

#include "XamlCPP/XamlCPP.h"
#include "XamlCPP/Core/Assembler/Settings.h"
#include "XamlCPP/Core/ReflEx/TypeConverterRegistration.h"
#include "XUI/UI/Window.h"
#include "XUI/UI/Grid.h"
#include "XUI/UI/Button.h"
#include "XUI/UI/Border.h"
#include "XUI/UI/ListBox.h"
#include "XUI/UI/TreeView.h"
#include "XUI/UI/TreeViewItem.h"
#include "XUI/UI/ContextMenu.h"
#include "XUI/UI/MenuItem.h"
#include "XUI/Core/Media/Brush.h"
#include "XUI/Core/TypeConverter/TypeConverters.h"
#include "XUI/Core/Application.h"

void WorldClient::DebugWindow::InitializeComponent()
{
    Show();
}

std::map<XUI::UI::Controls::Control*, XUI::UI::Controls::Control*> debug;

void AddToLogicalTree(XUI::UI::TreeViewItem* parentNode, XUI::UI::Controls::Control* control)
{
    for (auto const& child : control->LogicalChildren.GetNativeEnumerator())
    {
        auto item = std::make_shared<XUI::UI::TreeViewItem>();
        if (debug.find(child.get()) != debug.end())
            continue;
        //debug.insert(child.get());
        debug[child.get()] = control;
        item->Tag = child->weak_from_base_static<XUI::UI::Controls::Control>();
        //item->BeginInit();
        item->Header = std::string(System::String::format("[%u] %s", child->GetGID(), child->GetType()->GetName()));
        AddToLogicalTree(item.get(), child.get());
        //item->EndInit();

        parentNode->Items->Add(item);
    }
}

std::map<XUI::UI::Controls::Visual*, XUI::UI::Controls::Visual*> debugVisual;

void AddToVisualTree(XUI::UI::TreeViewItem* parentNode, XUI::UI::Controls::Visual* control)
{
    for (auto const& child : control->VisualChildren.GetNativeEnumerator())
    {
        auto item = std::make_shared<XUI::UI::TreeViewItem>();
        if (debugVisual.find(child.get()) != debugVisual.end())
            continue;
        //debugVisual.insert(child.get());
        debugVisual[child.get()] = control;
        item->Tag = child->weak_from_base_static<XUI::UI::Controls::Visual>();
        //item->BeginInit();
        item->Header = std::string(System::String::format("[%u] %s", child->GetGID(), child->GetType()->GetName()));
        AddToVisualTree(item.get(), child.get());
        //item->EndInit();

        parentNode->Items->Add(item);
    }
}

void WorldClient::DebugWindow::SetDebuggedWindow(XUI::UI::Window * wnd)
{
    //return;
    auto treeviewobj = Find("tvItems");
    if (treeviewobj)
    {
        auto treeview = treeviewobj->Static_As<XUI::UI::TreeView>();
        treeview->BeginInit();
        for (auto const& child : wnd->LogicalChildren.GetNativeEnumerator())
        {
            auto item = std::make_shared<XUI::UI::TreeViewItem>();
            if (debug.find(child.get()) != debug.end())
                continue;
            //debug.insert(child.get());
            debug[child.get()] = wnd;
            item->Tag = child->weak_from_base_static<XUI::UI::Controls::Control>();
            //item->BeginInit();
            item->Header = std::string(System::String::format("[%u] %s", child->GetGID(), child->GetType()->GetName()));
            AddToLogicalTree(item.get(), child.get());
            //item->EndInit();

            treeview->Items->Add(item);
        }
        treeview->EndInit();

        treeview->ContextMenu->Opened += System::make_eventhandler(&DebugWindow::PART_ContextMenuOpened, this);
    }

    treeviewobj = Find("tvVisualItems");
    if (treeviewobj)
    {
        auto treeview = treeviewobj->Static_As<XUI::UI::TreeView>();
        treeview->BeginInit();
        for (auto const& child : wnd->VisualChildren.GetNativeEnumerator())
        {
            auto item = std::make_shared<XUI::UI::TreeViewItem>();
            if (debugVisual.find(child.get()) != debugVisual.end())
                continue;
            //debugVisual.insert(child.get());
            debugVisual[child.get()] = wnd;
            item->Tag = child->weak_from_base_static<XUI::UI::Controls::Visual>();
            //item->BeginInit();
            item->Header = std::string(System::String::format("[%u] %s", child->GetGID(), child->GetType()->GetName()));
            AddToVisualTree(item.get(), child.get());
            //item->EndInit();

            treeview->Items->Add(item);
        }
        treeview->EndInit();
    
        treeview->ContextMenu->Opened += System::make_eventhandler(&DebugWindow::PART_ContextMenuOpenedVisual, this);
    }
}


void WorldClient::DebugWindow::PART_TreeviewSelectedItemChanged(void* sender, XUI::Core::Observer::RoutedPropertyChangedEventArgs<System::Reflection::RefObject> const& e)
{
    if (e.OldValue)
    {
        auto item = e.OldValue->Static_As<XUI::UI::TreeViewItem>();
        if (item && item->Tag.GetBase())
        {
            auto control = item->Tag.GetValue<std::weak_ptr<XUI::UI::Controls::Control>>();
            if (!control.expired())
            {
                auto _control = control.lock();
                //_control->SetValue(XUI::UI::Controls::UIElement::BorderBrushProperty, XUI::Core::Dependency::DependencyPropertyMgr::UnsetValue, XUI::Core::Dependency::BindingLevel::StyleTrigger);
                //_control->SetValue(XUI::UI::Controls::UIElement::BorderBrushProperty, XUI::Core::Dependency::DependencyPropertyMgr::UnsetValue, XUI::Core::Dependency::BindingLevel::StyleTrigger);
                //_control->SetValue(XUI::UI::Controls::UIElement::BorderThicknessProperty, XUI::Core::Dependency::DependencyPropertyMgr::UnsetValue, XUI::Core::Dependency::BindingLevel::StyleTrigger);
                _control->SetDebugFlag(_control->GetDebugFlag() & ~XUI::Core::Dependency::RenderDebugFlags::RDF_BOUND);
            }
        }
    }

    if (e.NewValue)
    {
        auto item = e.NewValue->Static_As<XUI::UI::TreeViewItem>();
        if (item && item->Tag.GetBase())
        {
            auto control = item->Tag.GetValue<std::weak_ptr<XUI::UI::Controls::Control>>();
            if (!control.expired())
            {
                auto _control = control.lock();
                auto color = std::make_shared<XUI::Core::Media::SolidColorBrush>(XUI::Core::Media::Color(255, 255, 255, 255));
                //_control->SetValue(XUI::UI::Controls::UIElement::BackgroundProperty, color, XUI::Core::Dependency::BindingLevel::StyleTrigger);
                //_control->SetValue(XUI::UI::Controls::UIElement::BorderBrushProperty, color, XUI::Core::Dependency::BindingLevel::StyleTrigger);
                //_control->SetValue(XUI::UI::Controls::UIElement::BorderThicknessProperty, 1.0, XUI::Core::Dependency::BindingLevel::StyleTrigger);
                _control->SetDebugFlag(_control->GetDebugFlag() | XUI::Core::Dependency::RenderDebugFlags::RDF_BOUND);
            }
        }
    }
}

void WorldClient::DebugWindow::PART_ContextMenuOpened(void * sender, XUI::Core::Observer::RoutedEventArgs & e)
{
    auto treeviewobj = Find("tvItems");
    if (treeviewobj)
    {
        auto treeview = treeviewobj->Static_As<XUI::UI::TreeView>();
        auto _objMenuItemVisible = treeview->ContextMenu->NamedScope->Find("VisibleControl");
        if (treeview->SelectedItem && _objMenuItemVisible)
        {
            XUI::UI::TreeViewItem* item = treeview->SelectedItem->Static_As<XUI::UI::TreeViewItem>();
            if (item && item->Tag.GetBase())
            {
                auto control = item->Tag.GetValue<std::weak_ptr<XUI::UI::Controls::Control>>();
                if (!control.expired())
                {
                    auto _control = control.lock();
                    _objMenuItemVisible->Static_As<XUI::UI::MenuItem>()->Checked = _control->IsVisible;
                }
            }
        }
    }
}

void WorldClient::DebugWindow::PART_ContextMenuOpenedVisual(void * sender, XUI::Core::Observer::RoutedEventArgs & e)
{
}

void WorldClient::DebugWindow::Connect(int connectionId, System::Reflection::meta::Object *target)
{
}
