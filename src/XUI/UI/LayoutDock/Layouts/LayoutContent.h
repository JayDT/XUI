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

#ifndef __XUI_UI_LAYOUT_DOCK_LAYOUT_CONTENT_H__
#define __XUI_UI_LAYOUT_DOCK_LAYOUT_CONTENT_H__

#include "UI/LayoutDock/Layouts/LayoutElement.h"
#include <chrono>

namespace XUI::UI
{
    class LayoutContent : public LayoutElement
    {
    protected:

        System::Reflection::RefObject _content;
        System::Reflection::RefObject _toolTip;
        std::chrono::system_clock::time_point _lastActivationTimeStamp;
        std::shared_ptr<LayoutContainer> _previousContainer;
        int _previousContainerIndex = -1;
        double _floatingWidth = 0.0;
        double _floatingHeight = 0.0;
        double _floatingLeft = 0.0;
        double _floatingTop = 0.0;
        bool _isSelected = false;
        bool _isActive = false;
        bool _isLastFocusedDocument = false;
        bool _isMaximized = false;
        bool _canClose = true;
        bool _canFloat = true;
        bool _isEnabled = true;

    public:

        static Core::Dependency::RefDependencyProperty TitleProperty;
        static Core::Dependency::RefDependencyProperty IconSourceProperty;

        static void StaticClassInitializer();

        System::Events::EventHandler<System::Events::EventArg> IsSelectedChanged;
        System::Events::EventHandler<System::Events::EventArg> IsActiveChanged;

        /// <summary>
        /// Event fired when the content is closed (i.e. removed definitely from the layout)
        /// </summary>
        System::Events::EventHandler<System::Events::EventArg> Closed;

        /// <summary>
        /// Event fired when the content is about to be closed (i.e. removed definitely from the layout)
        /// </summary>
        /// <remarks>Please note that LayoutAnchorable also can be hidden. Usually user hide anchorables when click the 'X' button. To completely close 
        /// an anchorable the user should click the 'Close' menu item from the context menu. When an LayoutAnchorable is hidden its visibility changes to false and
        /// IsHidden property is set to true.
        /// Hanlde the Hiding event for the LayoutAnchorable to cancel the hide operation.</remarks>
        System::Events::EventHandler<CancelEventArgs> Closing;


        LayoutContent();
        virtual ~LayoutContent();

        virtual bool GetIsVisible() const
        {
            return true;
        }

        DEPENDENCY_PROPERTY_(std::wstring, Title)
        DEPENDENCY_PROPERTY_(std::shared_ptr<Interfaces::IBitmapImpl>, IconSource)

        Meta(XamlCPP::ContentAttribute())
        PROPERTY_(System::Reflection::RefObject, Content);
        System::Reflection::RefObject const& GetContent() const
        {
            return _content;
        }
        void SetContent(System::Reflection::RefObject const& value)
        {
            if (_content != value)
            {
                _content = value;
                NotifyPropertyChanged::RaisePropertyChanged("Content");
            }
        }

        PROPERTY_(bool, IsSelected);
        bool GetIsSelected() const { return _isSelected; }
        void SetIsSelected(bool value);

        PROPERTY_(bool, IsActive);
        bool GetIsActive() const { return _isActive; }
        void SetIsActive(bool value);

        PROPERTY_(bool, IsLastFocusedDocument);
        bool GetIsLastFocusedDocument() const { return _isLastFocusedDocument; }
        void SetIsLastFocusedDocument(bool value)
        {
            if (_isLastFocusedDocument != value)
            {
                _isLastFocusedDocument = value;
                NotifyPropertyChanged::RaisePropertyChanged("IsLastFocusedDocument");
            }
        }

        PROPERTY_(LayoutContainer*, PreviousContainer);
        LayoutContainer* GetPreviousContainer() const override { return _previousContainer.get(); }
        void SetPreviousContainer(LayoutContainer* value) override;

        PROPERTY_(std::chrono::system_clock::time_point, LastActivationTimeStamp);
        std::chrono::system_clock::time_point GetLastActivationTimeStamp() const { return _lastActivationTimeStamp; }
        void SetLastActivationTimeStamp(std::chrono::system_clock::time_point value)
        {
            if (_lastActivationTimeStamp != value)
            {
                _lastActivationTimeStamp = value;
                NotifyPropertyChanged::RaisePropertyChanged("LastActivationTimeStamp");
            }
        }

        PROPERTY_(double, FloatingWidth);
        double GetFloatingWidth() const { return _floatingWidth; }
        void SetFloatingWidth(double value)
        {
            if (_floatingWidth != value)
            {
                _floatingWidth = value;
                NotifyPropertyChanged::RaisePropertyChanged("FloatingWidth");
            }
        }

        PROPERTY_(double, FloatingHeight);
        double GetFloatingHeight() const { return _floatingHeight; }
        void   SetFloatingHeight(double value)
        {
            if (_floatingHeight != value)
            {
                _floatingHeight = value;
                NotifyPropertyChanged::RaisePropertyChanged("FloatingHeight");
            }
        }

        PROPERTY_(double, FloatingLeft);
        double GetFloatingLeft() const { return _floatingLeft; }
        void   SetFloatingLeft(double value)
        {
            if (_floatingLeft != value)
            {
                _floatingLeft = value;
                NotifyPropertyChanged::RaisePropertyChanged("FloatingLeft");
            }
        }

        PROPERTY_(double, FloatingTop);
        double GetFloatingTop() const { return _floatingTop; }
        void   SetFloatingTop(double value)
        {
            if (_floatingTop != value)
            {
                _floatingTop = value;
                NotifyPropertyChanged::RaisePropertyChanged("FloatingTop");
            }
        }

        PROPERTY_(bool, IsMaximized);
        bool GetIsMaximized() const { return _isMaximized; }
        void SetIsMaximized(bool value)
        {
            if (_isMaximized != value)
            {
                _isMaximized = value;
                NotifyPropertyChanged::RaisePropertyChanged("IsMaximized");
            }
        }

        PROPERTY_(System::Reflection::RefObject, ToolTip);
        System::Reflection::RefObject GetToolTip() const { return _toolTip; }
        void SetToolTip(System::Reflection::RefObject value)
        {
            if (_toolTip != value)
            {
                _toolTip = value;
                NotifyPropertyChanged::RaisePropertyChanged("ToolTip");
            }
        }

        PROPERTY_(int, PreviousContainerIndex);
        int  GetPreviousContainerIndex() const override { return _previousContainerIndex; }
        void SetPreviousContainerIndex(int value) override
        {
            if (_previousContainerIndex != value)
            {
                _previousContainerIndex = value;
                NotifyPropertyChanged::RaisePropertyChanged("PreviousContainerIndex");
            }
        }

        PROPERTY_(bool, CanClose);
        bool GetCanClose() const { return _canClose; }
        void SetCanClose(bool value)
        {
            if (_canClose != value)
            {
                _canClose = value;
                NotifyPropertyChanged::RaisePropertyChanged("CanClose");
            }
        }

        PROPERTY_(bool, CanFloat);
        bool GetCanFloat() const { return _canFloat; }
        void SetCanFloat(bool value)
        {
            if (_canFloat != value)
            {
                _canFloat = value;
                NotifyPropertyChanged::RaisePropertyChanged("CanFloat");
            }
        }

        PROPERTY_(bool, IsEnabled);
        bool GetIsEnabled() const { return _isEnabled; }
        void SetIsEnabled(bool value)
        {
            if (_isEnabled != value)
            {
                _isEnabled = value;
                NotifyPropertyChanged::RaisePropertyChanged("IsEnabled");
            }
        }

        bool IsFloating();

        bool CompareTo(LayoutContent* other);

        bool operator>(LayoutContent const* other) const
        {
            if (!other)
                return false;

            return Title > other->Title;
        }

        bool operator<(LayoutContent const* other) const
        {
            if (!other)
                return false;

            return Title < other->Title;
        }

        /// <summary>
        /// Test if the content can be closed
        /// </summary>
        /// <returns></returns>
        bool TestCanClose();

        void CloseInternal();

        /// <summary>
        /// Close the content
        /// </summary>
        /// <remarks>Please note that usually the anchorable is only hidden (not closed). By default when user click the X button it only hides the content.</remarks>
        virtual void Close() = 0;


        /// <summary>
        /// Float the content in a popup window
        /// </summary>
        void Float();

        /// <summary>
        /// Dock the content as document
        /// </summary>
        void DockAsDocument();

        /// <summary>
        /// Re-dock the content to its previous container
        /// </summary>
        void Dock();

    protected:

        virtual void InternalDock();

        virtual void OnClosed();

        virtual void OnIsSelectedChanged(bool oldValue, bool newValue);

        virtual void OnIsActiveChanged(bool oldValue, bool newValue);

        void OnParentChanging(LayoutContainer* oldValue, LayoutContainer* newValue) override;

        void OnParentChanged(LayoutContainer* oldValue, LayoutContainer* newValue) override;
    };
}

#endif //!__XUI_UI_LAYOUT_DOCK_LAYOUT_CONTENT_H__