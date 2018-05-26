#include "StdAfx.h"
#include "LayoutAnchorSide.h"
#include "LayoutRoot.h"

void XUI::UI::LayoutAnchorSide::UpdateSide()
{
    if (Root->LeftSide.get() == this)
        Side = AnchorSide::Left;
    else if (Root->TopSide.get() == this)
        Side = AnchorSide::Top;
    else if (Root->RightSide.get() == this)
        Side = AnchorSide::Right;
    else if (Root->BottomSide.get() == this)
        Side = AnchorSide::Bottom;
}

XUI::UI::LayoutAnchorSide::LayoutAnchorSide()
{
}

XUI::UI::LayoutAnchorSide::~LayoutAnchorSide()
{
}

bool XUI::UI::LayoutAnchorSide::GetVisibility()
{
    return true;
}
