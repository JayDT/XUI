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
#include "LayoutElement.h"
#include "LayoutRoot.h"
#include "LayoutContainer.h"
#include "LayoutDocumentPaneGroup.h"
#include "LayoutPanel.h"
#include "DataGridDefines.h"

XUI::UI::LayoutElement::LayoutElement()
{
}

XUI::UI::LayoutElement::~LayoutElement()
{
}

void XUI::UI::LayoutElement::SetParent(LayoutContainer * value)
{
    auto oldParent = GetParent();
    if (oldParent != value)
    {
        auto root = GetRoot();
        OnParentChanging(oldParent, value);
        m_parent = value ? value->shared_from_base_static<LayoutContainer>() : nullptr;

        LayoutElement* newRoot = value;
        while (newRoot && !newRoot->Is<LayoutRoot>())
            newRoot = newRoot->GetParent();

        if (!newRoot || !newRoot->Is<LayoutRoot>())
            newRoot = nullptr;

        if (newRoot != root)
        {
            m_root = newRoot ? newRoot->shared_from_base_static<LayoutRoot>() : nullptr;
            OnRootChanged(root, static_cast<LayoutRoot*>(newRoot));
        }

        OnParentChanged(oldParent, value);
        NotifyPropertyChanged::RaisePropertyChanged("Parent");

        if (newRoot)
            static_cast<LayoutRoot*>(newRoot)->FireLayoutUpdated();
    }
}

XUI::UI::LayoutRoot * XUI::UI::LayoutElement::GetRoot() const
{
    if (m_root.expired())
    {
        LayoutElement* newRoot = GetParent();
        while (newRoot && !newRoot->Is<LayoutRoot>())
            newRoot = newRoot->GetParent();

        if (!newRoot || !newRoot->Is<LayoutRoot>())
            newRoot = nullptr;

        if (newRoot != nullptr)
        {
            m_root = newRoot ? newRoot->shared_from_base_static<LayoutRoot>() : nullptr;
            const_cast<LayoutElement*>(this)->OnRootChanged(nullptr, static_cast<LayoutRoot*>(newRoot));
        }

    }
    return m_root.lock().get();
}

XUI::UI::LayoutRoot * XUI::UI::LayoutElement::GetRoot(LayoutElement * element)
{
    if (element->Is<LayoutRoot>())
        return element->Static_As<LayoutRoot>();

    auto parent = element->GetParent();
    while (parent && !parent->Is<LayoutRoot>())
        parent = parent->GetParent();

    return parent ? parent->Static_As<LayoutRoot>() : nullptr;
}

XUI::UI::AnchorSide XUI::UI::LayoutElement::GetSide(LayoutElement * element)
{
    LayoutContainer* parentContainer = element->GetParent(); //  ILayoutOrientableGroup
    if (!parentContainer)
    {
        ASSERT(false && "Unable to find the side for an element, possible layout problem!");
        return AnchorSide::Right;
    }

    if (!LayoutContainer::ContainsChildOfType<LayoutPanel>(parentContainer))
        return GetSide(parentContainer);

    for(auto childElement : parentContainer->Children.GetNativeEnumerator())
    {
        if (childElement.get() == element ||
            LayoutContainer::ContainsChild(childElement->Static_As<LayoutContainer>(), [element](LayoutElement* x) { return x == element; }))
            return parentContainer->GetOrientation() == UI::Orientation::Horizontal ? AnchorSide::Left : AnchorSide::Top;

        auto childElementAsContainer = childElement->Static_As<LayoutContainer>();
        if (childElementAsContainer &&
            (childElementAsContainer->Is<LayoutDocumentPane>() || childElementAsContainer->Is<LayoutDocumentPaneGroup>() ||
                LayoutContainer::ContainsChildOfType<LayoutDocumentPane, LayoutDocumentPaneGroup>(childElementAsContainer)))
            return parentContainer->GetOrientation() == UI::Orientation::Horizontal ? AnchorSide::Right : AnchorSide::Bottom;
    }

    ASSERT(false && "Unable to find the side for an element, possible layout problem!");
    return AnchorSide::Right;
}

void XUI::UI::LayoutElement::OnRootChanged(LayoutRoot * oldRoot, LayoutRoot * newRoot)
{
    if (oldRoot)
        oldRoot->OnLayoutElementRemoved(this);
    if (newRoot)
        newRoot->OnLayoutElementAdded(this);
}

void XUI::UI::LayoutElement::LayoutElementReference::targetObjectBuildLink()
{
    getSource()->GetElements().insertLast(this);
}
