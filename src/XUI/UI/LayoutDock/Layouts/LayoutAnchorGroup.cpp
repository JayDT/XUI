#include "StdAfx.h"
#include "LayoutAnchorGroup.h"

XUI::UI::LayoutAnchorGroup::LayoutAnchorGroup()
{
}

XUI::UI::LayoutAnchorGroup::~LayoutAnchorGroup()
{
}

void XUI::UI::LayoutAnchorGroup::SetPreviousContainer(LayoutContainer * value)
{
    if (_previousContainer.get() != value)
    {
        if (value)
            _previousContainer = value->shared_from_base_static<LayoutContainer>();
        else
            _previousContainer.reset();
        NotifyPropertyChanged::RaisePropertyChanged("PreviousContainer");

        //auto paneSerializable = _previousContainer->Static_As<LayoutPaneSerializable>();
        //if (paneSerializable &&
        //    paneSerializable->Id.empty())
        //    paneSerializable->Id = Guid.NewGuid().ToString();
    }
}
