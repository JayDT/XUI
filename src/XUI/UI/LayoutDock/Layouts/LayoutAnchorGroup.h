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

#ifndef __XUI_UI_LAYOUT_DOCK_LAYOUT_ANCHOR_GROUP_H__
#define __XUI_UI_LAYOUT_DOCK_LAYOUT_ANCHOR_GROUP_H__

#include "UI/LayoutDock/Layouts/LayoutGroup.h"
#include "UI/LayoutDock/Layouts/LayoutAnchorable.h"

namespace XUI::UI
{
    class LayoutAnchorGroup : public LayoutGroup<LayoutAnchorable>
    {
    private:

        std::string _id;
        std::shared_ptr<LayoutContainer> _previousContainer;
        int _previousContainerIndex = -1;


    public:

        LayoutAnchorGroup();
        virtual ~LayoutAnchorGroup();

        PROPERTY_(LayoutContainer*, PreviousContainer);
        LayoutContainer* GetPreviousContainer() const { return _previousContainer.get(); }
        void SetPreviousContainer(LayoutContainer* value);

        PROPERTY_(int, PreviousContainerIndex);
        int  GetPreviousContainerIndex() const { return _previousContainerIndex; }
        void SetPreviousContainerIndex(int value)
        {
            if (_previousContainerIndex != value)
            {
                _previousContainerIndex = value;
                NotifyPropertyChanged::RaisePropertyChanged("PreviousContainerIndex");
            }
        }

        PROPERTY_(std::string, Id);
        std::string const& GetId() const { return _id; }
        void SetId(std::string const& value)
        {
            if (_id != value)
                _id = value;
        }

        bool GetVisibility() override
        {
            return Children.size() > 0;
        }
    };
}

#endif //!__XUI_UI_LAYOUT_DOCK_LAYOUT_ANCHOR_GROUP_H__