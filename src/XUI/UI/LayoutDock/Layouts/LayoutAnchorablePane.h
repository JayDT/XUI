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

#ifndef __XUI_UI_LAYOUT_DOCK_LAYOUT_ANCHORABLE_PANE_H__
#define __XUI_UI_LAYOUT_DOCK_LAYOUT_ANCHORABLE_PANE_H__

#include "UI/LayoutDock/Layouts/LayoutPositionableGroup.h"
#include "UI/LayoutDock/Layouts/LayoutAnchorable.h"

namespace XUI::UI
{
    class LayoutAnchorablePane : public LayoutPositionableGroup<LayoutAnchorable>
    {
    private:

        std::string _id;
        int _selectedIndex = -1;
        bool _autoFixSelectedContent = true;

    public:

        LayoutAnchorablePane();
        virtual ~LayoutAnchorablePane();

        PROPERTY_(int, SelectedContentIndex);
        int GetSelectedContentIndex() const { return _selectedIndex; }
        void SetSelectedContentIndex(int value);

        PROPERTY_(std::string, Id);
        std::string const& GetId() const { return _id; }
        void SetId(std::string const& value)
        {
            if (_id != value)
                _id = value;
        }

        PROPERTY_GET_ONLY(bool, CanHide);
        bool GetCanHide() const;

        PROPERTY_GET_ONLY(bool, CanClose);
        bool GetCanClose() const;

        bool GetVisibility() override;
        bool IsDirectlyHostedInFloatingWindow();
        bool IsHostedInFloatingWindow();

        void ChildMoved(int oldIndex, int newIndex) override;

        LayoutContent* SelectedContent();

        void OnChildrenCollectionChanged() override;

        int IndexOf(LayoutContent* content);

        void SetNextSelectedIndex();

        void UpdateIsDirectlyHostedInFloatingWindow();

        void OnParentChanged(LayoutContainer* oldValue, LayoutContainer* newValue) override;

        void OnParentChildrenCollectionChanged(void* sender, System::Events::EventArg& e);

    protected:

        void AutoFixSelectedContent();
    };
}

#endif //!__XUI_UI_LAYOUT_DOCK_LAYOUT_ANCHORABLE_PANE_H__