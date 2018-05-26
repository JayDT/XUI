#include "DocumentPaneGroupDropTarget.h"
#include "Layouts/LayoutDocumentPaneGroup.h"
#include "Layouts/LayoutAnchorable.h"
#include "Layouts/LayoutAnchorGroup.h"
#include "Layouts/LayoutAnchorablePaneGroup.h"
#include "Controls/OverlayWindow.h"
#include "Core/Media/Geometries/RectangleGeometry.h"

using namespace XUI;
using namespace XUI::UI;

DocumentPaneGroupDropTarget::DocumentPaneGroupDropTarget(LayoutDocumentPaneGroupControl *paneControl, Core::Media::Rect const& detectionRect, DropTargetType type)
    : DropTarget<LayoutDocumentPaneGroupControl>(paneControl->shared_from_base_static<LayoutDocumentPaneGroupControl>(), detectionRect, type)
{
    _targetPane = paneControl;
}

void DocumentPaneGroupDropTarget::Drop(LayoutDocumentFloatingWindow *floatingWindow)
{
    LayoutContainer *targetModel = _targetPane->Model->Static_As<LayoutContainer>();

    if (DropType == DropTargetType::DocumentPaneGroupDockInside)
    {
        auto paneGroupModel = static_cast<LayoutDocumentPaneGroup*>(targetModel);
        auto paneModel = static_cast<LayoutDocumentPane*>(paneGroupModel->GetChild(0));
        auto sourceModel = floatingWindow->RootDocument;

        paneModel->InsertChildAt(0, sourceModel.get());
    }

    DropTarget<LayoutDocumentPaneGroupControl>::Drop(floatingWindow);
}

void DocumentPaneGroupDropTarget::Drop(LayoutAnchorableFloatingWindow *floatingWindow)
{
    LayoutContainer *targetModel = _targetPane->Model->Static_As<LayoutContainer>();

    if (DropType == DropTargetType::DocumentPaneGroupDockInside)
    {
        auto paneGroupModel = static_cast<LayoutDocumentPaneGroup*>(targetModel);
        auto paneModel = static_cast<LayoutDocumentPane*>(paneGroupModel->GetChild(0));
        auto layoutAnchorablePaneGroup = static_cast<LayoutAnchorablePaneGroup*>(floatingWindow->RootPanel.get());

        int i = 0;
        LayoutContainer::ContainsChild(layoutAnchorablePaneGroup, [&](LayoutElement* anchorableToImport) -> bool
        {
            paneModel->InsertChildAt(i, anchorableToImport);
            i++;
            return false;
        });
    }

    DropTarget<LayoutDocumentPaneGroupControl>::Drop(floatingWindow);
}

std::shared_ptr<Core::Media::Geometry> DocumentPaneGroupDropTarget::GetPreviewPath(OverlayWindow *overlayWindow, LayoutFloatingWindow *floatingWindow)
{
    if (DropType == DropTargetType::DocumentPaneGroupDockInside)
    {
        Core::Media::Rect targetScreenRect = TargetElement->TransformedBounds;
        targetScreenRect.Offset(-overlayWindow->TransformedBounds.Left, -overlayWindow->TransformedBounds.Top);

        return std::make_shared<Core::Media::Geometries::RectangleGeometry>(targetScreenRect);
    }

    return nullptr;
}
