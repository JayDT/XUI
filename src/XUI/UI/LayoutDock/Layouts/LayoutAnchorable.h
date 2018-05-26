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

#ifndef __XUI_UI_LAYOUT_DOCK_LAYOUT_ANCHORABLE_H__
#define __XUI_UI_LAYOUT_DOCK_LAYOUT_ANCHORABLE_H__

#include "UI/LayoutDock/Layouts/LayoutContent.h"

MetaAssemblyBegin
Meta(modul: ReflInclude("UI/LayoutDock/Layouts/LayoutContainer.h"))
Meta(modul: ReflInclude("UI/LayoutDock/Controls/OverlayWindow.h"))
Meta(modul: ReflInclude("UI/LayoutDock/DockingManager.h"))
MetaAssemblyEnd

namespace XUI::UI
{
    class DockingManager;
    class LayoutAnchorablePane;

    class LayoutAnchorable : public LayoutContent
    {
    private:

        double _autohideWidth = 0.0;
        double _autohideHeight = 0.0;
        double _autohideMinWidth = 100.0;
        double _autohideMinHeight = 100.0;
        bool _canHide = true;
        bool _canAutoHide = true;

    public:

        LayoutAnchorable();
        virtual ~LayoutAnchorable();

        System::Events::EventHandler<System::Events::EventArg> IsVisibleChanged;
        System::Events::EventHandler<CancelEventArgs> Hiding;

        void NotifyIsVisibleChanged()
        {
            static System::Events::EventArg e;

            if (IsVisibleChanged)
                IsVisibleChanged(this, e);
        }

        PROPERTY_(bool, IsVisible);
        bool GetIsVisible() const override;
        void SetIsVisible(bool const& value);

        PROPERTY_(double, AutoHideWidth);
        double GetAutoHideWidth() const { return _autohideWidth; }
        void SetAutoHideWidth(double value)
        {
            if (_autohideWidth != value)
            {
                _autohideWidth = std::max(value, _autohideMinWidth);
                NotifyPropertyChanged::RaisePropertyChanged("AutoHideWidth");
            }
        }

        PROPERTY_(double, AutoHideMinWidth);
        double GetAutoHideMinWidth() const { return _autohideMinWidth; }
        void SetAutoHideMinWidth(double value)
        {
            if (_autohideMinWidth != value)
            {
                _autohideMinWidth = value;
                NotifyPropertyChanged::RaisePropertyChanged("AutoHideMinWidth");
            }
        }

        PROPERTY_(double, AutoHideHeight);
        double GetAutoHideHeight() const { return _autohideHeight; }
        void SetAutoHideHeight(double value)
        {
            if (_autohideHeight != value)
            {
                _autohideHeight = std::max(value, _autohideMinHeight);
                NotifyPropertyChanged::RaisePropertyChanged("AutoHideHeight");
            }
        }

        PROPERTY_(double, AutoHideMinHeight);
        double GetAutoHideMinHeight() const { return _autohideMinHeight; }
        void SetAutoHideMinHeight(double value)
        {
            if (_autohideMinHeight != value)
            {
                _autohideMinHeight = value;
                NotifyPropertyChanged::RaisePropertyChanged("AutoHideMinWidth");
            }
        }

        PROPERTY_(bool, CanHide);
        bool GetCanHide() const { return _canHide; }
        void SetCanHide(bool value)
        {
            if (_canHide != value)
            {
                _canHide = value;
                NotifyPropertyChanged::RaisePropertyChanged("CanHide");
            }
        }

        PROPERTY_(bool, CanAutoHide);
        bool GetCanAutoHide() const { return _canAutoHide; }
        void SetCanAutoHide(bool value)
        {
            if (_canAutoHide != value)
            {
                _canAutoHide = value;
                NotifyPropertyChanged::RaisePropertyChanged("CanAutoHide");
            }
        }

        bool IsHidden() const;
        bool IsAutoHidden() const;

        void OnParentChanged(LayoutContainer* oldValue, LayoutContainer* newValue) override;

        void UpdateParentVisibility();

        /// <summary>
        /// Hide this contents
        /// </summary>
        /// <remarks>Add this content to <see cref="ILayoutRoot.Hidden"/> collection of parent root.</remarks>
        void Hide(bool cancelable = true);

        /// <summary>
        /// Show the content
        /// </summary>
        /// <remarks>Try to show the content where it was previously hidden.</remarks>
        void Show();

        void InternalDock() override;

        /// <summary>
        /// Add the anchorable to a DockingManager layout
        /// </summary>
        /// <param name="manager"></param>
        /// <param name="strategy"></param>
        void AddToLayout(DockingManager* manager, AnchorableShowStrategy strategy);

        void ToggleAutoHide();

        void Close() override;

        void CloseAnchorable();
    };
}

#endif //!__XUI_UI_LAYOUT_DOCK_LAYOUT_ANCHORABLE_H__