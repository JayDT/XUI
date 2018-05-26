#include "FocusElementManager.h"
#include "DockingManager.h"

using namespace XUI;
using namespace XUI::UI;

System::Collection::Vector<std::shared_ptr<DockingManager>> FocusElementManager::_managers;
std::weak_ptr<System::Reflection::Object> FocusElementManager::_lastFocusedElement;
std::map<LayoutElement*, std::weak_ptr<Controls::InputElement>> FocusElementManager::_modelFocusedElement;
std::map<LayoutElement*, std::intptr_t> FocusElementManager::_modelFocusedWindowHandle;
std::shared_ptr<WindowHookHandler> FocusElementManager::_windowHandler;
//DispatcherOperation *FocusElementManager::_setFocusAsyncOperation;
std::weak_ptr<System::Reflection::Object> FocusElementManager::_lastFocusedElementBeforeEnterMenuMode;

void FocusElementManager::SetupFocusManagement(DockingManager *manager)
{
    //if (_managers.empty())
    //{
    //    //InputManager.Current.EnterMenuMode += new EventHandler(InputManager_EnterMenuMode);
    //    //InputManager.Current.LeaveMenuMode += new EventHandler(InputManager_LeaveMenuMode);
    //    _windowHandler = new WindowHookHandler();
    //    _windowHandler->FocusChanged += new EventHandler<FocusChangeEventArgs*>(WindowFocusChanging);
    //    //_windowHandler.Activate += new EventHandler<WindowActivateEventArgs>(WindowActivating);
    //    _windowHandler->Attach();
    //
    //    if (Application::Current != nullptr)
    //    {
    //        Application::Current->Exit += new ExitEventHandler(Current_Exit);
    //    }
    //}
    //
    //manager->PreviewGotKeyboardFocus += new KeyboardFocusChangedEventHandler(manager_PreviewGotKeyboardFocus);
    //_managers.push_back(manager);
}

void FocusElementManager::FinalizeFocusManagement(DockingManager *manager)
{
    //manager->PreviewGotKeyboardFocus -= new KeyboardFocusChangedEventHandler(manager_PreviewGotKeyboardFocus);
    //_managers.Remove(manager);
    //
    //if (_managers.empty())
    //{
    //    //InputManager.Current.EnterMenuMode -= new EventHandler(InputManager_EnterMenuMode);
    //    //InputManager.Current.LeaveMenuMode -= new EventHandler(InputManager_LeaveMenuMode);
    //    if (_windowHandler != nullptr)
    //    {
    //        _windowHandler->FocusChanged -= new EventHandler<FocusChangeEventArgs*>(WindowFocusChanging);
    //        //_windowHandler.Activate -= new EventHandler<WindowActivateEventArgs>(WindowActivating);
    //        _windowHandler->Detach();
    //        _windowHandler = nullptr;
    //    }
    //}
}

void FocusElementManager::Current_Exit(void* sender, System::Events::EventArg& e)
{
    //Application::Current->Exit -= new ExitEventHandler(Current_Exit);
    //if (_windowHandler != nullptr)
    //{
    //    _windowHandler->FocusChanged -= new EventHandler<FocusChangeEventArgs*>(WindowFocusChanging);
    //    //_windowHandler.Activate -= new EventHandler<WindowActivateEventArgs>(WindowActivating);
    //    _windowHandler->Detach();
    //    _windowHandler = nullptr;
    //}
}

void FocusElementManager::manager_PreviewGotKeyboardFocus(void* sender, Interfaces::GotFocusEventArgs& e)
{
    //auto focusedElement = dynamic_cast<Visual*>(e->NewFocus);
    //if (focusedElement != nullptr && !(dynamic_cast<LayoutAnchorableTabItem*>(focusedElement) != nullptr || dynamic_cast<LayoutDocumentTabItem*>(focusedElement) != nullptr))
    //{
    //    //Avoid tracking focus for elements like this
    //    auto parentAnchorable = focusedElement->FindVisualAncestor<LayoutAnchorableControl*>();
    //    if (parentAnchorable != nullptr)
    //    {
    //        _modelFocusedElement[parentAnchorable->Model] = e->NewFocus;
    //    }
    //    else
    //    {
    //        auto parentDocument = focusedElement->FindVisualAncestor<LayoutDocumentControl*>();
    //        if (parentDocument != nullptr)
    //        {
    //            _modelFocusedElement[parentDocument->Model] = e->NewFocus;
    //        }
    //    }
    //}
}

Controls::InputElement *FocusElementManager::GetLastFocusedElement(LayoutElement *model)
{
    //IInputElement *objectWithFocus;
    //if (_modelFocusedElement->GetValue(model, objectWithFocus))
    //{
    //    return objectWithFocus;
    //}

    return nullptr;
}

std::intptr_t FocusElementManager::GetLastWindowHandle(LayoutElement *model)
{
    //void* handleWithFocus;
    //if (_modelFocusedWindowHandle->GetValue(model, handleWithFocus))
    //{
    //    return handleWithFocus;
    //}

    return 0;
}

void FocusElementManager::SetFocusOnLastElement(LayoutElement *model)
{
    //bool focused = false;
    //IInputElement *objectToFocus;
    //if (_modelFocusedElement->GetValue(model, objectToFocus))
    //{
    //    focused = objectToFocus == Keyboard->Focus(objectToFocus);
    //}
    //
    //void* handleToFocus;
    //if (_modelFocusedWindowHandle->GetValue(model, handleToFocus))
    //{
    //    focused = void*::Zero != Win32Helper::SetFocus(handleToFocus);
    //}
    //
    //
    //if (focused)
    //{
    //    _lastFocusedElement = new WeakReference(model);
    //}
}

void FocusElementManager::WindowFocusChanging(void* sender, FocusChangeEventArgs& e)
{
    //for (auto manager : _managers)
    //{
    //    auto hostContainingFocusedHandle = manager->FindLogicalChildren<HwndHost*>().FirstOrDefault([&](System::Reflection::Variant hw)
    //    {
    //        Win32Helper::IsChild(hw->Handle, e->GotFocusWinHandle);
    //    });
    //
    //    if (hostContainingFocusedHandle != nullptr)
    //    {
    //        auto parentAnchorable = hostContainingFocusedHandle->FindVisualAncestor<LayoutAnchorableControl*>();
    //        if (parentAnchorable != nullptr)
    //        {
    //            _modelFocusedWindowHandle[parentAnchorable->Model] = e->GotFocusWinHandle;
    //            if (parentAnchorable->Model != nullptr)
    //            {
    //                parentAnchorable->Model->IsActive = true;
    //            }
    //        }
    //        else
    //        {
    //            auto parentDocument = hostContainingFocusedHandle->FindVisualAncestor<LayoutDocumentControl*>();
    //            if (parentDocument != nullptr)
    //            {
    //                _modelFocusedWindowHandle[parentDocument->Model] = e->GotFocusWinHandle;
    //                if (parentDocument->Model != nullptr)
    //                {
    //                    parentDocument->Model->IsActive = true;
    //                }
    //            }
    //        }
    //    }
    //}
}

void FocusElementManager::WindowActivating(void* sender, WindowActivateEventArgs& e)
{
    //if (Keyboard->FocusedElement == nullptr && _lastFocusedElement != nullptr && _lastFocusedElement->IsAlive)
    //{
    //    auto elementToSetFocus = dynamic_cast<ILayoutElement*>(_lastFocusedElement->Target);
    //    if (elementToSetFocus != nullptr)
    //    {
    //        auto manager = elementToSetFocus->Root.Manager;
    //        if (manager == nullptr)
    //        {
    //            return;
    //        }
    //
    //        void* parentHwnd;
    //        if (!manager->GetParentWindowHandle(parentHwnd))
    //        {
    //            return;
    //        }
    //
    //        if (e->HwndActivating != parentHwnd)
    //        {
    //            return;
    //        }
    //
    //        std::function<void()> tempVar([&]()
    //        {
    //            try
    //            {
    //                SetFocusOnLastElement(elementToSetFocus);
    //            }
    //            finally
    //            {
    //                _setFocusAsyncOperation = nullptr;
    //            }
    //        });
    //        _setFocusAsyncOperation = Dispatcher::CurrentDispatcher::BeginInvoke(&tempVar, DispatcherPriority::Input);
    //    }
    //}
}

void FocusElementManager::InputManager_EnterMenuMode(void* sender, System::Events::EventArg& e)
{
    //if (Keyboard->FocusedElement == nullptr)
    //{
    //    return;
    //}
    //
    //auto lastfocusDepObj = dynamic_cast<DependencyObject*>(Keyboard::FocusedElement);
    //if (lastfocusDepObj->FindLogicalAncestor<DockingManager*>() == nullptr)
    //{
    //    _lastFocusedElementBeforeEnterMenuMode = nullptr;
    //    return;
    //}
    //
    //_lastFocusedElementBeforeEnterMenuMode = new WeakReference(Keyboard::FocusedElement);
}

void FocusElementManager::InputManager_LeaveMenuMode(void* sender, System::Events::EventArg& e)
{
    //if (_lastFocusedElementBeforeEnterMenuMode != nullptr && _lastFocusedElementBeforeEnterMenuMode->IsAlive)
    //{
    //    auto lastFocusedInputElement = _lastFocusedElementBeforeEnterMenuMode->GetValueOrDefault<UIElement*>();
    //    if (lastFocusedInputElement != nullptr)
    //    {
    //        if (lastFocusedInputElement != Keyboard->Focus(lastFocusedInputElement))
    //        {
    //            Debug::WriteLine("Unable to activate the element");
    //        }
    //    }
    //}
}
