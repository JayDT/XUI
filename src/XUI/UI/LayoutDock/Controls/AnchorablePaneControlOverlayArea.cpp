#include "AnchorablePaneControlOverlayArea.h"
#include "OverlayWindow.h"
#include "LayoutAnchorablePaneControl.h"

using namespace XUI;
using namespace XUI::UI;

AnchorablePaneControlOverlayArea::AnchorablePaneControlOverlayArea(OverlayWindow *overlayWindow, LayoutAnchorablePaneControl *anchorablePaneControl)
    : OverlayArea(overlayWindow->shared_from_base_static<OverlayWindow>())
{
    _anchorablePaneControl = anchorablePaneControl->shared_from_base_static<LayoutAnchorablePaneControl>();
    //Rect tempVar(_anchorablePaneControl->PointToScreenDPI(new Point()), _anchorablePaneControl->TransformActualSizeToAncestor());
    OverlayArea::SetScreenDetectionArea(_anchorablePaneControl->Bounds);
}
