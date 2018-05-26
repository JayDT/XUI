#include "LayoutFloatingWindowControl.h"
#include "LayoutContainer.h"
#include "LayoutContent.h"
#include "OverlayWindow.h"
#include "DropTargetBase.h"
#include "Core/Input/MouseDevice.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XamlCPP::Core::Dependency;
using namespace XUI;
using namespace XUI::UI;

STATIC_CLASS_INITIALIZER(LayoutFloatingWindowControl, XUI::UI::LayoutFloatingWindowControl::StaticClassInitializer);

XUI::Core::Dependency::RefDependencyProperty LayoutFloatingWindowControl::IsMaximizedProperty;
XUI::Core::Dependency::RefDependencyProperty LayoutFloatingWindowControl::IsDraggingProperty;

void XUI::UI::LayoutFloatingWindowControl::StaticClassInitializer()
{
    IsMaximizedProperty = (Core::Dependency::BindProperty<bool>::Register<LayoutFloatingWindowControl>(nameof(IsMaximized)));;
    IsDraggingProperty = (Core::Dependency::BindProperty<bool>::Register<LayoutFloatingWindowControl>(nameof(IsDragging)));;

    //FrameworkPropertyMetadata tempVar(nullptr, nullptr, new CoerceValueCallback(CoerceContentValue));
    //LayoutFloatingWindowControl::ContentProperty::OverrideMetadata(LayoutFloatingWindowControl::typeid, &tempVar);
    //FrameworkPropertyMetadata tempVar2(false);
    //AllowsTransparencyProperty::OverrideMetadata(LayoutFloatingWindowControl::typeid, &tempVar2);
    //FrameworkPropertyMetadata tempVar3(false);
    //ShowInTaskbarProperty::OverrideMetadata(LayoutFloatingWindowControl::typeid, &tempVar3);
}

XUI::UI::LayoutFloatingWindowControl::LayoutFloatingWindowControl()
{
}

LayoutFloatingWindowControl::LayoutFloatingWindowControl(LayoutElement *model)
{
    this->Initialized += System::make_eventhandler(&LayoutFloatingWindowControl::OnLoaded, this);
    this->Closed += System::make_eventhandler(&LayoutFloatingWindowControl::OnUnloaded, this);
    Model = model->shared_from_base_static<LayoutElement>();
}

XUI::UI::LayoutFloatingWindowControl::~LayoutFloatingWindowControl()
{
}

void LayoutFloatingWindowControl::OnClosed(void*, System::Events::EventArg& e)
{
    Closed -= System::make_eventhandler(&LayoutFloatingWindowControl::OnClosed, this);
}

void LayoutFloatingWindowControl::AttachDrag(bool onActivated)
{
    if (onActivated)
    {
        _attachDrag = true;
        this->Activated += System::make_eventhandler(&LayoutFloatingWindowControl::OnActivated, this);
    }
}

void LayoutFloatingWindowControl::OnLoaded(void* sender, System::Events::EventArg& e)
{
    this->Initialized -= System::make_eventhandler(&LayoutFloatingWindowControl::OnLoaded, this);

    //this->SetParentToMainWindowOf(Model->Root.Manager);

    // Restore maximize state
    auto maximized = LayoutContainer::ContainsChild(GetModel()->Static_As<LayoutContainer>(), [](LayoutElement* x) -> bool
    {
        auto content = x->Static_As<LayoutContent>();
        if (content)
            return content->IsMaximized;
        return false;
    });
    UpdateMaximizedState(maximized);
}

void LayoutFloatingWindowControl::OnUnloaded(void* sender, System::Events::EventArg& e)
{
    this->Closed -= System::make_eventhandler(&LayoutFloatingWindowControl::OnUnloaded, this);

    //InternalClose();
}

void LayoutFloatingWindowControl::OnActivated(void* sender, System::Events::EventArg& e)
{
    this->Initialized -= System::make_eventhandler(&LayoutFloatingWindowControl::OnActivated, this);

    if (_attachDrag && _titleBar && Core::Input::MouseDevice::Modifiers & Interfaces::InputModifiers::LeftMouseButton)
    {
        auto mouseDevice = XUI::Platform::UIService::Instance()->Get<Interfaces::IMouseDevice>();

        auto mousePosition = mouseDevice->GetRelativePosition(VisualRoot->ToVisual());
        if (_titleBar->TransformedBounds.contains(mousePosition))
            DragMove();
    }
}

void LayoutFloatingWindowControl::OnWindowInitialized()
{
    Closed += System::make_eventhandler(&LayoutFloatingWindowControl::OnClosed, this);
    Window::OnWindowInitialized();
}

void LayoutFloatingWindowControl::OnIsDraggingChanged(DependencyObject *d, DependencyPropertyChangedEventArgs *e)
{
    (static_cast<LayoutFloatingWindowControl*>(d))->OnIsDraggingChanged(e);
}

void LayoutFloatingWindowControl::OnIsDraggingChanged(DependencyPropertyChangedEventArgs *e)
{
    //if (static_cast<bool>(e->NewValue))
    //{
    //    CaptureMouse();
    //}
    //else
    //{
    //    ReleaseMouseCapture();
    //}
}

void LayoutFloatingWindowControl::UpdatePositionAndSizeOfPanes()
{
    //for (auto posElement : Model->Descendents().OfType<ILayoutElementForFloatingWindow*>())
    //{
    //    posElement->FloatingLeft = Left;
    //    posElement->FloatingTop = Top;
    //    posElement->FloatingWidth = Width;
    //    posElement->FloatingHeight = Height;
    //}
}

void LayoutFloatingWindowControl::UpdateMaximizedState(bool isMaximized)
{
    //LayoutContainer::conta
    //for (auto posElement : Model->Descendents().OfType<ILayoutElementForFloatingWindow*>())
    //{
    //    posElement->IsMaximized = isMaximized;
    //}
    //IsMaximized = isMaximized;
    //_isInternalChange = true;
    //WindowState = isMaximized ? WindowState::Maximized : WindowState::Normal;
    //_isInternalChange = false;
}

void LayoutFloatingWindowControl::OnPointerMoved(Interfaces::PointerEventArgs& e)
{
    if (_internalCloseFlag)
        return;

    Window::OnPointerMoved(e);

    if (_inDragMove)
        UpdateDragPosition();
}

void LayoutFloatingWindowControl::OnPreviewPointerReleased(Interfaces::PointerReleasedEventArgs& e)
{
    UI::Window::OnPointerReleased(e);

    if (Model && Model->Root && Model->Root->GetManager())
    {
        auto root = Model->Root;
        auto mgr = root->GetManager();

        for (auto& floatingWnd : mgr->GetFloatingWindows())
        {
            IOverlayWindowHost* host = floatingWnd->ToIOverlayWindowHost();
            if (!host)
                continue;

            auto target_itr = _cacheDropTargets.find(host);
            if (target_itr != _cacheDropTargets.end())
            {
                for (auto& droptarget : target_itr->second)
                {
                    if (_currentDropTarget == droptarget)
                    {
                        target_itr->first->ShowOverlayWindow(nullptr)->DragDrop(_currentDropTarget.get());
                        _currentDropTarget = nullptr;
                    }

                    target_itr->first->ShowOverlayWindow(nullptr)->DragLeave(droptarget.get());
                }

                _cacheDropTargets.erase(target_itr);
            }

            auto itr = _cacheDropAreas.find(host);
            if (itr != _cacheDropAreas.end())
            {
                for (auto& droparea : itr->second)
                    itr->first->ShowOverlayWindow(nullptr)->DragLeave(droparea.get());
                _cacheDropAreas.erase(itr);
                _cacheDropTargets.erase(host);
            }

            host->HideOverlayWindow();
        }

        auto target_itr = _cacheDropTargets.find(mgr);
        if (target_itr != _cacheDropTargets.end())
        {
            for (auto& droptarget : target_itr->second)
            {
                if (_currentDropTarget == droptarget)
                {
                    target_itr->first->ShowOverlayWindow(nullptr)->DragDrop(_currentDropTarget.get());
                    _currentDropTarget = nullptr;
                }

                target_itr->first->ShowOverlayWindow(nullptr)->DragLeave(droptarget.get());
            }

            _cacheDropTargets.erase(target_itr);
        }

        auto itr = _cacheDropAreas.find(mgr);
        if (itr != _cacheDropAreas.end())
        {
            for (auto& droparea : itr->second)
                itr->first->ShowOverlayWindow(nullptr)->DragLeave(droparea.get());
            _cacheDropAreas.erase(itr);
            _cacheDropTargets.erase(mgr);
        }

        mgr->HideOverlayWindow();
        root->CollectGarbage();
    }

    _currentDropTarget = nullptr;
    _activeDropAreas.clear();
}

void LayoutFloatingWindowControl::UpdateDragPosition()
{
    if (Model && Model->Root && Model->Root->GetManager())
    {
        auto mouseDevice = XUI::Platform::UIService::Instance()->Get<Interfaces::IMouseDevice>();
        auto mgr = Model->Root->GetManager();

        for (auto& floatingWnd : mgr->GetFloatingWindows())
        {
            IOverlayWindowHost* host = floatingWnd->ToIOverlayWindowHost();
            if (!host || floatingWnd.get() == this)
                continue;

            auto mousePosition = mouseDevice->GetRelativePosition(floatingWnd->VisualRoot->ToVisual());

            if (host->HitTest(mousePosition))
            {
                UI::OverlayWindow* overlayWnd = host->ShowOverlayWindow(this);
                if (overlayWnd && overlayWnd->IsValidLayout())
                {
                    bool targetUpdate = true;

                    if (overlayWnd->GetFloatingWindow() != this)
                    {
                        overlayWnd->DragEnter(this);
                        _cacheDropAreas[host] = host->GetDropAreas(this);
                    }

                    auto itr = _cacheDropTargets.find(host);
                    if (itr == _cacheDropTargets.end())
                        itr = _cacheDropTargets.insert(std::make_pair(host, std::set<std::shared_ptr<DropTargetBase>>())).first;

                    if (_currentDropTarget && itr != _cacheDropTargets.end() && itr->second.find(_currentDropTarget) != itr->second.end() && !_currentDropTarget->HitTest(mousePosition))
                    {
                        overlayWnd->DragLeave(_currentDropTarget.get());
                        _currentDropTarget = nullptr;
                    }

                    auto areas_itr = _cacheDropAreas.find(host);
                    if (areas_itr != _cacheDropAreas.end())
                    {
                        for (auto& droparea : areas_itr->second)
                        {
                            if (!droparea->GetDetectionRect().contains(mousePosition) && _activeDropAreas.find(droparea.get()) != _activeDropAreas.end())
                            {
                                targetUpdate = false;
                                _changedAreas = true;
                                _activeDropAreas.erase(droparea.get());
                                overlayWnd->DragLeave(droparea.get());
                            }
                        }

                        for (auto& droparea : areas_itr->second)
                        {
                            if (droparea->GetDetectionRect().contains(mousePosition))
                            {
                                if (_activeDropAreas.find(droparea.get()) == _activeDropAreas.end())
                                {
                                    targetUpdate = false;
                                    _changedAreas = true;
                                    _activeDropAreas.insert(droparea.get());
                                    overlayWnd->DragEnter(droparea.get());
                                }
                            }
                        }
                    }

                    if (targetUpdate && _changedAreas)
                    {
                        _changedAreas = false;
                        itr->second.clear();
                        for (auto target : overlayWnd->GetTargets())
                            itr->second.insert(target);
                    }

                    if (itr != _cacheDropTargets.end())
                    {
                        for (auto& droptarget : itr->second)
                        {
                            if (droptarget->HitTest(mousePosition))
                            {
                                if (_currentDropTarget == droptarget)
                                    continue;

                                overlayWnd->DragEnter(droptarget.get());
                                _currentDropTarget = droptarget;
                            }
                            else if (_currentDropTarget == droptarget)
                            {
                                overlayWnd->DragLeave(_currentDropTarget.get());
                                _currentDropTarget = nullptr;
                            }
                        }
                    }
                }
            }
            else
            {
                auto itr = _cacheDropAreas.find(host);
                if (itr != _cacheDropAreas.end())
                {
                    for (auto& droparea : itr->second)
                    {
                        itr->first->ShowOverlayWindow(nullptr)->DragLeave(droparea.get());
                        _activeDropAreas.erase(droparea.get());
                    }

                    for (auto& droptarget : _cacheDropTargets[host])
                    {
                        if (_currentDropTarget == droptarget)
                        {
                            itr->first->ShowOverlayWindow(nullptr)->DragLeave(_currentDropTarget.get());
                            _currentDropTarget = nullptr;
                        }
                    }

                    _cacheDropAreas.erase(itr);
                    _cacheDropTargets.erase(host);
                    host->HideOverlayWindow();
                }
            }
        }

        auto mousePosition = mouseDevice->GetRelativePosition(mgr->VisualRoot->ToVisual());
        if (mgr->HitTest(mousePosition))
        {
            UI::OverlayWindow* overlayWnd = mgr->ShowOverlayWindow(this);
            if (overlayWnd && overlayWnd->IsValidLayout())
            {
                bool targetUpdate = true;

                if (overlayWnd->GetFloatingWindow() != this)
                {
                    overlayWnd->DragEnter(this);
                    _cacheDropAreas[mgr] = mgr->GetDropAreas(this);
                }

                auto itr = _cacheDropTargets.find(mgr);
                if (itr == _cacheDropTargets.end())
                    itr = _cacheDropTargets.insert(std::make_pair(mgr, std::set<std::shared_ptr<DropTargetBase>>())).first;

                if (_currentDropTarget && itr != _cacheDropTargets.end() && itr->second.find(_currentDropTarget) != itr->second.end() && !_currentDropTarget->HitTest(mousePosition))
                {
                    overlayWnd->DragLeave(_currentDropTarget.get());
                    _currentDropTarget = nullptr;
                }

                auto areas_itr = _cacheDropAreas.find(mgr);
                if (areas_itr != _cacheDropAreas.end())
                {
                    for (auto& droparea : areas_itr->second)
                    {
                        if (!droparea->GetDetectionRect().contains(mousePosition) && _activeDropAreas.find(droparea.get()) != _activeDropAreas.end())
                        {
                            targetUpdate = false;
                            _changedAreas = true;
                            _activeDropAreas.erase(droparea.get());
                            overlayWnd->DragLeave(droparea.get());
                        }
                    }

                    for (auto& droparea : areas_itr->second)
                    {
                        if (droparea->GetDetectionRect().contains(mousePosition))
                        {
                            if (_activeDropAreas.find(droparea.get()) == _activeDropAreas.end())
                            {
                                targetUpdate = false;
                                _changedAreas = true;
                                _activeDropAreas.insert(droparea.get());
                                overlayWnd->DragEnter(droparea.get());
                            }
                        }
                    }
                }

                if (targetUpdate && _changedAreas)
                {
                    _changedAreas = false;
                    itr->second.clear();
                    for (auto target : overlayWnd->GetTargets())
                        itr->second.insert(target);
                }

                if (itr != _cacheDropTargets.end())
                {
                    for (auto& droptarget : itr->second)
                    {
                        if (droptarget->HitTest(mousePosition))
                        {
                            if (_currentDropTarget == droptarget)
                                continue;

                            overlayWnd->DragEnter(droptarget.get());
                            _currentDropTarget = droptarget;
                        }
                        else if (_currentDropTarget == droptarget)
                        {
                            overlayWnd->DragLeave(_currentDropTarget.get());
                            _currentDropTarget = nullptr;
                        }
                    }
                }
            }
        }
        else
        {
            auto itr = _cacheDropAreas.find(mgr);
            if (itr != _cacheDropAreas.end())
            {
                for (auto& droparea : itr->second)
                {
                    itr->first->ShowOverlayWindow(nullptr)->DragLeave(droparea.get());
                    _activeDropAreas.erase(droparea.get());
                }

                for (auto& droptarget : _cacheDropTargets[mgr])
                {
                    if (_currentDropTarget == droptarget)
                    {
                        itr->first->ShowOverlayWindow(nullptr)->DragLeave(_currentDropTarget.get());
                        _currentDropTarget = nullptr;
                    }
                }

                _cacheDropAreas.erase(itr);
                _cacheDropTargets.erase(mgr);
                mgr->HideOverlayWindow();
            }
        }
    }
}

void LayoutFloatingWindowControl::InternalClose()
{
    _internalCloseFlag = true;
    Close();
}

bool LayoutFloatingWindowControl::GetCloseInitiatedByUser() const
{
    return !_internalCloseFlag;
}

void LayoutFloatingWindowControl::OnStateChanged(System::Events::EventArg& e)
{
    //if (!_isInternalChange)
    //{
    //    if (WindowState == WindowState::Maximized)
    //    {
    //        UpdateMaximizedState(true);
    //    }
    //    else
    //    {
    //        WindowState = IsMaximized ? WindowState::Maximized : WindowState::Normal;
    //    }
    //}
    //
    //Window::OnStateChanged(e);
}
