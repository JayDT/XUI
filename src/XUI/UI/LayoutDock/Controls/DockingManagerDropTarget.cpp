#include "DockingManagerDropTarget.h"
#include "DockingManager.h"
#include "LayoutRoot.h"
#include "LayoutPanel.h"
#include "LayoutAnchorablePaneGroup.h"
#include "LayoutAnchorableFloatingWindow.h"
#include "OverlayWindow.h"
#include "Core/Media/Geometries/RectangleGeometry.h"

using namespace XUI;
using namespace XUI::UI;

DockingManagerDropTarget::DockingManagerDropTarget(std::shared_ptr<DockingManager> manager, Core::Media::Rect const& detectionRect, DropTargetType type)
    : DropTarget<DockingManager>(manager, detectionRect, type)
{
    _manager = manager;
}

void DockingManagerDropTarget::Drop(LayoutAnchorableFloatingWindow *floatingWindow)
{
    switch (DropType)
    {
    case DropTargetType::DockingManagerDockLeft:
    {
        if (_manager->Layout->RootPanel->Orientation != UI::Orientation::Horizontal && _manager->Layout->RootPanel->Children.size() == 1)
        {
            _manager->Layout->RootPanel->Orientation = UI::Orientation::Horizontal;
        }

        if (_manager->Layout->RootPanel->Orientation == UI::Orientation::Horizontal)
        {
            auto layoutAnchorablePaneGroup = floatingWindow->RootPanel.get();
            if (layoutAnchorablePaneGroup != nullptr && layoutAnchorablePaneGroup->Orientation == UI::Orientation::Horizontal)
            {
                for (int i = 0; i < layoutAnchorablePaneGroup->Children.size(); i++)
                {
                    _manager->Layout->RootPanel->InsertChildAt(i, layoutAnchorablePaneGroup->GetChild(i));
                }
            }
            else
            {
                _manager->Layout->RootPanel->InsertChildAt(0, floatingWindow->RootPanel.get());
            }
        }
        else
        {
            auto newOrientedPanel = std::make_shared<LayoutPanel>();
            newOrientedPanel->Orientation = UI::Orientation::Horizontal;

            newOrientedPanel->AddChild(floatingWindow->RootPanel.get());
            newOrientedPanel->AddChild(_manager->Layout->RootPanel.get());

            _manager->Layout->RootPanel = newOrientedPanel;
        }
    }
    break;
    case DropTargetType::DockingManagerDockRight:
    {
        if (_manager->Layout->RootPanel->Orientation != UI::Orientation::Horizontal && _manager->Layout->RootPanel->Children.size() == 1)
        {
            _manager->Layout->RootPanel->Orientation = UI::Orientation::Horizontal;
        }

        if (_manager->Layout->RootPanel->Orientation == UI::Orientation::Horizontal)
        {
            auto layoutAnchorablePaneGroup = floatingWindow->RootPanel.get();
            if (layoutAnchorablePaneGroup != nullptr && layoutAnchorablePaneGroup->Orientation == UI::Orientation::Horizontal)
            {
                for (int i = 0; i < layoutAnchorablePaneGroup->Children.size(); i++)
                {
                    _manager->Layout->RootPanel->AddChild(layoutAnchorablePaneGroup->GetChild(i));
                }
            }
            else
            {
                _manager->Layout->RootPanel->AddChild(floatingWindow->RootPanel.get());
            }
        }
        else
        {
            auto newOrientedPanel = std::make_shared<LayoutPanel>();
            newOrientedPanel->Orientation = UI::Orientation::Horizontal;

            newOrientedPanel->AddChild(_manager->Layout->RootPanel.get());
            newOrientedPanel->AddChild(floatingWindow->RootPanel.get());

            _manager->Layout->RootPanel = newOrientedPanel;
        }
    }
    break;
    case DropTargetType::DockingManagerDockTop:
    {
        if (_manager->Layout->RootPanel->Orientation != UI::Orientation::Vertical && _manager->Layout->RootPanel->Children.size() == 1)
        {
            _manager->Layout->RootPanel->Orientation = UI::Orientation::Vertical;
        }

        if (_manager->Layout->RootPanel->Orientation == UI::Orientation::Vertical)
        {
            auto layoutAnchorablePaneGroup = floatingWindow->RootPanel.get();
            if (layoutAnchorablePaneGroup != nullptr && layoutAnchorablePaneGroup->Orientation == UI::Orientation::Vertical)
            {
                for (int i = 0; i < layoutAnchorablePaneGroup->Children.size(); i++)
                {
                    _manager->Layout->RootPanel->InsertChildAt(i, layoutAnchorablePaneGroup->GetChild(i));
                }
            }
            else
            {
                _manager->Layout->RootPanel->InsertChildAt(0, floatingWindow->RootPanel.get());
            }
        }
        else
        {
            auto newOrientedPanel = std::make_shared<LayoutPanel>();
            newOrientedPanel->Orientation = UI::Orientation::Vertical;

            newOrientedPanel->AddChild(floatingWindow->RootPanel.get());
            newOrientedPanel->AddChild(_manager->Layout->RootPanel.get());

            _manager->Layout->RootPanel = newOrientedPanel;
        }
    }
    break;
    case DropTargetType::DockingManagerDockBottom:
    {
        if (_manager->Layout->RootPanel->Orientation != UI::Orientation::Vertical && _manager->Layout->RootPanel->Children.size() == 1)
        {
            _manager->Layout->RootPanel->Orientation = UI::Orientation::Vertical;
        }

        if (_manager->Layout->RootPanel->Orientation == UI::Orientation::Vertical)
        {
            auto layoutAnchorablePaneGroup = floatingWindow->RootPanel.get();
            if (layoutAnchorablePaneGroup != nullptr && layoutAnchorablePaneGroup->Orientation == UI::Orientation::Vertical)
            {
                for (int i = 0; i < layoutAnchorablePaneGroup->Children.size(); i++)
                {
                    _manager->Layout->RootPanel->AddChild(layoutAnchorablePaneGroup->GetChild(i));
                }
            }
            else
            {
                _manager->Layout->RootPanel->AddChild(floatingWindow->RootPanel.get());
            }
        }
        else
        {
            auto newOrientedPanel = std::make_shared<LayoutPanel>();
            newOrientedPanel->Orientation = UI::Orientation::Vertical;

            newOrientedPanel->AddChild(_manager->Layout->RootPanel.get());
            newOrientedPanel->AddChild(floatingWindow->RootPanel.get());

            _manager->Layout->RootPanel = newOrientedPanel;
        }
    }
    break;
    }

    DropTarget<DockingManager>::Drop(floatingWindow);
}

std::shared_ptr<Core::Media::Geometry> DockingManagerDropTarget::GetPreviewPath(OverlayWindow *overlayWindow, LayoutFloatingWindow *floatingWindow)
{
    auto anchorableFloatingWindowModel = floatingWindow->Static_As<LayoutAnchorableFloatingWindow>();
    auto layoutAnchorablePane = anchorableFloatingWindowModel->RootPanel->Static_As<LayoutGroupBase>();
    auto layoutAnchorablePaneWithActualSize = anchorableFloatingWindowModel->RootPanel;

    auto targetScreenRect = TargetElement->TransformedBounds;

    switch (DropType)
    {
    case DropTargetType::DockingManagerDockLeft:
    {
        auto desideredWidth = layoutAnchorablePane->GetDockWidth().IsAbsolute ? layoutAnchorablePane->GetDockWidth().Value : layoutAnchorablePaneWithActualSize->ActualWidth;
        auto previewBoxRect = Core::Media::Rect(targetScreenRect.x0() - overlayWindow->TransformedBounds.Left, targetScreenRect.y0() - overlayWindow->TransformedBounds.Top, std::min(desideredWidth, targetScreenRect.Width / 2.0), targetScreenRect.Height);

        return std::make_shared<Core::Media::Geometries::RectangleGeometry>(previewBoxRect);
    }
    case DropTargetType::DockingManagerDockTop:
    {
        auto desideredHeight = layoutAnchorablePane->GetDockHeight().IsAbsolute ? layoutAnchorablePane->GetDockHeight().Value : layoutAnchorablePaneWithActualSize->ActualHeight;
        auto previewBoxRect = Core::Media::Rect(targetScreenRect.x0() - overlayWindow->TransformedBounds.Left, targetScreenRect.y0() - overlayWindow->TransformedBounds.Top, targetScreenRect.Width, std::min(desideredHeight, targetScreenRect.Height / 2.0));

        return std::make_shared<Core::Media::Geometries::RectangleGeometry>(previewBoxRect);
    }
    case DropTargetType::DockingManagerDockRight:
    {
        auto desideredWidth = layoutAnchorablePane->GetDockWidth().IsAbsolute ? layoutAnchorablePane->GetDockWidth().Value : layoutAnchorablePaneWithActualSize->ActualWidth;
        auto previewBoxRect = Core::Media::Rect(targetScreenRect.x1() - overlayWindow->TransformedBounds.Left - std::min(desideredWidth, targetScreenRect.Width / 2.0), targetScreenRect.y0() - overlayWindow->TransformedBounds.Top, std::min(desideredWidth, targetScreenRect.Width / 2.0), targetScreenRect.Height);

        return std::make_shared<Core::Media::Geometries::RectangleGeometry>(previewBoxRect);
    }
    case DropTargetType::DockingManagerDockBottom:
    {
        auto desideredHeight = layoutAnchorablePane->GetDockHeight().IsAbsolute ? layoutAnchorablePane->GetDockHeight().Value : layoutAnchorablePaneWithActualSize->ActualHeight;
        auto previewBoxRect = Core::Media::Rect(targetScreenRect.x0() - overlayWindow->TransformedBounds.Left, targetScreenRect.Bottom - overlayWindow->TransformedBounds.Top - std::min(desideredHeight, targetScreenRect.Height / 2.0), targetScreenRect.Width, std::min(desideredHeight, targetScreenRect.Height / 2.0));

        return std::make_shared<Core::Media::Geometries::RectangleGeometry>(previewBoxRect);
    }
    }


    throw System::InvalidOperationException("");
}