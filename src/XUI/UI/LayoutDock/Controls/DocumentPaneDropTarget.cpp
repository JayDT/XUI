#include "DocumentPaneDropTarget.h"
#include "DocumentPaneDropAsAnchorableTarget.h"
#include "LayoutPanel.h"
#include "LayoutDocumentPane.h"
#include "LayoutDocumentPaneGroup.h"
#include "OverlayWindow.h"
#include "DockingManager.h"
#include "Core/Media/Geometries/RectangleGeometry.h"

using namespace XUI;
using namespace XUI::UI;

DocumentPaneDropTarget::DocumentPaneDropTarget(LayoutDocumentPaneControl *paneControl, Core::Media::Rect const& detectionRect, DropTargetType type)
    : DropTarget<LayoutDocumentPaneControl>(paneControl->shared_from_base_static<LayoutDocumentPaneControl>(), detectionRect, type)
    , _targetPane(paneControl->shared_from_base_static<LayoutDocumentPaneControl>())
{
}

DocumentPaneDropTarget::DocumentPaneDropTarget(LayoutDocumentPaneControl *paneControl, Core::Media::Rect const& detectionRect, DropTargetType type, int tabIndex)
    : DropTarget<LayoutDocumentPaneControl>(paneControl->shared_from_base_static<LayoutDocumentPaneControl>(), detectionRect, type)
    , _targetPane(paneControl->shared_from_base_static<LayoutDocumentPaneControl>())
    , _tabIndex(tabIndex)
{
}

void DocumentPaneDropTarget::Drop(LayoutDocumentFloatingWindow *floatingWindow)
{
    LayoutGroupBase *targetModel = _targetPane->Model->Static_As<LayoutGroupBase>();

    switch (DropType)
    {
    case DropTargetType::DocumentPaneDockBottom:
    {
        auto newLayoutDocumentPane = std::make_shared<LayoutDocumentPane>();
        newLayoutDocumentPane->AddChild(floatingWindow->RootDocument.get());
        auto parentModel = targetModel->Parent->Static_As<LayoutDocumentPaneGroup>();

        if (parentModel == nullptr)
        {
            auto parentContainer = (targetModel->Parent);
            auto newParentModel = std::make_shared<LayoutDocumentPaneGroup>();
            newParentModel->Orientation = UI::Orientation::Vertical;
            parentContainer->ReplaceChild(targetModel, newParentModel.get());
            newParentModel->AddChild((targetModel));
            newParentModel->AddChild(newLayoutDocumentPane.get());
        }
        else
        {
            auto manager = parentModel->Root->GetManager();
            if (!manager->AllowMixedOrientation || parentModel->Orientation == UI::Orientation::Vertical)
            {
                parentModel->Orientation = UI::Orientation::Vertical;
                int targetPaneIndex = parentModel->IndexOfChild(targetModel);
                parentModel->InsertChildAt(targetPaneIndex + 1, newLayoutDocumentPane.get());
            }
            else
            {
                auto newChildGroup = std::make_shared<LayoutDocumentPaneGroup>();
                newChildGroup->Orientation = UI::Orientation::Vertical;
                parentModel->ReplaceChild(targetModel, newChildGroup.get());
                newChildGroup->AddChild(targetModel);
                newChildGroup->AddChild(newLayoutDocumentPane.get());
            }

        }
    }
    break;
    case DropTargetType::DocumentPaneDockTop:
    {
        auto newLayoutDocumentPane = std::make_shared<LayoutDocumentPane>();
        newLayoutDocumentPane->AddChild(floatingWindow->RootDocument.get());
        auto parentModel = targetModel->Parent->Static_As<LayoutDocumentPaneGroup>();

        if (parentModel == nullptr)
        {
            auto parentContainer = (targetModel->Parent);
            auto newParentModel = std::make_shared<LayoutDocumentPaneGroup>();
            newParentModel->Orientation = UI::Orientation::Vertical;
            parentContainer->ReplaceChild(targetModel, newParentModel.get());
            newParentModel->AddChild((targetModel));
            newParentModel->InsertChildAt(0, newLayoutDocumentPane.get());
        }
        else
        {
            auto manager = parentModel->Root->GetManager();
            if (!manager->AllowMixedOrientation || parentModel->Orientation == UI::Orientation::Vertical)
            {
                parentModel->Orientation = UI::Orientation::Vertical;
                int targetPaneIndex = parentModel->IndexOfChild(targetModel);
                parentModel->InsertChildAt(targetPaneIndex, newLayoutDocumentPane.get());
            }
            else
            {
                auto newChildGroup = std::make_shared<LayoutDocumentPaneGroup>();
                newChildGroup->Orientation = UI::Orientation::Vertical;
                parentModel->ReplaceChild(targetModel, newChildGroup.get());
                newChildGroup->AddChild(newLayoutDocumentPane.get());
                newChildGroup->AddChild(targetModel);
            }

        }
    }
    break;
    case DropTargetType::DocumentPaneDockLeft:
    {
        auto newLayoutDocumentPane = std::make_shared<LayoutDocumentPane>();
        newLayoutDocumentPane->AddChild(floatingWindow->RootDocument.get());
        auto parentModel = targetModel->Parent->Static_As<LayoutDocumentPaneGroup>();

        if (parentModel == nullptr)
        {
            auto parentContainer = (targetModel->Parent);
            auto newParentModel = std::make_shared<LayoutDocumentPaneGroup>();
            newParentModel->Orientation = UI::Orientation::Horizontal;
            parentContainer->ReplaceChild(targetModel, newParentModel.get());
            newParentModel->AddChild(targetModel);
            newParentModel->InsertChildAt(0, newLayoutDocumentPane.get());
        }
        else
        {
            auto manager = parentModel->Root->GetManager();
            if (!manager->AllowMixedOrientation || parentModel->Orientation == UI::Orientation::Horizontal)
            {
                parentModel->Orientation = UI::Orientation::Horizontal;
                int targetPaneIndex = parentModel->IndexOfChild(targetModel);
                parentModel->InsertChildAt(targetPaneIndex, newLayoutDocumentPane.get());
            }
            else
            {
                auto newChildGroup = std::make_shared<LayoutDocumentPaneGroup>();
                newChildGroup->Orientation = UI::Orientation::Horizontal;
                parentModel->ReplaceChild(targetModel, newChildGroup.get());
                newChildGroup->AddChild(newLayoutDocumentPane.get());
                newChildGroup->AddChild(targetModel);
            }
        }
    }
    break;
    case DropTargetType::DocumentPaneDockRight:
    {
        auto newLayoutDocumentPane = std::make_shared<LayoutDocumentPane>();
        newLayoutDocumentPane->AddChild(floatingWindow->RootDocument.get());
        auto parentModel = targetModel->Parent->Static_As<LayoutDocumentPaneGroup>();

        if (parentModel == nullptr)
        {
            auto parentContainer = (targetModel->Parent);
            auto newParentModel = std::make_shared<LayoutDocumentPaneGroup>();
            newParentModel->Orientation = UI::Orientation::Horizontal;
            parentContainer->ReplaceChild(targetModel, newParentModel.get());
            newParentModel->AddChild((targetModel));
            newParentModel->AddChild(newLayoutDocumentPane.get());
        }
        else
        {
            auto manager = parentModel->Root->GetManager();
            if (!manager->AllowMixedOrientation || parentModel->Orientation == UI::Orientation::Horizontal)
            {
                parentModel->Orientation = UI::Orientation::Horizontal;
                int targetPaneIndex = parentModel->IndexOfChild(targetModel);
                parentModel->InsertChildAt(targetPaneIndex + 1, newLayoutDocumentPane.get());
            }
            else
            {
                auto newChildGroup = std::make_shared<LayoutDocumentPaneGroup>();
                newChildGroup->Orientation = UI::Orientation::Horizontal;
                parentModel->ReplaceChild(targetModel, newChildGroup.get());
                newChildGroup->AddChild(targetModel);
                newChildGroup->AddChild(newLayoutDocumentPane.get());
            }

        }
    }
    break;
    case DropTargetType::DocumentPaneDockInside:
    {
        auto paneModel = targetModel->Static_As<LayoutDocumentPane>();
        auto sourceModel = floatingWindow->RootDocument;

        int i = 0;
        if (_tabIndex != -1)
        {
            i = _tabIndex;
        }
        else
        {
            auto previousContainer = sourceModel->PreviousContainer;
            if (previousContainer == targetModel && (sourceModel->PreviousContainerIndex != -1))
            {
                i = sourceModel->PreviousContainerIndex;
            }
        }
        sourceModel->IsActive = false;
        paneModel->InsertChildAt(i, sourceModel.get());
        sourceModel->IsActive = true;
    }
    break;
    }

    DropTarget<LayoutDocumentPaneControl>::Drop(floatingWindow);
}

void DocumentPaneDropTarget::Drop(LayoutAnchorableFloatingWindow *floatingWindow)
{
    LayoutGroupBase *targetModel = _targetPane->Model->Static_As<LayoutGroupBase>();

    switch (DropType)
    {
        case DropTargetType::DocumentPaneDockBottom:
        {
            auto parentModel = dynamic_cast<LayoutDocumentPaneGroup*>(targetModel->Parent);
            auto newLayoutDocumentPane = std::make_shared<LayoutDocumentPane>();

            if (parentModel == nullptr)
            {
                auto parentContainer = (targetModel->Parent);
                auto newParentModel = std::make_shared<LayoutDocumentPaneGroup>();
                newParentModel->Orientation = UI::Orientation::Vertical;
                parentContainer->ReplaceChild(targetModel, newParentModel.get());
                newParentModel->AddChild((targetModel));
                newParentModel->AddChild(newLayoutDocumentPane.get());
            }
            else
            {
                auto manager = parentModel->Root->GetManager();
                if (!manager->AllowMixedOrientation || parentModel->Orientation == UI::Orientation::Vertical)
                {
                    parentModel->Orientation = UI::Orientation::Vertical;
                    int targetPaneIndex = parentModel->IndexOfChild(targetModel);
                    parentModel->InsertChildAt(targetPaneIndex + 1, newLayoutDocumentPane.get());
                }
                else
                {
                    auto newChildGroup = std::make_shared<LayoutDocumentPaneGroup>();
                    newChildGroup->Orientation = UI::Orientation::Vertical;
                    parentModel->ReplaceChild(targetModel, newChildGroup.get());
                    newChildGroup->AddChild(targetModel);
                    newChildGroup->AddChild(newLayoutDocumentPane.get());
                }
            }

            LayoutContainer::ContainsChild(floatingWindow->RootPanel.get(), [&newLayoutDocumentPane](LayoutElement* x) -> bool
            {
                newLayoutDocumentPane->AddChild(x);
                return false;
            });
        }
        break;
        case DropTargetType::DocumentPaneDockTop:
        {
            auto parentModel = dynamic_cast<LayoutDocumentPaneGroup*>(targetModel->Parent);
            auto newLayoutDocumentPane = std::make_shared<LayoutDocumentPane>();

            if (parentModel == nullptr)
            {
                auto parentContainer = (targetModel->Parent);
                auto newParentModel = std::make_shared<LayoutDocumentPaneGroup>();
                newParentModel->Orientation = UI::Orientation::Vertical;
                parentContainer->ReplaceChild(targetModel, newParentModel.get());
                newParentModel->AddChild(newLayoutDocumentPane.get());
                newParentModel->AddChild((targetModel));
            }
            else
            {
                auto manager = parentModel->Root->GetManager();
                if (!manager->AllowMixedOrientation || parentModel->Orientation == UI::Orientation::Vertical)
                {
                    parentModel->Orientation = UI::Orientation::Vertical;
                    int targetPaneIndex = parentModel->IndexOfChild(targetModel);
                    parentModel->InsertChildAt(targetPaneIndex, newLayoutDocumentPane.get());
                }
                else
                {
                    auto newChildGroup = std::make_shared<LayoutDocumentPaneGroup>();
                    newChildGroup->Orientation = UI::Orientation::Vertical;
                    parentModel->ReplaceChild(targetModel, newChildGroup.get());
                    newChildGroup->AddChild(newLayoutDocumentPane.get());
                    newChildGroup->AddChild(targetModel);
                }
            }


            LayoutContainer::ContainsChild(floatingWindow->RootPanel.get(), [&newLayoutDocumentPane](LayoutElement* x) -> bool
            {
                newLayoutDocumentPane->AddChild(x);
                return false;
            });
        }
        break;
        case DropTargetType::DocumentPaneDockLeft:
        {
            auto parentModel = dynamic_cast<LayoutDocumentPaneGroup*>(targetModel->Parent);
            auto newLayoutDocumentPane = std::make_shared<LayoutDocumentPane>();

            if (parentModel == nullptr)
            {
                auto parentContainer = (targetModel->Parent);
                auto newParentModel = std::make_shared<LayoutDocumentPaneGroup>();
                newParentModel->Orientation = UI::Orientation::Horizontal;
                parentContainer->ReplaceChild(targetModel, newParentModel.get());
                newParentModel->AddChild(newLayoutDocumentPane.get());
                newParentModel->AddChild((targetModel));
            }
            else
            {
                auto manager = parentModel->Root->GetManager();
                if (!manager->AllowMixedOrientation || parentModel->Orientation == UI::Orientation::Horizontal)
                {
                    parentModel->Orientation = UI::Orientation::Horizontal;
                    int targetPaneIndex = parentModel->IndexOfChild(targetModel);
                    parentModel->InsertChildAt(targetPaneIndex, newLayoutDocumentPane.get());
                }
                else
                {
                    auto newChildGroup = std::make_shared<LayoutDocumentPaneGroup>();
                    newChildGroup->Orientation = UI::Orientation::Horizontal;
                    parentModel->ReplaceChild(targetModel, newChildGroup.get());
                    newChildGroup->AddChild(newLayoutDocumentPane.get());
                    newChildGroup->AddChild(targetModel);
                }

            }

            LayoutContainer::ContainsChild(floatingWindow->RootPanel.get(), [&newLayoutDocumentPane](LayoutElement* x) -> bool
            {
                newLayoutDocumentPane->AddChild(x);
                return false;
            });
        }
        break;
        case DropTargetType::DocumentPaneDockRight:
        {
            auto parentModel = dynamic_cast<LayoutDocumentPaneGroup*>(targetModel->Parent);
            auto newLayoutDocumentPane = std::make_shared<LayoutDocumentPane>();

            if (parentModel == nullptr)
            {
                auto parentContainer = (targetModel->Parent);
                auto newParentModel = std::make_shared<LayoutDocumentPaneGroup>();
                newParentModel->Orientation = UI::Orientation::Horizontal;
                parentContainer->ReplaceChild(targetModel, newParentModel.get());
                newParentModel->AddChild((targetModel));
                newParentModel->AddChild(newLayoutDocumentPane.get());
            }
            else
            {
                auto manager = parentModel->Root->GetManager();
                if (!manager->AllowMixedOrientation || parentModel->Orientation == UI::Orientation::Horizontal)
                {
                    parentModel->Orientation = UI::Orientation::Horizontal;
                    int targetPaneIndex = parentModel->IndexOfChild(targetModel);
                    parentModel->InsertChildAt(targetPaneIndex + 1, newLayoutDocumentPane.get());
                }
                else
                {
                    auto newChildGroup = std::make_shared<LayoutDocumentPaneGroup>();
                    newChildGroup->Orientation = UI::Orientation::Horizontal;
                    parentModel->ReplaceChild(targetModel, newChildGroup.get());
                    newChildGroup->AddChild(targetModel);
                    newChildGroup->AddChild(newLayoutDocumentPane.get());
                }
            }

            LayoutContainer::ContainsChild(floatingWindow->RootPanel.get(), [&newLayoutDocumentPane](LayoutElement* x) -> bool
            {
                newLayoutDocumentPane->AddChild(x);
                return false;
            });
        }
        break;
        case DropTargetType::DocumentPaneDockInside:
        {
            auto paneModel = targetModel->Static_As<LayoutDocumentPane>();
            auto layoutAnchorablePaneGroup = floatingWindow->RootPanel->Static_As<LayoutAnchorablePaneGroup>();

            bool checkPreviousContainer = true;
            int i = 0;
            if (_tabIndex != -1)
            {
                i = _tabIndex;
                checkPreviousContainer = false;
            }
            LayoutAnchorable *anchorableToActivate = nullptr;

            LayoutContainer::ContainsChild(floatingWindow->RootPanel.get(), [&](LayoutElement* anchorableToImport) -> bool
            {
                if (checkPreviousContainer)
                {
                    auto previousContainer = anchorableToImport->GetPreviousContainer();
                    if (previousContainer == targetModel && (anchorableToImport->GetPreviousContainerIndex() != -1))
                    {
                        i = anchorableToImport->GetPreviousContainerIndex();
                    }
                    checkPreviousContainer = false;
                }

                paneModel->InsertChildAt(i, anchorableToImport);
                i++;
                anchorableToActivate = anchorableToImport->Static_As<LayoutAnchorable>();
                return false;
            });

            anchorableToActivate->IsActive = true;
        }
        break;
    }

    DropTarget<LayoutDocumentPaneControl>::Drop(floatingWindow);
}

std::shared_ptr<Core::Media::Geometry> DocumentPaneDropTarget::GetPreviewPath(OverlayWindow *overlayWindow, LayoutFloatingWindow *floatingWindow)
{
    auto targetScreenRect = TargetElement->TransformedBounds;

    switch (DropType)
    {
    case DropTargetType::DocumentPaneDockInside:
    {
        targetScreenRect.Offset(-overlayWindow->TransformedBounds.Left, -overlayWindow->TransformedBounds.Top);

        if (_tabIndex == -1)
        {
            return std::make_shared<Core::Media::Geometries::RectangleGeometry>(targetScreenRect);
        }
        else
        {
            auto translatedDetectionRect = DetectionRects[0]; // Core::Media::Rect(DetectionRects[0].TopLeft, DetectionRects[0].BottomRight);
            translatedDetectionRect.Offset(-overlayWindow->TransformedBounds.Left, -overlayWindow->TransformedBounds.Top);

            std::shared_ptr<XUI::Core::Media::Geometry> geometry = std::make_shared<XUI::Core::Media::Geometries::PathGeometry>();
            auto pathFigure = geometry->GetStreamGeometryContext()->Open();

            pathFigure->BeginFigure(targetScreenRect.BottomRight, true);
            pathFigure->LineTo(Core::Media::Point(targetScreenRect.Right, translatedDetectionRect.Bottom));
            pathFigure->LineTo(translatedDetectionRect.BottomRight);
            pathFigure->LineTo(translatedDetectionRect.TopRight);
            pathFigure->LineTo(translatedDetectionRect.TopLeft);
            pathFigure->LineTo(translatedDetectionRect.BottomLeft);
            pathFigure->LineTo(Core::Media::Point(targetScreenRect.Left, translatedDetectionRect.Bottom));
            pathFigure->LineTo(targetScreenRect.BottomLeft);
            pathFigure->EndFigure(true);

            return geometry;
        }
    }
    case DropTargetType::DocumentPaneDockBottom:
    {
        targetScreenRect.Offset(-overlayWindow->TransformedBounds.Left, -overlayWindow->TransformedBounds.Top);
        targetScreenRect.Offset(0.0, targetScreenRect.Height / 2.0);
        targetScreenRect.Height /= 2.0;
        return std::make_shared<Core::Media::Geometries::RectangleGeometry>(targetScreenRect);
    }
    case DropTargetType::DocumentPaneDockTop:
    {
        targetScreenRect.Offset(-overlayWindow->TransformedBounds.Left, -overlayWindow->TransformedBounds.Top);
        targetScreenRect.Height /= 2.0;
        return std::make_shared<Core::Media::Geometries::RectangleGeometry>(targetScreenRect);
    }
    case DropTargetType::DocumentPaneDockLeft:
    {
        targetScreenRect.Offset(-overlayWindow->TransformedBounds.Left, -overlayWindow->TransformedBounds.Top);
        targetScreenRect.Width /= 2.0;
        return std::make_shared<Core::Media::Geometries::RectangleGeometry>(targetScreenRect);
    }
    case DropTargetType::DocumentPaneDockRight:
    {
        targetScreenRect.Offset(-overlayWindow->TransformedBounds.Left, -overlayWindow->TransformedBounds.Top);
        targetScreenRect.Offset(targetScreenRect.Width / 2.0, 0.0);
        targetScreenRect.Width /= 2.0;
        return std::make_shared<Core::Media::Geometries::RectangleGeometry>(targetScreenRect);
    }
    }

    return nullptr;
}
