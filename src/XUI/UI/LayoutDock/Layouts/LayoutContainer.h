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

#ifndef __XUI_UI_LAYOUT_DOCK_LAYOUT_CONTAINER_H__
#define __XUI_UI_LAYOUT_DOCK_LAYOUT_CONTAINER_H__

#include "UI/LayoutDock/Layouts/LayoutElement.h"
#include "UI/DataGrid/DataGridDefines.h"
#include "standard/LinkedReference/RefManager.h"

namespace XUI::UI
{
    class TC_XUI_API LayoutContainer : public LayoutElement
    {
    protected:

        System::Collection::List<std::shared_ptr<LayoutElement>> m_children;

    public:

        LayoutContainer();
        virtual ~LayoutContainer() {}

        virtual void AddChild(LayoutElement* child);
        virtual void RemoveChild(LayoutElement* child);
        virtual void ReplaceChild(LayoutElement* oldchild, LayoutElement* newchild);

        virtual void MoveChild(int oldIndex, int newIndex) {}
        virtual void RemoveChildAt(int childIndex);
        virtual int IndexOfChild(LayoutElement* element) { return -1; }
        virtual void InsertChildAt(int index, LayoutElement* element);
        
        Meta(XamlCPP::ContentAttribute())
        PROPERTY_(System::Collection::List<std::shared_ptr<LayoutElement>>&, Children);
        virtual System::Collection::List<std::shared_ptr<LayoutElement>> const& GetChildren() const { return m_children; }
        virtual void SetChildren(System::Collection::List<std::shared_ptr<LayoutElement>> & value);

		PROPERTY_GET_ONLY(size_t, ChildrenCount);
		virtual size_t GetChildrenCount() const { return m_children.size(); }

        virtual UI::Orientation GetOrientation() const { return UI::Orientation::Horizontal; }
        virtual void SetOrientation(UI::Orientation) { }

        //void OnItemsChanged(void * sender, System::Collection::NotifyCollectionChangedEventArgs & e);

        template<typename TFunc>
        static bool ContainsChild(LayoutContainer* element, TFunc func, bool deep = true)
        {
            if (!element)
                return false;

            for (auto itr = element->m_children.GetNativeEnumerator().begin(); itr != element->m_children.GetNativeEnumerator().end();)
            {
                auto childElement = *itr;
                ++itr;

                if (func(childElement.get()))
                    return true;

                auto container = childElement->Static_As<LayoutContainer>();
                if (container && LayoutContainer::ContainsChild<TFunc>(container, func))
                    return true;
            }

            return false;
        }

        template<class T>
        static bool ContainsChildOfType(LayoutContainer* element)
        {
            for (auto const& childElement : element->m_children.GetNativeEnumerator())
            {
                if (childElement->Is<T>())
                    return true;

                auto container = childElement->Static_As<LayoutContainer>();
                if (container && LayoutContainer::ContainsChildOfType<T>(container))
                    return true;
            }

            return false;
        }

        template<class T, class S>
        static bool ContainsChildOfType(LayoutContainer* container)
        {
            for (auto const& childElement : container->m_children.GetNativeEnumerator())
            {
                if (childElement->Is<T>() || childElement->Is<S>())
                    return true;

                auto container = childElement->Static_As<LayoutContainer>();
                if (container && LayoutContainer::ContainsChildOfType<T, S>(container))
                    return true;
            }

            return false;
        }
    };
}

#endif //!__XUI_UI_LAYOUT_DOCK_LAYOUT_CONTAINER_H__