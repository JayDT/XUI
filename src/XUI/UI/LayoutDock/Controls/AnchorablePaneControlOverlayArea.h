#ifndef __XUI_ANCHORABLEPANECONTROLOVERLAYAREA_H__
#define __XUI_ANCHORABLEPANECONTROLOVERLAYAREA_H__

#include "UI/LayoutDock/Controls/OverlayArea.h"

namespace XUI::UI
{
    class LayoutAnchorablePaneControl;

    class AnchorablePaneControlOverlayArea : public OverlayArea
    {
    public:
        AnchorablePaneControlOverlayArea(OverlayWindow *overlayWindow, LayoutAnchorablePaneControl *anchorablePaneControl);

    private:
        std::shared_ptr<LayoutAnchorablePaneControl> _anchorablePaneControl;
    };
}


#endif	//#ifndef __XUI_ANCHORABLEPANECONTROLOVERLAYAREA_H__
