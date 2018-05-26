#include "StdAfx.h"
#include "AnchorablePaneDropTarget.h"
#include "LayoutFloatingWindow.h"
#include "OverlayWindow.h"
#include "Core/Media/Geometries/RectangleGeometry.h"

using namespace XUI;
using namespace XUI::UI;

XUI::UI::AnchorablePaneDropTarget::AnchorablePaneDropTarget(LayoutAnchorablePaneControl * paneControl, Core::Media::Rect const & detectionRect, DropTargetType type)
    : DropTarget<LayoutAnchorablePaneControl>(paneControl->shared_from_base_static<LayoutAnchorablePaneControl>(), detectionRect, type)
    , _targetPane(paneControl->shared_from_base_static<LayoutAnchorablePaneControl>())
{
}

XUI::UI::AnchorablePaneDropTarget::AnchorablePaneDropTarget(LayoutAnchorablePaneControl * paneControl, Core::Media::Rect const & detectionRect, DropTargetType type, int tabIndex)
    : DropTarget<LayoutAnchorablePaneControl>(paneControl->shared_from_base_static<LayoutAnchorablePaneControl>(), detectionRect, type)
    , _targetPane(paneControl->shared_from_base_static<LayoutAnchorablePaneControl>())
    , _tabIndex(tabIndex)
{
}

std::shared_ptr<Core::Media::Geometry> XUI::UI::AnchorablePaneDropTarget::GetPreviewPath(OverlayWindow * overlayWindow, LayoutFloatingWindow * floatingWindow)
{
    auto anchorableFloatingWindowModel = floatingWindow->Static_As<LayoutAnchorableFloatingWindow>();
    auto layoutAnchorablePane = anchorableFloatingWindowModel->RootPanel->Static_As<LayoutGroupBase>();
    auto layoutAnchorablePaneWithActualSize = anchorableFloatingWindowModel->RootPanel;

    switch (DropType)
    {
        case DropTargetType::AnchorablePaneDockBottom:
        {
            auto targetScreenRect = TargetElement->TransformedBounds;
            targetScreenRect.Offset(-overlayWindow->TransformedBounds.Left, -overlayWindow->TransformedBounds.Top);

            targetScreenRect.Offset(0.0, targetScreenRect.Height / 2.0);
            targetScreenRect.Height /= 2.0;

            return std::make_shared<Core::Media::Geometries::RectangleGeometry>(targetScreenRect);
        }
        case DropTargetType::AnchorablePaneDockTop:
        {
            auto targetScreenRect = TargetElement->TransformedBounds;
            targetScreenRect.Offset(-overlayWindow->TransformedBounds.Left, -overlayWindow->TransformedBounds.Top);

            targetScreenRect.Height /= 2.0;

            return std::make_shared<Core::Media::Geometries::RectangleGeometry>(targetScreenRect);
        }
        case DropTargetType::AnchorablePaneDockLeft:
        {
            auto targetScreenRect = TargetElement->TransformedBounds;
            targetScreenRect.Offset(-overlayWindow->TransformedBounds.Left, -overlayWindow->TransformedBounds.Top);

            targetScreenRect.Width /= 2.0;

            return std::make_shared<Core::Media::Geometries::RectangleGeometry>(targetScreenRect);
        }
        case DropTargetType::AnchorablePaneDockRight:
        {
            auto targetScreenRect = TargetElement->TransformedBounds;
            targetScreenRect.Offset(-overlayWindow->TransformedBounds.Left, -overlayWindow->TransformedBounds.Top);

            targetScreenRect.Offset(targetScreenRect.Width / 2.0, 0.0);
            targetScreenRect.Width /= 2.0;

            return std::make_shared<Core::Media::Geometries::RectangleGeometry>(targetScreenRect);
        }
        case DropTargetType::AnchorablePaneDockInside:
        {
            auto targetScreenRect = TargetElement->TransformedBounds;
            targetScreenRect.Offset(-overlayWindow->TransformedBounds.Left, -overlayWindow->TransformedBounds.Top);

            if (_tabIndex == -1)
            {
                return std::make_shared<Core::Media::Geometries::RectangleGeometry>(targetScreenRect);
            }
            else
            {
                auto translatedDetectionRect = Core::Media::Rect(DetectionRects[0].TopLeft, DetectionRects[0].BottomRight);
                translatedDetectionRect.Offset(-overlayWindow->TransformedBounds.Left, -overlayWindow->TransformedBounds.Top);

                std::shared_ptr<XUI::Core::Media::Geometry> geometry = std::make_shared<XUI::Core::Media::Geometries::PathGeometry>();
                auto pathFigure = geometry->GetStreamGeometryContext()->Open();

                pathFigure->BeginFigure(targetScreenRect.TopLeft, true);
                pathFigure->LineTo(Core::Media::Point(targetScreenRect.x0(), translatedDetectionRect.y0()));
                pathFigure->LineTo(translatedDetectionRect.TopLeft);
                pathFigure->LineTo(translatedDetectionRect.BottomLeft);
                pathFigure->LineTo(translatedDetectionRect.BottomRight);
                pathFigure->LineTo(translatedDetectionRect.TopRight);
                pathFigure->LineTo(Core::Media::Point(targetScreenRect.Right, translatedDetectionRect.y0()));
                pathFigure->LineTo(targetScreenRect.TopRight);
                pathFigure->EndFigure(true);

                return geometry;
            }
        }
    }

    return nullptr;
}

void XUI::UI::AnchorablePaneDropTarget::Drop(LayoutAnchorableFloatingWindow * floatingWindow)
{
    auto targetModel = _targetPane->Model.get();
    LayoutAnchorable *anchorableActive = nullptr;
    LayoutContainer::ContainsChild(floatingWindow, [&anchorableActive](LayoutElement* x) -> bool
    {
        return anchorableActive = x->Static_As<LayoutAnchorable>();
    });

    System::ThrowIfFailed<System::InvalidOperationException>(anchorableActive);

    switch (DropType)
    {
        case DropTargetType::AnchorablePaneDockBottom:
        {
            auto parentModel = targetModel->Parent->Static_As<LayoutGroupBase>();
            auto parentModelOrientable = parentModel;
            int insertToIndex = parentModel->IndexOfChild(targetModel);

            if (parentModelOrientable->GetOrientation() != UI::Orientation::Vertical && parentModel->Children.size() == 1)
            {
                parentModelOrientable->SetOrientation(UI::Orientation::Vertical);
            }

            if (parentModelOrientable->GetOrientation() == UI::Orientation::Vertical)
            {
                auto layoutAnchorablePaneGroup = floatingWindow->RootPanel->Static_As<LayoutAnchorablePaneGroup>();
                if (layoutAnchorablePaneGroup != nullptr && (layoutAnchorablePaneGroup->Children.size() == 1 || layoutAnchorablePaneGroup->GetOrientation() == UI::Orientation::Vertical))
                {
                    for (int i = 0; i < layoutAnchorablePaneGroup->Children.size(); i++)
                    {
                        parentModel->InsertChildAt(insertToIndex + 1 + i, layoutAnchorablePaneGroup->GetChild(i));
                    }
                }
                else
                {
                    parentModel->InsertChildAt(insertToIndex + 1, floatingWindow->RootPanel.get());
                }
            }
            else
            {
                auto targetModelAsPositionableElement = targetModel->Static_As<LayoutGroupBase>();
                auto newOrientedPanel = std::make_shared<LayoutAnchorablePaneGroup>();
                newOrientedPanel->Orientation = UI::Orientation::Vertical;
                newOrientedPanel->DockWidth = targetModelAsPositionableElement->GetDockWidth();
                newOrientedPanel->DockHeight = targetModelAsPositionableElement->GetDockHeight();

                parentModel->InsertChildAt(insertToIndex, newOrientedPanel.get());
                newOrientedPanel->AddChild(targetModel);
                newOrientedPanel->AddChild(floatingWindow->RootPanel.get());

            }
        }
        break;
        case DropTargetType::AnchorablePaneDockTop:
        {
            auto parentModel = targetModel->Parent->Static_As<LayoutGroupBase>();
            auto parentModelOrientable = parentModel;
            int insertToIndex = parentModel->IndexOfChild(targetModel);

            if (parentModelOrientable->GetOrientation() != UI::Orientation::Vertical && parentModel->Children.size() == 1)
            {
                parentModelOrientable->SetOrientation(UI::Orientation::Vertical);
            }

            if (parentModelOrientable->GetOrientation() == UI::Orientation::Vertical)
            {
                auto layoutAnchorablePaneGroup = floatingWindow->RootPanel->Static_As<LayoutAnchorablePaneGroup>();
                if (layoutAnchorablePaneGroup != nullptr && (layoutAnchorablePaneGroup->Children.size() == 1 || layoutAnchorablePaneGroup->GetOrientation() == UI::Orientation::Vertical))
                {
                    for (int i = 0; i < layoutAnchorablePaneGroup->Children.size(); i++)
                    {
                        parentModel->InsertChildAt(insertToIndex + i, layoutAnchorablePaneGroup->GetChild(i));
                    }
                }
                else
                {
                    parentModel->InsertChildAt(insertToIndex, floatingWindow->RootPanel.get());
                }
            }
            else
            {
                auto targetModelAsPositionableElement = targetModel->Static_As<LayoutGroupBase>();
                auto newOrientedPanel = std::make_shared<LayoutAnchorablePaneGroup>();
                newOrientedPanel->Orientation = UI::Orientation::Vertical;
                newOrientedPanel->DockWidth = targetModelAsPositionableElement->GetDockWidth();
                newOrientedPanel->DockHeight = targetModelAsPositionableElement->GetDockHeight();

                parentModel->InsertChildAt(insertToIndex, newOrientedPanel.get());
                //the floating window must be added after the target modal as it could be raise a CollectGarbage call
                newOrientedPanel->AddChild(targetModel);
                newOrientedPanel->InsertChildAt(0, floatingWindow->RootPanel.get());

            }
        }
        break;
        case DropTargetType::AnchorablePaneDockLeft:
        {
            auto parentModel = targetModel->Parent->Static_As<LayoutGroupBase>();
            auto parentModelOrientable = parentModel;
            int insertToIndex = parentModel->IndexOfChild(targetModel);

            if (parentModelOrientable->GetOrientation() != UI::Orientation::Horizontal && parentModel->Children.size() == 1)
            {
                parentModelOrientable->SetOrientation(UI::Orientation::Horizontal);
            }

            if (parentModelOrientable->GetOrientation() == UI::Orientation::Horizontal)
            {
                auto layoutAnchorablePaneGroup = floatingWindow->RootPanel->Static_As<LayoutAnchorablePaneGroup>();
                if (layoutAnchorablePaneGroup != nullptr && (layoutAnchorablePaneGroup->Children.size() == 1 || layoutAnchorablePaneGroup->GetOrientation() == UI::Orientation::Horizontal))
                {
                    for (int i = 0; i < layoutAnchorablePaneGroup->Children.size(); i++)
                    {
                        parentModel->InsertChildAt(insertToIndex + i, layoutAnchorablePaneGroup->GetChild(i));
                    }
                }
                else
                {
                    parentModel->InsertChildAt(insertToIndex, floatingWindow->RootPanel.get());
                }
            }
            else
            {
                auto targetModelAsPositionableElement = targetModel->Static_As<LayoutGroupBase>();
                auto newOrientedPanel = std::make_shared<LayoutAnchorablePaneGroup>();
                newOrientedPanel->Orientation = UI::Orientation::Horizontal;
                newOrientedPanel->DockWidth = targetModelAsPositionableElement->GetDockWidth();
                newOrientedPanel->DockHeight = targetModelAsPositionableElement->GetDockHeight();

                parentModel->InsertChildAt(insertToIndex, newOrientedPanel.get());
                //the floating window must be added after the target modal as it could be raise a CollectGarbage call
                newOrientedPanel->AddChild(targetModel);
                newOrientedPanel->InsertChildAt(0, floatingWindow->RootPanel.get());

            }
        }
        break;
        case DropTargetType::AnchorablePaneDockRight:
        {
            auto parentModel = targetModel->Parent->Static_As<LayoutGroupBase>();
            auto parentModelOrientable = parentModel;
            int insertToIndex = parentModel->IndexOfChild(targetModel);

            if (parentModelOrientable->GetOrientation() != UI::Orientation::Horizontal && parentModel->Children.size() == 1)
            {
                parentModelOrientable->SetOrientation(UI::Orientation::Horizontal);
            }

            if (parentModelOrientable->GetOrientation() == UI::Orientation::Horizontal)
            {
                auto layoutAnchorablePaneGroup = floatingWindow->RootPanel->Static_As<LayoutAnchorablePaneGroup>();
                if (layoutAnchorablePaneGroup != nullptr && (layoutAnchorablePaneGroup->Children.size() == 1 || layoutAnchorablePaneGroup->GetOrientation() == UI::Orientation::Horizontal))
                {
                    for (int i = 0; i < layoutAnchorablePaneGroup->Children.size(); i++)
                    {
                        parentModel->InsertChildAt(insertToIndex + 1 + i, layoutAnchorablePaneGroup->GetChild(i));
                    }
                }
                else
                {
                    parentModel->InsertChildAt(insertToIndex + 1, floatingWindow->RootPanel.get());
                }
            }
            else
            {
                auto targetModelAsPositionableElement = targetModel->Static_As<LayoutGroupBase>();
                auto newOrientedPanel = std::make_shared<LayoutAnchorablePaneGroup>();
                newOrientedPanel->Orientation = UI::Orientation::Horizontal;
                newOrientedPanel->DockWidth = targetModelAsPositionableElement->GetDockWidth();
                newOrientedPanel->DockHeight = targetModelAsPositionableElement->GetDockHeight();

                parentModel->InsertChildAt(insertToIndex, newOrientedPanel.get());
                newOrientedPanel->AddChild(targetModel);
                newOrientedPanel->AddChild(floatingWindow->RootPanel.get());

            }
        }
        break;
        case DropTargetType::AnchorablePaneDockInside:
        {
            auto paneModel = targetModel->Static_As<LayoutAnchorablePane>();
            auto layoutAnchorablePaneGroup = floatingWindow->RootPanel->Static_As<LayoutAnchorablePaneGroup>();

            int i = _tabIndex == -1 ? 0 : _tabIndex;
            LayoutContainer::ContainsChild(floatingWindow, [&](LayoutElement* anchorableToImport) -> bool
            {
                paneModel->InsertChildAt(i, anchorableToImport);
                i++;
                return false;
            });
        }
        break;
    }

    anchorableActive->IsActive = true;

    DropTarget<LayoutAnchorablePaneControl>::Drop(floatingWindow);
}
