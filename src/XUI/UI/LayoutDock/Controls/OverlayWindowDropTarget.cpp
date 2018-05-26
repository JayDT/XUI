#include "OverlayWindowDropTarget.h"

using namespace XUI;
using namespace XUI::UI;

OverlayWindowDropTarget::OverlayWindowDropTarget(std::shared_ptr<OverlayArea> const& overlayArea, OverlayWindowDropTargetType targetType, Controls::Control *element)
{
    _overlayArea = overlayArea;
    _type = targetType;
    _screenDetectionArea = element->TransformedBounds; //new Rect(element->TransformToDeviceDPI(&tempVar), element->TransformActualSizeToAncestor());
}

Core::Media::Rect OverlayWindowDropTarget::GetScreenDetectionArea() const
{
    return _screenDetectionArea;
}

OverlayWindowDropTargetType OverlayWindowDropTarget::GetType() const
{
    return _type;
}
