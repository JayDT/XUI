#include "DockingManagerOverlayArea.h"
#include "DockingManager.h"
#include "OverlayWindow.h"

using namespace XUI;
using namespace XUI::UI;

DockingManagerOverlayArea::DockingManagerOverlayArea(OverlayWindow *overlayWindow, DockingManager *manager)
    : OverlayArea(overlayWindow->shared_from_base_static<OverlayWindow>())
{
    _manager = manager;

    //Rect tempVar(_manager->PointToScreenDPI(new Point()), _manager->TransformActualSizeToAncestor());
    OverlayArea::SetScreenDetectionArea(_manager->Bounds);
}
