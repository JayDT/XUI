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

#include "TopLevel.h"
#include "Window.h"
#include "Platform/UIService.h"
#include "Platform/UIRenderer.h"

#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;
using namespace XUI::UI;

STATIC_CLASS_INITIALIZER(TopLevel, XUI::UI::TopLevel::StaticClassInitializer)

Core::Dependency::RefDependencyProperty     XUI::UI::TopLevel::ClientSizeProperty;
Core::Dependency::RefDependencyProperty     XUI::UI::TopLevel::IsActiveProperty;
Core::Dependency::RefDependencyProperty     XUI::UI::TopLevel::PointerOverElementProperty;

void XUI::UI::TopLevel::StaticClassInitializer()
{
    ClientSizeProperty = XUI_DIRECT_PROPERTY_REGISTER(TopLevel, Core::Media::Size, nameof(ClientSize), IDENTITY_VA([](XUI::UI::TopLevel* x) { return x->ClientSize; }), nullptr);
    IsActiveProperty = XUI_DIRECT_PROPERTY_REGISTER(TopLevel, bool, nameof(IsActive), IDENTITY_VA([](XUI::UI::TopLevel* x) { return x->IsActive; }), nullptr);
    PointerOverElementProperty = XUI_DEPENDENCY_PROPERTY_REGISTER(XUI::UI::TopLevel, std::weak_ptr<XUI::UI::Controls::InputElement>, nameof(PointerOverElement));
}

void XUI::UI::TopLevel::InstanceInitializer()
{
}

XUI::UI::TopLevel::TopLevel()
{
}

void XUI::UI::TopLevel::SetClientSize(Core::Media::Size const & value)
{
    SetAndRaise(ClientSizeProperty.get(), _clientSize, value);
}

void XUI::UI::TopLevel::SetIsActive(bool value)
{
    SetAndRaise(IsActiveProperty.get(), _isActive, value);
}

Core::Media::Point XUI::UI::TopLevel::GetPosition() const
{
    return _position;
}

void XUI::UI::TopLevel::SetPosition(Core::Media::Point const & value)
{
    _position = value;
    if (_windowImpl)
        _windowImpl->SetPosition(value);
}

std::weak_ptr<Controls::InputElement> XUI::UI::TopLevel::GetPointerOverElement() const
{
    return GetValue<std::weak_ptr<Controls::InputElement>>(PointerOverElementProperty.get());
}

void XUI::UI::TopLevel::SetPointerOverElement(std::weak_ptr<InputElement> const & value)
{
    SetValue(PointerOverElementProperty.get(), value);
}

float XUI::UI::TopLevel::GetLayoutScaling() const
{
    return _layoutScaling;
}

void XUI::UI::TopLevel::InitializePlatformWindow()
{
    auto _background = Background.get();
    Core::Media::Size size(Width, Height);
    if (size.Width == -1)
        size.Width = 0;
    if (size.Height == -1)
        size.Height = 0;

    _windowImpl->Resize(size);
    _windowImpl->SetPosition(Position);
    _clientSize = _windowImpl->GetClientSize();
    _windowImpl->SetInputRoot(static_cast<Interfaces::UIControls::IInputRoot *>(this));
    _windowImpl->SetBackgroundColor(Core::Media::Color(0xFF, 0xFF, 0xFF, _background ? 0xFF * Background->GetOpacity() : 0xFF));

    InitializeWindowHooks();
}

void XUI::UI::TopLevel::InitializeWindowHooks()
{
    if (!GetWindowImpl())
        return;

    GetWindowImpl()->SetPaint(std::bind(&TopLevel::HandlePaint, this, std::placeholders::_1));
    GetWindowImpl()->SetClosed(std::bind(&TopLevel::HandleClosed, this));
    GetWindowImpl()->SetResized(std::bind(&TopLevel::HandleResized, this, std::placeholders::_1));
    GetWindowImpl()->SetInput(std::bind(&TopLevel::HandleInput, this, std::placeholders::_1));
    GetWindowImpl()->SetScalingChanged(std::bind(&TopLevel::HandleScalingChanged, this, std::placeholders::_1));
    GetWindowImpl()->SetPositionChanged(std::bind(&TopLevel::HandlePositionChanged, this, std::placeholders::_1));
    GetWindowImpl()->SetActivated(std::bind(&TopLevel::HandleActivated, this));
    GetWindowImpl()->SetDeactivated(std::bind(&TopLevel::HandleDeactivated, this));
}

void XUI::UI::TopLevel::Activate()
{
    if (_windowImpl)
        _windowImpl->Activate();
    //else
    //    this->BringIntoView();
}

void XUI::UI::TopLevel::SetOnAutoSizing(bool on)
{
    m_AutoSizing = on;
}

Core::Media::Size XUI::UI::TopLevel::ArrangeOverride(Core::Media::Size const & finalSize)
{
    SetOnAutoSizing(true);
    Core::Media::Size size = ContentControl::ArrangeOverride(finalSize);
    SetOnAutoSizing(false);

    return size;
}

void XUI::UI::TopLevel::HandleResized(Core::Media::Size const & clientSize)
{
    if (!AutoSizing)
    {
        Width = clientSize.Width;
        Height = clientSize.Height;
    }

    ClientSize = clientSize;
    //XUI::Platform::UIRender* renderMgr = XUI::Platform::UIService::Instance()->Get<XUI::Platform::UIRender>();
    //if (renderMgr)
    //    renderMgr->Invalidate(Core::Media::Rect(ClientSize));
}

void XUI::UI::TopLevel::HandleInput(Interfaces::RawInputEventArgs * e)
{
    XUI::Platform::UIService::Instance()->GetCache<XUI::Interfaces::IInputMgr>()->ProcessInput(*e);
}

void XUI::UI::TopLevel::HandleScalingChanged(double scaling)
{
    std::list<Visual*> visuals;
    GetVisualDescendents(visuals);

    for (Visual* visual : visuals)
    {
        visual->InvalidateMeasure();
    }
}

void XUI::UI::TopLevel::HandleActivated()
{
    if (Activated)
    {
        System::Events::EventArg e;
        Activated(this, e);
    }

    IsActive = true;
}

void XUI::UI::TopLevel::HandleClosed()
{
    if (Closed)
    {
        System::Events::EventArg e;
        Closed(this, e);
    }
}

void XUI::UI::TopLevel::OnApplicationExiting(void * sender, System::Events::EventArg & args)
{
}

void XUI::UI::TopLevel::HandleApplicationExiting()
{
}

void XUI::UI::TopLevel::HandleDeactivated()
{
    IsActive = false;

    if (Deactivated)
    {
        System::Events::EventArg e;
        Deactivated(this, e);
    }
}

void XUI::UI::TopLevel::HandlePositionChanged(Core::Media::Point const & pos)
{
    if (PositionChanged)
    {
        PointEventArgs e(pos);
        PositionChanged(this, e);
    }
}

void XUI::UI::TopLevel::BeginMoveDrag()
{
}

void XUI::UI::TopLevel::BeginResizeDrag(WindowEdge edge)
{
}

void XUI::UI::TopLevel::SetWindowDevice(std::shared_ptr<Interfaces::IWindowImpl> const & wnd)
{
    InitializeWindowHooks();
}

XUI::UI::Controls::Visual * XUI::UI::TopLevel::ToVisualBase() const
{
    return const_cast<XUI::UI::TopLevel*>(this);
}

Core::Media::Point XUI::UI::TopLevel::PointToClient(Core::Media::Point const& point)
{
    if (_windowImpl)
        return _windowImpl->PointToClient(point);
    return XUI::Platform::UIService::Instance()->GetCache<Platform::UIRender>()->PointToClient(point);
}

Core::Media::Point XUI::UI::TopLevel::PointToScreen(Core::Media::Point const& point)
{
    if (_windowImpl)
        return _windowImpl->PointToScreen(point);
    return XUI::Platform::UIService::Instance()->GetCache<Platform::UIRender>()->PointToScreen(point);
}

void XUI::UI::TopLevel::Render(Interfaces::IDrawingContextImpl * context)
{
    Controls::ContentControl::Render(context);
}

void XUI::UI::TopLevel::AddDirty(UI::Controls::Visual * context)
{
    XUI::Platform::UIService::Instance()->GetCache<Platform::UIRender>()->AddDirty(context);
}

void XUI::UI::TopLevel::Invalidate(Core::Media::Rect const & rect)
{
    XUI::Platform::UIService::Instance()->GetCache<Platform::UIRender>()->Invalidate(rect);
}

UI::Controls::Control * XUI::UI::TopLevel::HitTest(Core::Media::Point const & point, std::function<bool(UI::Controls::Control*)> const & filter)
{
    return XUI::Platform::UIService::Instance()->GetCache<Platform::UIRender>()->HitTest(point, this, filter);
}

UI::Controls::Control * XUI::UI::TopLevel::HitTest(Core::Media::Point const & point, UI::Controls::Control * root, std::function<bool(UI::Controls::Control*)> const & filter)
{
    return XUI::Platform::UIService::Instance()->GetCache<Platform::UIRender>()->HitTest(point, root, filter);
}

UI::Controls::Visual * XUI::UI::TopLevel::HitTestVisualTree(Core::Media::Point const & point, UI::Controls::Visual * root, std::function<bool(UI::Controls::Visual*)> const & filter)
{
    return XUI::Platform::UIService::Instance()->GetCache<Platform::UIRender>()->HitTestVisualTree(point, root, filter);
}

void XUI::UI::TopLevel::HitTestChain(std::vector<UI::Controls::Visual*>& result, Core::Media::Point const & point, UI::Controls::Visual * root, std::function<bool(UI::Controls::Visual*)> const & filter)
{
    XUI::Platform::UIService::Instance()->GetCache<Platform::UIRender>()->HitTestChain(result, point, root, filter);
}
