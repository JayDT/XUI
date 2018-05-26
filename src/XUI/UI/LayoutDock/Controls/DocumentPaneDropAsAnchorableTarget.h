#ifndef __XUI_DOCUMENTPANEDROPASANCHORABLETARGET_H__
#define __XUI_DOCUMENTPANEDROPASANCHORABLETARGET_H__

#include "UI/LayoutDock/Controls/DropTarget.h"
#include "UI/LayoutDock/Controls/LayoutDocumentPaneControl.h"

MetaAssemblyBegin
Meta(modul: ReflInclude("UI/LayoutDock/Controls/OverlayWindow.h"))
Meta(modul: ReflInclude("UI/LayoutDock/Controls/LayoutFloatingWindowControl.h"))
MetaAssemblyEnd

namespace XUI::UI
{
    class DocumentPaneDropAsAnchorableTarget : public DropTarget<LayoutDocumentPaneControl>
    {
    private:
        LayoutDocumentPaneControl *_targetPane;

        int _tabIndex = -1;

    public:
        DocumentPaneDropAsAnchorableTarget(LayoutDocumentPaneControl *paneControl, Core::Media::Rect const& detectionRect, DropTargetType type);

        DocumentPaneDropAsAnchorableTarget(LayoutDocumentPaneControl *paneControl, Core::Media::Rect const& detectionRect, DropTargetType type, int tabIndex);

    public:
        std::shared_ptr<Core::Media::Geometry> GetPreviewPath(OverlayWindow *overlayWindow, LayoutFloatingWindow *floatingWindow) override;

    protected:
        void Drop(LayoutAnchorableFloatingWindow *floatingWindow) override;

    private:
        Meta(Disable) bool FindParentLayoutDocumentPane(LayoutDocumentPane *documentPane, LayoutDocumentPaneGroup *&containerPaneGroup, LayoutPanel *&containerPanel);

    };
}


#endif	//#ifndef __XUI_DOCUMENTPANEDROPASANCHORABLETARGET_H__
