#ifndef __XUI_DOCUMENTPANETABPANEL_H__
#define __XUI_DOCUMENTPANETABPANEL_H__

#include "UI/Detail/TabStrip.h"
#include "UI/LayoutDock/Controls/LayoutDocumentTabItem.h"

namespace XUI::UI
{
    class LayoutDocumentTabItem;

    class DocumentPaneTabPanel : public TabStrip
    {
    public:

        static void StaticClassInitializer();

        DocumentPaneTabPanel();
        virtual ~DocumentPaneTabPanel() {}

    protected:

        std::shared_ptr<Generators::IItemContainerGenerator> CreateItemContainerGenerator() override;

        void OnPointerLeave(Interfaces::PointerEventArgs& e) override;
    };
}


#endif	//#ifndef __XUI_DOCUMENTPANETABPANEL_H__
