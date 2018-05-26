#include "DocumentPaneControlOverlayArea.h"
#include "OverlayWindow.h"
#include "LayoutDocumentPaneControl.h"

using namespace XUI;
using namespace XUI::UI;

DocumentPaneControlOverlayArea::DocumentPaneControlOverlayArea(OverlayWindow *overlayWindow, LayoutDocumentPaneControl *documentPaneControl)
    : OverlayArea(overlayWindow->shared_from_base_static<OverlayWindow>())
    , _documentPaneControl(documentPaneControl->shared_from_base_static<LayoutDocumentPaneControl>())
{
    //Rect tempVar(_documentPaneControl->PointToScreenDPI(new Point()), _documentPaneControl->TransformActualSizeToAncestor());
    OverlayArea::SetScreenDetectionArea(_documentPaneControl->Bounds);
}
