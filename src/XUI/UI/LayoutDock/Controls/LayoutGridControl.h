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

#ifndef __XUI_UI_LAYOUT_DOCK_LAYOUT_GRID_CONTROL_H__
#define __XUI_UI_LAYOUT_DOCK_LAYOUT_GRID_CONTROL_H__

#include "UI/Canvas.h"
#include "UI/Grid.h"
#include "UI/Border.h"
#include "UI/Window.h"
#include "UI/LayoutDock/Controls/ILayoutControl.h"
#include "UI/LayoutDock/Layouts/LayoutRoot.h"
#include "UI/LayoutDock/Layouts/LayoutPositionableGroup.h"
#include "UI/LayoutDock/Controls/LayoutGridResizerControl.h"
#include "Core/Input/Cursors.h"
#include "Core/GlobalInitializerScheduler.h"

namespace XUI::UI
{
    class LayoutGridControl : public Grid, public ILayoutControl
    {
    private:

        bool _initialized = false;
        ChildrenTreeChange _asyncRefreshCalled;
        ReentrantFlag _fixingChildrenDockLengths;

        std::shared_ptr<UI::Border> _resizerGhost;
        std::shared_ptr<UI::Canvas> _resizerHost;
        Core::Media::Vector _initialStartPoint;

    public:

        LayoutGridControl(std::shared_ptr<LayoutGroupBase> const& model, UI::Orientation orientation);

        virtual ~LayoutGridControl();

        UI::Orientation _orientation = UI::Orientation::Vertical;
        std::shared_ptr<LayoutGroupBase> Model;

        UI::Orientation Orientation() const;
        virtual LayoutElement* GetModel();
        LayoutElement* GetLayoutModel() override;

        void OnControlInitialized(void*, System::Events::EventArg& e);

        void OnLayoutUpdated(void* sender, System::Events::EventArg& e);

        void UpdateChildren();

        void OnChildModelPropertyChanged(void* sender, XamlCPP::Core::PropertyChangedEventArgs& e);

        void UpdateRowColDefinitions();

    protected:

        void FixChildrenDockLengths();

        virtual void OnFixChildrenDockLengths() = 0;

        void AttachPropertyChangeHandler();

        void DetachPropertChangeHandler();

#pragma region Splitters

        void CreateSplitters();

        void DetachOldSplitters();

        void AttachNewSplitters();

        void OnSplitterDragStarted(void* sender, Interfaces::VectorEventArgs& e);

        void OnSplitterDragDelta(void* sender, Interfaces::VectorEventArgs& e);

        void OnSplitterDragCompleted(void* sender, Interfaces::VectorEventArgs& e);

        Controls::Control* GetNextVisibleChild(int index);

        void ShowResizerOverlayWindow(LayoutGridResizerControl* splitter);

        void HideResizerOverlayWindow();

#pragma endregion

    };
}

#endif //!__XUI_UI_LAYOUT_DOCK_LAYOUT_DOCUMENT_PANE_GROUP_CONTROL_H__