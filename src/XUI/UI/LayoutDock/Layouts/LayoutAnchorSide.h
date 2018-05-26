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

#ifndef __XUI_UI_LAYOUT_DOCK_LAYOUT_ANCHOR_SIDE_H__
#define __XUI_UI_LAYOUT_DOCK_LAYOUT_ANCHOR_SIDE_H__

#include "UI/LayoutDock/Layouts/LayoutGroup.h"
#include "UI/LayoutDock/Layouts/LayoutAnchorGroup.h"

namespace XUI::UI
{
    class LayoutAnchorSide : public LayoutGroup<LayoutAnchorGroup>
    {
    private:

        AnchorSide _side = AnchorSide::Top;

        void UpdateSide();

    public:

        LayoutAnchorSide();
        virtual ~LayoutAnchorSide();

        PROPERTY_(AnchorSide, Side);
        AnchorSide GetSide() const { return _side; }
        void SetSide(AnchorSide side)
        {
            _side = side;
            NotifyPropertyChanged::RaisePropertyChanged("Side");
        }

        // Inherited via LayoutGroup
        virtual bool GetVisibility() override;

    protected:

        void OnParentChanged(LayoutContainer* oldValue, LayoutContainer* newValue) override
        {
            LayoutGroup<LayoutAnchorGroup>::OnParentChanged(oldValue, newValue);

            UpdateSide();
        }

    };
}

#endif //!__XUI_UI_LAYOUT_DOCK_LAYOUT_ANCHOR_SIDE_H__