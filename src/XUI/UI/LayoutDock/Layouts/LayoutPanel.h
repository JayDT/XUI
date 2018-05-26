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

#ifndef __XUI_UI_LAYOUT_DOCK_LAYOUT_PANEL_H__
#define __XUI_UI_LAYOUT_DOCK_LAYOUT_PANEL_H__

#include "UI/LayoutDock/Layouts/LayoutPositionableGroup.h"
#include "UI/DataGrid/DataGridDefines.h"

MetaAssemblyBegin
Meta(modul: ReflInclude("UI/LayoutDock/Controls/LayoutPanelControl.h"))
Meta(modul: ReflInclude("UI/LayoutDock/Controls/OverlayWindow.h"))
MetaAssemblyEnd

namespace XUI::UI
{
    class LayoutPanel : public LayoutPositionableGroup<LayoutElement>
    {
    private:

        UI::Orientation _orientation = UI::Orientation::Vertical;

    public:

        LayoutPanel();
        LayoutPanel(LayoutElement* child)
        {
            AddChild(child);
        }
        virtual ~LayoutPanel();

        PROPERTY_(UI::Orientation, Orientation);
        UI::Orientation GetOrientation() const override { return _orientation; }
        void SetOrientation(UI::Orientation value) override
        {
            if (_orientation != value)
            {
                _orientation = value;
                NotifyPropertyChanged::RaisePropertyChanged("Orientation");
            }
        }

    protected:

        bool GetVisibility() override;

    };
}

#endif //!__XUI_UI_LAYOUT_DOCK_LAYOUT_PANEL_H__