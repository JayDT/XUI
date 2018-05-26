#ifndef __XUI_LAYOUTDOCUMENTFLOATINGWINDOWCONTROL_H__
#define __XUI_LAYOUTDOCUMENTFLOATINGWINDOWCONTROL_H__

#include "UI/LayoutDock/Controls/LayoutFloatingWindowControl.h"

MetaAssemblyBegin
Meta(modul: ReflInclude("UI/LayoutDock/Controls/OverlayWindow.h"))
Meta(modul: ReflInclude("UI/LayoutDock/DockingManager.h"))
Meta(modul: ReflInclude("UI/LayoutDock/Controls/LayoutItem.h"))
Meta(modul: ReflInclude("UI/LayoutDock/Layouts/LayoutDocumentFloatingWindow.h"))
MetaAssemblyEnd

namespace XUI::UI
{
    class LayoutDocumentFloatingWindow;
    class LayoutItem;

    class LayoutDocumentFloatingWindowControl : public LayoutFloatingWindowControl
    {
    private:

    public:
        LayoutDocumentFloatingWindowControl(LayoutDocumentFloatingWindow *model);

        UI::LayoutItem *GetRootDocumentLayoutItem() const;

    protected:
        void OnWindowInitialized() override;
        //void* FilterMessage(void* hwnd, int msg, void* wParam, void* lParam, bool &handled) override;
        void _model_RootDocumentChanged(void* sender, System::Events::EventArg& e);
        bool OpenContextMenu();
        void HandleClosed() override;
    };
}


#endif	//#ifndef __XUI_LAYOUTDOCUMENTFLOATINGWINDOWCONTROL_H__
