#include "DocumentPaneDropAsAnchorableTarget.h"
#include "LayoutPanel.h"
#include "LayoutDocumentPaneGroup.h"
#include "OverlayWindow.h"
#include "DockingManager.h"
#include "Core/Media/Geometries/RectangleGeometry.h"

using namespace XUI;
using namespace XUI::UI;

DocumentPaneDropAsAnchorableTarget::DocumentPaneDropAsAnchorableTarget(LayoutDocumentPaneControl *paneControl, Core::Media::Rect const& detectionRect, DropTargetType type)
    : DropTarget<LayoutDocumentPaneControl>(paneControl->shared_from_base_static<LayoutDocumentPaneControl>(), detectionRect, type)
{
    _targetPane = paneControl;
}

DocumentPaneDropAsAnchorableTarget::DocumentPaneDropAsAnchorableTarget(LayoutDocumentPaneControl *paneControl, Core::Media::Rect const& detectionRect, DropTargetType type, int tabIndex)
    : DropTarget<LayoutDocumentPaneControl>(paneControl->shared_from_base_static<LayoutDocumentPaneControl>(), detectionRect, type)
{
    _targetPane = paneControl;
    _tabIndex = tabIndex;
}

void DocumentPaneDropAsAnchorableTarget::Drop(LayoutAnchorableFloatingWindow *floatingWindow)
{
    LayoutDocumentPane *targetModel = _targetPane->Model.get();
    LayoutDocumentPaneGroup *parentGroup;
    LayoutPanel *parentGroupPanel;
    FindParentLayoutDocumentPane(targetModel, parentGroup, parentGroupPanel);

    switch (DropType)
    {
        case DropTargetType::DocumentPaneDockAsAnchorableBottom:

        {
            if (parentGroupPanel != nullptr && parentGroupPanel->Children.size() == 1)
            {
                parentGroupPanel->Orientation = UI::Orientation::Vertical;
            }

            if (parentGroupPanel != nullptr && parentGroupPanel->Orientation == UI::Orientation::Vertical)
            {
                parentGroupPanel->InsertChildAt(parentGroupPanel->IndexOfChild(parentGroup != nullptr ? static_cast<LayoutElement*>(parentGroup) : targetModel) + 1, floatingWindow->RootPanel.get());
            }
            else if (parentGroupPanel != nullptr)
            {
                auto newParentPanel = new LayoutPanel();
                newParentPanel->Orientation = UI::Orientation::Vertical;
                parentGroupPanel->ReplaceChild(parentGroup != nullptr ? static_cast<LayoutElement*>(parentGroup) : targetModel, newParentPanel);
                newParentPanel->AddChild(parentGroup != nullptr ? static_cast<LayoutElement*>(parentGroup) : targetModel);
                newParentPanel->AddChild(floatingWindow->RootPanel.get());
            }
            else
            {
                throw System::NotImplementedException();
            }


        }
        break;
        case DropTargetType::DocumentPaneDockAsAnchorableTop:
        {
            if (parentGroupPanel != nullptr && parentGroupPanel->Children.size() == 1)
            {
                parentGroupPanel->Orientation = UI::Orientation::Vertical;
            }

            if (parentGroupPanel != nullptr && parentGroupPanel->Orientation == UI::Orientation::Vertical)
            {
                parentGroupPanel->InsertChildAt(parentGroupPanel->IndexOfChild(parentGroup != nullptr ? static_cast<LayoutElement*>(parentGroup) : targetModel), floatingWindow->RootPanel.get());
            }
            else if (parentGroupPanel != nullptr)
            {
                auto newParentPanel = new LayoutPanel();
                newParentPanel->Orientation = UI::Orientation::Vertical;
                parentGroupPanel->ReplaceChild(parentGroup != nullptr ? static_cast<LayoutElement*>(parentGroup) : targetModel, newParentPanel);
                newParentPanel->AddChild(parentGroup != nullptr ? static_cast<LayoutElement*>(parentGroup) : targetModel);
                newParentPanel->InsertChildAt(0, floatingWindow->RootPanel.get());
            }
            else
            {
                throw System::NotImplementedException();
            }

        }
        break;
        case DropTargetType::DocumentPaneDockAsAnchorableLeft:
        {
            if (parentGroupPanel != nullptr && parentGroupPanel->Children.size() == 1)
            {
                parentGroupPanel->Orientation = UI::Orientation::Horizontal;
            }

            if (parentGroupPanel != nullptr && parentGroupPanel->Orientation == UI::Orientation::Horizontal)
            {
                parentGroupPanel->InsertChildAt(parentGroupPanel->IndexOfChild(parentGroup != nullptr ? static_cast<LayoutElement*>(parentGroup) : targetModel), floatingWindow->RootPanel.get());
            }
            else if (parentGroupPanel != nullptr)
            {
                auto newParentPanel = new LayoutPanel();
                newParentPanel->Orientation = UI::Orientation::Horizontal;
                parentGroupPanel->ReplaceChild(parentGroup != nullptr ? static_cast<LayoutElement*>(parentGroup) : targetModel, newParentPanel);
                newParentPanel->AddChild(parentGroup != nullptr ? static_cast<LayoutElement*>(parentGroup) : targetModel);
                newParentPanel->InsertChildAt(0, floatingWindow->RootPanel.get());
            }
            else
            {
                throw System::NotImplementedException();
            }
        }
        break;
        case DropTargetType::DocumentPaneDockAsAnchorableRight:
        {
            if (parentGroupPanel != nullptr && parentGroupPanel->Children.size() == 1)
            {
                parentGroupPanel->Orientation = UI::Orientation::Horizontal;
            }

            if (parentGroupPanel != nullptr && parentGroupPanel->Orientation == UI::Orientation::Horizontal)
            {
                parentGroupPanel->InsertChildAt(parentGroupPanel->IndexOfChild(parentGroup != nullptr ? static_cast<LayoutElement*>(parentGroup) : targetModel) + 1, floatingWindow->RootPanel.get());
            }
            else if (parentGroupPanel != nullptr)
            {
                auto newParentPanel = new LayoutPanel();
                newParentPanel->Orientation = UI::Orientation::Horizontal;
                parentGroupPanel->ReplaceChild(parentGroup != nullptr ? static_cast<LayoutElement*>(parentGroup) : targetModel, newParentPanel);
                newParentPanel->AddChild(parentGroup != nullptr ? static_cast<LayoutElement*>(parentGroup) : targetModel);
                newParentPanel->AddChild(floatingWindow->RootPanel.get());
            }
            else
            {
                throw System::NotImplementedException();
            }
        }
        break;
    }

    DropTarget<LayoutDocumentPaneControl>::Drop(floatingWindow);
}

std::shared_ptr<Core::Media::Geometry> DocumentPaneDropAsAnchorableTarget::GetPreviewPath(OverlayWindow *overlayWindow, LayoutFloatingWindow *floatingWindow)
{
    Core::Media::Rect targetScreenRect;
    LayoutDocumentPane *targetModel = _targetPane->Model.get();
    auto manager = targetModel->Root->GetManager();

    //ILayoutDocumentPane targetModel = _targetPane.Model as ILayoutDocumentPane;
    LayoutDocumentPaneGroup *parentGroup = nullptr;
    LayoutPanel *parentGroupPanel;
    if (!FindParentLayoutDocumentPane(targetModel, parentGroup, parentGroupPanel))
        return nullptr;

    //if (targetModel.Parent is LayoutDocumentPaneGroup)
    //{
    //    var parentGroup = targetModel.Parent as LayoutDocumentPaneGroup;
    //    var documentPaneGroupControl = manager.FindLogicalChildren<LayoutDocumentPaneGroupControl>().First(d => d.Model == parentGroup);
    //    targetScreenRect = documentPaneGroupControl.GetScreenArea();
    //}
    //else
    //{
    //    var documentPaneControl = manager.FindLogicalChildren<LayoutDocumentPaneControl>().First(d => d.Model == targetModel);
    //    targetScreenRect = documentPaneControl.GetScreenArea();
    //}

    //var parentPanel = targetModel.FindParent<LayoutPanel>();
    Controls::Control* documentPaneControl = nullptr;

    manager->VisitLogicalAncestors([parentGroup, parentGroupPanel](Controls::Control* x) -> bool
    {
        auto layoutControl = x->Dynamic_As<ILayoutControl>();
        if (layoutControl)
            return parentGroup != nullptr ? layoutControl->GetLayoutModel() == parentGroup : layoutControl->GetLayoutModel() == parentGroupPanel;
        return false;
    });

    targetScreenRect = documentPaneControl->TransformedBounds;

    switch (DropType)
    {
    case DropTargetType::DocumentPaneDockAsAnchorableBottom:
    {
        targetScreenRect.Offset(-overlayWindow->TransformedBounds.Left, -overlayWindow->TransformedBounds.Top);
        targetScreenRect.Offset(0.0, targetScreenRect.Height - targetScreenRect.Height / 3.0);
        targetScreenRect.Height /= 3.0;
        return std::make_shared<Core::Media::Geometries::RectangleGeometry>(targetScreenRect);
    }
    case DropTargetType::DocumentPaneDockAsAnchorableTop:
    {
        targetScreenRect.Offset(-overlayWindow->TransformedBounds.Left, -overlayWindow->TransformedBounds.Top);
        targetScreenRect.Height /= 3.0;
        return std::make_shared<Core::Media::Geometries::RectangleGeometry>(targetScreenRect);
    }
    case DropTargetType::DocumentPaneDockAsAnchorableRight:
    {
        targetScreenRect.Offset(-overlayWindow->TransformedBounds.Left, -overlayWindow->TransformedBounds.Top);
        targetScreenRect.Offset(targetScreenRect.Width - targetScreenRect.Width / 3.0, 0.0);
        targetScreenRect.Width /= 3.0;
        return std::make_shared<Core::Media::Geometries::RectangleGeometry>(targetScreenRect);
    }
    case DropTargetType::DocumentPaneDockAsAnchorableLeft:
    {
        targetScreenRect.Offset(-overlayWindow->TransformedBounds.Left, -overlayWindow->TransformedBounds.Top);
        targetScreenRect.Width /= 3.0;
        return std::make_shared<Core::Media::Geometries::RectangleGeometry>(targetScreenRect);
    }
    }

    return nullptr;
}

bool DocumentPaneDropAsAnchorableTarget::FindParentLayoutDocumentPane(LayoutDocumentPane *documentPane, LayoutDocumentPaneGroup *&containerPaneGroup, LayoutPanel *&containerPanel)
{
    containerPaneGroup = nullptr;
    containerPanel = nullptr;

    if (documentPane->Parent->Is<LayoutPanel>())
    {
        containerPaneGroup = nullptr;
        containerPanel = documentPane->Parent->Static_As<LayoutPanel>();
        return true;
    }
    else if (documentPane->Parent->Is<LayoutDocumentPaneGroup>())
    {
        auto currentDocumentPaneGroup = static_cast<LayoutDocumentPaneGroup*>(documentPane->Parent);
        while (!currentDocumentPaneGroup->Parent->Is<LayoutPanel>())
        {
            currentDocumentPaneGroup = currentDocumentPaneGroup->Parent->Static_As<LayoutDocumentPaneGroup>();

            if (currentDocumentPaneGroup == nullptr)
                break;
        }

        if (currentDocumentPaneGroup == nullptr)
            return false;

        containerPaneGroup = currentDocumentPaneGroup;
        containerPanel = currentDocumentPaneGroup->Parent->Static_As<LayoutPanel>();
        return true;
    }

    return false;
}
