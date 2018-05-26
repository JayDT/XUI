#ifndef __XUI_DOCUMENTPANEGROUPDROPTARGET_H__
#define __XUI_DOCUMENTPANEGROUPDROPTARGET_H__

#include "UI/LayoutDock/Controls/DropTarget.h"
#include "UI/LayoutDock/Controls/LayoutDocumentPaneGroupControl.h"

MetaAssemblyBegin
Meta(modul: ReflInclude("UI/LayoutDock/Controls/OverlayWindow.h"))
MetaAssemblyEnd

namespace XUI::UI
{
    class DocumentPaneGroupDropTarget : public DropTarget<LayoutDocumentPaneGroupControl>
    {
    private:
        LayoutDocumentPaneGroupControl *_targetPane;

    public:
        DocumentPaneGroupDropTarget(LayoutDocumentPaneGroupControl *paneControl, Core::Media::Rect const& detectionRect, DropTargetType type);

        std::shared_ptr<Core::Media::Geometry> GetPreviewPath(OverlayWindow *overlayWindow, LayoutFloatingWindow *floatingWindow) override;

    protected:

        void Drop(LayoutDocumentFloatingWindow *floatingWindow) override;
        void Drop(LayoutAnchorableFloatingWindow *floatingWindow) override;

    };
}


#endif	//#ifndef __XUI_DOCUMENTPANEGROUPDROPTARGET_H__
