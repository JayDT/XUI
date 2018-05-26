#ifndef __XUI_DOCKINGMANAGEROVERLAYAREA_H__
#define __XUI_DOCKINGMANAGEROVERLAYAREA_H__

#include "UI/LayoutDock/Controls/OverlayArea.h"

namespace XUI::UI
{
    class OverlayWindow;
    class DockingManager;

    class DockingManagerOverlayArea : public OverlayArea
    {
    public:
        DockingManagerOverlayArea(OverlayWindow *overlayWindow, DockingManager *manager);

    private:
        DockingManager *_manager = nullptr;

    };
}


#endif	//#ifndef __XUI_DOCKINGMANAGEROVERLAYAREA_H__
