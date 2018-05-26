/*
* Copyright (C) 2017-2018 Tauri JayD <https://www.>
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

#include "DockingManager.h"
#include "AutoHideWindowManager.h"
#include "LayoutAnchorableFloatingWindowControl.h"
#include "LayoutAnchorableFloatingWindow.h"
#include "LayoutDocumentFloatingWindowControl.h"
#include "LayoutDocumentFloatingWindow.h"
#include "LayoutDock/Controls/LayoutFloatingWindowControl.h"
#include "LayoutDock/Controls/OverlayWindow.h"
#include "LayoutDock/Layouts/LayoutDocumentPaneGroup.h"
#include "LayoutDock/Layouts/LayoutPanel.h"
#include "LayoutDock/Controls/LayoutAnchorablePaneControl.h"
#include "LayoutDock/Controls/LayoutDocumentPaneControl.h"
#include "LayoutDock/Controls/LayoutDocumentPaneGroupControl.h"
#include "LayoutDock/Layouts/LayoutAnchorableFloatingWindow.h"
#include "LayoutDock/Controls/LayoutAutoHideWindowControl.h"
#include "LayoutDock/Controls/LayoutPanelControl.h"
#include "LayoutDock/Controls/LayoutAnchorSideControl.h"
#include "LayoutDock/Controls/LayoutAnchorablePaneGroupControl.h"
#include "LayoutDock/Controls/LayoutDocumentControl.h"
#include "LayoutDock/Controls/LayoutAnchorableItem.h"
#include "LayoutDock/Controls/LayoutDocumentItem.h"
#include "FocusElementManager.h"
#include "LayoutDocumentFloatingWindow.h"
#include "Core/Observer/RoutedCommand.h"
#include "Core/GlobalInitializerScheduler.h"
#include "LayoutDock/Controls/DropArea.h"
#include "Platform/UIRenderer.h"
#include "cpplinq/linq.hpp"

using namespace XamlCPP::Core::Dependency;
using namespace XUI;
using namespace XUI::UI;

STATIC_CLASS_INITIALIZER(DockingManager, XUI::UI::DockingManager::StaticClassInitializer)

Core::Dependency::RefDependencyProperty DockingManager::LayoutProperty;
Core::Dependency::RefDependencyProperty DockingManager::LayoutUpdateStrategyProperty;
Core::Dependency::RefDependencyProperty DockingManager::DocumentPaneTemplateProperty;
Core::Dependency::RefDependencyProperty DockingManager::AnchorablePaneTemplateProperty;
Core::Dependency::RefDependencyProperty DockingManager::AnchorSideTemplateProperty;
Core::Dependency::RefDependencyProperty DockingManager::AnchorGroupTemplateProperty;
Core::Dependency::RefDependencyProperty DockingManager::AnchorTemplateProperty;
Core::Dependency::RefDependencyProperty DockingManager::DocumentPaneControlStyleProperty;
Core::Dependency::RefDependencyProperty DockingManager::AnchorablePaneControlStyleProperty;
Core::Dependency::RefDependencyProperty DockingManager::DocumentHeaderTemplateProperty;
Core::Dependency::RefDependencyProperty DockingManager::DocumentHeaderTemplateSelectorProperty;
Core::Dependency::RefDependencyProperty DockingManager::DocumentTitleTemplateProperty;
Core::Dependency::RefDependencyProperty DockingManager::DocumentTitleTemplateSelectorProperty;
Core::Dependency::RefDependencyProperty DockingManager::AnchorableTitleTemplateProperty;
Core::Dependency::RefDependencyProperty DockingManager::AnchorableTitleTemplateSelectorProperty;
Core::Dependency::RefDependencyProperty DockingManager::AnchorableHeaderTemplateProperty;
Core::Dependency::RefDependencyProperty DockingManager::AnchorableHeaderTemplateSelectorProperty;
Core::Dependency::RefDependencyProperty DockingManager::LayoutRootPanelProperty;
Core::Dependency::RefDependencyProperty DockingManager::RightSidePanelProperty;
Core::Dependency::RefDependencyProperty DockingManager::LeftSidePanelProperty;
Core::Dependency::RefDependencyProperty DockingManager::TopSidePanelProperty;
Core::Dependency::RefDependencyProperty DockingManager::BottomSidePanelProperty;
Core::Dependency::RefDependencyProperty DockingManager::AutoHideWindowProperty;
Core::Dependency::RefDependencyProperty DockingManager::LayoutItemTemplateProperty;
Core::Dependency::RefDependencyProperty DockingManager::LayoutItemTemplateSelectorProperty;
Core::Dependency::RefDependencyProperty DockingManager::DocumentsSourceProperty;
Core::Dependency::RefDependencyProperty DockingManager::DocumentContextMenuProperty;
Core::Dependency::RefDependencyProperty DockingManager::AnchorablesSourceProperty;
Core::Dependency::RefDependencyProperty DockingManager::ActiveContentProperty;
Core::Dependency::RefDependencyProperty DockingManager::AnchorableContextMenuProperty;
Core::Dependency::RefDependencyProperty DockingManager::ThemeProperty;
Core::Dependency::RefDependencyProperty DockingManager::GridSplitterWidthProperty;
Core::Dependency::RefDependencyProperty DockingManager::GridSplitterHeightProperty;
Core::Dependency::RefDependencyProperty DockingManager::DocumentPaneMenuItemHeaderTemplateProperty;
Core::Dependency::RefDependencyProperty DockingManager::DocumentPaneMenuItemHeaderTemplateSelectorProperty;
Core::Dependency::RefDependencyProperty DockingManager::IconContentTemplateProperty;
Core::Dependency::RefDependencyProperty DockingManager::IconContentTemplateSelectorProperty;
Core::Dependency::RefDependencyProperty DockingManager::LayoutItemContainerStyleProperty;
Core::Dependency::RefDependencyProperty DockingManager::LayoutItemContainerStyleSelectorProperty;
Core::Dependency::RefDependencyProperty DockingManager::ShowSystemMenuProperty;
Core::Dependency::RefDependencyProperty DockingManager::AllowMixedOrientationProperty;

void XUI::UI::DockingManager::StaticClassInitializer()
{
    LayoutProperty = (Core::Dependency::BindProperty<std::shared_ptr<LayoutRoot>>::Register<DockingManager>(nameof(Layout), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<DockingManager*>(d);
        control->OnLayoutChanged(e.OldValue.ToObject()->Static_As<LayoutRoot>(), e.NewValue.ToObject()->Static_As<LayoutRoot>());
    }, nullptr));

    LayoutUpdateStrategyProperty = (Core::Dependency::BindProperty<std::shared_ptr<ILayoutUpdateStrategy>>::Register<DockingManager>(nameof(LayoutUpdateStrategy)));
    DocumentPaneTemplateProperty = (Core::Dependency::BindProperty<std::shared_ptr<XUI::UI::DataTemplates::ControlTemplate>>::Register<DockingManager>(nameof(DocumentPaneTemplate)));
    AnchorablePaneTemplateProperty = (Core::Dependency::BindProperty<std::shared_ptr<XUI::UI::DataTemplates::ControlTemplate>>::Register<DockingManager>(nameof(AnchorablePaneTemplate)));
    AnchorSideTemplateProperty = (Core::Dependency::BindProperty<std::shared_ptr<XUI::UI::DataTemplates::ControlTemplate>>::Register<DockingManager>(nameof(AnchorSideTemplate)));
    AnchorGroupTemplateProperty = (Core::Dependency::BindProperty<std::shared_ptr<XUI::UI::DataTemplates::ControlTemplate>>::Register<DockingManager>(nameof(AnchorGroupTemplate)));
    AnchorTemplateProperty = (Core::Dependency::BindProperty<std::shared_ptr<XUI::UI::DataTemplates::ControlTemplate>>::Register<DockingManager>(nameof(AnchorTemplate)));
    DocumentPaneControlStyleProperty = (Core::Dependency::BindProperty<std::shared_ptr<XUI::Styling::Style>>::Register<DockingManager>(nameof(DocumentPaneControlStyle)));
    AnchorablePaneControlStyleProperty = (Core::Dependency::BindProperty<std::shared_ptr<XUI::Styling::Style>>::Register<DockingManager>(nameof(AnchorablePaneControlStyle)));
    DocumentHeaderTemplateProperty = (Core::Dependency::BindProperty<std::shared_ptr<XUI::UI::DataTemplates::DataTemplate>>::Register<DockingManager>(nameof(DocumentHeaderTemplate)));
    //DocumentHeaderTemplateSelectorProperty = (Core::Dependency::BindProperty<bool>::Register<DockingManager>(nameof(DocumentHeaderTemplateSelector)));
    DocumentTitleTemplateProperty = (Core::Dependency::BindProperty<std::shared_ptr<XUI::UI::DataTemplates::DataTemplate>>::Register<DockingManager>(nameof(DocumentTitleTemplate)));
    //DocumentTitleTemplateSelectorProperty = (Core::Dependency::BindProperty<bool>::Register<DockingManager>(nameof(DocumentTitleTemplateSelector)));
    AnchorableTitleTemplateProperty = (Core::Dependency::BindProperty<std::shared_ptr<XUI::UI::DataTemplates::DataTemplate>>::Register<DockingManager>(nameof(AnchorableTitleTemplate)));
    //AnchorableTitleTemplateSelectorProperty = (Core::Dependency::BindProperty<bool>::Register<DockingManager>(nameof(AnchorableTitleTemplateSelector)));
    AnchorableHeaderTemplateProperty = (Core::Dependency::BindProperty<std::shared_ptr<XUI::UI::DataTemplates::DataTemplate>>::Register<DockingManager>(nameof(AnchorableHeaderTemplate)));
    //AnchorableHeaderTemplateSelectorProperty = (Core::Dependency::BindProperty<bool>::Register<DockingManager>(nameof(AnchorableHeaderTemplateSelector)));
    LayoutRootPanelProperty = (Core::Dependency::BindProperty<std::shared_ptr<LayoutPanelControl>>::Register<DockingManager>(nameof(LayoutRootPanel), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<DockingManager*>(d);
        if (e.OldValue)
            control->InternalRemoveLogicalChild(e.OldValue.ToPtrObject<Controls::Control>());
        if (e.NewValue)
            control->InternalAddLogicalChild(e.NewValue.ToPtrObject<Controls::Control>());
    }, nullptr));

    RightSidePanelProperty = (Core::Dependency::BindProperty<std::shared_ptr<LayoutAnchorSideControl>>::Register<DockingManager>(nameof(RightSidePanel), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<DockingManager*>(d);
        if (e.OldValue)
            control->InternalRemoveLogicalChild(e.OldValue.ToPtrObject<Controls::Control>());
        if (e.NewValue)
            control->InternalAddLogicalChild(e.NewValue.ToPtrObject<Controls::Control>());
    }, nullptr));

    LeftSidePanelProperty = (Core::Dependency::BindProperty<std::shared_ptr<LayoutAnchorSideControl>>::Register<DockingManager>(nameof(LeftSidePanel), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<DockingManager*>(d);
        if (e.OldValue)
            control->InternalRemoveLogicalChild(e.OldValue.ToPtrObject<Controls::Control>());
        if (e.NewValue)
            control->InternalAddLogicalChild(e.NewValue.ToPtrObject<Controls::Control>());
    }, nullptr));

    TopSidePanelProperty = (Core::Dependency::BindProperty<std::shared_ptr<LayoutAnchorSideControl>>::Register<DockingManager>(nameof(TopSidePanel), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<DockingManager*>(d);
        if (e.OldValue)
            control->InternalRemoveLogicalChild(e.OldValue.ToPtrObject<Controls::Control>());
        if (e.NewValue)
            control->InternalAddLogicalChild(e.NewValue.ToPtrObject<Controls::Control>());
    }, nullptr));

    BottomSidePanelProperty = (Core::Dependency::BindProperty<std::shared_ptr<LayoutAnchorSideControl>>::Register<DockingManager>(nameof(BottomSidePanel), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<DockingManager*>(d);
        if (e.OldValue)
            control->InternalRemoveLogicalChild(e.OldValue.ToPtrObject<Controls::Control>());
        if (e.NewValue)
            control->InternalAddLogicalChild(e.NewValue.ToPtrObject<Controls::Control>());
    }, nullptr));

    AutoHideWindowProperty = (Core::Dependency::BindProperty<std::shared_ptr<LayoutAutoHideWindowControl>>::Register<DockingManager>(nameof(AutoHideWindow), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<DockingManager*>(d);
        if (e.OldValue)
            control->InternalRemoveLogicalChild(e.OldValue.ToPtrObject<Controls::Control>());
        if (e.NewValue)
            control->InternalAddLogicalChild(e.NewValue.ToPtrObject<Controls::Control>());
    }, nullptr));

    LayoutItemTemplateProperty = (Core::Dependency::BindProperty<std::shared_ptr<XUI::UI::DataTemplates::DataTemplate>>::Register<DockingManager>(nameof(LayoutItemTemplate)));
    //LayoutItemTemplateSelectorProperty = (Core::Dependency::BindProperty<bool>::Register<DockingManager>(nameof(LayoutItemTemplateSelector)));
    DocumentsSourceProperty = (Core::Dependency::BindProperty<std::shared_ptr<System::Collection::Generic::IContainer>>::Register<DockingManager>(nameof(DocumentsSource), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<DockingManager*>(d);
        control->DetachDocumentsSource(control->Layout.get(), e.OldValue.ToPtrObject<System::Collection::Generic::IContainer>());
        control->AttachDocumentsSource(control->Layout.get(), e.NewValue.ToPtrObject<System::Collection::Generic::IContainer>());
    }, nullptr));

    DocumentContextMenuProperty = (Core::Dependency::BindProperty<std::shared_ptr<UI::ContextMenu>>::Register<DockingManager>(nameof(DocumentContextMenu)));
    AnchorablesSourceProperty = (Core::Dependency::BindProperty<std::shared_ptr<System::Collection::Generic::IContainer>>::Register<DockingManager>(nameof(AnchorablesSource), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<DockingManager*>(d);
        control->DetachAnchorablesSource(control->Layout.get(), e.OldValue.ToPtrObject<System::Collection::Generic::IContainer>());
        control->DetachAnchorablesSource(control->Layout.get(), e.NewValue.ToPtrObject<System::Collection::Generic::IContainer>());
    }, nullptr));

    ActiveContentProperty = (Core::Dependency::BindProperty<System::Reflection::RefObject>::Register<DockingManager>(nameof(ActiveContent), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<DockingManager*>(d);
        control->InternalSetActiveContent(e.NewValue.ToObject());
        control->OnActiveContentChanged(e);
    }, nullptr));

    AnchorableContextMenuProperty = (Core::Dependency::BindProperty<std::shared_ptr<UI::ContextMenu>>::Register<DockingManager>(nameof(AnchorableContextMenu)));
    ThemeProperty = (Core::Dependency::BindProperty<bool>::Register<DockingManager>(nameof(Theme), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<DockingManager*>(d);
        control->OnThemeChanged(e);
    }, nullptr));

    GridSplitterWidthProperty = (Core::Dependency::BindProperty<double>::Register<DockingManager>(nameof(GridSplitterWidth), 6.0));
    GridSplitterHeightProperty = (Core::Dependency::BindProperty<double>::Register<DockingManager>(nameof(GridSplitterHeight), 6.0));
    DocumentPaneMenuItemHeaderTemplateProperty = (Core::Dependency::BindProperty<std::shared_ptr<XUI::UI::DataTemplates::DataTemplate>>::Register<DockingManager>(nameof(DocumentPaneMenuItemHeaderTemplate)));
    //DocumentPaneMenuItemHeaderTemplateSelectorProperty = (Core::Dependency::BindProperty<bool>::Register<DockingManager>(nameof(DocumentPaneMenuItemHeaderTemplateSelector)));
    IconContentTemplateProperty = (Core::Dependency::BindProperty<std::shared_ptr<XUI::UI::DataTemplates::DataTemplate>>::Register<DockingManager>(nameof(IconContentTemplate)));
    //IconContentTemplateSelectorProperty = (Core::Dependency::BindProperty<bool>::Register<DockingManager>(nameof(IconContentTemplateSelector)));
    LayoutItemContainerStyleProperty = (Core::Dependency::BindProperty<std::shared_ptr<XUI::Styling::Style>>::Register<DockingManager>(nameof(LayoutItemContainerStyle), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<DockingManager*>(d);
        control->AttachLayoutItems();
    }, nullptr));

    //LayoutItemContainerStyleSelectorProperty = (Core::Dependency::BindProperty<bool>::Register<DockingManager>(nameof(LayoutItemContainerStyleSelector)));
    ShowSystemMenuProperty = (Core::Dependency::BindProperty<bool>::Register<DockingManager>(nameof(ShowSystemMenu), true));
    AllowMixedOrientationProperty = (Core::Dependency::BindProperty<bool>::Register<DockingManager>(nameof(AllowMixedOrientation)));
}

XUI::UI::DockingManager::DockingManager()
    : _autoHideWindowManager(this)
	, ActiveContentChanged(&_activeContentChanged)
{
    //Initialized += System::make_eventhandler(&DockingManager::DockingManager_Loaded, this);
    //DetachedFromVisualTree += System::make_eventhandler(&DockingManager::DockingManager_Unloaded, this);
}

XUI::UI::DockingManager::~DockingManager()
{
}

void DockingManager::EndInit()
{
    bool notRef = _initCount == 1;
    Controls::Control::EndInit();

    if (notRef)
    {
        System::Events::EventArg e;
        DockingManager_Loaded(this, e);
    }
}

std::shared_ptr<LayoutRoot> XUI::UI::DockingManager::GetLayout() const
{
    auto layout = GetValue<std::shared_ptr<LayoutRoot>>(LayoutProperty);
    if (layout == nullptr)
    {
        auto root = std::make_shared<LayoutRoot>();
        root->Initialize();
        auto ac = std::make_shared<LayoutDocumentPane>();
        //auto ld = std::make_shared<LayoutDocumentPaneGroup>();
        //ld->AddChild(ac.get());
        root->RootPanel = std::make_shared<LayoutPanel>();
        root->RootPanel->AddChild(ac.get());
        const_cast<DockingManager*>(this)->Layout = root;
        return root;
    }
    return layout;
}

void XUI::UI::DockingManager::SetLayout(std::shared_ptr<LayoutRoot> const& value)
{
    SetValue(LayoutProperty, value);
}

void XUI::UI::DockingManager::StartDraggingFloatingWindowForContent(LayoutContent * contentModel, bool startDrag)
{
    if (!contentModel->CanFloat)
        return;

    auto contentModelAsAnchorable = contentModel->Static_As<LayoutAnchorable>();
    if (contentModelAsAnchorable &&
        contentModelAsAnchorable->IsAutoHidden())
        contentModelAsAnchorable->ToggleAutoHide();
    
    auto parentPane = contentModel->Parent;
    auto parentPaneAsPositionableElement = contentModel->Parent->Static_As<LayoutGroupBase>();
    auto parentPaneAsWithActualSize = contentModel->Parent->Static_As<LayoutGroupBase>();
    auto contentModelParentChildrenIndex = parentPane->Children.IndexOf(contentModel->shared_from_base_static<LayoutContent>());
    
    if (!LayoutElement::FindParent<LayoutFloatingWindow>(contentModel))
    {
        contentModel->PreviousContainer = parentPane;
        contentModel->PreviousContainerIndex = contentModelParentChildrenIndex;
    }
    
    parentPane->RemoveChildAt(contentModelParentChildrenIndex);
    
    double fwWidth = contentModel->FloatingWidth; 
    double fwHeight = contentModel->FloatingHeight; 
    
    if (fwWidth == 0.0)
        fwWidth = parentPaneAsPositionableElement->GetFloatingWidth();
    if (fwHeight == 0.0)
        fwHeight = parentPaneAsPositionableElement->GetFloatingHeight();
    
    if (fwWidth == 0.0)
        fwWidth = parentPaneAsWithActualSize->GetActualWidth();
    if (fwHeight == 0.0)
        fwHeight = parentPaneAsWithActualSize->GetActualHeight();
    
    std::shared_ptr<LayoutFloatingWindow> fw;
    std::shared_ptr<LayoutFloatingWindowControl> fwc;

    System::Reflection::Type const* windowType = &typeof(UI::Window);
    if (contentModel->Is<LayoutAnchorable>())
    {
        auto anchorableContent = contentModel->Static_As<LayoutAnchorable>();
        auto _fw = std::make_shared<LayoutAnchorableFloatingWindow>();
        fw = _fw;
        auto ac = std::make_shared<LayoutAnchorablePane>();
        ac->AddChild(anchorableContent);
        auto pane = std::make_shared<LayoutAnchorablePaneGroup>();
        _fw->AddChild(pane.get());
        _fw->RootPanel->AddChild(ac.get());
        ac->DockWidth = parentPaneAsPositionableElement->GetDockWidth();
        ac->DockHeight = parentPaneAsPositionableElement->GetDockHeight();
        ac->DockMinHeight = parentPaneAsPositionableElement->GetDockMinHeight();
        ac->DockMinWidth = parentPaneAsPositionableElement->GetDockMinWidth();
        ac->FloatingLeft = parentPaneAsPositionableElement->GetFloatingLeft();
        ac->FloatingTop = parentPaneAsPositionableElement->GetFloatingTop();
        ac->FloatingWidth = parentPaneAsPositionableElement->GetFloatingWidth();
        ac->FloatingHeight = parentPaneAsPositionableElement->GetFloatingHeight();
    
        Layout->FloatingWindows.Add(fw);
    
        auto _fwc = std::make_shared<LayoutAnchorableFloatingWindowControl>(fw->Static_As<LayoutAnchorableFloatingWindow>());
        fwc = _fwc;
        fwc->BeginInit();
        fwc->Title = System::String::ToWString(contentModel->Name);
        //fwc->Template = FindControlTemplate(windowType);
        _fwc->Width = fwWidth;
        _fwc->Height = fwHeight;
        _fwc->Left = contentModel->FloatingLeft;
        _fwc->Top = contentModel->FloatingTop;
    }
    else
    {
        auto anchorableDocument = contentModel->Static_As<LayoutDocument>();
        auto _fw = std::make_shared<LayoutDocumentFloatingWindow>();
        fw = _fw;
        _fw->AddChild(anchorableDocument);
    
        Layout->FloatingWindows.Add(fw);
    
        auto _fwc = std::make_shared<LayoutDocumentFloatingWindowControl>(fw->Static_As<LayoutDocumentFloatingWindow>());
        fwc = _fwc;
        fwc->BeginInit();
        fwc->Title = System::String::ToWString(contentModel->Name);
        fwc->Template = FindControlTemplate(windowType);
        _fwc->Width = fwWidth;
        _fwc->Height = fwHeight;
        _fwc->Left = contentModel->FloatingLeft;
        _fwc->Top = contentModel->FloatingTop;
    }

    bool isGUIWnd = this->VisualRoot->ToVisual()->Is<Platform::UIRender>();
    if (isGUIWnd)
    {
        fwc->WindowStyle = UI::WindowStyle::GUIWindow;
    }

    fwc->EndInit();
    
    //fwc.Owner = Window.GetWindow(this);
    //fwc.SetParentToMainWindowOf(this);
    
    _fwList.Add(fwc);
    
    Layout->CollectGarbage();
    
    InvalidateArrange();
    //UpdateLayout();

    fwc->Show();

    //Core::UIDispatcher::OnRender += [startDrag, fwc]()
    //{
    //    if (startDrag)
    //        fwc->AttachDrag();
    //    fwc->Show();
    //};
}

void XUI::UI::DockingManager::StartDraggingFloatingWindowForPane(LayoutAnchorablePane * paneModel)
{
    if (LayoutContainer::ContainsChild(paneModel, [](LayoutElement* x)
    {
        if (x->Is<LayoutContent>())
            return !static_cast<LayoutContent*>(x)->CanFloat;
        return false;
    }))
    {
        return;
    }

    auto paneAsPositionableElement = paneModel->Static_As<LayoutGroupBase>();
    auto paneAsWithActualSize = paneModel->Static_As<LayoutGroupBase>();
    
    double fwWidth = paneAsPositionableElement->GetFloatingWidth();
    double fwHeight = paneAsPositionableElement->GetFloatingHeight();
    double fwLeft = paneAsPositionableElement->GetFloatingLeft();
    double fwTop = paneAsPositionableElement->GetFloatingTop();
    
    
    
    if (fwWidth == 0.0)
        fwWidth = paneAsWithActualSize->GetActualWidth();
    if (fwHeight == 0.0)
        fwHeight = paneAsWithActualSize->GetActualHeight();
    
    auto destPane = std::shared_ptr<LayoutAnchorablePane>();
    destPane->DockWidth = paneAsPositionableElement->GetDockWidth();
    destPane->DockHeight = paneAsPositionableElement->GetDockHeight();
    destPane->DockMinHeight = paneAsPositionableElement->GetDockMinHeight();
    destPane->DockMinWidth = paneAsPositionableElement->GetDockMinWidth();
    destPane->FloatingLeft = paneAsPositionableElement->GetFloatingLeft();
    destPane->FloatingTop = paneAsPositionableElement->GetFloatingTop();
    destPane->FloatingWidth = paneAsPositionableElement->GetFloatingWidth();
    destPane->FloatingHeight = paneAsPositionableElement->GetFloatingHeight();
    
    bool savePreviousContainer = LayoutElement::FindParent<LayoutFloatingWindow>(paneModel) == nullptr;
    int currentSelectedContentIndex = paneModel->SelectedContentIndex;
    while (!paneModel->Children.empty())
    {
        auto contentModel = paneModel->Children[paneModel->Children.size() - 1]->Static_As<LayoutAnchorable>();
    
        if (savePreviousContainer)
        {
            auto contentModelAsPreviousContainer = contentModel;
            contentModelAsPreviousContainer->PreviousContainer = paneModel;
            contentModel->PreviousContainerIndex = paneModel->Children.size() - 1;
        }
    
        paneModel->RemoveChildAt(paneModel->Children.size() - 1);
        destPane->InsertChildAt(0, contentModel);
    }
    
    if (destPane->Children.size() > 0)
    {
        destPane->SelectedContentIndex = currentSelectedContentIndex;
    }
    
    
    std::shared_ptr<LayoutFloatingWindow> fw;
    std::shared_ptr<LayoutFloatingWindowControl> fwc;
    auto _fw = std::make_shared<LayoutAnchorableFloatingWindow>();
    fw = _fw;
    auto root = std::make_shared<LayoutAnchorablePaneGroup>();
    root->AddChild(destPane.get());
    _fw->AddChild(root.get());
    root->DockHeight = destPane->GetDockHeight();
    root->DockWidth = destPane->GetDockWidth();
    root->DockMinHeight = destPane->GetDockMinHeight();
    root->DockMinWidth = destPane->GetDockMinWidth();
    
    Layout->FloatingWindows.Add(fw);
    
    fwc = std::make_shared<LayoutAnchorableFloatingWindowControl>(fw->Static_As<LayoutAnchorableFloatingWindow>());
    fwc->Width = fwWidth;
    fwc->Height = fwHeight;
    
    
    //fwc.Owner = Window.GetWindow(this);
    //fwc.SetParentToMainWindowOf(this);
    
    
    _fwList.Add(fwc);
    
    Layout->CollectGarbage();
    
    InvalidateArrange();
    
    fwc->AttachDrag();
    fwc->Show(); 
}

std::vector<LayoutFloatingWindowControl*> XUI::UI::DockingManager::GetFloatingWindowsByZOrder()
{
    std::vector<LayoutFloatingWindowControl*> result;

    auto parentWindow = FindLogicalAncestor<Window>();
    
    while (parentWindow)
    {
        Interfaces::PlatformHandle windowParentHanlde = parentWindow->GetWindowImpl()->GetHandle();
        for (auto const& wnd : _fwList)
        {
            if (wnd->GetWindowImpl()->GetHandle() == windowParentHanlde && wnd->GetModel()->Root->GetManager() == this)
                result.push_back(wnd->Static_As<LayoutFloatingWindowControl>());
        }

        parentWindow = parentWindow->FindLogicalAncestor<Window>();
    }

    return std::move(result);
}

void XUI::UI::DockingManager::RemoveFloatingWindow(LayoutFloatingWindowControl * floatingWindow)
{
    _fwList.Remove(floatingWindow->shared_from_base_static<LayoutFloatingWindowControl>());
}

bool XUI::UI::DockingManager::HitTest(Core::Media::Point const & dragPoint)
{
    Core::Media::Rect detectionRect = TransformedBounds;
    return detectionRect.contains(dragPoint);
}

void XUI::UI::DockingManager::CreateOverlayWindow()
{
    if (!_overlayWindow)
        return;

    Core::Media::Rect rectWindow = TransformedBounds;
    _overlayWindow->Width = rectWindow.Width;
    _overlayWindow->Height = rectWindow.Height;
}

void XUI::UI::DockingManager::DestroyOverlayWindow()
{
}

OverlayWindow * XUI::UI::DockingManager::ShowOverlayWindow(LayoutFloatingWindowControl * draggingWindow)
{
    if (draggingWindow)
    {
        CreateOverlayWindow();
        if (!_overlayWindow->IsVisible)
        {
            _overlayWindow->EnableDropTargets();
            _overlayWindow->IsVisible = true;
        }
    }
    return _overlayWindow.get();
}

void XUI::UI::DockingManager::HideOverlayWindow()
{
    if (_overlayWindow && _overlayWindow->IsVisible)
    {
        _areas.clear();
        _overlayWindow->HideDropTargets();
        _overlayWindow->IsVisible = false;
        _overlayWindow->SetFloatingWindow(nullptr);
    }
}

std::list<std::shared_ptr<IDropArea>> const & XUI::UI::DockingManager::GetDropAreas(LayoutFloatingWindowControl * draggingWindow)
{
    if (!_areas.empty())
        return _areas;
    
    bool isDraggingDocuments = draggingWindow->GetModel()->Is<LayoutDocumentFloatingWindow>();
    
    if (!isDraggingDocuments)
    {
        _areas.push_back(std::make_shared<DropArea<DockingManager>>(
            shared_from_base_static<DockingManager>(),
            DropAreaType::DockingManager));

        std::list<Controls::Visual*> visuals;
        GetVisualDescendents(visuals, [](Controls::Visual* v) -> bool { return v->Is<LayoutAnchorablePaneControl>(); });
        for(auto visual : visuals)
        {
            LayoutAnchorablePaneControl* areaHost = visual->Static_As<LayoutAnchorablePaneControl>();
            if (!areaHost->Model->Children.empty())
            {
                _areas.push_back(std::make_shared<DropArea<LayoutAnchorablePaneControl>>(
                    areaHost->shared_from_base_static<LayoutAnchorablePaneControl>(),
                    DropAreaType::AnchorablePane));
            }
        }
    }
    
    std::list<Controls::Visual*> visuals;
    GetVisualDescendents(visuals, [](Controls::Visual* v) -> bool { return v->Is<LayoutDocumentPaneControl>(); });
    for (auto visual : visuals)
    {
        LayoutDocumentPaneControl* areaHost = visual->Static_As<LayoutDocumentPaneControl>();
        _areas.push_back(std::make_shared<DropArea<LayoutDocumentPaneControl>>(
            areaHost->shared_from_base_static<LayoutDocumentPaneControl>(),
            DropAreaType::DocumentPane));
    }
    
    visuals.clear();
    GetVisualDescendents(visuals, [](Controls::Visual* v) -> bool { return v->Is<LayoutDocumentPaneGroupControl>(); });
    for (auto visual : visuals)
    {
        LayoutDocumentPaneGroupControl* areaHost = visual->Static_As<LayoutDocumentPaneGroupControl>();
        auto documentGroupModel = areaHost->Model->Static_As<LayoutDocumentPaneGroup>();
        int count = cpplinq::from(documentGroupModel->Children.GetNativeEnumerator()).where([](std::shared_ptr<LayoutElement> const& x) { return x->GetIsVisible(); }).count();
        if (!count)
        {
            _areas.push_back(std::make_shared<DropArea<LayoutDocumentPaneControl>>(
                areaHost->shared_from_base_static<LayoutDocumentPaneControl>(),
                DropAreaType::DocumentPaneGroup));
        }
    }
    
    return _areas;
}

Core::Media::Size XUI::UI::DockingManager::ArrangeOverride(Core::Media::Size const & arrangeBounds)
{
    _areas.clear();
    return UI::Controls::Control::ArrangeOverride(arrangeBounds);
}

void XUI::UI::DockingManager::_ExecuteCloseCommand(LayoutDocument * document)
{
    if (DocumentClosing)
    {
        DocumentCloseEventArgs e(document->shared_from_base_static<LayoutDocument>());
        DocumentClosing(this, e);
        if (e.Cancel)
            return;
    }
    
    if (document->CloseDocument())
    {
        RemoveViewFromLogicalChild(document);
    
        if (DocumentClosed)
        {
            DocumentCloseEventArgs e(document->shared_from_base_static<LayoutDocument>());
            DocumentClosed(this, e);
        }
    }
}

void XUI::UI::DockingManager::_ExecuteCloseAllButThisCommand(LayoutContent * contentSelected)
{
    LayoutContainer::ContainsChild(Layout.get(), [this, contentSelected](LayoutElement* d)
    {
        if (d != contentSelected && (d->Parent->Is<LayoutDocumentPane>() || d->Parent->Is<LayoutDocumentFloatingWindow>()))
            Close(d->Static_As<LayoutContent>());
        return false;
    });
}

void XUI::UI::DockingManager::_ExecuteCloseAllCommand(LayoutContent * contentSelected)
{
    LayoutContainer::ContainsChild(Layout.get(), [this](LayoutElement* d)
    {
        if ((d->Parent->Is<LayoutDocumentPane>() || d->Parent->Is<LayoutDocumentFloatingWindow>()))
            Close(d->Static_As<LayoutContent>());
        return false;
    });
}

/// <summary>
/// Handles changes to the AnchorablesSource property.
/// </summary>

void XUI::UI::DockingManager::OnAnchorablesSourceChanged(DependencyObject * d, Core::Dependency::DependencyPropertyChangedEventArgs & e)
{
    static_cast<DockingManager*>(d)->OnAnchorablesSourceChanged(e);
}

/// <summary>
/// Provides derived classes an opportunity to handle changes to the AnchorablesSource property.
/// </summary>

void XUI::UI::DockingManager::OnAnchorablesSourceChanged(Core::Dependency::DependencyPropertyChangedEventArgs & e)
{
    DetachDocumentsSource(Layout.get(), e.OldValue.ToObject()->Static_As<System::Collection::Generic::IContainer>());
    AttachDocumentsSource(Layout.get(), e.NewValue.ToObject()->Static_As<System::Collection::Generic::IContainer>());
}

void XUI::UI::DockingManager::AttachAnchorablesSource(LayoutRoot* layout, System::Collection::Generic::IContainer * anchorablesSource)
{
    if (anchorablesSource == nullptr)
        return;
    
    if (layout == nullptr)
        return;
    
    //if (layout.Descendents().OfType<LayoutAnchorable>().Any())
    //    throw new InvalidOperationException("Unable to set the AnchorablesSource property if LayoutAnchorable objects are already present in the model");
    std::set<LayoutAnchorable*> anchorablesImported;
    LayoutContainer::ContainsChild(layout, [&anchorablesImported](LayoutElement* d)
    {
        auto c = d->Static_As<LayoutAnchorable>();
        if (c && c->Content)
            anchorablesImported.insert(c);
        return false;
    });

    auto anchorables = anchorablesSource;
    std::set<Core::Dependency::DependencyObject*> listOfAnchorablesToImport;

    for (auto itr = anchorablesSource->GetGenericEnumerator(); !itr->IsFinished(); ++(*itr))
    {
        LayoutAnchorable* element = itr->Current().ToObject()->Static_As<LayoutAnchorable>();
        if (!element || anchorablesImported.find(element) == anchorablesImported.end())
            listOfAnchorablesToImport.insert(element);
    }
     
    LayoutAnchorablePane* anchorablePane = nullptr;
    if (layout->ActiveContent)
    {
        //look for active content parent pane
        anchorablePane = layout->ActiveContent->Parent->Static_As<LayoutAnchorablePane>();
    }
    
    if (anchorablePane == nullptr)
    {
        //look for a pane on the right side
        LayoutContainer::ContainsChild(layout, [&anchorablePane](LayoutElement* d)
        {
            auto c = d->Static_As<LayoutAnchorablePane>();
            if (c && !c->IsHostedInFloatingWindow() && LayoutElement::GetSide(c) == AnchorSide::Right)
            {
                anchorablePane = c;
                return true;
            }
            return false;
        });
    }
    
    if (anchorablePane == nullptr)
    {
        //look for an available pane
        LayoutContainer::ContainsChild(layout, [&anchorablePane](LayoutElement* d)
        {
            auto c = d->Static_As<LayoutAnchorablePane>();
            if (c)
            {
                anchorablePane = c;
                return true;
            }
            return false;
        });
    }
    
    _suspendLayoutItemCreation = true;
    for(auto& anchorableContentToImport : listOfAnchorablesToImport)
    {
        auto anchorableToImport = std::make_shared<LayoutAnchorable>();
        anchorableToImport->SetContent(anchorableContentToImport->shared_from_this());
    
        bool added = false;
        if (LayoutUpdateStrategy)
        {
            added = LayoutUpdateStrategy->BeforeInsertAnchorable(layout, anchorableToImport.get(), anchorablePane);
        }
    
        if (!added)
        {
            if (anchorablePane == nullptr)
            {
                auto mainLayoutPanel = std::shared_ptr<LayoutPanel>();
                mainLayoutPanel->Orientation = UI::Orientation::Horizontal;
                if (layout->RootPanel)
                {
                    mainLayoutPanel->AddChild(layout->RootPanel.get());
                }
    
                layout->RootPanel = mainLayoutPanel;
                auto ac = std::make_shared<LayoutAnchorablePane>();
                anchorablePane = ac.get();
                mainLayoutPanel->AddChild(anchorablePane);
            }
    
            anchorablePane->AddChild(anchorableToImport.get());
            added = true;
        }
    
        if (LayoutUpdateStrategy)
            LayoutUpdateStrategy->AfterInsertAnchorable(layout, anchorableToImport.get());
    
    
        CreateAnchorableLayoutItem(anchorableToImport.get());
    
    }
    
    _suspendLayoutItemCreation = false;
    
    anchorablesSource->CollectionChanged += System::make_eventhandler(&DockingManager::anchorablesSourceElementsChanged, this);
}

void XUI::UI::DockingManager::anchorablesSourceElementsChanged(void * sender, System::Collection::NotifyCollectionChangedEventArgs & e)
{
    if (Layout == nullptr)
        return;
    
    //When deserializing documents are created automatically by the deserializer
    if (SuspendAnchorablesSourceBinding)
        return;

    System::Collection::Generic::IContainer * container = GENERIC_SAFE_CAST(System::Collection::Generic::IContainer *, sender);
    
    //handle remove
    if (e.Action == System::Collection::NotifyCollectionChangedAction::Remove ||
        e.Action == System::Collection::NotifyCollectionChangedAction::Replace)
    {
        auto oldItem = container->ElementAt(e.OldStartingIndex);
        if (oldItem.Valid() && oldItem.IsObject())
        {
            LayoutContent* item = oldItem.ToObject()->Static_As<LayoutContent>();
            if (item && LayoutContainer::ContainsChild(Layout.get(), [item](LayoutElement* x)
            {
                return x->Is<LayoutAnchorable>() && static_cast<LayoutAnchorable*>(x)->Content.get() == item;
            }))
            {
                item->Content = nullptr;
                item->Parent->RemoveChild(item);
                RemoveViewFromLogicalChild(item);
            }
        }
    }
    
    //handle add
    if (e.Action == System::Collection::NotifyCollectionChangedAction::Add ||
            e.Action == System::Collection::NotifyCollectionChangedAction::Replace)
    {
        auto newItem = container->ElementAt(e.NewStartingIndex);
        if (newItem.Valid() && newItem.IsObject())
        {
            LayoutAnchorablePane* anchorablePane = nullptr;
    
            if (Layout->ActiveContent)
            {
                //look for active content parent pane
                anchorablePane = Layout->ActiveContent->Parent->Static_As<LayoutAnchorablePane>();
            }
    
            if (anchorablePane == nullptr)
            {
                //look for a pane on the right side
                LayoutContainer::ContainsChild(Layout.get(), [&anchorablePane](LayoutElement* x)
                {
                    LayoutAnchorablePane* d = x->Static_As<LayoutAnchorablePane>();
                    if (d && !d->IsHostedInFloatingWindow() && LayoutElement::GetSide(x) == AnchorSide::Right)
                    {
                        anchorablePane = d;
                        return true;
                    }

                    return false;
                });
            }
    
            if (anchorablePane == nullptr)
            {
                //look for an available pane
                LayoutContainer::ContainsChild(Layout.get(), [&anchorablePane](LayoutElement* x)
                {
                    LayoutAnchorablePane* d = x->Static_As<LayoutAnchorablePane>();
                    if (d)
                    {
                        anchorablePane = d;
                        return true;
                    }

                    return false;
                });
            }
    
            _suspendLayoutItemCreation = true;
            {
                auto anchorableToImport = std::make_shared<LayoutAnchorable>();
                anchorableToImport->Content = newItem.ToSharedObject();
    
                bool added = false;
                if (LayoutUpdateStrategy)
                {
                    added = LayoutUpdateStrategy->BeforeInsertAnchorable(Layout.get(), newItem.ToObject()->Static_As<LayoutAnchorable>(), anchorablePane);
                }
    
                if (!added)
                {
                    if (anchorablePane == nullptr)
                    {
                        auto mainLayoutPanel = std::make_shared<LayoutPanel>();
                        mainLayoutPanel->Orientation = UI::Orientation::Horizontal;

                        if (Layout->RootPanel)
                        {
                            mainLayoutPanel->AddChild(Layout->RootPanel.get());
                        }
    
                        Layout->RootPanel = mainLayoutPanel;
                        auto _anchorablePane = std::make_shared<LayoutAnchorablePane>();
                        anchorablePane = _anchorablePane.get();
                        mainLayoutPanel->AddChild(anchorablePane);
                    }
    
                    anchorablePane->AddChild(anchorableToImport.get());
                    added = true;
                }
    
                if (LayoutUpdateStrategy)
                {
                    LayoutUpdateStrategy->AfterInsertAnchorable(Layout.get(), anchorableToImport.get());
                }
    
                auto root = anchorableToImport->Root;
    
                if (root && root->GetManager() == this)
                {
                    CreateAnchorableLayoutItem(anchorableToImport.get());
                }
    
            }
            _suspendLayoutItemCreation = false;
        }
    }
    
    if (e.Action == System::Collection::NotifyCollectionChangedAction::Reset)
    {
        //NOTE: I'm going to clear every anchorable present in layout but
        //some anchorable may have been added directly to the layout, for now I clear them too
        LayoutContainer::ContainsChild(Layout.get(), [this](LayoutElement* x)
        {
            LayoutAnchorable* anchorableToRemove = x->Static_As<LayoutAnchorable>();
            if (anchorableToRemove)
            {
                anchorableToRemove->Parent->RemoveChild(anchorableToRemove);
                RemoveViewFromLogicalChild(anchorableToRemove);
            }

            return false;
        });
    }
    
    if (Layout)
        Layout->CollectGarbage();
}

void XUI::UI::DockingManager::DetachAnchorablesSource(LayoutRoot* layout, System::Collection::Generic::IContainer * anchorablesSource)
{
    if (anchorablesSource == nullptr)
        return;
    
    if (layout == nullptr)
        return;
    
    LayoutAnchorable* anchorablesToRemove = nullptr;
    LayoutContainer::ContainsChild(Layout.get(), [&](LayoutElement* x)
    {
        LayoutAnchorable* anchorableToRemove = x->Static_As<LayoutAnchorable>();
        if (anchorableToRemove)
        {
            for (auto itr = anchorablesSource->GetGenericEnumerator(); !itr->IsFinished(); ++(*itr))
            {
                auto obj = itr->Current().ToObject();
                if (obj == x)
                {
                    anchorableToRemove->Parent->RemoveChild(anchorableToRemove);
                    RemoveViewFromLogicalChild(anchorableToRemove);
                    break;
                }
            }
        }

        return false;
    });
    
    anchorablesSource->CollectionChanged -= System::make_eventhandler(&DockingManager::anchorablesSourceElementsChanged, this);
}

void XUI::UI::DockingManager::_ExecuteCloseCommand(LayoutAnchorable* anchorable)
{
    auto model = anchorable->Static_As<LayoutAnchorable>();
    if (model)
    {
        model->CloseAnchorable();
        RemoveViewFromLogicalChild(anchorable);
    }
}

void XUI::UI::DockingManager::_ExecuteHideCommand(LayoutAnchorable* anchorable)
{
    auto model = anchorable->Static_As<LayoutAnchorable>();
    if (model)
    {
        //by default hide the anchorable
        model->Hide();
    }
}

void XUI::UI::DockingManager::_ExecuteAutoHideCommand(LayoutAnchorable * _anchorable)
{
    _anchorable->ToggleAutoHide();
}

void XUI::UI::DockingManager::_ExecuteFloatCommand(LayoutContent * contentToFloat)
{
    contentToFloat->Float();
}

void XUI::UI::DockingManager::_ExecuteDockCommand(LayoutAnchorable * anchorable)
{
    anchorable->Dock();
}

void XUI::UI::DockingManager::_ExecuteDockAsDocumentCommand(LayoutContent * content)
{
    content->DockAsDocument();
}

/// <summary>
/// Handles changes to the AnchorableTemplate property.
/// </summary>

void XUI::UI::DockingManager::OnLayoutItemTemplateChanged(DependencyObject * d, Core::Dependency::DependencyPropertyChangedEventArgs & e)
{
    static_cast<DockingManager*>(d)->OnLayoutItemTemplateChanged(e);
}

/// <summary>
/// Handles changes to the LayoutItemTemplateSelector property.
/// </summary>

void XUI::UI::DockingManager::OnLayoutItemTemplateSelectorChanged(DependencyObject * d, Core::Dependency::DependencyPropertyChangedEventArgs & e)
{
    static_cast<DockingManager*>(d)->OnLayoutItemTemplateSelectorChanged(e);
}

/// <summary>
/// Handles changes to the DocumentsSource property.
/// </summary>

void XUI::UI::DockingManager::OnDocumentsSourceChanged(DependencyObject * d, Core::Dependency::DependencyPropertyChangedEventArgs & e)
{
    static_cast<DockingManager*>(d)->OnDocumentsSourceChanged(e);
}

/// <summary>
/// Handles changes to the ActiveContent property.
/// </summary>

void XUI::UI::DockingManager::OnActiveContentChanged(DependencyObject * d, Core::Dependency::DependencyPropertyChangedEventArgs & e)
{
    static_cast<DockingManager*>(d)->InternalSetActiveContent(e.NewValue.ToObject());
    static_cast<DockingManager*>(d)->OnActiveContentChanged(e);
}

void XUI::UI::DockingManager::_ExecuteContentActivateCommand(LayoutContent * content)
{
    content->IsActive = true;
}

/// <summary>
/// Handles changes to the DocumentPaneMenuItemHeaderTemplate property.
/// </summary>

void XUI::UI::DockingManager::OnDocumentPaneMenuItemHeaderTemplateChanged(DependencyObject * d, Core::Dependency::DependencyPropertyChangedEventArgs & e)
{
    static_cast<DockingManager*>(d)->OnDocumentPaneMenuItemHeaderTemplateChanged(e);
}

/// <summary>
/// Provides derived classes an opportunity to handle changes to the DocumentPaneMenuItemHeaderTemplate property.
/// </summary>

void XUI::UI::DockingManager::OnDocumentPaneMenuItemHeaderTemplateChanged(Core::Dependency::DependencyPropertyChangedEventArgs & e)
{
}

/// <summary>
/// Coerces the DocumentPaneMenuItemHeaderTemplate value.
/// </summary>

System::Reflection::Variant XUI::UI::DockingManager::CoerceDocumentPaneMenuItemHeaderTemplateValue(DependencyObject* d, System::Reflection::Variant const & value)
{
    //if (value && d->GetValue(DocumentPaneMenuItemHeaderTemplateSelectorProperty))
    //    return null;
    //if (value == nullptr)
    //    return d.GetValue(DocumentHeaderTemplateProperty);

    return value;
}

void XUI::UI::DockingManager::ShowNavigatorWindow()
{
    //if (_navigatorWindow == nullptr)
    //{
    //    _navigatorWindow = new NavigatorWindow(this)
    //    {
    //        Owner = Window.GetWindow(this),
    //        WindowStartupLocation = WindowStartupLocation.CenterOwner
    //    };
    //}
    //
    //_navigatorWindow.ShowDialog();
    //_navigatorWindow = null;
}

void XUI::UI::DockingManager::DockingManager_Loaded(void * sender, System::Events::EventArg & e)
{
    {
        if (Layout->GetManager() == this)
        {
            LayoutRootPanel = CreateUIElementForModel(Layout->RootPanel.get())->shared_from_base_static<LayoutPanelControl>();
            LeftSidePanel = CreateUIElementForModel(Layout->LeftSide.get())->shared_from_base_static<LayoutAnchorSideControl>();
            TopSidePanel = CreateUIElementForModel(Layout->TopSide.get())->shared_from_base_static<LayoutAnchorSideControl>();
            RightSidePanel = CreateUIElementForModel(Layout->RightSide.get())->shared_from_base_static<LayoutAnchorSideControl>();
            BottomSidePanel = CreateUIElementForModel(Layout->BottomSide.get())->shared_from_base_static<LayoutAnchorSideControl>();
        }
    
        SetupAutoHideWindow();
    
        //load windows not already loaded!
        for (auto const& fw : Layout->FloatingWindows.GetNativeEnumerator())
        {
            bool sameModel = false;
            for (auto const& content : _fwList.GetNativeEnumerator())
            {
                if (content->GetModel().get() == fw.get())
                {
                    sameModel = true;
                    break;
                }
            }

            if (sameModel)
                continue;

            _fwList.Add(CreateUIElementForModel(fw.get())->shared_from_base_static<LayoutFloatingWindowControl>());
        }
    
        //create the overlaywindow if it's possible
        if (IsVisible)
            CreateOverlayWindow();

        FocusElementManager::SetupFocusManagement(this);
    }
}

void XUI::UI::DockingManager::DockingManager_Unloaded(void * sender, Controls::VisualTreeAttachmentEventArgs & e)
{
    {
        _autoHideWindowManager.HideAutoWindow();
    
        if (AutoHideWindow)
            AutoHideWindow = nullptr;
    
        for (auto const& fw : _fwList)
        {
            fw->Owner = nullptr;
            fw->KeepContentVisibleOnClose = true;
            fw->Close();
        }
    
        DestroyOverlayWindow();
        FocusElementManager::FinalizeFocusManagement(this);
    }
}

std::shared_ptr<Controls::Control> XUI::UI::DockingManager::CreateUIElementForModel(LayoutElement * model)
{
    if (model->Is<LayoutPanel>())
        return std::make_shared<LayoutPanelControl>(model->shared_from_base_static<LayoutPanel>());
    if (model->Is<LayoutAnchorablePaneGroup>())
        return std::make_shared<LayoutAnchorablePaneGroupControl>(model->Static_As<LayoutAnchorablePaneGroup>());
    if (model->Is<LayoutDocumentPaneGroup>())
        return std::make_shared<LayoutDocumentPaneGroupControl>(model->Static_As<LayoutDocumentPaneGroup>());
    
    if (model->Is<LayoutAnchorSide>())
    {
        auto templateModelView = std::make_shared<LayoutAnchorSideControl>();
        templateModelView->Initialize(model->Static_As<LayoutAnchorSide>());
        //templateModelView.SetBinding(LayoutAnchorSideControl??TemplateProperty, new Binding("AnchorSideTemplate"){ Source = this });
        return templateModelView;
    }
    if (model->Is<LayoutAnchorGroup>())
    {
        auto templateModelView = std::make_shared<LayoutAnchorGroupControl>(model->Static_As<LayoutAnchorGroup>());
        //templateModelView.SetBinding(LayoutAnchorGroupControl.TemplateProperty, new Binding("AnchorGroupTemplate"){ Source = this });
        return templateModelView;
    }
    
    if (model->Is<LayoutDocumentPane>())
    {
        auto templateModelView = std::make_shared<LayoutDocumentPaneControl>(model->shared_from_base_static<LayoutDocumentPane>());
        templateModelView->Initialize();
        //templateModelView.SetBinding(LayoutDocumentPaneControl.StyleProperty, new Binding("DocumentPaneControlStyle"){ Source = this });
        return templateModelView;
    }
    if (model->Is<LayoutAnchorablePane>())
    {
        auto templateModelView = std::make_shared<LayoutAnchorablePaneControl>(model->shared_from_base_static<LayoutAnchorablePane>());
        templateModelView->Initialize();
        //templateModelView.SetBinding(LayoutAnchorablePaneControl.StyleProperty, new Binding("AnchorablePaneControlStyle"){ Source = this });
        return templateModelView;
    }
    
    if (model->Is<LayoutAnchorableFloatingWindow>())
    {
        //if (DesignerProperties.GetIsInDesignMode(this))
        //    return null;
        auto modelFW = model->Static_As<LayoutAnchorableFloatingWindow>();
        auto newFW = std::make_shared<LayoutAnchorableFloatingWindowControl>(modelFW);
        auto pwnd = FindLogicalAncestor<Window>();
        if (pwnd)
            newFW->Owner = pwnd->shared_from_base_static<UI::Window>();
    
        LayoutAnchorablePane* paneForExtensions = static_cast<LayoutAnchorablePane*>(cpplinq::from(modelFW->RootPanel->Children.GetNativeEnumerator()).where([](std::shared_ptr<LayoutElement> const& x) { return x->Is<LayoutAnchorablePane>(); }).first_or_default().get());
        if (paneForExtensions)
        {
            //ensure that floating window position->Is<inside>() current (or nearest) monitor
            //paneForExtensions->KeepInsideNearestMonitor();
    
            newFW->Left = paneForExtensions->FloatingLeft;
            newFW->Top = paneForExtensions->FloatingTop;
            newFW->Width = paneForExtensions->FloatingWidth;
            newFW->Height = paneForExtensions->FloatingHeight;
        }
    
        //newFW->ShowInTaskbar = false;
        //newFW->Title = ;
        newFW->Show();

        // Do not set the WindowState before showing or it will be lost
        if (paneForExtensions && paneForExtensions->IsMaximized)
        {
            newFW->WindowState = WindowState::Maximized;
        }
        return newFW;
    }
    
    if (model->Is<LayoutDocumentFloatingWindow>())
    {
        //if (DesignerProperties.GetIsInDesignMode(this))
        //    return null;
        auto modelFW = model->Static_As<LayoutDocumentFloatingWindow>();
        auto newFW = std::make_shared<LayoutDocumentFloatingWindowControl>(modelFW);
        auto pwnd = FindLogicalAncestor<Window>();
        if (pwnd)
            newFW->Owner = pwnd->shared_from_base_static<UI::Window>();
    
        auto paneForExtensions = modelFW->RootDocument;
        if (paneForExtensions)
        {
            //ensure that floating window position->Is<inside>() current (or nearest) monitor
            //paneForExtensions->KeepInsideNearestMonitor();
    
            newFW->Left = paneForExtensions->FloatingLeft;
            newFW->Top = paneForExtensions->FloatingTop;
            newFW->Width = paneForExtensions->FloatingWidth;
            newFW->Height = paneForExtensions->FloatingHeight;
        }
    
        //newFW->ShowInTaskbar = false;
        newFW->Show();
        // Do not set the WindowState before showing or it will be lost
        if (paneForExtensions && paneForExtensions->IsMaximized)
        {
            newFW->WindowState = WindowState::Maximized;
        }
        return newFW;
    }
    
    if (model->Is<LayoutDocument>())
    {
        auto templateModelView = std::make_shared<LayoutDocumentControl>();
        templateModelView->Model = model->shared_from_base_static<LayoutDocument>();
        return templateModelView;
    }

    return nullptr;
}

/// <summary>
/// Return the LayoutItem wrapper for the content passed as argument
/// </summary>
/// <param name="content">LayoutContent to search</param>
/// <returns>Either a LayoutAnchorableItem or LayoutDocumentItem which contains the LayoutContent passed as argument</returns>

LayoutItem * XUI::UI::DockingManager::GetLayoutItemFromModel(LayoutContent * content)
{
    for (auto const& layout : _layoutItems)
    {
        if (layout->Element == content)
            return layout.get();
    }
    return nullptr;
}

void XUI::UI::DockingManager::ShowAutoHideWindow(LayoutAnchorControl * anchor)
{
    _autoHideWindowManager.ShowAutoHideWindow(anchor);
    //if (_autohideArea == nullptr)
    //    return;

    //if (AutoHideWindow && AutoHideWindow.Model == anchor.Model)
    //    return;

    //Trace.WriteLine("ShowAutoHideWindow()");

    //_currentAutohiddenAnchor = new WeakReference(anchor);

    //HideAutoHideWindow(anchor);

    //SetAutoHideWindow(new LayoutAutoHideWindowControl(anchor));
    //AutoHideWindow.Show();
}

void XUI::UI::DockingManager::HideAutoHideWindow(LayoutAnchorControl * anchor)
{
    _autoHideWindowManager.HideAutoWindow(anchor);
}

void XUI::UI::DockingManager::InternalAddLogicalChild(Controls::Control * element)
{
    if (element)
    {
        //#if DEBUG
        //            if (_logicalChildren.Select(ch = > ch.GetValueOrDefault<object>()).Contains(element))
        //                new InvalidOperationException();
        //#endif

        AddLogicalChild(element);
    }
}

void XUI::UI::DockingManager::InternalRemoveLogicalChild(Controls::Control * element)
{
    if (element)
    {
        //auto wrToRemove = _logicalChildren.;//FirstOrDefault(ch = > ch.GetValueOrDefault<object>() == element);
        //if (wrToRemove)
        //    _logicalChildren.Remove(wrToRemove);
        RemoveLogicalChild(element);
    }
}

void XUI::UI::DockingManager::OnTemplateApplied(Controls::TemplateAppliedEventArgs const& e)
{
    UI::Controls::Control::OnTemplateApplied(e);

    _overlayWindow = Template->FindNameByManagedType<OverlayWindow>("PART_OverlayWindow", this);
}

void XUI::UI::DockingManager::DetachLayoutItems()
{
    if (Layout)
    {
        for (auto const& i : _layoutItems)
        {
            LayoutItem* item = i->Static_As<LayoutItem>();
            if (item)
                item->Detach();
        }

        _layoutItems.clear();
        Layout->ElementAdded -= System::make_eventhandler(&DockingManager::Layout_ElementAdded, this);
        Layout->ElementRemoved -= System::make_eventhandler(&DockingManager::Layout_ElementRemoved, this);
    }
}

void XUI::UI::DockingManager::Layout_ElementRemoved(void * sender, LayoutElementEventArgs & e)
{
    if (_suspendLayoutItemCreation)
        return;
    
    CollectLayoutItemsDeleted();
}

void XUI::UI::DockingManager::Layout_ElementAdded(void * sender, LayoutElementEventArgs & e)
{
    if (_suspendLayoutItemCreation)
        return;
    
    LayoutContainer::ContainsChild(Layout.get(), [&](LayoutElement* content)
    {
        if (content->Is<LayoutDocument>())
            CreateDocumentLayoutItem(content->Static_As<LayoutDocument>());
        else if (content->Is<LayoutAnchorable>())
            CreateAnchorableLayoutItem(content->Static_As<LayoutAnchorable>());

        return false;
    });
    
    CollectLayoutItemsDeleted();
}

void XUI::UI::DockingManager::CollectLayoutItemsDeleted()
{
    if (_collectLayoutItemsOperations)
        return;

    _collectLayoutItemsOperations = true;

    Core::UIDispatcher::OnRender += [this]()
    {
        _collectLayoutItemsOperations = false;
        for (auto itr = _layoutItems.begin(); itr != _layoutItems.end();)
        {
            if (!(*itr) || (*itr)->Element->Root == Layout.get())
            {
                ++itr;
                continue;
            }
    
            //if ((*itr)->Model &&
            //    (*itr)->Model->Is<UIElement>())
            //{
            //    //((ILogicalChildrenContainer)this).InternalRemoveLogicalChild(itemToRemove.Model->Static_As<UIElement)>();
            //}
    
            (*itr)->Detach();
            _layoutItems.erase(itr++);
    
        }
    };
}

void XUI::UI::DockingManager::AttachLayoutItems()
{
    if (Layout)
    {
        LayoutContainer::ContainsChild(Layout.get(), [&](LayoutElement* doc)
        {
            if (auto document = doc->Static_As<LayoutDocument>())
                CreateDocumentLayoutItem(document);
            return false;
        });

        LayoutContainer::ContainsChild(Layout.get(), [&](LayoutElement* anc)
        {
            if (auto anchorable = anc->Static_As<LayoutAnchorable>())
                CreateAnchorableLayoutItem(anchorable);
            return false;
        });
    
        Layout->ElementAdded += System::make_eventhandler(&DockingManager::Layout_ElementAdded, this);
        Layout->ElementRemoved += System::make_eventhandler(&DockingManager::Layout_ElementRemoved, this);
    }
}

void XUI::UI::DockingManager::ApplyStyleToLayoutItem(LayoutItem* layoutItem)
{
    layoutItem->_ClearDefaultBindings();
    if (LayoutItemContainerStyle)
        layoutItem->Style = LayoutItemContainerStyle;
    //else if (LayoutItemContainerStyleSelector)
    //    layoutItem->.Style = LayoutItemContainerStyleSelector.SelectStyle(layoutItem.Model, layoutItem);
    layoutItem->_SetDefaultBindings();
}

void XUI::UI::DockingManager::CreateAnchorableLayoutItem(LayoutAnchorable* contentToAttach)
{
    bool hasValue = false;
    for (auto item : _layoutItems)
    {
        if (item->Element == contentToAttach)
        {
            hasValue = true;
            break;
        }
    }

    if (hasValue)
        return;
    
    auto layoutItem = std::make_shared<LayoutAnchorableItem>();
    layoutItem->Attach(contentToAttach);
    ApplyStyleToLayoutItem(layoutItem.get());
    _layoutItems.push_back(layoutItem);
    
    if (contentToAttach &&
        contentToAttach->Content &&
        contentToAttach->Content->Is<Controls::Control>())
    {
        InternalAddLogicalChild(contentToAttach->Content->Static_As<Controls::Control>());
    }
}

void XUI::UI::DockingManager::CreateDocumentLayoutItem(LayoutDocument * contentToAttach)
{
    bool hasValue = false;
    for (auto item : _layoutItems)
    {
        if (item->Element == contentToAttach)
        {
            hasValue = true;
            break;
        }
    }

    if (hasValue)
        return;
    
    auto layoutItem = std::make_shared<LayoutDocumentItem>();
    layoutItem->Attach(contentToAttach);
    ApplyStyleToLayoutItem(layoutItem.get());
    _layoutItems.push_back(layoutItem);
    
    if (contentToAttach &&
        contentToAttach->Content &&
        contentToAttach->Content->Is<Controls::Control>())
    {
        InternalAddLogicalChild(contentToAttach->Content->Static_As<Controls::Control>());
    }

}


/// <summary>
/// Provides derived classes an opportunity to handle changes to the ActiveContent property.
/// </summary>


/// <summary>
/// Provides derived classes an opportunity to handle changes to the Theme property.
/// </summary>

void XUI::UI::DockingManager::OnThemeChanged(Core::Dependency::DependencyPropertyChangedEventArgs & e)
{
    //auto oldTheme = e.OldValue->Static_As<Theme>();
    //auto newTheme = e.NewValue->Static_As<Theme>();
    //auto resources = this.Resources;
    //if (oldTheme)
    //{
    //    if (oldTheme->Is<DictionaryTheme>())
    //    {
    //        if (currentThemeResourceDictionary)
    //        {
    //            resources.MergedDictionaries.Remove(currentThemeResourceDictionary);
    //            currentThemeResourceDictionary = null;
    //        }
    //    }
    //    else
    //    {
    //        auto resourceDictionaryToRemove =
    //            resources.MergedDictionaries.FirstOrDefault(r = > r.Source == oldTheme.GetResourceUri());
    //        if (resourceDictionaryToRemove)
    //            resources.MergedDictionaries.Remove(
    //                resourceDictionaryToRemove);
    //    }
    //}
    //
    //if (newTheme)
    //{
    //    if (newTheme->Is<DictionaryTheme>())
    //    {
    //        currentThemeResourceDictionary = ((DictionaryTheme)newTheme).ThemeResourceDictionary;
    //        resources.MergedDictionaries.Add(currentThemeResourceDictionary);
    //    }
    //    else
    //    {
    //        resources.MergedDictionaries.Add(new ResourceDictionary(){ Source = newTheme.GetResourceUri() });
    //    }
    //}
    //
    //foreach(auto fwc in _fwList)
    //    fwc.UpdateThemeResources(oldTheme);
    //
    //if (_navigatorWindow)
    //    _navigatorWindow.UpdateThemeResources();
    //
    //if (_overlayWindow)
    //    _overlayWindow.UpdateThemeResources();
}

void XUI::UI::DockingManager::OnActiveContentChanged(Core::Dependency::DependencyPropertyChangedEventArgs & e)
{
    static System::Events::EventArg _e;
    if (_activeContentChanged)
		_activeContentChanged(this, _e);
}

void XUI::UI::DockingManager::InternalSetActiveContent(System::Reflection::Object * contentObject)
{
    LayoutContent* layoutContent = nullptr;
    LayoutContainer::ContainsChild(Layout.get(), [&layoutContent, &contentObject](LayoutElement* lc)
    {
        LayoutContent* ct = lc->Static_As<LayoutContent>();
        if (lc == contentObject || (ct->Content.get() == contentObject))
        {
            layoutContent = ct;
            return true;
        }
        return false;
    });

    _insideInternalSetActiveContent = true;
    Layout->ActiveContent = layoutContent;
    _insideInternalSetActiveContent = false;
}

void XUI::UI::DockingManager::RemoveViewFromLogicalChild(LayoutContent * layoutContent)
{
    if (layoutContent == nullptr)
        return;
    
    auto layoutItem = GetLayoutItemFromModel(layoutContent);
    if (layoutItem)
    {
        if (layoutItem->IsViewExists())
        {
            InternalRemoveLogicalChild(layoutItem->GetView());
        }
    }
}

/// <summary>
/// Provides derived classes an opportunity to handle changes to the DocumentsSource property.
/// </summary>

void XUI::UI::DockingManager::OnDocumentsSourceChanged(Core::Dependency::DependencyPropertyChangedEventArgs & e)
{
    DetachDocumentsSource(Layout.get(), e.OldValue.ToObject()->Static_As<System::Collection::Generic::IContainer>());
    AttachDocumentsSource(Layout.get(), e.NewValue.ToObject()->Static_As<System::Collection::Generic::IContainer>());
}

void XUI::UI::DockingManager::AttachDocumentsSource(LayoutRoot * layout, System::Collection::Generic::IContainer * documentsSource)
{
    if (documentsSource == nullptr)
        return;
    
    if (layout == nullptr)
        return;
    
    //if (layout.Descendents().OfType<LayoutDocument>().Any())
    //    throw new InvalidOperationException("Unable to set the DocumentsSource property if LayoutDocument objects are already present in the model");
    std::set<LayoutDocument*> documentsImported;
    LayoutContainer::ContainsChild(layout, [&documentsImported](LayoutElement* d)
    {
        auto c = d->Static_As<LayoutDocument>();
        if (c && c->Content)
            documentsImported.insert(c);
        return false;
    });

    std::set<Core::Dependency::DependencyObject*> listOfDocumentsToImport;

    for (auto itr = documentsSource->GetGenericEnumerator(); !itr->IsFinished(); ++(*itr))
    {
        LayoutDocument* element = itr->Current().ToObject()->Static_As<LayoutDocument>();
        if (!element || documentsImported.find(element) == documentsImported.end())
            listOfDocumentsToImport.insert(element);
    }

    LayoutDocumentPane* documentPane = nullptr;
    if (layout->LastFocusedDocument)
    {
        documentPane = layout->LastFocusedDocument->Parent->Static_As<LayoutDocumentPane>();
    }
    
    if (documentPane == nullptr)
    {
        LayoutContainer::ContainsChild(layout, [&documentPane](LayoutElement* d) -> bool { return documentPane = d->Static_As<LayoutDocumentPane>(); });
    }
    
    //if (documentPane == nullptr)
    //    throw new InvalidOperationException("Layout must contains at least one LayoutDocumentPane in order to host documents");
    
    _suspendLayoutItemCreation = true;
    for (auto const& documentContentToImport : listOfDocumentsToImport)
    {
    
        //documentPane.Children.Add(new LayoutDocument() { Content = documentToImport });
    
        auto documentToImport = std::make_shared<LayoutDocument>();
        documentToImport->Content = documentContentToImport->shared_from_base_static<System::Reflection::Object>();
    
        bool added = false;
        if (LayoutUpdateStrategy)
        {
            added = LayoutUpdateStrategy->BeforeInsertDocument(layout, documentToImport.get(), documentPane);
        }
    
        if (!added)
        {
            if (documentPane == nullptr)
                throw System::InvalidOperationException("Layout must contains at least one LayoutDocumentPane in order to host documents");
    
            documentPane->AddChild(documentToImport.get());
            added = true;
        }
    
        if (LayoutUpdateStrategy)
            LayoutUpdateStrategy->AfterInsertDocument(layout, documentToImport.get());
    
    
        CreateDocumentLayoutItem(documentToImport.get());
    
    }
    _suspendLayoutItemCreation = false;
    
    
    documentsSource->CollectionChanged += System::make_eventhandler(&DockingManager::documentsSourceElementsChanged, this);
}

void XUI::UI::DockingManager::documentsSourceElementsChanged(void * sender, System::Collection::NotifyCollectionChangedEventArgs & e)
{
    if (Layout == nullptr)
        return;
    
    //When deserializing documents are created automatically by the deserializer
    if (SuspendDocumentsSourceBinding)
        return;
    
    System::Collection::Generic::IContainer * container = GENERIC_SAFE_CAST(System::Collection::Generic::IContainer *, sender);

    //handle remove
    if (e.Action == System::Collection::NotifyCollectionChangedAction::Remove ||
        e.Action == System::Collection::NotifyCollectionChangedAction::Replace)
    {
        auto oldItem = container->ElementAt(e.OldStartingIndex);
        if (oldItem.Valid() && oldItem.IsObject())
        {
            LayoutContent* item = oldItem.ToObject()->Static_As<LayoutContent>();
            if (item && LayoutContainer::ContainsChild(Layout.get(), [item](LayoutElement* x)
            {
                return x->Is<LayoutDocument>() && static_cast<LayoutDocument*>(x)->Content.get() == item;
            }))
            {
                item->Parent->RemoveChild(item);
                RemoveViewFromLogicalChild(item);
            }
        }
    }
    
    //handle add
    if (e.Action == System::Collection::NotifyCollectionChangedAction::Add || e.Action == System::Collection::NotifyCollectionChangedAction::Replace)
    {
        auto newItem = container->ElementAt(e.NewStartingIndex);
        if (newItem.Valid() && newItem.IsObject())
        {
            LayoutDocumentPane* documentPane = nullptr;
            if (Layout->LastFocusedDocument)
            {
                documentPane = Layout->LastFocusedDocument->Parent->Static_As<LayoutDocumentPane>();
            }
    
            if (documentPane == nullptr)
            {
                LayoutContainer::ContainsChild(Layout.get(), [&documentPane](LayoutElement* d) -> bool { return documentPane = d->Static_As<LayoutDocumentPane>(); });
            }
    
            //if (documentPane == nullptr)
            //    throw new InvalidOperationException("Layout must contains at least one LayoutDocumentPane in order to host documents");
    
            _suspendLayoutItemCreation = true;
    
            {
                auto documentToImport = std::make_shared<LayoutDocument>();
                documentToImport->Content = newItem.ToSharedObject();
    
                bool added = false;
                if (LayoutUpdateStrategy)
                {
                    added = LayoutUpdateStrategy->BeforeInsertDocument(Layout.get(), documentToImport.get(), documentPane);
                }
    
                if (!added)
                {
                    if (documentPane == nullptr)
                        throw System::InvalidOperationException("Layout must contains at least one LayoutDocumentPane in order to host documents");
    
                    documentPane->AddChild(documentToImport.get());
                    added = true;
                }
    
                if (LayoutUpdateStrategy)
                {
                    LayoutUpdateStrategy->AfterInsertDocument(Layout.get(), documentToImport.get());
                }
    
    
                auto root = documentToImport->Root;
    
                if (root && root->GetManager() == this)
                {
                    CreateDocumentLayoutItem(documentToImport.get());
                }
            }
            _suspendLayoutItemCreation = false;
        }
    }
    
    if (e.Action == System::Collection::NotifyCollectionChangedAction::Reset)
    {
        //NOTE: I'm going to clear every document present in layout but
        //some documents may have been added directly to the layout, for now I clear them too
        LayoutContainer::ContainsChild(Layout.get(), [this](LayoutElement* x)
        {
            LayoutDocument* anchorableToRemove = x->Static_As<LayoutDocument>();
            if (anchorableToRemove)
            {
                anchorableToRemove->Parent->RemoveChild(anchorableToRemove);
                RemoveViewFromLogicalChild(anchorableToRemove);
            }

            return false;
        });
    }
    
    if (Layout)
    {
        Layout->CollectGarbage();
    }
}

void XUI::UI::DockingManager::DetachDocumentsSource(LayoutRoot * layout, System::Collection::Generic::IContainer * documentsSource)
{
    if (documentsSource == nullptr)
        return;
    
    if (layout == nullptr)
        return;
    
    std::set<LayoutDocument*> documentsToRemove;
    LayoutContainer::ContainsChild(layout, [&](LayoutElement* d)
    {
        auto c = d->Static_As<LayoutDocument>();
        if (c && documentsSource->Contains(c->Content))
            documentsToRemove.insert(c);
        return false;
    });

   
    for (auto documentToRemove : documentsToRemove)
    {
        documentToRemove->Parent->RemoveChild(documentToRemove);
        RemoveViewFromLogicalChild(documentToRemove);
    }
    
    documentsSource->CollectionChanged -= System::make_eventhandler(&DockingManager::documentsSourceElementsChanged, this);
}

void XUI::UI::DockingManager::Close(LayoutContent * contentToClose)
{
    if (!contentToClose->CanClose)
        return;
    
    auto layoutItem = GetLayoutItemFromModel(contentToClose);
    if (layoutItem->CloseCommand)
    {
        if (layoutItem->CloseCommand->CanExecute(nullptr, this))
            layoutItem->CloseCommand->Execute(nullptr, this);
    }
    else
    {
        if (contentToClose->Is<LayoutDocument>())
            _ExecuteCloseCommand(contentToClose->Static_As<LayoutDocument>());
        else if (contentToClose->Is<LayoutAnchorable>())
            _ExecuteCloseCommand(contentToClose->Static_As<LayoutAnchorable>());
    }
}

void XUI::UI::DockingManager::SetupAutoHideWindow()
{
    _autohideArea = Template->FindNameByManagedType<Controls::Control>("PART_AutoHideArea", this);
    _autoHideWindowManager.HideAutoWindow();

    if (AutoHideWindow)
        AutoHideWindow = nullptr;

    SetAutoHideWindow(std::make_shared<LayoutAutoHideWindowControl>());
}

void DockingManager::ClearLogicalChildrenList()
{
    for (auto itr = LogicalChildren.begin(); itr != LogicalChildren.end();)
        LogicalChildren.Erase(itr++);
}

void DockingManager::OnLayoutChanged(LayoutRoot* oldLayout, LayoutRoot* newLayout)
{
    if (oldLayout)
    {
        oldLayout->PropertyChanged -= System::make_eventhandler(&DockingManager::OnLayoutRootPropertyChanged, this);
        oldLayout->Updated -= System::make_eventhandler(&DockingManager::OnLayoutRootUpdated, this);
    }

    for (auto& fwc : _fwList)
    {
        fwc->KeepContentVisibleOnClose = true;
        fwc->InternalClose();
    }

    _fwList.Clear();

    DetachDocumentsSource(oldLayout, DocumentsSource.get());
    DetachAnchorablesSource(oldLayout, AnchorablesSource.get());

    if (oldLayout &&
        oldLayout->GetManager() == this)
        oldLayout->SetManager(nullptr);

    ClearLogicalChildrenList();
    DetachLayoutItems();

    auto _layout = Layout;
    newLayout->Initialize();
    _layout->SetManager(this);

    AttachLayoutItems();
    AttachDocumentsSource(newLayout, DocumentsSource.get());
    AttachAnchorablesSource(newLayout, AnchorablesSource.get());

    if (IsAttachedToLogicalTree)
    {
        LayoutRootPanel = CreateUIElementForModel(_layout->RootPanel.get())->shared_from_base_static<LayoutPanelControl>();
        LeftSidePanel = CreateUIElementForModel(_layout->LeftSide.get())->shared_from_base_static<LayoutAnchorSideControl>();
        TopSidePanel = CreateUIElementForModel(_layout->TopSide.get())->shared_from_base_static<LayoutAnchorSideControl>();
        RightSidePanel = CreateUIElementForModel(_layout->RightSide.get())->shared_from_base_static<LayoutAnchorSideControl>();
        BottomSidePanel = CreateUIElementForModel(_layout->BottomSide.get())->shared_from_base_static<LayoutAnchorSideControl>();

        for (auto fw : _layout->FloatingWindows)
        {
            if (fw->IsValid())
                _fwList.Add(CreateUIElementForModel(fw.get())->shared_from_base_static<LayoutFloatingWindowControl>());
        }

        for (auto fw : _fwList)
        {
            auto owner = FindLogicalAncestor<Window>();
            if (owner)
                fw->Owner = owner->shared_from_base_static<Window>();
        }
    }


    if (newLayout)
    {
        newLayout->PropertyChanged += System::make_eventhandler(&DockingManager::OnLayoutRootPropertyChanged, this);
        newLayout->Updated += System::make_eventhandler(&DockingManager::OnLayoutRootUpdated, this);
    }

    if (LayoutChanged)
    {
        System::Events::EventArg e;
        LayoutChanged(this, e);
    }

    //if (Layout)
    //    Layout->CollectGarbage();

    Core::Observer::RoutedCommand::InvalidateRequerySuggested();
}

void XUI::UI::DockingManager::OnLayoutRootPropertyChanged(void * sender, XamlCPP::Core::PropertyChangedEventArgs & e)
{
    if (e.PropertyName == "RootPanel")
    {
        if (IsInitialized)
            LayoutRootPanel = CreateUIElementForModel(Layout->RootPanel.get())->shared_from_base_static<LayoutPanelControl>();
    }
    else if (e.PropertyName == "ActiveContent")
    {
        if (Layout->ActiveContent)
        {
            //Debug.WriteLine(new StackTrace().ToString());

            //set focus on active element only after a layout pass is completed
            //it's possible that it is not yet visible in the visual tree
            //if (_setFocusAsyncOperation == null)
            //{
            //    _setFocusAsyncOperation = Dispatcher.BeginInvoke(new Action(() =>
            // {
            if (Layout->ActiveContent)
                FocusElementManager::SetFocusOnLastElement(Layout->ActiveContent);
            //_setFocusAsyncOperation = null;
            //  } ), DispatcherPriority.Input );
            //}
        }

        //if (!_insideInternalSetActiveContent)
        //    ActiveContent = Layout->ActiveContent != null ?
        //        Layout->ActiveContent.Content : null;
        if (!_insideInternalSetActiveContent && Layout->ActiveContent)
        {
            ActiveContent = Layout->ActiveContent->Content;
        }
    }
}

void XUI::UI::DockingManager::OnLayoutRootUpdated(void * sender, System::Events::EventArg & e)
{
    Core::Observer::RoutedCommand::InvalidateRequerySuggested();
}
