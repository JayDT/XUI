#ifndef __XUI_ANCHORABLEPANEDROPTARGET_H__
#define __XUI_ANCHORABLEPANEDROPTARGET_H__

#include "UI/LayoutDock/Controls/DropTarget.h"
#include "UI/LayoutDock/Controls/LayoutAnchorablePaneControl.h"

MetaAssemblyBegin
Meta(modul: ReflInclude("UI/LayoutDock/DockingManager.h"))
Meta(modul: ReflInclude("UI/LayoutDock/Controls/OverlayWindow.h"))
MetaAssemblyEnd

namespace XUI::UI
{
    class AnchorablePaneDropTarget : public DropTarget<LayoutAnchorablePaneControl>
    {
    private:
        std::shared_ptr<LayoutAnchorablePaneControl> _targetPane;
        int _tabIndex = -1;

    public:
        AnchorablePaneDropTarget(LayoutAnchorablePaneControl *paneControl, Core::Media::Rect const& detectionRect, DropTargetType type);
        AnchorablePaneDropTarget(LayoutAnchorablePaneControl *paneControl, Core::Media::Rect const& detectionRect, DropTargetType type, int tabIndex);

        std::shared_ptr<Core::Media::Geometry> GetPreviewPath(OverlayWindow *overlayWindow, LayoutFloatingWindow *floatingWindow) override;

    protected:
        void Drop(LayoutAnchorableFloatingWindow *floatingWindow) override;

    };
}


#endif	//#ifndef __XUI_ANCHORABLEPANEDROPTARGET_H__
