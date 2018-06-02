#ifndef __XUI_LAYOUTANCHORABLEPANEGROUPCONTROL_H__
#define __XUI_LAYOUTANCHORABLEPANEGROUPCONTROL_H__

#include "UI/LayoutDock/Controls/LayoutGridControl.h"
#include "UI/LayoutDock/Layouts/LayoutAnchorablePane.h"

MetaAssemblyBegin
Meta(modul: ReflInclude("UI/LayoutDock/Layouts/LayoutAnchorablePaneGroup.h"))
MetaAssemblyEnd

namespace XUI::UI
{
    class LayoutAnchorablePaneGroup;

    class LayoutAnchorablePaneGroupControl : public LayoutGridControl //<LayoutAnchorablePane>
    {
    public:
        LayoutAnchorablePaneGroupControl(LayoutAnchorablePaneGroup *model);
        virtual ~LayoutAnchorablePaneGroupControl() {}

    protected:
        void OnFixChildrenDockLengths() override;

    };
}


#endif	//#ifndef __XUI_LAYOUTANCHORABLEPANEGROUPCONTROL_H__
