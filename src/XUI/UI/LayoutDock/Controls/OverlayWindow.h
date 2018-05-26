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

#ifndef __XUI_UI_LAYOUT_DOCK_LAYOUT_OVERLAY_WINDOW_CONTROL_H__
#define __XUI_UI_LAYOUT_DOCK_LAYOUT_OVERLAY_WINDOW_CONTROL_H__

#include "UI/Window.h"
#include "UI/Canvas.h"
#include "UI/Grid.h"
#include "UI/Shapes/Path.h"
#include "UI/LayoutDock/DockingEventArgs.h"

MetaAssemblyBegin
Meta(modul: ReflInclude("UI/LayoutDock/DockingManager.h"))
Meta(modul: ReflInclude("UI/LayoutDock/Controls/LayoutFloatingWindowControl.h"))
Meta(modul: ReflInclude("UI/LayoutDock/Controls/DropTargetBase.h"))
MetaAssemblyEnd

namespace XUI::UI
{
    class Theme;
    class DropTargetBase;
    class LayoutGroupBase;
    class LayoutContent;

    class OverlayWindow : public Controls::Control
    {
    private:

        std::shared_ptr<IOverlayWindowHost> _host;

        std::shared_ptr<Canvas> _mainCanvasPanel;
        std::shared_ptr<Grid> _gridDockingManagerDropTargets;
        std::shared_ptr<Grid> _gridAnchorablePaneDropTargets;
        std::shared_ptr<Grid> _gridDocumentPaneDropTargets;
        std::shared_ptr<Grid> _gridDocumentPaneFullDropTargets;

        std::shared_ptr<Controls::Control> _dockingManagerDropTargetBottom;
        std::shared_ptr<Controls::Control> _dockingManagerDropTargetTop;
        std::shared_ptr<Controls::Control> _dockingManagerDropTargetLeft;
        std::shared_ptr<Controls::Control> _dockingManagerDropTargetRight;

        std::shared_ptr<Controls::Control> _anchorablePaneDropTargetBottom;
        std::shared_ptr<Controls::Control> _anchorablePaneDropTargetTop;
        std::shared_ptr<Controls::Control> _anchorablePaneDropTargetLeft;
        std::shared_ptr<Controls::Control> _anchorablePaneDropTargetRight;
        std::shared_ptr<Controls::Control> _anchorablePaneDropTargetInto;

        std::shared_ptr<Controls::Control> _documentPaneDropTargetBottom;
        std::shared_ptr<Controls::Control> _documentPaneDropTargetTop;
        std::shared_ptr<Controls::Control> _documentPaneDropTargetLeft;
        std::shared_ptr<Controls::Control> _documentPaneDropTargetRight;
        std::shared_ptr<Controls::Control> _documentPaneDropTargetInto;

        std::shared_ptr<Controls::Control> _documentPaneDropTargetBottomAsAnchorablePane;
        std::shared_ptr<Controls::Control> _documentPaneDropTargetTopAsAnchorablePane;
        std::shared_ptr<Controls::Control> _documentPaneDropTargetLeftAsAnchorablePane;
        std::shared_ptr<Controls::Control> _documentPaneDropTargetRightAsAnchorablePane;

        std::shared_ptr<Controls::Control> _documentPaneFullDropTargetBottom;
        std::shared_ptr<Controls::Control> _documentPaneFullDropTargetTop;
        std::shared_ptr<Controls::Control> _documentPaneFullDropTargetLeft;
        std::shared_ptr<Controls::Control> _documentPaneFullDropTargetRight;
        std::shared_ptr<Controls::Control> _documentPaneFullDropTargetInto;

        std::shared_ptr<Shapes::Path> _previewBox;

        std::shared_ptr<LayoutFloatingWindowControl> _floatingWindow;

        std::list<std::shared_ptr<IDropArea>> _visibleAreas;

    public:

        static void StaticClassInitializer();

        OverlayWindow();
        virtual ~OverlayWindow();


        void EnableDropTargets()
        {
            if (_mainCanvasPanel)
                _mainCanvasPanel->IsVisible = true;
        }

        void HideDropTargets()
        {
            if (_mainCanvasPanel)
                _mainCanvasPanel->IsVisible = true;

        }

        void DragEnter(LayoutFloatingWindowControl *floatingWindow);
        void DragLeave(LayoutFloatingWindowControl *floatingWindow);

        void DragEnter(IDropArea *area);
        void DragLeave(IDropArea *area);
        void DragEnter(DropTargetBase *target);
        void DragLeave(DropTargetBase *target);
        void DragDrop(DropTargetBase *target);

        void OnClosing(CancelEventArgs& e) /*override*/;

        void SetFloatingWindow(LayoutFloatingWindowControl* wnd);
        LayoutFloatingWindowControl* GetFloatingWindow() const { return _floatingWindow.get(); }

        std::list<std::shared_ptr<DropTargetBase>> GetTargets();

    protected:

        void OnTemplateApplied(Controls::TemplateAppliedEventArgs const& e) override;

        void UpdateThemeResources(Theme* oldTheme = nullptr)
        {
        }

        void SetDropTargetIntoVisibility(LayoutGroupBase *positionableElement);

        /// <summary>
        /// Find any LayoutDocument or LayoutAnchorable from a given source (e.g. LayoutDocumentPane, LayoutAnchorableFloatingWindow, etc.)
        /// </summary>
        /// <param name="source">The given source to search in</param>
        /// <returns>A list of all LayoutContent's</returns>
        void GetAllLayoutContents(System::Reflection::Object* source, std::list<LayoutContent*>& result);
    };
}

#endif //!__XUI_UI_LAYOUT_DOCK_LAYOUT_OVERLAY_WINDOW_CONTROL_H__