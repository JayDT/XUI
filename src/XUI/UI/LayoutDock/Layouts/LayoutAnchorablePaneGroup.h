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

#ifndef __XUI_UI_LAYOUT_DOCK_LAYOUT_ANCHORABLE_PANE_GROUP_H__
#define __XUI_UI_LAYOUT_DOCK_LAYOUT_ANCHORABLE_PANE_GROUP_H__

#include "UI/LayoutDock/Layouts/LayoutPositionableGroup.h"
#include "UI/LayoutDock/Layouts/LayoutAnchorablePane.h"

namespace XUI::UI
{
    //[ContentProperty("Children")]
    class LayoutAnchorablePaneGroup : public LayoutPositionableGroup<LayoutAnchorablePane>
    {
    private:

        UI::Orientation _orientation = UI::Orientation::Vertical;

    public:

        LayoutAnchorablePaneGroup();
        virtual ~LayoutAnchorablePaneGroup();

        PROPERTY_(UI::Orientation, Orientation);
        UI::Orientation GetOrientation() const override { return _orientation; }
        void SetOrientation(UI::Orientation value)
        {
            if (_orientation != value)
            {
                _orientation = value;
                NotifyPropertyChanged::RaisePropertyChanged("Orientation");
            }
        }

    protected:

        bool GetVisibility() override;

        void OnIsVisibleChanged() override;

        void UpdateParentVisibility();

        void OnDockWidthChanged() override;

        void OnDockHeightChanged() override;

        void OnChildrenCollectionChanged() override;
    };
}

#endif //!__XUI_UI_LAYOUT_DOCK_LAYOUT_ANCHORABLE_PANE_GROUP_H__