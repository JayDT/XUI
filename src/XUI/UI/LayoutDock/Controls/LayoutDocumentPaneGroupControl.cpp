#include "StdAfx.h"
#include "UI/LayoutDock/DockingManager.h"
#include "LayoutDocumentPaneGroupControl.h"
#include "LayoutDocumentPaneGroup.h"

using namespace XUI;
using namespace XUI::UI;

XUI::UI::LayoutDocumentPaneGroupControl::LayoutDocumentPaneGroupControl(LayoutDocumentPaneGroup* model)
    : LayoutGridControl(model->shared_from_base_static<LayoutGroupBase>(), model->Orientation)
{
}

XUI::UI::LayoutDocumentPaneGroupControl::~LayoutDocumentPaneGroupControl()
{
}

void XUI::UI::LayoutDocumentPaneGroupControl::OnFixChildrenDockLengths()
{
    LayoutDocumentPaneGroup* model = static_cast<LayoutDocumentPaneGroup*>(Model.get());
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
