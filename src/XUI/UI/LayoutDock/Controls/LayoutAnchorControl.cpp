#include "LayoutAnchorControl.h"
#include "LayoutAnchorSide.h"
#include "LayoutRoot.h"
#include "DockingManager.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XamlCPP::Core::Dependency;
using namespace XUI;
using namespace XUI::UI;

STATIC_CLASS_INITIALIZER(LayoutAnchorControl, XUI::UI::LayoutAnchorControl::StaticClassInitializer)

XUI::Core::Dependency::RefDependencyProperty LayoutAnchorControl::SideProperty;

void XUI::UI::LayoutAnchorControl::StaticClassInitializer()
{
    SideProperty = (Core::Dependency::BindProperty<AnchorSide>::Register<LayoutAnchorControl>(nameof(Side)));

    //FrameworkPropertyMetadata tempVar(LayoutAnchorControl::typeid);
    //DefaultStyleKeyProperty::OverrideMetadata(LayoutAnchorControl::typeid, &tempVar);
    //FrameworkPropertyMetadata tempVar2(true);
    //Control->IsHitTestVisibleProperty.AddOwner(LayoutAnchorControl::typeid, &tempVar2);
}

LayoutAnchorControl::LayoutAnchorControl(LayoutAnchorable *model)
{
    Model = model->shared_from_base_static<LayoutAnchorable>();
    Model->IsActiveChanged += System::make_eventhandler(&LayoutAnchorControl::_model_IsActiveChanged, this);
    Model->IsSelectedChanged += System::make_eventhandler(&LayoutAnchorControl::_model_IsSelectedChanged, this);

    SetSide(LayoutElement::FindParent<LayoutAnchorSide>(model)->Side);
}

void LayoutAnchorControl::_model_IsSelectedChanged(void* sender, System::Events::EventArg& e)
{
    if (!Model->IsAutoHidden())
    {
        Model->IsSelectedChanged -= System::make_eventhandler(&LayoutAnchorControl::_model_IsSelectedChanged, this);
    }
    else if (Model->IsSelected)
    {
        Model->Root->GetManager()->ShowAutoHideWindow(this);
        Model->IsSelected = false;
    }
}

void LayoutAnchorControl::_model_IsActiveChanged(void* sender, System::Events::EventArg& e)
{
    if (!Model->IsAutoHidden())
    {
        Model->IsActiveChanged -= System::make_eventhandler(&LayoutAnchorControl::_model_IsActiveChanged, this);
    }
    else if (Model->IsActive)
    {
        Model->Root->GetManager()->ShowAutoHideWindow(this);
    }
}

void LayoutAnchorControl::OnPointerPressed(Interfaces::PointerPressedEventArgs& e)
{
    Control::OnPointerPressed(e);

    if (!e.Handled)
    {
        Model->Root->GetManager()->ShowAutoHideWindow(this);
        Model->IsActive = true;
    }
}

void LayoutAnchorControl::OnPointerEnter(Interfaces::PointerEventArgs& e)
{
    Control::OnPointerEnter(e);

    if (!e.Handled)
    {
        _openUpTimer = std::make_shared<Core::DispatcherTimer>(400, [this]()
        {
            System::Events::EventArg e;
            _openUpTimer_Tick(this, e);
        });
        _openUpTimer->Start();
    }
}

void LayoutAnchorControl::_openUpTimer_Tick(void* sender, System::Events::EventArg& e)
{
    _openUpTimer->Stop();
    _openUpTimer = nullptr;
    Model->Root->GetManager()->ShowAutoHideWindow(this);
}

void LayoutAnchorControl::OnPointerLeave(Interfaces::PointerEventArgs& e)
{
    if (_openUpTimer != nullptr)
    {
        _openUpTimer->Stop();
        _openUpTimer = nullptr;
    }
    Control::OnPointerLeave(e);
}
