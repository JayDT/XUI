#include "OverlayArea.h"

using namespace XUI;
using namespace XUI::UI;

OverlayArea::OverlayArea(std::shared_ptr<OverlayWindow> const& overlayWindow)
{
    _overlayWindow = overlayWindow;
}

Core::Media::Rect const& OverlayArea::GetScreenDetectionArea() const
{
    return _screenDetectionArea;
}

void OverlayArea::SetScreenDetectionArea(Core::Media::Rect const& rect)
{
    _screenDetectionArea = rect;
}
