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

#ifndef __XUI_UI_LAYOUT_DOCK_LAYOUT_FLOATING_WINDOW_CONTROL_H__
#define __XUI_UI_LAYOUT_DOCK_LAYOUT_FLOATING_WINDOW_CONTROL_H__

#include "UI/Window.h"
#include "UI/Border.h"
#include "UI/LayoutDock/Controls/ILayoutControl.h"

MetaAssemblyBegin
Meta(modul: ReflInclude("UI/LayoutDock/Controls/OverlayWindow.h"))
Meta(modul: ReflInclude("UI/LayoutDock/Controls/LayoutItem.h"))
Meta(modul: ReflInclude("UI/LayoutDock/Controls/LayoutAnchorControl.h"))
Meta(modul: ReflInclude("UI/LayoutDock/Layouts/LayoutAnchorGroup.h"))
Meta(modul: ReflInclude("UI/LayoutDock/Controls/DropTargetBase.h"))
MetaAssemblyEnd

namespace XUI::UI
{
    struct IOverlayWindowHost;

    //class FloatingWindowContentHost : public Controls::ContentControl
    //{
    //private:
    //    LayoutFloatingWindowControl * _owner;
    //    //HwndSource * _wpfContentHost = nullptr;
    //    std::shared_ptr<Border> _rootPresenter;
    //    DockingManager *_manager = nullptr;
    //
    //public:
    //
    //    FloatingWindowContentHost(LayoutFloatingWindowControl *owner);
    //    virtual ~FloatingWindowContentHost() {}
    //
    //    Controls::Visual *GetRootVisual() const;
    //
    //protected:
    //    //System::Runtime::InteropServices::HandleRef BuildWindowCore(System::Runtime::InteropServices::HandleRef hwndParent) override;
    //    //void DestroyWindowCore(HandleRef hwnd);
    //
    //    Core::Media::Size MeasureOverride(Core::Media::Size const& constraint) override;
    //
    //    /// <summary>
    //    /// Handles changes to the Content property.
    //    /// </summary>
    //    static void OnContentChanged(Core::Dependency::DependencyObject *d, Core::Dependency::DependencyPropertyChangedEventArgs *e);
    //
    //    /// <summary>
    //    /// Provides derived classes an opportunity to handle changes to the Content property.
    //    /// </summary>
    //    virtual void OnContentChanged(Core::Dependency::DependencyPropertyChangedEventArgs *e);
    //
    //};

    class LayoutFloatingWindowControl : public UI::Window, public ILayoutControl
    {
    private:
        std::map<IOverlayWindowHost*, std::list<std::shared_ptr<IDropArea>>> _cacheDropAreas;
        std::map<IOverlayWindowHost*, std::set<std::shared_ptr<DropTargetBase>>> _cacheDropTargets;
        std::set<IDropArea*> _activeDropAreas;
        std::shared_ptr<DropTargetBase> _currentDropTarget;
        std::shared_ptr<LayoutElement> _model;
        bool _isInternalChange = false;
        bool _attachDrag = false;
        bool _internalCloseFlag = false;
        bool _changedAreas = false;


    public:

        /// <summary>
        /// IsMaximized Dependency Property
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty IsMaximizedProperty;
        static XUI::Core::Dependency::RefDependencyProperty IsDraggingProperty;

        static void StaticClassInitializer();

        LayoutFloatingWindowControl();
        LayoutFloatingWindowControl(LayoutElement *model);
        virtual ~LayoutFloatingWindowControl();

        void AttachDrag(bool onActivated = true);
        void InternalClose();

        PROPERTY_(std::shared_ptr<UI::LayoutElement>, Model);
        std::shared_ptr<UI::LayoutElement> const& GetModel() const { return _model; }
        void SetModel(std::shared_ptr<UI::LayoutElement> const& value)
        {
            _model = value;
            NotifyPropertyChanged::RaisePropertyChanged("Model");
        }

        LayoutElement* GetLayoutModel() override { return _model.get(); }

        bool GetCloseInitiatedByUser() const;

        DEPENDENCY_PROPERTY_(bool, IsMaximized)
        DEPENDENCY_PROPERTY_(bool, IsDragging)
        bool KeepContentVisibleOnClose = false;

        virtual IOverlayWindowHost* ToIOverlayWindowHost() { return nullptr; }

    protected:

        //static System::Reflection::Variant CoerceContentValue(DependencyObject *sender, System::Reflection::Variant content);

        void OnClosed(void*, System::Events::EventArg& e);
        void OnLoaded(void* sender, System::Events::EventArg& e);
        void OnUnloaded(void* sender, System::Events::EventArg& e);
        void OnActivated(void* sender, System::Events::EventArg& e);
        virtual void OnWindowInitialized() override;
        virtual void OnPointerMoved(Interfaces::PointerEventArgs& e) override;
        virtual void OnPreviewPointerReleased(Interfaces::PointerReleasedEventArgs& e) override;

        /// <summary>
        /// Provides a secure method for setting the IsMaximized property.  
        /// This dependency property indicates if the window is maximized.
        /// </summary>
        /// <param name="value">The new value for the property.</param>
        void OnStateChanged(System::Events::EventArg& e);

        /// <summary>
        /// Handles changes to the IsDragging property.
        /// </summary>
        static void OnIsDraggingChanged(Core::Dependency::DependencyObject *d, Core::Dependency::DependencyPropertyChangedEventArgs *e);

        /// <summary>
        /// Provides derived classes an opportunity to handle changes to the IsDragging property.
        /// </summary>
        virtual void OnIsDraggingChanged(Core::Dependency::DependencyPropertyChangedEventArgs *e);

        void UpdatePositionAndSizeOfPanes();
        void UpdateMaximizedState(bool isMaximized);
        //virtual void* FilterMessage(void* hwnd, int msg, void* wParam, void* lParam, bool &handled);
        void UpdateDragPosition();
       
    };
}

#endif //!__XUI_UI_LAYOUT_DOCK_LAYOUT_FLOATING_WINDOW_CONTROL_H__