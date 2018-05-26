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

#ifndef __XUI_UI_LAYOUT_DOCK_LAYOUT_POSITIONABLE_GROUP_H__
#define __XUI_UI_LAYOUT_DOCK_LAYOUT_POSITIONABLE_GROUP_H__

#include "UI/LayoutDock/Layouts/LayoutGroup.h"
#include "UI/Detail/GridLength.h"

namespace XUI::UI
{
    template<class TElement>
    class TC_XUI_API LayoutPositionableGroup : public LayoutGroup<TElement>
    {
    private:

        GridLength _dockWidth = GridLength(1.0, GridUnitType::Star);
        GridLength _dockHeight = GridLength(1.0, GridUnitType::Star);
        double _dockMinWidth = 25.0;
        double _dockMinHeight = 25.0;
        double _floatingWidth = 0.0;
        double _floatingHeight = 0.0;
        double _floatingLeft = 0.0;
        double _floatingTop = 0.0;
        bool _allowDuplicateContent = true;
        bool _canRepositionItems = true;
        bool _isMaximized = false;

    public:

        double ActualWidth = 0.0;
        double ActualHeight = 0.0;

        LayoutPositionableGroup()
        {
        }

        virtual ~LayoutPositionableGroup()
        {
        }

        void SetActualHeight(double d) override { ActualHeight = d; }
        void SetActualWidth(double d) override  { ActualWidth = d; }

        double GetActualHeight() const override { return ActualHeight; }
        double GetActualWidth() const override { return ActualWidth; }

        PROPERTY_(GridLength, DockWidth);
        GridLength GetDockWidth() const override { return _dockWidth; }
        void SetDockWidth(GridLength const& value) override
        {
            if (_dockWidth != value)
            {
                _dockWidth = value;
                NotifyPropertyChanged::RaisePropertyChanged("DockWidth");

                OnDockWidthChanged();
            }
        }

        PROPERTY_(GridLength, DockHeight);
        GridLength GetDockHeight() const override { return _dockHeight; }
        void SetDockHeight(GridLength const& value) override
        {
            if (_dockHeight != value)
            {
                _dockHeight = value;
                NotifyPropertyChanged::RaisePropertyChanged("DockHeight");

                OnDockHeightChanged();
            }
        }

        PROPERTY_(bool, AllowDuplicateContent);
        bool GetAllowDuplicateContent() const { return _allowDuplicateContent; }
        void SetAllowDuplicateContent(bool value)
        {
            if (_allowDuplicateContent != value)
            {
                _allowDuplicateContent = value;
                NotifyPropertyChanged::RaisePropertyChanged("AllowDuplicateContent");
            }
        }

        PROPERTY_(bool, CanRepositionItems);
        bool GetCanRepositionItems() const { return _canRepositionItems; }
        void SetCanRepositionItems(bool value)
        {
            if (_canRepositionItems != value)
            {
                _canRepositionItems = value;
                NotifyPropertyChanged::RaisePropertyChanged("CanRepositionItems");
            }
        }

        PROPERTY_(double, DockMinWidth);
        double GetDockMinWidth() const { return _dockMinWidth; }
        void SetDockMinWidth(double value)
        {
            if (_dockMinWidth != value)
            {
                _dockMinWidth = value;
                NotifyPropertyChanged::RaisePropertyChanged("DockMinWidth");
            }
        }

        PROPERTY_(double, DockMinHeight);
        double GetDockMinHeight() const { return _dockMinHeight; }
        void SetDockMinHeight(double value)
        {
            if (_dockMinHeight != value)
            {
                _dockMinHeight = value;
                NotifyPropertyChanged::RaisePropertyChanged("DockMinHeight");
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
        void SetFloatingHeight(double value)
        {
            if (_floatingHeight != value)
            {
                _floatingHeight = value;
                NotifyPropertyChanged::RaisePropertyChanged("FloatingHeight");
            }
        }

        PROPERTY_(double, FloatingLeft);
        double GetFloatingLeft() const { return _floatingLeft; }
        void SetFloatingLeft(double value)
        {
            if (_floatingLeft != value)
            {
                _floatingLeft = value;
                NotifyPropertyChanged::RaisePropertyChanged("FloatingLeft");
            }
        }

        PROPERTY_(double, FloatingTop);
        double GetFloatingTop() const { return _floatingTop; }
        void SetFloatingTop(double value)
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

    protected:

        virtual void OnDockWidthChanged()
        {
        }

        virtual void OnDockHeightChanged()
        {
        }

    };
}

#endif //!__XUI_UI_LAYOUT_DOCK_LAYOUT_POSITIONABLE_GROUP_H__