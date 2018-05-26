#ifndef __XUI_AUTOHIDEWINDOWMANAGER_H__
#define __XUI_AUTOHIDEWINDOWMANAGER_H__

MetaAssemblyBegin
Meta(modul: ReflInclude("UI/LayoutDock/Controls/OverlayWindow.h"))
Meta(modul: ReflInclude("UI/LayoutDock/DockingManager.h"))
Meta(modul: ReflInclude("UI/LayoutDock/Controls/LayoutAnchorControl.h"))
MetaAssemblyEnd

namespace XUI::UI
{
    class DockingManager;
    class LayoutAnchorControl;

    class AutoHideWindowManager
    {
    private:
        DockingManager *_manager = nullptr;
        std::weak_ptr<System::Reflection::Object> _currentAutohiddenAnchor;
        //DispatcherTimer *_closeTimer = nullptr;


    public:
        AutoHideWindowManager(DockingManager *manager);

        void ShowAutoHideWindow(LayoutAnchorControl *anchor);
        void HideAutoWindow(LayoutAnchorControl *anchor = nullptr);

    private:

        void SetupCloseTimer();
        void StartCloseTimer();
        void StopCloseTimer();
    };
}


#endif	//#ifndef __XUI_AUTOHIDEWINDOWMANAGER_H__
