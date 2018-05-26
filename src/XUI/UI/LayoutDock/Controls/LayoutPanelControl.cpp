#include "UI/LayoutDock/DockingManager.h"
#include "LayoutPanelControl.h"
#include "LayoutPanel.h"
#include "LayoutElement.h"
#include "LayoutDocumentPaneGroup.h"
#include "LayoutDocumentPane.h"

using namespace XUI;
using namespace XUI::UI;

LayoutPanelControl::LayoutPanelControl(std::shared_ptr<LayoutPanel> const& model)
    : LayoutGridControl(model->shared_from_base_static<LayoutGroupBase>(), model->Orientation)
    //, _model(model)
{
}

void LayoutPanelControl::OnFixChildrenDockLengths()
{
    if (ActualWidth() == 0.0 || ActualHeight() == 0.0)
    {
        return;
    }

    auto modelAsPositionableElement = Model->Static_As<LayoutPanel>();

    if (modelAsPositionableElement->Orientation == Orientation::Horizontal)
    {
        if (LayoutContainer::ContainsChildOfType<LayoutDocumentPane, LayoutDocumentPaneGroup>(Model.get()))
        {
            for (int i = 0; i < Model->Children.size(); i++)
            {
                auto childContainerModel = Model->GetChildElement(i)->Static_As<LayoutContainer>();
                auto childPositionableModel = Model->GetChildElement(i)->Static_As<LayoutGroupBase>();

                if (childContainerModel != nullptr && (childContainerModel->Is<LayoutDocumentPane>() || childContainerModel->Is<LayoutDocumentPaneGroup>() || LayoutContainer::ContainsChildOfType<LayoutDocumentPane, LayoutDocumentPaneGroup>(childContainerModel)))
                {
                    childPositionableModel->SetDockWidth(GridLength(1.0, GridUnitType::Star));
                }
                else if (childPositionableModel != nullptr && childPositionableModel->GetDockWidth().IsStar)
                {
                    auto childPositionableModelWidthActualSize = childPositionableModel->Static_As<LayoutGroupBase>();

                    auto widthToSet = std::max(childPositionableModelWidthActualSize->GetActualWidth(), childPositionableModel->GetDockMinWidth());

                    widthToSet = std::min(widthToSet, ActualWidth() / 2.0);
                    widthToSet = std::max(widthToSet, childPositionableModel->GetDockMinWidth());

                    childPositionableModel->SetDockWidth(GridLength(widthToSet, GridUnitType::Pixel));
                }
            }
        }
        else
        {
            for (int i = 0; i < Model->Children.size(); i++)
            {
                auto childPositionableModel = Model->GetChildElement(i)->Static_As<LayoutGroupBase>();
                if (!childPositionableModel->GetDockWidth().IsStar)
                {
                    childPositionableModel->SetDockWidth(GridLength(1.0, GridUnitType::Star));
                }
            }
        }
    }
    else
    {
        if (LayoutContainer::ContainsChildOfType<LayoutDocumentPane, LayoutDocumentPaneGroup>(Model.get()))
        {
            for (int i = 0; i < Model->Children.size(); i++)
            {
                auto childContainerModel = Model->GetChildElement(i)->Static_As<LayoutContainer>();
                auto childPositionableModel = Model->GetChildElement(i)->Static_As<LayoutGroupBase>();

                if (childContainerModel != nullptr && (childContainerModel->Is<LayoutDocumentPane>() || childContainerModel->Is<LayoutDocumentPaneGroup>() || LayoutContainer::ContainsChildOfType<LayoutDocumentPane, LayoutDocumentPaneGroup>(childContainerModel)))
                {
                    childPositionableModel->SetDockHeight(GridLength(1.0, GridUnitType::Star));
                }
                else if (childPositionableModel != nullptr && childPositionableModel->GetDockHeight().IsStar)
                {
                    auto childPositionableModelWidthActualSize = childPositionableModel->Static_As<LayoutGroupBase>();

                    auto heightToSet = std::max(childPositionableModelWidthActualSize->GetActualHeight(), childPositionableModel->GetDockMinHeight());
                    heightToSet = std::min(heightToSet, ActualHeight() / 2.0);
                    heightToSet = std::max(heightToSet, childPositionableModel->GetDockMinHeight());

                    childPositionableModel->SetDockHeight(GridLength(heightToSet, GridUnitType::Pixel));
                }
            }
        }
        else
        {
            for (int i = 0; i < Model->Children.size(); i++)
            {
                auto childPositionableModel = Model->GetChildElement(i)->Static_As<LayoutGroupBase>();
                if (!childPositionableModel->GetDockHeight().IsStar)
                {
                    childPositionableModel->SetDockHeight(GridLength(1.0, GridUnitType::Star));
                }
            }
        }
    }
}
