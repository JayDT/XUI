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

#ifndef __XUI_UI_LAYOUT_DOCK_MANAGER_H__
#define __XUI_UI_LAYOUT_DOCK_MANAGER_H__

#include "UI/ContextMenu.h"
#include "UI/Controls/Control.h"
#include "UI/LayoutDock/Controls/AutoHideWindowManager.h"
#include "UI/LayoutDock/DockingDefines.h"
#include "UI/LayoutDock/DockingEventArgs.h"
#include "UI/LayoutDock/Layouts/LayoutRoot.h"
#include "UI/LayoutDock/Controls/LayoutItem.h"
#include "UI/LayoutDock/Controls/LayoutAnchorControl.h"
#include "UI/LayoutDock/Controls/LayoutAnchorGroupControl.h"
#include "UI/LayoutDock/Controls/LayoutFloatingWindowControl.h"
#include "UI/LayoutDock/Controls/LayoutPanelControl.h"
#include "UI/LayoutDock/Controls/LayoutAnchorSideControl.h"
#include "UI/LayoutDock/Controls/LayoutAutoHideWindowControl.h"

MetaAssemblyBegin
Meta(modul: ReflInclude("UI/LayoutDock/Layouts/LayoutDocument.h"))
Meta(modul: ReflInclude("UI/LayoutDock/Layouts/LayoutAnchorablePane.h"))
Meta(modul: ReflInclude("UI/LayoutDock/Controls/OverlayWindow.h"))
MetaAssemblyEnd

namespace XUI::UI
{
    class LayoutAnchorablePane;
    class LayoutContent;
    class LayoutAnchorControl;
    class LayoutFloatingWindowControl;
    class LayoutPanelControl;
    class LayoutAnchorSideControl;
    class LayoutAutoHideWindowControl;
    class LayoutAnchorGroupControl;
    class OverlayWindow;

    class DockingManager : public UI::Controls::Control, public IOverlayWindowHost
    {
    private:

		System::Events::EventHandler<System::Events::EventArg> _activeContentChanged;

        System::Collection::List<std::shared_ptr<LayoutFloatingWindowControl>> _fwList;
        std::list<std::shared_ptr<IDropArea>> _areas;
        std::shared_ptr<OverlayWindow> _overlayWindow;
        std::list<std::shared_ptr<LayoutItem>> _layoutItems;
        //DispatcherOperation _collectLayoutItemsOperations = null;
        //NavigatorWindow _navigatorWindow = null;
        AutoHideWindowManager _autoHideWindowManager;
        std::shared_ptr<Controls::Control> _autohideArea;
        bool _suspendLayoutItemCreation = false;
        bool SuspendDocumentsSourceBinding = false;
        bool SuspendAnchorablesSourceBinding = false;
        bool _insideInternalSetActiveContent = false;
        bool _collectLayoutItemsOperations = false;

    public:
        static Core::Dependency::RefDependencyProperty LayoutProperty;
        static Core::Dependency::RefDependencyProperty LayoutUpdateStrategyProperty;
        static Core::Dependency::RefDependencyProperty DocumentPaneTemplateProperty;
        static Core::Dependency::RefDependencyProperty AnchorablePaneTemplateProperty;
        static Core::Dependency::RefDependencyProperty AnchorSideTemplateProperty;
        static Core::Dependency::RefDependencyProperty AnchorGroupTemplateProperty;
        static Core::Dependency::RefDependencyProperty AnchorTemplateProperty;
        static Core::Dependency::RefDependencyProperty DocumentPaneControlStyleProperty;
        static Core::Dependency::RefDependencyProperty AnchorablePaneControlStyleProperty;
        static Core::Dependency::RefDependencyProperty DocumentHeaderTemplateProperty;
        static Core::Dependency::RefDependencyProperty DocumentHeaderTemplateSelectorProperty;
        static Core::Dependency::RefDependencyProperty DocumentTitleTemplateProperty;
        static Core::Dependency::RefDependencyProperty DocumentTitleTemplateSelectorProperty;
        static Core::Dependency::RefDependencyProperty AnchorableTitleTemplateProperty;
        static Core::Dependency::RefDependencyProperty AnchorableTitleTemplateSelectorProperty;
        static Core::Dependency::RefDependencyProperty AnchorableHeaderTemplateProperty;
        static Core::Dependency::RefDependencyProperty AnchorableHeaderTemplateSelectorProperty;
        static Core::Dependency::RefDependencyProperty LayoutRootPanelProperty;
        static Core::Dependency::RefDependencyProperty RightSidePanelProperty;
        static Core::Dependency::RefDependencyProperty LeftSidePanelProperty;
        static Core::Dependency::RefDependencyProperty TopSidePanelProperty;
        static Core::Dependency::RefDependencyProperty BottomSidePanelProperty;
        static Core::Dependency::RefDependencyProperty AutoHideWindowProperty;
        static Core::Dependency::RefDependencyProperty LayoutItemTemplateProperty;
        static Core::Dependency::RefDependencyProperty LayoutItemTemplateSelectorProperty;
        static Core::Dependency::RefDependencyProperty DocumentsSourceProperty;
        static Core::Dependency::RefDependencyProperty DocumentContextMenuProperty;
        static Core::Dependency::RefDependencyProperty AnchorablesSourceProperty;
        static Core::Dependency::RefDependencyProperty ActiveContentProperty;
        static Core::Dependency::RefDependencyProperty AnchorableContextMenuProperty;
        static Core::Dependency::RefDependencyProperty ThemeProperty;
        static Core::Dependency::RefDependencyProperty GridSplitterWidthProperty;
        static Core::Dependency::RefDependencyProperty GridSplitterHeightProperty;
        static Core::Dependency::RefDependencyProperty DocumentPaneMenuItemHeaderTemplateProperty;
        static Core::Dependency::RefDependencyProperty DocumentPaneMenuItemHeaderTemplateSelectorProperty;
        static Core::Dependency::RefDependencyProperty IconContentTemplateProperty;
        static Core::Dependency::RefDependencyProperty IconContentTemplateSelectorProperty;
        static Core::Dependency::RefDependencyProperty LayoutItemContainerStyleProperty;
        static Core::Dependency::RefDependencyProperty LayoutItemContainerStyleSelectorProperty;
        static Core::Dependency::RefDependencyProperty ShowSystemMenuProperty;
        static Core::Dependency::RefDependencyProperty AllowMixedOrientationProperty;

        static void StaticClassInitializer();

        DockingManager();
        virtual ~DockingManager();

        System::Events::EventHandler<System::Events::EventArg> LayoutChanged;
        System::Events::EventHandler<System::Events::EventArg> LayoutChanging;
        System::Events::EventHandler<UI::DocumentCloseEventArgs> DocumentClosing;
        System::Events::EventHandler<UI::DocumentCloseEventArgs> DocumentClosed;

        Meta(XamlCPP::EventAttribute())
        Meta(XamlCPP::TypeConverterAttribute(typeid(XamlCPP::Core::ReflEx::TypeConversion::EventHookInstanceValueConverter)))
		System::Events::PropertyEvent<System::Events::EventArg> ActiveContentChanged;

		Meta(XamlCPP::ContentAttribute())
        PROPERTY_(std::shared_ptr<LayoutRoot>, Layout);
        std::shared_ptr<LayoutRoot> GetLayout() const;
        void SetLayout(std::shared_ptr<LayoutRoot> const& value);
        DEPENDENCY_PROPERTY_(std::shared_ptr<ILayoutUpdateStrategy>, LayoutUpdateStrategy)
        DEPENDENCY_PROPERTY_(std::shared_ptr<XUI::UI::DataTemplates::ControlTemplate>, DocumentPaneTemplate)
        DEPENDENCY_PROPERTY_(std::shared_ptr<XUI::UI::DataTemplates::ControlTemplate>, AnchorablePaneTemplate)
        DEPENDENCY_PROPERTY_(std::shared_ptr<XUI::UI::DataTemplates::ControlTemplate>, AnchorSideTemplate)
        DEPENDENCY_PROPERTY_(std::shared_ptr<XUI::UI::DataTemplates::ControlTemplate>, AnchorGroupTemplate)
        DEPENDENCY_PROPERTY_(std::shared_ptr<XUI::UI::DataTemplates::ControlTemplate>, AnchorTemplate)
        DEPENDENCY_PROPERTY_(std::shared_ptr<XUI::Styling::Style>, DocumentPaneControlStyle)
        DEPENDENCY_PROPERTY_(std::shared_ptr<XUI::Styling::Style>, AnchorablePaneControlStyle)
        DEPENDENCY_PROPERTY_(std::shared_ptr<XUI::UI::DataTemplates::DataTemplate>, DocumentHeaderTemplate)
        //DEPENDENCY_PROPERTY_(bool, DocumentHeaderTemplateSelector)
        DEPENDENCY_PROPERTY_(std::shared_ptr<XUI::UI::DataTemplates::DataTemplate>, DocumentTitleTemplate)
        //DEPENDENCY_PROPERTY_(bool, DocumentTitleTemplateSelector)
        DEPENDENCY_PROPERTY_(std::shared_ptr<XUI::UI::DataTemplates::DataTemplate>, AnchorableTitleTemplate)
        //DEPENDENCY_PROPERTY_(bool, AnchorableTitleTemplateSelector)
        DEPENDENCY_PROPERTY_(std::shared_ptr<XUI::UI::DataTemplates::DataTemplate>, AnchorableHeaderTemplate)
        //DEPENDENCY_PROPERTY_(bool, AnchorableHeaderTemplateSelector)
        DEPENDENCY_PROPERTY_(std::shared_ptr<LayoutPanelControl>, LayoutRootPanel)
        DEPENDENCY_PROPERTY_(std::shared_ptr<LayoutAnchorSideControl>, RightSidePanel)
        DEPENDENCY_PROPERTY_(std::shared_ptr<LayoutAnchorSideControl>, LeftSidePanel)
        DEPENDENCY_PROPERTY_(std::shared_ptr<LayoutAnchorSideControl>, TopSidePanel)
        DEPENDENCY_PROPERTY_(std::shared_ptr<LayoutAnchorSideControl>, BottomSidePanel)
        DEPENDENCY_PROPERTY_(std::shared_ptr<LayoutAutoHideWindowControl>, AutoHideWindow)
        DEPENDENCY_PROPERTY_(std::shared_ptr<XUI::UI::DataTemplates::DataTemplate>, LayoutItemTemplate)
        //DEPENDENCY_PROPERTY_(bool, LayoutItemTemplateSelector)
        DEPENDENCY_PROPERTY_(std::shared_ptr<System::Collection::Generic::IContainer>, DocumentsSource)
        DEPENDENCY_PROPERTY_(std::shared_ptr<UI::ContextMenu>, DocumentContextMenu)
        DEPENDENCY_PROPERTY_(std::shared_ptr<System::Collection::Generic::IContainer>, AnchorablesSource)
        DEPENDENCY_PROPERTY_(System::Reflection::RefObject, ActiveContent)
        DEPENDENCY_PROPERTY_(std::shared_ptr<UI::ContextMenu>, AnchorableContextMenu)
        DEPENDENCY_PROPERTY_(bool, Theme)
        DEPENDENCY_PROPERTY_(double, GridSplitterWidth)
        DEPENDENCY_PROPERTY_(double, GridSplitterHeight)
        DEPENDENCY_PROPERTY_(std::shared_ptr<XUI::UI::DataTemplates::DataTemplate>, DocumentPaneMenuItemHeaderTemplate)
        //DEPENDENCY_PROPERTY_(bool, DocumentPaneMenuItemHeaderTemplateSelector)
        DEPENDENCY_PROPERTY_(std::shared_ptr<XUI::UI::DataTemplates::DataTemplate>, IconContentTemplate)
        //DEPENDENCY_PROPERTY_(bool, IconContentTemplateSelector)
        DEPENDENCY_PROPERTY_(std::shared_ptr<XUI::Styling::Style>, LayoutItemContainerStyle)
        //DEPENDENCY_PROPERTY_(bool, LayoutItemContainerStyleSelector)
        DEPENDENCY_PROPERTY_(bool, ShowSystemMenu)
        DEPENDENCY_PROPERTY_(bool, AllowMixedOrientation);

        bool IsNavigatorWindowActive() { return false; /* _navigatorWindow; */ }
        System::Collection::List<std::shared_ptr<LayoutFloatingWindowControl>>& GetFloatingWindows() { return _fwList; }
        AutoHideWindowManager& GetautoHideWindowManager() { return _autoHideWindowManager; }
        Controls::Control* GetAutoHideAreaElement() { return _autohideArea.get(); }
        DockingManager* GetManager() override { return this; }

        void EndInit() override;

        void StartDraggingFloatingWindowForContent(LayoutContent* contentModel, bool startDrag = true);

        void StartDraggingFloatingWindowForPane(LayoutAnchorablePane* paneModel);

        std::vector<LayoutFloatingWindowControl*> GetFloatingWindowsByZOrder();

        void RemoveFloatingWindow(LayoutFloatingWindowControl* floatingWindow);

        bool HitTest(Core::Media::Point const& dragPoint);

        void CreateOverlayWindow();

        void DestroyOverlayWindow();

        OverlayWindow* ShowOverlayWindow(LayoutFloatingWindowControl* draggingWindow);

        void HideOverlayWindow();

        std::list<std::shared_ptr<IDropArea>> const& GetDropAreas(LayoutFloatingWindowControl* draggingWindow);

        Core::Media::Size ArrangeOverride(Core::Media::Size const& arrangeBounds) override;

        void _ExecuteCloseCommand(LayoutDocument* document);

        void _ExecuteCloseAllButThisCommand(LayoutContent* contentSelected);

        void _ExecuteCloseAllCommand(LayoutContent* contentSelected);

        /// <summary>
        /// Handles changes to the AnchorablesSource property.
        /// </summary>
        static void OnAnchorablesSourceChanged(DependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs & e);

        /// <summary>
        /// Provides derived classes an opportunity to handle changes to the AnchorablesSource property.
        /// </summary>
        virtual void OnAnchorablesSourceChanged(Core::Dependency::DependencyPropertyChangedEventArgs & e);

        void AttachAnchorablesSource(LayoutRoot* layout, System::Collection::Generic::IContainer* anchorablesSource);

        void anchorablesSourceElementsChanged(void* sender, System::Collection::NotifyCollectionChangedEventArgs& e);

        void DetachAnchorablesSource(LayoutRoot* layout, System::Collection::Generic::IContainer* anchorablesSource);

        void _ExecuteCloseCommand(LayoutAnchorable* anchorable);

        void _ExecuteHideCommand(LayoutAnchorable* anchorable);

        void _ExecuteAutoHideCommand(LayoutAnchorable* _anchorable);

        void _ExecuteFloatCommand(LayoutContent* contentToFloat);

        void _ExecuteDockCommand(LayoutAnchorable* anchorable);

        void _ExecuteDockAsDocumentCommand(LayoutContent* content);

        /// <summary>
        /// Handles changes to the AnchorableTemplate property.
        /// </summary>
        static void OnLayoutItemTemplateChanged(DependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e);

        /// <summary>
        /// Handles changes to the LayoutItemTemplateSelector property.
        /// </summary>
        static void OnLayoutItemTemplateSelectorChanged(DependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e);

        /// <summary>
        /// Handles changes to the DocumentsSource property.
        /// </summary>
        static void OnDocumentsSourceChanged(DependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e);

        /// <summary>
        /// Handles changes to the ActiveContent property.
        /// </summary>
        static void OnActiveContentChanged(DependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e);

        void _ExecuteContentActivateCommand(LayoutContent* content);

        /// <summary>
        /// Handles changes to the DocumentPaneMenuItemHeaderTemplate property.
        /// </summary>
        static void OnDocumentPaneMenuItemHeaderTemplateChanged(DependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e);

        /// <summary>
        /// Provides derived classes an opportunity to handle changes to the DocumentPaneMenuItemHeaderTemplate property.
        /// </summary>
        virtual void OnDocumentPaneMenuItemHeaderTemplateChanged(Core::Dependency::DependencyPropertyChangedEventArgs& e);

        /// <summary>
        /// Coerces the DocumentPaneMenuItemHeaderTemplate value.
        /// </summary>
        static System::Reflection::Variant CoerceDocumentPaneMenuItemHeaderTemplateValue(DependencyObject* d, System::Reflection::Variant const& value);

        /// <summary>
        /// Handles changes to the LayoutItemContainerStyle property.
        /// </summary>
        static void OnLayoutItemContainerStyleChanged(DependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
        {
            static_cast<DockingManager*>(d)->OnLayoutItemContainerStyleChanged(e);
        }

        /// <summary>
        /// Provides derived classes an opportunity to handle changes to the LayoutItemContainerStyle property.
        /// </summary>
        virtual void OnLayoutItemContainerStyleChanged(Core::Dependency::DependencyPropertyChangedEventArgs& e)
        {
            AttachLayoutItems();
        }

        void ShowNavigatorWindow();

        void DockingManager_Loaded(void* sender, System::Events::EventArg& e);

        void DockingManager_Unloaded(void* sender, Controls::VisualTreeAttachmentEventArgs& e);

        std::shared_ptr<Controls::Control> CreateUIElementForModel(LayoutElement* model);

        /// <summary>
        /// Return the LayoutItem wrapper for the content passed as argument
        /// </summary>
        /// <param name="content">LayoutContent to search</param>
        /// <returns>Either a LayoutAnchorableItem or LayoutDocumentItem which contains the LayoutContent passed as argument</returns>
        LayoutItem* GetLayoutItemFromModel(LayoutContent* content);

        void ShowAutoHideWindow(LayoutAnchorControl* anchor);

        void HideAutoHideWindow(LayoutAnchorControl* anchor);

        void InternalAddLogicalChild(Controls::Control* element);

        void InternalRemoveLogicalChild(Controls::Control* element);

    protected:

        //void OnPreviewKeyDown(Interfaces::KeyEventArgs& e) override
        //{
        //    //if (Keyboard.IsKeyDown(Key.LeftCtrl) || Keyboard.IsKeyDown(Key.RightCtrl))
        //    //{
        //    //    if (e.IsDown && e.Key == Key.Tab)
        //    //    {
        //    //        if (!IsNavigatorWindowActive)
        //    //        {
        //    //            ShowNavigatorWindow();
        //    //            e.Handled = true;
        //    //        }
        //    //    }
        //    //}
        //    //
        //    //base.OnPreviewKeyDown(e);
        //}

        void OnTemplateApplied(Controls::TemplateAppliedEventArgs const& e) override;

        void DetachLayoutItems();

        void Layout_ElementRemoved(void* sender, LayoutElementEventArgs& e);

        void Layout_ElementAdded(void* sender, LayoutElementEventArgs& e);

        void CollectLayoutItemsDeleted();

        void AttachLayoutItems();

        void ApplyStyleToLayoutItem(LayoutItem* layoutItem);

        void CreateAnchorableLayoutItem(LayoutAnchorable* contentToAttach);

        void CreateDocumentLayoutItem(LayoutDocument* contentToAttach);

        /// <summary>
        /// Provides derived classes an opportunity to handle changes to the Theme property.
        /// </summary>
        virtual void OnThemeChanged(Core::Dependency::DependencyPropertyChangedEventArgs& e);

        /// <summary>
        /// Provides derived classes an opportunity to handle changes to the ActiveContent property.
        /// </summary>
        virtual void OnActiveContentChanged(Core::Dependency::DependencyPropertyChangedEventArgs& e);


        void InternalSetActiveContent(System::Reflection::Object* contentObject);

        void RemoveViewFromLogicalChild(LayoutContent* layoutContent);

        /// <summary>
        /// Provides derived classes an opportunity to handle changes to the AnchorableTemplate property.
        /// </summary>
        virtual void OnLayoutItemTemplateChanged(Core::Dependency::DependencyPropertyChangedEventArgs& e)
        {
        }

        /// <summary>
        /// Provides derived classes an opportunity to handle changes to the LayoutItemTemplateSelector property.
        /// </summary>
        virtual void OnLayoutItemTemplateSelectorChanged(Core::Dependency::DependencyPropertyChangedEventArgs& e)
        {
        }

        /// <summary>
        /// Provides derived classes an opportunity to handle changes to the DocumentsSource property.
        /// </summary>
        virtual void OnDocumentsSourceChanged(Core::Dependency::DependencyPropertyChangedEventArgs& e);

        void AttachDocumentsSource(LayoutRoot* layout, System::Collection::Generic::IContainer* documentsSource);

        void documentsSourceElementsChanged(void* sender, System::Collection::NotifyCollectionChangedEventArgs& e);

        void DetachDocumentsSource(LayoutRoot* layout, System::Collection::Generic::IContainer* documentsSource);

        void Close(LayoutContent* contentToClose);

        void SetupAutoHideWindow();

        void ClearLogicalChildrenList();

        /// <summary>
        /// Provides derived classes an opportunity to handle changes to the <see cref="DockingManager.Layout"/> property.
        /// </summary>
        virtual void OnLayoutChanged(LayoutRoot* oldLayout, LayoutRoot* newLayout);

        void OnLayoutRootPropertyChanged(void* sender, XamlCPP::Core::PropertyChangedEventArgs& e);

        void OnLayoutRootUpdated(void* sender, System::Events::EventArg& e);
    };
}

#endif //!__XUI_UI_LAYOUT_DOCK_MANAGER_H__