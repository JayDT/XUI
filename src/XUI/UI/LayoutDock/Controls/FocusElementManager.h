#ifndef __XUI_FOCUSELEMENTMANAGER_H__
#define __XUI_FOCUSELEMENTMANAGER_H__

#include "UI/Controls/Control.h"
#include "UI/LayoutDock/DockingEventArgs.h"

MetaAssemblyBegin
Meta(modul: ReflInclude("UI/LayoutDock/DockingManager.h"))
MetaAssemblyEnd

namespace XUI::UI
{
    class DockingManager;
    class LayoutElement;
    class WindowHookHandler;

    class FocusElementManager final
    {
    private:
        static System::Collection::Vector<std::shared_ptr<DockingManager>> _managers;
        static std::weak_ptr<System::Reflection::Object> _lastFocusedElement;
        static std::map<LayoutElement*, std::weak_ptr<Controls::InputElement>> _modelFocusedElement;
        static std::map<LayoutElement*, std::intptr_t> _modelFocusedWindowHandle;
        static std::shared_ptr<WindowHookHandler> _windowHandler;
        //static DispatcherOperation *_setFocusAsyncOperation;
        static std::weak_ptr<System::Reflection::Object> _lastFocusedElementBeforeEnterMenuMode;

    public:
        static void SetupFocusManagement(DockingManager *manager);
        static void FinalizeFocusManagement(DockingManager *manager);

        /// <summary>
        /// Get the input element that was focused before user left the layout element
        /// </summary>
        /// <param name="model">Element to look for</param>
        /// <returns>Input element </returns>
        static Controls::InputElement *GetLastFocusedElement(LayoutElement *model);


        /// <summary>
        /// Get the last window handle focused before user left the element passed as argument
        /// </summary>
        /// <param name="model"></param>
        /// <returns></returns>
        static std::intptr_t GetLastWindowHandle(LayoutElement *model);

        /// <summary>
        /// Given a layout element tries to set the focus of the keyword where it was before user moved to another element
        /// </summary>
        /// <param name="model"></param>
        static void SetFocusOnLastElement(LayoutElement *model);

    private:
        static void Current_Exit(void* sender, System::Events::EventArg& e);
        static void manager_PreviewGotKeyboardFocus(void* sender, Interfaces::GotFocusEventArgs& e);
        static void WindowFocusChanging(void*  sender, FocusChangeEventArgs& e);
        static void WindowActivating(void*  sender, WindowActivateEventArgs& e);
        static void InputManager_EnterMenuMode(void* sender, System::Events::EventArg& e);
        static void InputManager_LeaveMenuMode(void* sender, System::Events::EventArg& e);

        #pragma endregion

    };
}


#endif	//#ifndef __XUI_FOCUSELEMENTMANAGER_H__
