#ifndef __XUI_ANCHORABLEPANETABPANEL_H__
#define __XUI_ANCHORABLEPANETABPANEL_H__

#include "UI/Detail/TabStrip.h"
#include "UI/LayoutDock/Controls/LayoutAnchorableTabItem.h"

namespace XUI::UI
{
    class AnchorablePaneTabPanel : public TabStrip
    {
    public:

        static void StaticClassInitializer();

        AnchorablePaneTabPanel();

    protected:

        std::shared_ptr<Generators::IItemContainerGenerator> CreateItemContainerGenerator() override;

        void OnPointerLeave(Interfaces::PointerEventArgs& e) override;
    };
}


#endif	//#ifndef __XUI_ANCHORABLEPANETABPANEL_H__
