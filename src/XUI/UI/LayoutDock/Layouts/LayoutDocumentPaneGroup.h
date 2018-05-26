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

#ifndef __XUI_UI_LAYOUT_DOCK_LAYOUT_DOCUMENT_PANE_GROUP_H__
#define __XUI_UI_LAYOUT_DOCK_LAYOUT_DOCUMENT_PANE_GROUP_H__

#include "UI/LayoutDock/Layouts/LayoutPositionableGroup.h"
#include "UI/LayoutDock/Layouts/LayoutDocumentPane.h"

MetaAssemblyBegin
Meta(modul: ReflInclude("UI/LayoutDock/Layouts/LayoutDocumentPaneGroup.h"))
MetaAssemblyEnd

namespace XUI::UI
{
    //[ContentProperty("Children")]
    class TC_XUI_API LayoutDocumentPaneGroup : public LayoutPositionableGroup<LayoutDocumentPane>
    {
    private:

        UI::Orientation _orientation = UI::Orientation::Vertical;

    public:

        LayoutDocumentPaneGroup();
        virtual ~LayoutDocumentPaneGroup();

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
    };
}

#endif //!__XUI_UI_LAYOUT_DOCK_LAYOUT_DOCUMENT_PANE_GROUP_H__