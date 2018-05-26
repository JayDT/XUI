#include "DockingManager.h"
#include "LayoutAnchorablePaneGroupControl.h"
#include "LayoutAnchorablePaneGroup.h"

using namespace XUI;
using namespace XUI::UI;

LayoutAnchorablePaneGroupControl::LayoutAnchorablePaneGroupControl(LayoutAnchorablePaneGroup *model)
    : LayoutGridControl(model->shared_from_base_static<LayoutGroupBase>() , model->Orientation)
{
}

void LayoutAnchorablePaneGroupControl::OnFixChildrenDockLengths()
{
    LayoutAnchorablePaneGroup* model = static_cast<LayoutAnchorablePaneGroup*>(Model.get());
    if (model->Orientation == Orientation::Horizontal)
    {
        for (int i = 0; i < model->Children.size(); i++)
        {
            auto childModel = model->Children[i]->Static_As<LayoutGroupBase>();
            if (!childModel->GetDockWidth().IsStar)
            {
                childModel->SetDockWidth(GridLength(1.0, GridUnitType::Star));
            }
        }
    }
    else
    {
        for (int i = 0; i < model->Children.size(); i++)
        {
            auto childModel = model->Children[i]->Static_As<LayoutGroupBase>();;
            if (!childModel->GetDockHeight().IsStar)
            {
                childModel->SetDockHeight(GridLength(1.0, GridUnitType::Star));
            }
        }
    }
}
