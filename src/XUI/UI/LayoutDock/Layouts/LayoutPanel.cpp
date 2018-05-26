#include "StdAfx.h"
#include "LayoutPanel.h"
#include "LayoutGroup.h"
#include "cpplinq/linq.hpp"

XUI::UI::LayoutPanel::LayoutPanel()
{
}

XUI::UI::LayoutPanel::~LayoutPanel()
{
}

bool XUI::UI::LayoutPanel::GetVisibility()
{
    return cpplinq::from(Children.GetNativeEnumerator()).any([](std::shared_ptr<LayoutElement> const& c) { return c->GetIsVisible(); });
}
