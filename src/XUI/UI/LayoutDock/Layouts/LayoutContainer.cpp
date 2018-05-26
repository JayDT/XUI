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

#include "StdAfx.h"
#include "LayoutContainer.h"

XUI::UI::LayoutContainer::LayoutContainer()
{
    //m_children.CollectionChanged += System::make_eventhandler(&LayoutContainer::OnItemsChanged, this);
}

void XUI::UI::LayoutContainer::AddChild(LayoutElement * child)
{
    m_children.Add(child->shared_from_base_static<LayoutElement>());
}

void XUI::UI::LayoutContainer::RemoveChildAt(int childIndex)
{
    m_children.RemoveAt(childIndex);
}

void XUI::UI::LayoutContainer::InsertChildAt(int Index, LayoutElement * child)
{
    m_children.Insert(Index, child->shared_from_base_static<LayoutElement>());
}

void XUI::UI::LayoutContainer::RemoveChild(LayoutElement * child)
{
    System::ThrowIfFailed<System::ArgumentException>(child->Parent == this || child->Parent == nullptr);

    m_children.Remove(child->shared_from_base_static<LayoutElement>());
}

void XUI::UI::LayoutContainer::ReplaceChild(LayoutElement * oldchild, LayoutElement * newchild)
{
}

void XUI::UI::LayoutContainer::SetChildren(System::Collection::List<std::shared_ptr<LayoutElement>> & value)
{
    for (auto itr = m_children.begin(); itr != m_children.end();)
    {
        auto child = (itr++)->get();
        RemoveChild(child);
    }
    for (auto itr = value.begin(); itr != value.end(); ++itr)
    {
        AddChild(itr->get());
    }
}

//void XUI::UI::LayoutContainer::OnItemsChanged(void * sender, System::Collection::NotifyCollectionChangedEventArgs & e)
//{
//    switch (e.Action)
//    {
//        case System::Collection::NotifyCollectionChangedAction::Add:
//        {
//            break;
//        }
//        case System::Collection::NotifyCollectionChangedAction::Remove:
//        {
//            break;
//        }
//        case System::Collection::NotifyCollectionChangedAction::Reset:
//        {
//            break;
//        }
//        default:
//        {
//            break;
//        }
//    }
//}
