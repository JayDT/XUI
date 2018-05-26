#include "StdAfx.h"
#include "LayoutAnchorablePaneGroup.h"
#include "cpplinq/linq.hpp"

XUI::UI::LayoutAnchorablePaneGroup::LayoutAnchorablePaneGroup()
{
}

XUI::UI::LayoutAnchorablePaneGroup::~LayoutAnchorablePaneGroup()
{
}

bool XUI::UI::LayoutAnchorablePaneGroup::GetVisibility()
{
    return Children.size() > 0 && cpplinq::from(Children.GetNativeEnumerator()).any([](std::shared_ptr<LayoutElement> const& x) { return static_cast<LayoutAnchorablePane*>(x.get())->IsVisible; });
}

void XUI::UI::LayoutAnchorablePaneGroup::OnIsVisibleChanged()
{
    UpdateParentVisibility();
    LayoutPositionableGroup::OnIsVisibleChanged();
}

void XUI::UI::LayoutAnchorablePaneGroup::UpdateParentVisibility()
{
    auto parentPane = GetParent();
    if (parentPane)
        parentPane->ComputeVisibility();
}

void XUI::UI::LayoutAnchorablePaneGroup::OnDockWidthChanged()
{
    if (DockWidth.IsAbsolute && Children.size() == 1)
        GetChild(0)->DockWidth = DockWidth;

    LayoutPositionableGroup::OnDockWidthChanged();
}

void XUI::UI::LayoutAnchorablePaneGroup::OnDockHeightChanged()
{
    if (DockHeight.IsAbsolute && Children.size() == 1)
        GetChild(0)->DockHeight = DockHeight;
    LayoutPositionableGroup::OnDockHeightChanged();
}

void XUI::UI::LayoutAnchorablePaneGroup::OnChildrenCollectionChanged()
{
    if (DockWidth.IsAbsolute && Children.size() == 1)
        GetChild(0)->DockWidth = DockWidth;
    if (DockHeight.IsAbsolute && Children.size() == 1)
        GetChild(0)->DockHeight = DockHeight;
    LayoutPositionableGroup::OnChildrenCollectionChanged();
}
