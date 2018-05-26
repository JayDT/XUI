#ifndef __XUI_DOCUMENTPANECONTROLOVERLAYAREA_H__
#define __XUI_DOCUMENTPANECONTROLOVERLAYAREA_H__

#include "UI/LayoutDock/Controls/OverlayArea.h"

namespace XUI::UI
{
    class LayoutDocumentPaneControl;

    class DocumentPaneControlOverlayArea : public OverlayArea
    {
    private:
        std::shared_ptr<LayoutDocumentPaneControl> _documentPaneControl;
    public:
        DocumentPaneControlOverlayArea(OverlayWindow *overlayWindow, LayoutDocumentPaneControl *documentPaneControl);
    };
}


#endif	//#ifndef __XUI_DOCUMENTPANECONTROLOVERLAYAREA_H__
