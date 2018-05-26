#ifndef __XUI_DOCUMENTPANEDROPTARGET_H__
#define __XUI_DOCUMENTPANEDROPTARGET_H__

#include "UI/LayoutDock/Controls/DropTarget.h"
#include "UI/LayoutDock/Controls/LayoutDocumentPaneControl.h"

MetaAssemblyBegin
Meta(modul: ReflInclude("UI/LayoutDock/Controls/OverlayWindow.h"))
MetaAssemblyEnd

namespace XUI::UI
{
    class DocumentPaneDropTarget : public DropTarget<LayoutDocumentPaneControl>
    {
    private:
        std::shared_ptr<LayoutDocumentPaneControl> _targetPane;

        int _tabIndex = -1;

    public:
        DocumentPaneDropTarget(LayoutDocumentPaneControl *paneControl, Core::Media::Rect const& detectionRect, DropTargetType type);
        DocumentPaneDropTarget(LayoutDocumentPaneControl *paneControl, Core::Media::Rect const& detectionRect, DropTargetType type, int tabIndex);

        std::shared_ptr<Core::Media::Geometry> GetPreviewPath(OverlayWindow *overlayWindow, LayoutFloatingWindow *floatingWindow) override;

    protected:

        void Drop(LayoutDocumentFloatingWindow *floatingWindow) override;
        void Drop(LayoutAnchorableFloatingWindow *floatingWindow) override;

    };
}


#endif	//#ifndef __XUI_DOCUMENTPANEDROPTARGET_H__
