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

#ifndef __XUI_UI_LAYOUT_DOCK_LAYOUT_DOCUMENT_PANE_H__
#define __XUI_UI_LAYOUT_DOCK_LAYOUT_DOCUMENT_PANE_H__

#include "UI/LayoutDock/Layouts/LayoutPositionableGroup.h"
#include "UI/LayoutDock/Layouts/LayoutContent.h"

namespace XUI::UI
{
    class LayoutDocumentPane : public LayoutPositionableGroup<LayoutContent>
    {
    private:

        std::string _id;
        int _selectedIndex = -1;
        bool _showHeader = true;

    public:

        LayoutDocumentPane();
        //LayoutDocumentPane(LayoutContent*);
        virtual ~LayoutDocumentPane();

        PROPERTY_(bool, ShowHeader);
        bool GetShowHeader() const { return _showHeader; }
        void SetShowHeader(bool value)
        {
            if (_showHeader != value)
            {
                _showHeader = value;
                NotifyPropertyChanged::RaisePropertyChanged("ShowHeader");
            }
        }

        PROPERTY_(int, SelectedContentIndex);
        int  GetSelectedContentIndex() const { return _selectedIndex; }
        void SetSelectedContentIndex(int value)
        {
            if (_selectedIndex != value)
            {
                if (_selectedIndex >= 0 &&
                    _selectedIndex < Children.size())
                    GetChild(_selectedIndex)->IsSelected = false;

                _selectedIndex = value;

                if (_selectedIndex >= 0 &&
                    _selectedIndex < Children.size())
                    GetChild(_selectedIndex)->IsSelected = true;

                NotifyPropertyChanged::RaisePropertyChanged("SelectedContentIndex");
                NotifyPropertyChanged::RaisePropertyChanged("SelectedContent");
            }
        }

        PROPERTY_(std::string, Id);
        std::string const& GetId() const { return _id; }
        void SetId(std::string const& value)
        {
            if (_id != value)
                _id = value;
        }

        LayoutContent* GetSelectedContent();

        void SetNextSelectedIndex();

    protected:

        bool GetVisibility() override;

        void ChildMoved(int oldIndex, int newIndex) override;

        void OnChildrenCollectionChanged() override;

        int IndexOf(LayoutContent* content);

        void OnIsVisibleChanged() override;

        void UpdateParentVisibility();

    };
}

#endif //!__XUI_UI_LAYOUT_DOCK_LAYOUT_DOCUMENT_PANE_H__