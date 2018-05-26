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

#ifndef __XUI_UI_LAYOUT_DOCK_LAYOUT_ROOT_H__
#define __XUI_UI_LAYOUT_DOCK_LAYOUT_ROOT_H__

#include "UI/LayoutDock/Layouts/LayoutContainer.h"
#include "UI/LayoutDock/Layouts/LayoutAnchorSide.h"
#include "UI/LayoutDock/Layouts/LayoutFloatingWindow.h"

MetaAssemblyBegin
Meta(modul: ReflInclude("UI/LayoutDock/DockingManager.h"))
Meta(modul: ReflInclude("UI/LayoutDock/Layouts/LayoutPanel.h"))
MetaAssemblyEnd

namespace XUI::UI
{
    class DockingManager;
    class LayoutFloatingWindow;
    class LayoutPanel;
    class LayoutContent;
    class LayoutDocumentPane;

    class TC_XUI_API LayoutRoot : public LayoutContainer
    {
    private:

        System::Collection::RefManager<LayoutElement, LayoutRoot> m_elements;

        DockingManager* _manager = nullptr;

        std::shared_ptr<LayoutPanel> _rootPanel;

        std::shared_ptr<LayoutAnchorSide> _topSide;
        std::shared_ptr<LayoutAnchorSide> _leftSide;
        std::shared_ptr<LayoutAnchorSide> _rightSide;
        std::shared_ptr<LayoutAnchorSide> _bottomSide;

        std::weak_ptr<LayoutContent> _activeContent;
        std::weak_ptr<LayoutContent> _lastFocusedDocument;

        System::Collection::List<std::shared_ptr<LayoutFloatingWindow>> _floatingWindows;
        System::Collection::List<std::shared_ptr<LayoutAnchorable>> _hidden;

        bool _activeContentSet = false;
        bool _lastFocusedDocumentSet = false;


    public:

        LayoutRoot();
        virtual ~LayoutRoot();

        System::Events::EventHandler<System::Events::EventArg> Updated;
        System::Events::EventHandler<LayoutElementEventArgs> ElementAdded;
        System::Events::EventHandler<LayoutElementEventArgs> ElementRemoved;

        PROPERTY_GET_ONLY(System::Collection::List<std::shared_ptr<LayoutFloatingWindow>>&, FloatingWindows);
        System::Collection::List<std::shared_ptr<LayoutFloatingWindow>>& GetFloatingWindows() { return _floatingWindows; }

        PROPERTY_GET_ONLY(System::Collection::List<std::shared_ptr<LayoutAnchorable>>&, Hidden);
        System::Collection::List<std::shared_ptr<LayoutAnchorable>>& GetHidden() { return _hidden; }

        Meta(XamlCPP::ContentAttribute())
        PROPERTY_(std::shared_ptr<LayoutPanel>, RootPanel);
        std::shared_ptr<LayoutPanel> const& GetRootPanel() const { return _rootPanel; }
        void SetRootPanel(std::shared_ptr<LayoutPanel> const& value);

        PROPERTY_(std::shared_ptr<LayoutAnchorSide>, TopSide);
        std::shared_ptr<LayoutAnchorSide> const& GetTopSide() const { return _topSide; }
        void SetTopSide(std::shared_ptr<LayoutAnchorSide> const& value)
        {
            if (_topSide != value)
            {
                _topSide = value;
                if (_topSide != nullptr)
                    _topSide->SetParent(this);
                NotifyPropertyChanged::RaisePropertyChanged("TopSide");
            }
        }

        PROPERTY_(std::shared_ptr<LayoutAnchorSide>, LeftSide);
        std::shared_ptr<LayoutAnchorSide> const& GetLeftSide() const { return _leftSide; }
        void SetLeftSide(std::shared_ptr<LayoutAnchorSide> const& value)
        {
            if (_leftSide != value)
            {
                _leftSide = value;
                if (_leftSide != nullptr)
                    _leftSide->SetParent(this);
                NotifyPropertyChanged::RaisePropertyChanged("LeftSide");
            }
        }

        PROPERTY_(std::shared_ptr<LayoutAnchorSide>, RightSide);
        std::shared_ptr<LayoutAnchorSide> const& GetRightSide() const { return _rightSide; }
        void SetRightSide(std::shared_ptr<LayoutAnchorSide> const& value)
        {
            if (_rightSide != value)
            {
                _rightSide = value;
                if (_rightSide != nullptr)
                    _rightSide->SetParent(this);
                NotifyPropertyChanged::RaisePropertyChanged("RightSide");
            }
        }

        PROPERTY_(std::shared_ptr<LayoutAnchorSide>, BottomSide);
        std::shared_ptr<LayoutAnchorSide> const& GetBottomSide() const { return _bottomSide; }
        void SetBottomSide(std::shared_ptr<LayoutAnchorSide> const& value)
        {
            if (_bottomSide != value)
            {
                _bottomSide = value;
                if (_bottomSide != nullptr)
                    _bottomSide->SetParent(this);
                NotifyPropertyChanged::RaisePropertyChanged("BottomSide");
            }
        }

        PROPERTY_(LayoutContent*, ActiveContent);
        LayoutContent* GetActiveContent() const { return _activeContent.lock().get(); }
        void SetActiveContent(LayoutContent* value)
        {
            if (_activeContent.lock().get() != value)
            {
                if (value)
                    InternalSetActiveContent(_activeContent.lock().get(), value);
                else
                    _activeContent.reset();
            }
        }

        PROPERTY_(LayoutContent*, LastFocusedDocument);
        LayoutContent* GetLastFocusedDocument() const { return _lastFocusedDocument.lock().get(); }
        void SetLastFocusedDocument(LayoutContent* value)
        {
            auto currentValue = _lastFocusedDocument.lock().get();
            if (currentValue != value)
            {

                if (currentValue)
                    currentValue->IsLastFocusedDocument = false;
                if (value)
                    _lastFocusedDocument = value->weak_from_base_static<LayoutContent>();
                else
                    _lastFocusedDocument.reset();
                currentValue = LastFocusedDocument;
                if (currentValue)
                    currentValue->IsLastFocusedDocument = true;
                _lastFocusedDocumentSet = currentValue != nullptr;
                NotifyPropertyChanged::RaisePropertyChanged("LastFocusedDocument");
            }
        }

        PROPERTY_(DockingManager*, Manager);
        DockingManager* GetManager() { return _manager; }
        void SetManager(DockingManager* mgr)
        {
            if (_manager != mgr)
            {
                _manager = mgr;
                NotifyPropertyChanged::RaisePropertyChanged("Manager");
            }
        }

        void Initialize();

        void AddChild(LayoutElement* child) override final;
        void RemoveChild(LayoutElement* child) override final;
        void ReplaceChild(LayoutElement* oldchild, LayoutElement* newchild) override final;

        void FireLayoutUpdated()
        {
            if (Updated)
            {
                static System::Events::EventArg e;
                Updated(this, e);
            }
        }

        void CollectGarbage();

        System::Collection::RefManager<LayoutElement, LayoutRoot>& GetElements() { return m_elements; }

        void CollectionChanged_hiddenAnchorables(void* sender, System::Collection::NotifyCollectionChangedEventArgs& e);

        void CollectionChanged_floatingWindows(void* sender, System::Collection::NotifyCollectionChangedEventArgs& e);

        void OnLayoutElementAdded(LayoutElement* element);

        void OnLayoutElementRemoved(LayoutElement* element);

    private:

        void InternalSetActiveContent(LayoutContent* currentValue, LayoutContent* newActiveContent);

        void UpdateActiveContentProperty();
    };
}

#endif //!__XUI_UI_LAYOUT_DOCK_LAYOUT_ROOT_H__