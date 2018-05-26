#include "LayoutAnchorableItem.h"
#include "LayoutAnchorable.h"
#include "LayoutRoot.h"
#include "LayoutAnchorableFloatingWindow.h"
#include "DockingManager.h"
#include "Core/Observer/RoutedCommand.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XamlCPP::Core::Dependency;
using namespace XUI;
using namespace XUI::UI;

STATIC_CLASS_INITIALIZER(LayoutAnchorableItem, XUI::UI::LayoutAnchorableItem::StaticClassInitializer)

XUI::Core::Dependency::RefDependencyProperty LayoutAnchorableItem::HideCommandProperty;
XUI::Core::Dependency::RefDependencyProperty LayoutAnchorableItem::AutoHideCommandProperty;
XUI::Core::Dependency::RefDependencyProperty LayoutAnchorableItem::DockCommandProperty;
XUI::Core::Dependency::RefDependencyProperty LayoutAnchorableItem::CanHideProperty;

void LayoutAnchorableItem::StaticClassInitializer()
{
    HideCommandProperty = (Core::Dependency::BindProperty<std::shared_ptr<Interfaces::ICommand>>::Register<LayoutAnchorableItem>(nameof(HideCommand)));
    AutoHideCommandProperty = (Core::Dependency::BindProperty<std::shared_ptr<Interfaces::ICommand>>::Register<LayoutAnchorableItem>(nameof(AutoHideCommand)));
    DockCommandProperty = (Core::Dependency::BindProperty<std::shared_ptr<Interfaces::ICommand>>::Register<LayoutAnchorableItem>(nameof(DockCommand)));
    CanHideProperty = (Core::Dependency::BindProperty<std::shared_ptr<Interfaces::ICommand>>::Register<LayoutAnchorableItem>(nameof(CanHideCommand)));
}

LayoutAnchorableItem::LayoutAnchorableItem()
{
}

void LayoutAnchorableItem::Attach(LayoutContent *model)
{
    _anchorable = model->shared_from_base_static<LayoutAnchorable>();
    _anchorable->IsVisibleChanged += System::make_eventhandler(&LayoutAnchorableItem::_anchorable_IsVisibleChanged, this);

    LayoutItem::Attach(model);
}

void LayoutAnchorableItem::Detach()
{
    _anchorable->IsVisibleChanged -= System::make_eventhandler(&LayoutAnchorableItem::_anchorable_IsVisibleChanged, this);
    _anchorable = nullptr;
    LayoutItem::Detach();
}

void LayoutAnchorableItem::Close()
{
    if ((_anchorable->Root != nullptr) && (_anchorable->Root->GetManager() != nullptr))
    {
        auto dockingManager = _anchorable->Root->GetManager();
        dockingManager->_ExecuteCloseCommand(_anchorable.get());
    }
}

void LayoutAnchorableItem::InitDefaultCommands()
{
    _defaultHideCommand = std::make_shared<Core::Observer::EventHandleCommand>(System::make_shared_eventhandler([this](void*, Core::Observer::ExecutedRoutedEventArgs& e) { ExecuteHideCommand(e.Parameter); }), System::make_shared_eventhandler([this](void*, Core::Observer::CanExecuteRoutedEventArgs& e) { e.CanExecute = CanExecuteHideCommand(e.Parameter); }));
    _defaultAutoHideCommand = std::make_shared<Core::Observer::EventHandleCommand>(System::make_shared_eventhandler([this](void*, Core::Observer::ExecutedRoutedEventArgs& e) { ExecuteAutoHideCommand(e.Parameter); }), System::make_shared_eventhandler([this](void*, Core::Observer::CanExecuteRoutedEventArgs& e) { e.CanExecute = CanExecuteAutoHideCommand(e.Parameter); }));
    _defaultDockCommand = std::make_shared<Core::Observer::EventHandleCommand>(System::make_shared_eventhandler([this](void*, Core::Observer::ExecutedRoutedEventArgs& e) { ExecuteDockCommand(e.Parameter); }), System::make_shared_eventhandler([this](void*, Core::Observer::CanExecuteRoutedEventArgs& e) { e.CanExecute = CanExecuteDockCommand(e.Parameter); }));

    LayoutItem::InitDefaultCommands();
}

void LayoutAnchorableItem::ClearDefaultBindings()
{
    if (HideCommand == _defaultHideCommand)
        ClearValue(HideCommandProperty.get());
    if (AutoHideCommand == _defaultAutoHideCommand)
        ClearValue(AutoHideCommandProperty.get());
    if (DockCommand == _defaultDockCommand)
        ClearValue(DockCommandProperty.get());

    LayoutItem::ClearDefaultBindings();
}

void LayoutAnchorableItem::SetDefaultBindings()
{
    if (HideCommand == nullptr)
        HideCommand = _defaultHideCommand;
    if (AutoHideCommand == nullptr)
        AutoHideCommand = _defaultAutoHideCommand;
    if (DockCommand == nullptr)
        DockCommand = _defaultDockCommand;

    IsVisible = _anchorable->IsVisible;
    LayoutItem::SetDefaultBindings();
}

void LayoutAnchorableItem::OnHideCommandChanged(DependencyObject *d, Core::Dependency::DependencyPropertyChangedEventArgs & e)
{
    (static_cast<LayoutAnchorableItem*>(d))->OnHideCommandChanged(e);
}

void LayoutAnchorableItem::OnHideCommandChanged(Core::Dependency::DependencyPropertyChangedEventArgs & e)
{
}

System::Reflection::Variant LayoutAnchorableItem::CoerceHideCommandValue(DependencyObject *d, System::Reflection::Variant value)
{
    return value;
}

bool LayoutAnchorableItem::CanExecuteHideCommand(System::Reflection::Variant parameter)
{
    if (Element == nullptr)
        return false;

    return _anchorable->CanHide;
}

void LayoutAnchorableItem::ExecuteHideCommand(System::Reflection::Variant parameter)
{
    if (_anchorable != nullptr && _anchorable->Root != nullptr && _anchorable->Root->GetManager() != nullptr)
    {
        _anchorable->Root->GetManager()->_ExecuteHideCommand(_anchorable.get());
    }
}

void LayoutAnchorableItem::OnAutoHideCommandChanged(DependencyObject *d, Core::Dependency::DependencyPropertyChangedEventArgs &e)
{
    (static_cast<LayoutAnchorableItem*>(d))->OnAutoHideCommandChanged(e);
}

void LayoutAnchorableItem::OnAutoHideCommandChanged(Core::Dependency::DependencyPropertyChangedEventArgs &e)
{
}

System::Reflection::Variant LayoutAnchorableItem::CoerceAutoHideCommandValue(DependencyObject *d, System::Reflection::Variant value)
{
    return value;
}

bool LayoutAnchorableItem::CanExecuteAutoHideCommand(System::Reflection::Variant parameter)
{
    if (Element == nullptr)
    {
        return false;
    }

    if (LayoutElement::FindParent<LayoutAnchorableFloatingWindow>(Element) != nullptr)
    {
        return false; //is floating
    }

    return _anchorable->CanAutoHide;
}

void LayoutAnchorableItem::ExecuteAutoHideCommand(System::Reflection::Variant parameter)
{
    if (_anchorable != nullptr && _anchorable->Root != nullptr && _anchorable->Root->GetManager() != nullptr)
    {
        _anchorable->Root->GetManager()->_ExecuteAutoHideCommand(_anchorable.get());
    }
}

void LayoutAnchorableItem::OnDockCommandChanged(DependencyObject *d, Core::Dependency::DependencyPropertyChangedEventArgs &e)
{
    (static_cast<LayoutAnchorableItem*>(d))->OnDockCommandChanged(e);
}

void LayoutAnchorableItem::OnDockCommandChanged(Core::Dependency::DependencyPropertyChangedEventArgs &e)
{
}

System::Reflection::Variant LayoutAnchorableItem::CoerceDockCommandValue(DependencyObject *d, System::Reflection::Variant value)
{
    return value;
}

bool LayoutAnchorableItem::CanExecuteDockCommand(System::Reflection::Variant parameter)
{
    if (Element == nullptr)
    {
        return false;
    }
    return LayoutElement::FindParent<LayoutAnchorableFloatingWindow>(Element) != nullptr;
}

void LayoutAnchorableItem::ExecuteDockCommand(System::Reflection::Variant parameter)
{
    Element->Root->GetManager()->_ExecuteDockCommand(_anchorable.get());
}

void LayoutAnchorableItem::OnVisibilityChanged()
{
    if (_anchorable != nullptr && _anchorable->Root != nullptr)
    {
        if (_visibilityReentrantFlag.CanEnter())
        {
            {
                _visibilityReentrantFlag.Enter();
                if (!IsVisible)
                {
                    _anchorable->Hide(false);
                }
                else if (IsVisible)
                {
                    _anchorable->Show();
                }
            }
        }
    }

    LayoutItem::OnVisibilityChanged();
}

void LayoutAnchorableItem::_anchorable_IsVisibleChanged(void* sender, System::Events::EventArg& e)
{
    if (_anchorable != nullptr && _anchorable->Root != nullptr)
    {
        if (_visibilityReentrantFlag.CanEnter())
        {
            {
                _visibilityReentrantFlag.Enter();
                if (_anchorable->IsVisible)
                {
                    IsVisible = true;
                }
                else
                {
                    IsVisible = false;
                }
            }
        }
    }
}

void LayoutAnchorableItem::OnCanHideChanged(DependencyObject *d, Core::Dependency::DependencyPropertyChangedEventArgs &e)
{
    (static_cast<LayoutAnchorableItem*>(d))->OnCanHideChanged(e);
}

void LayoutAnchorableItem::OnCanHideChanged(Core::Dependency::DependencyPropertyChangedEventArgs &e)
{
    if (_anchorable != nullptr)
    {
        _anchorable->CanHide = e.NewValue.ToBool();
    }
}
