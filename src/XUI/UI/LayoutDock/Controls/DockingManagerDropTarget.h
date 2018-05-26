#ifndef __XUI_DOCKINGMANAGERDROPTARGET_H__
#define __XUI_DOCKINGMANAGERDROPTARGET_H__

#include "UI/LayoutDock/Controls/DropTarget.h"
#include "UI/LayoutDock/DockingManager.h"

MetaAssemblyBegin
Meta(modul: ReflInclude("UI/LayoutDock/Controls/OverlayWindow.h"))
MetaAssemblyEnd

namespace XUI::UI
{
    class DockingManagerDropTarget : public DropTarget<DockingManager>
    {
    private:
        std::shared_ptr<DockingManager> _manager;

    public:
        DockingManagerDropTarget(std::shared_ptr<DockingManager> targetElement, Core::Media::Rect const& detectionRect, DropTargetType type);

        std::shared_ptr<Core::Media::Geometry> GetPreviewPath(OverlayWindow *overlayWindow, LayoutFloatingWindow *floatingWindow) override;

    protected:
        void Drop(LayoutAnchorableFloatingWindow *floatingWindow) override;
    };
}


#endif	//#ifndef __XUI_DOCKINGMANAGERDROPTARGET_H__
