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

#ifndef __XUI_UI_LAYOUT_DOCK_EVENT_ARGS_H__
#define __XUI_UI_LAYOUT_DOCK_EVENT_ARGS_H__

#include "standard/enum.h"
#include "standard/events.h"
#include "UI/LayoutDock/DockingDefines.h"

MetaAssemblyBegin
Meta(modul: ReflInclude("UI/LayoutDock/Layouts/LayoutDocument.h"))
Meta(modul: ReflInclude("UI/LayoutDock/Layouts/LayoutAnchorablePane.h"))
Meta(modul: ReflInclude("UI/LayoutDock/Controls/OverlayWindow.h"))
MetaAssemblyEnd

namespace XUI::UI
{
    class LayoutDocument;
    class LayoutElement;
    class LayoutRoot;
    class LayoutAnchorable;
    class LayoutContainer;
    class LayoutFloatingWindowControl;
    class IDropArea;
    class OverlayWindow;
    class DockingManager;

    struct CancelEventArgs : System::Events::EventArg
    {
        bool Cancel = false;
    };

    struct DocumentCloseEventArgs : CancelEventArgs
    {
        DocumentCloseEventArgs(std::shared_ptr<LayoutDocument> document)
        {
            Document = document;
        }

        std::shared_ptr<LayoutDocument> Document;
    };

    struct ChildrenTreeChangedEventArgs : System::Events::EventArg
    {
        ChildrenTreeChangedEventArgs(ChildrenTreeChange change)
        {
            Change = change;
        }

        ChildrenTreeChange Change = ChildrenTreeChange::DirectChildrenChanged;
    };

    struct LayoutElementEventArgs : System::Events::EventArg
    {
        LayoutElementEventArgs(LayoutElement* element)
        {
            Element = element;
        }

        LayoutElement* Element = nullptr;
    };

    struct FocusChangeEventArgs : System::Events::EventArg
    {
        FocusChangeEventArgs(std::intptr_t gotFocusWinHandle, std::intptr_t lostFocusWinHandle)
        {
            GotFocusWinHandle = gotFocusWinHandle;
            LostFocusWinHandle = lostFocusWinHandle;
        }

        std::intptr_t GotFocusWinHandle = 0;
        std::intptr_t LostFocusWinHandle = 0;
    };

    struct WindowActivateEventArgs : System::Events::EventArg
    {
        WindowActivateEventArgs(std::intptr_t hwndActivating)
        {
            HwndActivating = hwndActivating;
        }

        std::intptr_t HwndActivating = 0;
    };

    /// Interfaces
    struct Meta(Enable) ILayoutUpdateStrategy
    {
        virtual ~ILayoutUpdateStrategy() {}

        virtual bool BeforeInsertAnchorable(
            LayoutRoot* layout,
            LayoutAnchorable* anchorableToShow,
            LayoutContainer* destinationContainer) = 0;

        virtual void AfterInsertAnchorable(
            LayoutRoot* layout,
            LayoutAnchorable* anchorableShown) = 0;

        virtual bool BeforeInsertDocument(
            LayoutRoot* layout,
            LayoutDocument* anchorableToShow,
            LayoutContainer* destinationContainer) = 0;

        virtual void AfterInsertDocument(
            LayoutRoot* layout,
            LayoutDocument* anchorableShown) = 0;
    };

    struct IDropArea : System::Reflection::Object, System::EnableSharedFromThisEx<IDropArea>
    {
        virtual ~IDropArea() {}
        virtual Core::Media::Rect GetDetectionRect() = 0;
        DropAreaType Type = DropAreaType::DockingManager;
    };

    struct Meta(Enable) IOverlayWindowHost
    {
        virtual bool HitTest(Core::Media::Point const& dragPoint) = 0;

        virtual UI::OverlayWindow* ShowOverlayWindow(LayoutFloatingWindowControl* draggingWindow) = 0;

        virtual void HideOverlayWindow() = 0;

        virtual std::list<std::shared_ptr<IDropArea>> const& GetDropAreas(LayoutFloatingWindowControl* draggingWindow) = 0;

        virtual DockingManager* GetManager() = 0;
    };

    class ReentrantFlag;

    struct ReentrantFlag
    {
        struct _ReentrantFlagHandler
        {
            ReentrantFlag* _owner;
            _ReentrantFlagHandler(ReentrantFlag* owner)
            {
                _owner = owner;
                _owner->_flag = true;
            }

            ~_ReentrantFlagHandler()
            {
                _owner->_flag = false;
            }
        };

        bool _flag = false;

        _ReentrantFlagHandler Enter()
        {
            if (_flag)
                throw System::InvalidOperationException("");
            return _ReentrantFlagHandler(this);
        }

        bool CanEnter()
        {
            return !_flag;
        }
    };
}

#endif //!__XUI_UI_LAYOUT_DOCK_EVENT_ARGS_H__