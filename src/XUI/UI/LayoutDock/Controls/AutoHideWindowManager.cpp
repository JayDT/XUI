#include "AutoHideWindowManager.h"
#include "LayoutAnchorable.h"
#include "LayoutDock/DockingManager.h"

using namespace XUI;
using namespace XUI::UI;

AutoHideWindowManager::AutoHideWindowManager(DockingManager *manager)
{
    _manager = manager;
    SetupCloseTimer();
}

void AutoHideWindowManager::ShowAutoHideWindow(LayoutAnchorControl *anchor)
{
    //auto current = _currentAutohiddenAnchor.expired() ? nullptr : _currentAutohiddenAnchor.lock()->Static_As<LayoutAnchorControl>();
    //if (current != anchor)
    //{
    //    StopCloseTimer();
    //    _currentAutohiddenAnchor = anchor->;
    //    _manager->AutoHideWindow.Show(anchor);
    //    StartCloseTimer();
    //}
}

void AutoHideWindowManager::HideAutoWindow(LayoutAnchorControl *anchor)
{
    if (anchor == nullptr || anchor == _currentAutohiddenAnchor.lock().get())
    {
        StopCloseTimer();
    }
    else
    {
        assert(false);
    }
}

void AutoHideWindowManager::SetupCloseTimer()
{
    //_closeTimer = new DispatcherTimer(DispatcherPriority::Background);
    //_closeTimer->Interval = TimeSpan::FromMilliseconds(1500);
    //_closeTimer->Tick += [&](s, e)
    //{
    //    if (_manager->AutoHideWindow.IsWin32MouseOver || (static_cast<LayoutAnchorable*>(_manager->AutoHideWindow.Model))->IsActive || _manager->AutoHideWindow.IsResizing)
    //    {
    //        return;
    //    }
    //
    //    StopCloseTimer();
    //};
}

void AutoHideWindowManager::StartCloseTimer()
{
    //_closeTimer->Start();

}

void AutoHideWindowManager::StopCloseTimer()
{
    //_closeTimer->Stop();
    //_manager->AutoHideWindow->Hide();
    //_currentAutohiddenAnchor = nullptr;
}
