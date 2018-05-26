#include "StdAfx.h"
#include "LayoutDocumentPane.h"
#include "LayoutDocumentPaneGroup.h"
#include "LayoutPanel.h"
#include "DockingManager.h"
#include "cpplinq/linq.hpp"

XUI::UI::LayoutDocumentPane::LayoutDocumentPane()
{
}

//XUI::UI::LayoutDocumentPane::LayoutDocumentPane(LayoutContent * content)
//{
//    AddChild(content);
//}

XUI::UI::LayoutDocumentPane::~LayoutDocumentPane()
{
}

XUI::UI::LayoutContent * XUI::UI::LayoutDocumentPane::GetSelectedContent()
{
    return _selectedIndex == -1 ? nullptr : GetChild(_selectedIndex);
}

void XUI::UI::LayoutDocumentPane::SetNextSelectedIndex()
{
    SelectedContentIndex = -1;
    for (int i = 0; i < Children.size(); ++i)
    {
        if (GetChild(i)->IsEnabled)
        {
            SelectedContentIndex = i;
            return;
        }
    }
}

bool XUI::UI::LayoutDocumentPane::GetVisibility()
{
    if (Parent && (Parent->Is<LayoutDocumentPaneGroup>() || Parent->Is<LayoutPanel>()))
        return !Children.empty();

    return true;
}

void XUI::UI::LayoutDocumentPane::ChildMoved(int oldIndex, int newIndex)
{
    if (_selectedIndex == oldIndex)
    {
        _selectedIndex = newIndex;
        NotifyPropertyChanged::RaisePropertyChanged("SelectedContentIndex");
    }


    LayoutPositionableGroup::ChildMoved(oldIndex, newIndex);
}

void XUI::UI::LayoutDocumentPane::OnChildrenCollectionChanged()
{
    if (SelectedContentIndex >= Children.size())
        SelectedContentIndex = Children.size() - 1;
    if (SelectedContentIndex == -1 && Children.size() > 0)
    {
        if (Root == nullptr)
        {
            SetNextSelectedIndex();
        }
        else
        {
            LayoutElement* childrenToSelect = nullptr; //cpplinq::from(Children.GetNativeEnumerator()).order(c = > c.LastActivationTimeStamp.GetValueOrDefault()).First();
            for (auto const& element : Children.GetNativeEnumerator())
            {
                if (!childrenToSelect || static_cast<LayoutContent*>(childrenToSelect)->LastActivationTimeStamp > static_cast<LayoutContent*>(element.get())->LastActivationTimeStamp)
                    childrenToSelect = element.get();
            }

            SelectedContentIndex = childrenToSelect ? Children.IndexOf(childrenToSelect->shared_from_base_static<LayoutElement>()) : -1;
            if (childrenToSelect)
                static_cast<LayoutContent*>(childrenToSelect)->IsActive = true;
        }
    }

    LayoutPositionableGroup::OnChildrenCollectionChanged();

    NotifyPropertyChanged::RaisePropertyChanged("ChildrenSorted");
}

int XUI::UI::LayoutDocumentPane::IndexOf(LayoutContent * content)
{
    return Children.IndexOf(content->shared_from_base_static<LayoutElement>());
}

void XUI::UI::LayoutDocumentPane::OnIsVisibleChanged()
{
    UpdateParentVisibility();
    LayoutPositionableGroup::OnIsVisibleChanged();
}

void XUI::UI::LayoutDocumentPane::UpdateParentVisibility()
{
    auto parentPane = Parent;
    if (parentPane)
        parentPane->ComputeVisibility();
}
