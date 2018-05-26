#ifndef __XUI_LAYOUTPANELCONTROL_H__
#define __XUI_LAYOUTPANELCONTROL_H__

#include "UI/LayoutDock/Controls/LayoutGridControl.h"
#include "UI/LayoutDock/Layouts/LayoutElement.h"

MetaAssemblyBegin
Meta(modul: ReflInclude("UI/LayoutDock/Layouts/LayoutPanel.h"))
Meta(modul: ReflInclude("UI/LayoutDock/Controls/OverlayWindow.h"))
MetaAssemblyEnd

namespace XUI::UI
{
    class LayoutPanelControl : public LayoutGridControl //<LayoutElement>
    {
    private:
        //std::shared_ptr<LayoutPanel> _model;

    public:
        LayoutPanelControl(std::shared_ptr<LayoutPanel> const& model);

    protected:
        void OnFixChildrenDockLengths() override;

    };
}


#endif	//#ifndef __XUI_LAYOUTPANELCONTROL_H__
