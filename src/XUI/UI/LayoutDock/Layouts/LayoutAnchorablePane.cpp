#include "StdAfx.h"
#include "LayoutAnchorablePane.h"
#include "LayoutAnchorableFloatingWindow.h"
#include "cpplinq/linq.hpp"

XUI::UI::LayoutAnchorablePane::LayoutAnchorablePane()
{
}

XUI::UI::LayoutAnchorablePane::~LayoutAnchorablePane()
{
}

void XUI::UI::LayoutAnchorablePane::SetSelectedContentIndex(int value)
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

bool XUI::UI::LayoutAnchorablePane::GetVisibility()
{
    return Children.size() > 0 && cpplinq::from(Children.GetNativeEnumerator()).any([](std::shared_ptr<LayoutElement> const& x) { return static_cast<LayoutAnchorable*>(x.get())->IsVisible; });
}

bool XUI::UI::LayoutAnchorablePane::IsDirectlyHostedInFloatingWindow()
{
    auto parentFloatingWindow = LayoutElement::FindParent<XUI::UI::LayoutAnchorableFloatingWindow>(this);
    if (parentFloatingWindow)
        return parentFloatingWindow->IsSinglePane();

    return false;
}

bool XUI::UI::LayoutAnchorablePane::IsHostedInFloatingWindow()
{
    return LayoutElement::FindParent<XUI::UI::LayoutFloatingWindow>(this);
}

void XUI::UI::LayoutAnchorablePane::ChildMoved(int oldIndex, int newIndex)
{
    if (_selectedIndex == oldIndex)
    {
        _selectedIndex = newIndex;
        NotifyPropertyChanged::RaisePropertyChanged("SelectedContentIndex");
    }

    LayoutPositionableGroup::ChildMoved(oldIndex, newIndex);
}

XUI::UI::LayoutContent * XUI::UI::LayoutAnchorablePane::SelectedContent()
{
    return _selectedIndex == -1 ? nullptr : GetChild(_selectedIndex);
}

void XUI::UI::LayoutAnchorablePane::OnChildrenCollectionChanged()
{
    AutoFixSelectedContent();
    for (int i = 0; i < Children.size(); i++)
    {
        if (GetChild(i)->IsSelected)
        {
            SelectedContentIndex = i;
            break;
        }
    }

    NotifyPropertyChanged::RaisePropertyChanged("CanClose");
    NotifyPropertyChanged::RaisePropertyChanged("CanHide");
    NotifyPropertyChanged::RaisePropertyChanged("IsDirectlyHostedInFloatingWindow");
    LayoutPositionableGroup::OnChildrenCollectionChanged();
}

int XUI::UI::LayoutAnchorablePane::IndexOf(LayoutContent * content)
{
    auto anchorableChild = content->shared_from_base_static<LayoutAnchorable>();
    if (anchorableChild == nullptr)
        return -1;

    return Children.IndexOf(anchorableChild);
}

void XUI::UI::LayoutAnchorablePane::SetNextSelectedIndex()
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

void XUI::UI::LayoutAnchorablePane::UpdateIsDirectlyHostedInFloatingWindow()
{
    NotifyPropertyChanged::RaisePropertyChanged("IsDirectlyHostedInFloatingWindow");
}

void XUI::UI::LayoutAnchorablePane::OnParentChanged(LayoutContainer * oldValue, LayoutContainer * newValue)
{
    auto oldGroup = oldValue ? oldValue->Static_As<LayoutGroup>() : nullptr;
    if (oldGroup)
        oldGroup->ChildrenCollectionChanged -= System::make_eventhandler(&LayoutAnchorablePane::OnParentChildrenCollectionChanged, this);

    NotifyPropertyChanged::RaisePropertyChanged("IsDirectlyHostedInFloatingWindow");

    auto newGroup = newValue ? newValue->Static_As<LayoutGroup>() : nullptr;
    if (newGroup)
        newGroup->ChildrenCollectionChanged += System::make_eventhandler(&LayoutAnchorablePane::OnParentChildrenCollectionChanged, this);

    LayoutPositionableGroup::OnParentChanged(oldValue, newValue);
}

void XUI::UI::LayoutAnchorablePane::OnParentChildrenCollectionChanged(void * sender, System::Events::EventArg & e)
{
    NotifyPropertyChanged::RaisePropertyChanged("IsDirectlyHostedInFloatingWindow");
}

void XUI::UI::LayoutAnchorablePane::AutoFixSelectedContent()
{
    if (_autoFixSelectedContent)
    {
        if (SelectedContentIndex >= Children.size())
            SelectedContentIndex = Children.size() - 1;

        if (SelectedContentIndex == -1 && Children.size() > 0)
            SetNextSelectedIndex();
    }
}

bool XUI::UI::LayoutAnchorablePane::GetCanHide() const
{
    return cpplinq::from(m_children.GetNativeEnumerator()).all([](std::shared_ptr<LayoutElement> const& x) { return static_cast<LayoutAnchorable*>(x.get())->CanHide; });
}

bool XUI::UI::LayoutAnchorablePane::GetCanClose() const
{
    return cpplinq::from(m_children.GetNativeEnumerator()).all([](std::shared_ptr<LayoutElement> const& x) { return static_cast<LayoutAnchorable*>(x.get())->CanClose; });
}
