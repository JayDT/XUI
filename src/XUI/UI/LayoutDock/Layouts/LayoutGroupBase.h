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

#ifndef __XUI_UI_LAYOUT_DOCK_LAYOUT_GROUP_BASE_H__
#define __XUI_UI_LAYOUT_DOCK_LAYOUT_GROUP_BASE_H__

#include "UI/LayoutDock/Layouts/LayoutContainer.h"
#include "UI/Detail/GridLength.h"

MetaAssemblyBegin
Meta(modul: ReflInclude("UI/LayoutDock/Layouts/LayoutContent.h"))
MetaAssemblyEnd

namespace XUI::UI
{
    class LayoutContent;

    class TC_XUI_API LayoutGroupBase : public LayoutContainer
    {
    public:

        System::Events::EventHandler<System::Events::EventArg> ChildrenCollectionChanged;
        System::Events::EventHandler<ChildrenTreeChangedEventArgs> ChildrenTreeChanged;

        virtual int IndexOf(LayoutContent* content) { return -1; }

        virtual GridLength GetDockHeight() const { return GridLength(1.0, GridUnitType::Star); }
        virtual GridLength GetDockWidth() const { return GridLength(1.0, GridUnitType::Star); }
        virtual void SetDockHeight(GridLength const& value) { }
        virtual void SetDockWidth(GridLength const& value) { }

        virtual void SetActualHeight(double) { }
        virtual void SetActualWidth(double) { }
        virtual double GetActualHeight() const { return 0; }
        virtual double GetActualWidth() const { return 0; }

        virtual bool GetAllowDuplicateContent() const { return false; }
        virtual void SetAllowDuplicateContent(bool value) {}
        virtual bool GetCanRepositionItems() const { return false; }
        virtual void SetCanRepositionItems(bool value) {}
        virtual double GetDockMinWidth() const { return 0; }
        virtual void SetDockMinWidth(double value) {}
        virtual double GetDockMinHeight() const { return 0; }
        virtual void SetDockMinHeight(double value) {}
        virtual double GetFloatingWidth() const { return 0; }
        virtual void SetFloatingWidth(double value) {}
        virtual double GetFloatingHeight() const { return 0; }
        virtual void SetFloatingHeight(double value) {}
        virtual double GetFloatingLeft() const { return 0; }
        virtual void SetFloatingLeft(double value) {}
        virtual double GetFloatingTop() const { return 0; }
        virtual void SetFloatingTop(double value) {}
        virtual bool GetIsMaximized() const { return false; }
        virtual void SetIsMaximized(bool value) {}

        virtual int GetSelectedContentIndex() const { return -1; }
        virtual void SetSelectedContentIndex(int value) {}

        virtual LayoutElement* GetChildElement(int Index) const { return nullptr; }

    protected:

        virtual void OnChildrenCollectionChanged()
        {
            if (ChildrenCollectionChanged)
            {
                static System::Events::EventArg e;
                ChildrenCollectionChanged(this, e);
            }
        }

        virtual void OnChildrenTreeChanged(ChildrenTreeChange change)
        {
            if (ChildrenTreeChanged)
            {
                ChildrenTreeChangedEventArgs e(change);
                ChildrenTreeChanged(this, e);
            }
        }

        void NotifyChildrenTreeChanged(ChildrenTreeChange change)
        {
            OnChildrenTreeChanged(change);

        	auto parentGroup = GetParent() ? GetParent()->Static_As<LayoutGroupBase>() : nullptr;
            if (parentGroup)
                parentGroup->NotifyChildrenTreeChanged(ChildrenTreeChange::TreeChanged);
        }
    };
}

#endif //!__XUI_UI_LAYOUT_DOCK_LAYOUT_GROUP_BASE_H__