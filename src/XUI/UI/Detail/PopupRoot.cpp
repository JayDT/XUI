/*
* Copyright (C) 2017-2018 Tauri JayD <https://www.>
* Copyright (c) 2014 Steven Kirk <The Avalonia Project>
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the
* Free Software Foundation; either version 2 of the License, or (at your
* option) any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
* You should have received a copy of the GNU General Public License along
* with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "PopupRoot.h"
#include "UI/Detail/Popup.h"
#include "Interfaces/IRenderer.h"
#include "Platform/UIRenderer.h"
#include "Platform/UIService.h"
#include "Core/GlobalInitializerScheduler.h"
#include "Core/Media/Colors.h"

using namespace XUI;
using namespace XUI::Interfaces;
using namespace XUI::UI;

STATIC_CLASS_INITIALIZER(PopupRoot, PopupRoot::StaticClassInitializer)

void PopupRoot::StaticClassInitializer()
{
    BackgroundProperty->OverrideDefaultValue<PopupRoot>(std::make_shared<Core::Media::SolidColorBrush>(Core::Media::Colors::GetWhite()));
    IsVisibleProperty->OverrideDefaultValue<PopupRoot>(false);
    HorizontalAlignmentProperty->OverrideDefaultValue<PopupRoot>(Controls::HorizontalAlignment::Left);
    VerticalAlignmentProperty->OverrideDefaultValue<PopupRoot>(Controls::VerticalAlignment::Top);

    BoundsProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const & e)
    {
        PopupRoot* control = e.Sender->Static_As<PopupRoot>();
        if (control && control->GetPlatformImpl())
            control->GetPlatformImpl()->Resize(control->Bounds.GetSize());
    });

    GotFocusEvent->AddClassSpecHandler<GotFocusEventArgs, PopupRoot>(System::make_eventhandler([](void* x, GotFocusEventArgs& e)
    {
        if (e.Handled)
            static_cast<PopupRoot*>((System::Reflection::Object*)x)->OnGotFocus(e);
    }), Core::Observer::RoutingStrategies(Core::Observer::RoutingStrategies::Direct | Core::Observer::RoutingStrategies::Bubble), true);

    LostFocusEvent->AddClassSpecHandler<GotFocusEventArgs, PopupRoot>(System::make_eventhandler([](void* x, GotFocusEventArgs& e)
    {
        if (e.Handled)
            static_cast<PopupRoot*>((System::Reflection::Object*)x)->OnLostFocus(e);
    }), Core::Observer::RoutingStrategies(Core::Observer::RoutingStrategies::Direct | Core::Observer::RoutingStrategies::Bubble), true);
}

void XUI::UI::PopupRoot::InstanceInitializer()
{
}

PopupRoot::PopupRoot()
{
}

Controls::InputElement* PopupRoot::GetInteractiveParent() const
{
    return Parent;
}

Controls::Visual* PopupRoot::GetHost() const
{
    return Parent;
}

PopupRoot::~PopupRoot()
{
    if (_presenterSubscription.is_subscribed())
        _presenterSubscription.unsubscribe();
}

void XUI::UI::PopupRoot::EndInit()
{
    XUI::UI::TopLevel::EndInit();

    if (_initCount == 0)
    {
        if (!_platformImplInitialized)
            InitializePlatformWindow();
    }
}

void XUI::UI::PopupRoot::Close()
{
    auto renderService = Platform::UIService::Instance()->Get<Platform::UIRender>();
    if (VisualParent == nullptr)
    {
        if (!_windowImpl /*&& Parent != renderService*/)
        {
            renderService->RemoveLogicalChild(this);
            renderService->RemoveVisualChild(this);
        }
        else
        {
            renderService->RemoveTopLevelHandle(this);
        }
    }
}

void XUI::UI::PopupRoot::Hide()
{
    IsVisible = false;
    if (_windowImpl)
        _windowImpl->Hide();
}

void XUI::UI::PopupRoot::Show()
{
    if (_initCount == 0 && !_platformImplInitialized)
        InitializePlatformWindow();

    if (IsVisible)
        return;

    if (_initCount == 0)
        BeginInit();
    EndInit();
    IsVisible = true;

    auto renderService = Platform::UIService::Instance()->Get<Platform::UIRender>();
    renderService->ExecuteInitialLayoutPass(this);

    if (_windowImpl)
    {
        _windowImpl->Show();
        _windowImpl->SetActiveWindow();
    }
}

void XUI::UI::PopupRoot::OnGotFocus(Interfaces::GotFocusEventArgs & e)
{
    if (Parent && Parent->Is<Popup>())
        static_cast<Popup*>(Parent)->SetInternalStaysOpen(true);
}

void XUI::UI::PopupRoot::OnLostFocus(Core::Observer::RoutedEventArgs & e)
{
    if (Parent && Parent->Is<Popup>())
        static_cast<Popup*>(Parent)->SetInternalStaysOpen(false);
}

void PopupRoot::OnTemplateApplied(Controls::TemplateAppliedEventArgs const& e)
{
    TopLevel::OnTemplateApplied(e);

    if (Parent && Parent->TemplatedParent != nullptr)
    {
        if (_presenterSubscription.is_subscribed())
            _presenterSubscription.unsubscribe();

        if (Presenter)
        {
            Presenter->ApplyTemplate();
            auto observer = Presenter->GetObservable(Presenters::ContentPresenter::ChildProperty.get());
            _presenterSubscription = observer->subscribe([this, observer](System::Reflection::Variant& x)
            {
                SetTemplatedParentAndApplyChildTemplates(x.TryGetValue<Controls::Control*>());
            });
        }
    }
}

void XUI::UI::PopupRoot::InitializePlatformWindow()
{
    _platformImplInitialized = true;
    _parentWnd = nullptr;
    if (IsEmbed() && GetParent() && GetParent()->GetVisualRoot())
    {
        UI::TopLevel* toplevel = GetParent()->GetVisualRoot()->ToVisual()->Static_As<UI::TopLevel>();
        if (toplevel)
        {
            if (auto popupRoot = toplevel->Static_As<UI::PopupRoot>())
                _parentWnd = popupRoot->GetParentWnd();
            else
                _parentWnd = toplevel->GetWindowImpl();
        }
    }

    _windowImpl = XUI::Platform::UIService::Instance()->Get<Interfaces::IPlatformRenderInterface>()->CreatePopup(UI::WindowStyle::None, _parentWnd);
    TopLevel::InitializePlatformWindow();

    auto renderService = Platform::UIService::Instance()->Get<Platform::UIRender>();
    if (VisualParent == nullptr)
    {
        if (!_windowImpl /*&& Parent != renderService*/)
        {
            renderService->AddLogicalChild(this);
            renderService->AddVisualChild(this);
        }
        else
        {
            _visualRoot = this->shared_from_base_dynamic<Interfaces::IRenderRoot>();
            renderService->AddTopLevelHandle(this);
        }
    }

}

void PopupRoot::SetTemplatedParentAndApplyChildTemplates(Controls::Control *control)
{
    if (control)
    {
        auto templatedParent = Parent->TemplatedParent;

        if (control->TemplatedParent == nullptr)
            control->SetValue(TemplatedParentProperty, templatedParent);

        control->ApplyTemplate();

        if (!control->Is<Presenters::ContentPresenter>() && control->TemplatedParent == templatedParent)
        {
            for (auto const& child : control->GetVisualChildren())
                SetTemplatedParentAndApplyChildTemplates(child.get()->Static_As<Controls::Control>());
        }
    }
}
