#ifndef __XUI_OVERLAYWINDOWDROPTARGET_H__
#define __XUI_OVERLAYWINDOWDROPTARGET_H__

#include "UI/Controls/Control.h"
#include "UI/LayoutDock/DockingDefines.h"

namespace XUI::UI
{
    class OverlayArea;

    class OverlayWindowDropTarget
    {
    private:
        std::shared_ptr<OverlayArea> _overlayArea;

        Core::Media::Rect _screenDetectionArea;
        OverlayWindowDropTargetType _type = OverlayWindowDropTargetType::DockingManagerDockLeft;

    public:
        OverlayWindowDropTarget(std::shared_ptr<OverlayArea> const& overlayArea, OverlayWindowDropTargetType targetType, Controls::Control *element);

        PROPERTY_GET_ONLY(Core::Media::Rect , ScreenDetectionArea);
        Core::Media::Rect GetScreenDetectionArea() const;

        PROPERTY_GET_ONLY(OverlayWindowDropTargetType , Type);
        OverlayWindowDropTargetType GetType() const;

    };
}


#endif	//#ifndef __XUI_OVERLAYWINDOWDROPTARGET_H__
