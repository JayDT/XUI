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

#include "Window.h"
#include "Core/Input/Cursors.h"
#include "UI/Controls/WindowIcon.h"
#include "Platform/UIService.h"
#include "Platform/UIRenderer.h"

#include "UI/Button.h"
#include "Interfaces/ICommand.h"

#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;
using namespace XUI::UI;

STATIC_CLASS_INITIALIZER(Window, XUI::UI::Window::StaticClassInitializer)

XUI::Core::Dependency::RefDependencyProperty  XUI::UI::Window::SizeToContentProperty;
XUI::Core::Dependency::RefDependencyProperty  XUI::UI::Window::HasSystemDecorationsProperty;
XUI::Core::Dependency::RefDependencyProperty  XUI::UI::Window::CoverTaskbarOnMaximizeProperty;
XUI::Core::Dependency::RefDependencyProperty  XUI::UI::Window::TitleProperty;
XUI::Core::Dependency::RefDependencyProperty  XUI::UI::Window::IconProperty;
XUI::Core::Dependency::RefDependencyProperty  XUI::UI::Window::WindowStyleProperty;

Core::Observer::RefRoutedCommand XUI::UI::Window::MinimizeWindowCommand;
Core::Observer::RefRoutedCommand XUI::UI::Window::MaximizeWindowCommand;
Core::Observer::RefRoutedCommand XUI::UI::Window::CloseWindowCommand;
Core::Observer::RefRoutedCommand XUI::UI::Window::RestoreWindowCommand;

/// <summary>
/// Initializes static members of the <see cref="Window"/> class.
/// </summary>
void XUI::UI::Window::StaticClassInitializer()
{
    SizeToContentProperty               = XUI_DEPENDENCY_PROPERTY_REGISTER(XUI::UI::Window, XUI::UI::SizeToContent, nameof(SizeToContent));
    HasSystemDecorationsProperty        = XUI_DEPENDENCY_PROPERTY_REGISTER(XUI::UI::Window, bool, nameof(HasSystemDecorations));
    CoverTaskbarOnMaximizeProperty      = XUI_DEPENDENCY_PROPERTY_REGISTER(XUI::UI::Window, bool, nameof(CoverTaskbarOnMaximize));
    TitleProperty                       = XUI_DEPENDENCY_PROPERTY_REGISTER(XUI::UI::Window, std::wstring, nameof(Title));
    IconProperty                        = XUI_DEPENDENCY_PROPERTY_REGISTER(XUI::UI::Window, std::shared_ptr<WindowIcon>, nameof(Icon));
    WindowStyleProperty                 = (Core::Dependency::DirectProperty<Window, UI::WindowStyle>::RegisterDirect(nameof(WindowStyle),
        [](Window* x) { return x->WindowStyle; },
        [](Window* x, UI::WindowStyle v) { x->WindowStyle = v; },
        UI::WindowStyle::None)
    );

    MinimizeWindowCommand = (Core::Observer::RoutedCommand::Register<Window>(nameof(MinimizeWindow)));
    MaximizeWindowCommand = (Core::Observer::RoutedCommand::Register<Window>(nameof(MaximizeWindow)));
    CloseWindowCommand = (Core::Observer::RoutedCommand::Register<Window>(nameof(CloseWindow)));
    RestoreWindowCommand = (Core::Observer::RoutedCommand::Register<Window>(nameof(RestoreWindow)));

    MinimizeWindowCommand->AddClassHandler(&typeof(Window), std::make_shared<Core::Observer::CommandBinding>(MinimizeWindowCommand.get(), System::make_shared_eventhandler(Window::OnExecutedMinimizeWindow)));
    MaximizeWindowCommand->AddClassHandler(&typeof(Window), std::make_shared<Core::Observer::CommandBinding>(MaximizeWindowCommand.get(), System::make_shared_eventhandler(Window::OnExecutedMaximizeWindow)));
    CloseWindowCommand->AddClassHandler(&typeof(Window), std::make_shared<Core::Observer::CommandBinding>(CloseWindowCommand.get(), System::make_shared_eventhandler(Window::OnExecutedCloseWindow)));
    RestoreWindowCommand->AddClassHandler(&typeof(Window), std::make_shared<Core::Observer::CommandBinding>(RestoreWindowCommand.get(), System::make_shared_eventhandler(Window::OnExecutedRestoreWindow)));

    IsVisibleProperty->OverrideDefaultValue<Window>(false);
    HorizontalAlignmentProperty->OverrideDefaultValue<Window>(Controls::HorizontalAlignment::Left);
    VerticalAlignmentProperty->OverrideDefaultValue<Window>(Controls::VerticalAlignment::Top);

    IsVisibleProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const & e)
    {
        Window* control = e.Sender->Dynamic_As<Window>();
        if (control && control->GetPlatformImpl())
        {
            if (e.NewValue.GetValue<bool>())
                control->GetPlatformImpl()->Show();
            else
                control->GetPlatformImpl()->Hide();
        }
    });
}

XUI::UI::Window::Window()
{
    IsVisible = false;
    HorizontalAlignment = Controls::HorizontalAlignment::Left;
    VerticalAlignment = Controls::VerticalAlignment::Top;

    _isAttachedToLogicalTree = true;
    _isRootWindow = true;
}

XUI::UI::Window::~Window()
{
}

void XUI::UI::Window::InstanceInitializer()
{
    if (!Template)
    {
        auto _template = FindControlTemplate(System::Reflection::meta::Variant::Empty);
        if (!_template)
            _template = FindControlTemplate(&typeof(XUI::UI::Window));
        Template = _template;
    }
}

void XUI::UI::Window::InitializePlatformWindow()
{
    _platformImplInitialized = true;
    if (WindowStyle != UI::WindowStyle::GUIWindow)
    {
        _windowImpl = XUI::Platform::UIService::Instance()->Get<Interfaces::IPlatformRenderInterface>()->CreateWindow(WindowStyle);
        TopLevel::InitializePlatformWindow();
    }
    else
    {
        XUI::Platform::UIService::Instance()->Get<XUI::Platform::UIRender>()->InitializeInlineMainWindow();
        _isRootMode = false;
        _clientSize = Core::Media::Size(Width, Height);
    }

    OnWindowInitialized();
}

void XUI::UI::Window::EndInit()
{
    XUI::UI::TopLevel::EndInit();

    if (_initCount == 0)
    {
        if (!_platformImplInitialized)
            InitializePlatformWindow();
    }
}

XUI::UI::SizeToContent XUI::UI::Window::GetSizeToContent() const
{
    return GetValue<XUI::UI::SizeToContent>(SizeToContentProperty.get());
}

void XUI::UI::Window::SetSizeToContent(XUI::UI::SizeToContent value)
{
    SetValue(SizeToContentProperty.get(), value);
}

std::wstring XUI::UI::Window::GetTitle() const
{
    return GetValue<std::wstring>(TitleProperty.get());
}

void XUI::UI::Window::SetTitle(std::wstring value)
{
    SetValue(TitleProperty.get(), value);
}

bool XUI::UI::Window::GetHasSystemDecorations() const
{
    return GetValue<bool>(HasSystemDecorationsProperty.get());
}

void XUI::UI::Window::SetHasSystemDecorations(bool value)
{
    SetValue(HasSystemDecorationsProperty.get(), value);
}

bool XUI::UI::Window::GetCoverTaskbarOnMaximize() const
{
    return GetValue<bool>(CoverTaskbarOnMaximizeProperty.get());
}

void XUI::UI::Window::SetCoverTaskbarOnMaximize(bool value)
{
    SetValue(CoverTaskbarOnMaximizeProperty.get(), value);
}

XUI::UI::WindowState XUI::UI::Window::GetWindowState() const
{
    return _windowState;
}

void XUI::UI::Window::SetWindowState(XUI::UI::WindowState value)
{
    _windowState = value;
}

std::shared_ptr<WindowIcon> XUI::UI::Window::GetIcon() const
{
    return GetValue<std::shared_ptr<WindowIcon>>(IconProperty.get());
}

void XUI::UI::Window::SetIcon(std::shared_ptr<WindowIcon> value)
{
    SetValue(IconProperty.get(), value);
}

Core::Media::Size XUI::UI::Window::GetMaxClientSize() const
{
    return _maxPlatformClientSize;
}

void XUI::UI::Window::SetMaxClientSize(Core::Media::Size value)
{
    _maxPlatformClientSize = value;
}

System::Reflection::Type const * XUI::UI::Window::GetStyleKey() const
{
    return &typeof(Window);
}

void XUI::UI::Window::SetStyleKey(System::Reflection::Type const * value)
{
}

double XUI::UI::Window::GetLeft() const
{
    if (_windowImpl)
        return _windowImpl->GetPosition().x;
    return Margin.Left;
}

void XUI::UI::Window::SetLeft(double value)
{
    if (_windowImpl)
    {
        auto pos = _windowImpl->GetPosition();
        pos.x += value;
        _windowImpl->SetPosition(pos);
    }
    else
    {
        Core::Media::Thickness margin = Margin;
        margin.Left = value;
        Margin = margin;
        InvalidateArrange();
    }
}

double XUI::UI::Window::GetTop() const
{
    if (_windowImpl)
        return _windowImpl->GetPosition().y;
    return Margin.Top;
}

void XUI::UI::Window::SetTop(double value)
{
    if (_windowImpl)
    {
        auto pos = _windowImpl->GetPosition();
        pos.y += value;
        _windowImpl->SetPosition(pos);
    }
    else
    {
        Core::Media::Thickness margin = Margin;
        margin.Top = value;
        Margin = margin;
        InvalidateArrange();
    }
}

UI::WindowStyle XUI::UI::Window::GetWindowStyle() const
{
    return m_windowStyle;
}

void XUI::UI::Window::SetWindowStyle(UI::WindowStyle value)
{
    if (SetAndRaise(WindowStyleProperty, m_windowStyle, value) && value == UI::WindowStyle::GUIWindow)
    {
        _isRootWindow = false;
        _isRootMode = false;
        _isAttachedToLogicalTree = false; // remove constructor default value
        // this gui window workaround
        Platform::UIRender* renderRoot = Platform::UIService::Instance()->Get<Platform::UIRender>();
        _visualRoot = renderRoot->weak_from_base_dynamic<Interfaces::IRenderRoot>();
        assert(!_visualRoot.expired());
        renderRoot->AddLogicalChild(this);
    }
}

void XUI::UI::Window::Close()
{
    auto renderService = Platform::UIService::Instance()->Get<Platform::UIRender>();
    //if (VisualParent == nullptr)
    //{
    //    if (!_windowImpl /*&& Parent != renderService*/)
    //    {
    //        renderService->RemoveLogicalChild(this);
    //        renderService->RemoveVisualChild(this);
    //    }
    //    else
    //    {
    //        renderService->RemoveTopLevelHandle(this);
    //    }
    //}

    renderService->RemoveLogicalChild(this);
    renderService->RemoveVisualChild(this);
    renderService->RemoveTopLevelHandle(this);

    TopLevel::Close();
}

void XUI::UI::Window::Hide()
{
    IsVisible = false;
    if (_windowImpl)
        _windowImpl->Hide();
}

void XUI::UI::Window::Show()
{
    if (IsVisible)
        return;

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

    EnsureInitialized();
    IsVisible = true;

    renderService->ExecuteInitialLayoutPass(this);

    if (_windowImpl)
        _windowImpl->Show();
}

void XUI::UI::Window::DragMove()
{
    _inDragMove = true;
    Interfaces::IMouseDevice* mouseDevice = Platform::UIService::Instance()->Get<Interfaces::IMouseDevice>();
    _lastMousePosition = mouseDevice->GetRelativePosition(VisualRoot->ToVisual());
    mouseDevice->Capture(this);
}

void XUI::UI::Window::__OnTitle_Click(void * sender, XUI::Core::Observer::RoutedEventArgs& e)
{
    Interfaces::PointerPressedEventArgs* pressArg = e.As<Interfaces::PointerPressedEventArgs>();
    if (pressArg)
    {
        DragMove();
    }
    else
    {
        Interfaces::PointerReleasedEventArgs* releasedArg = e.As<Interfaces::PointerReleasedEventArgs>();
        if (releasedArg)
        {
            _inDragMove = false;
            auto mouseDevice = XUI::Platform::UIService::Instance()->Get<Interfaces::IMouseDevice>();
            mouseDevice->Capture(nullptr);
        }
    }
}

void XUI::UI::Window::__OnTitle_PointerLeave(void * sender, XUI::Core::Observer::RoutedEventArgs& e)
{
    //_inDragMove = false;
}

void XUI::UI::Window::OnTemplateApplied(Controls::TemplateAppliedEventArgs const & e)
{
    auto const& _template = Template;
    if (_template && (WindowStyle == UI::WindowStyle::None || WindowStyle == UI::WindowStyle::GUIWindow))
    {
        BeginInit();
        _titleBar = _template->FindNameByManagedType<Controls::Control>("PART_TITLEBAR", this);
        if (_titleBar)
        {
            _titleBar->Focusable = true;

            //if (control->Parent)
            //    control->Parent->RemoveLogicalChild(control);

            _titleBar->AddHandler(Controls::InputElement::PointerPressedEvent.get(), System::make_eventhandler(&XUI::UI::Window::__OnTitle_Click, shared_from_base_static<XUI::UI::Window>()));
            _titleBar->AddHandler(Controls::InputElement::PointerReleasedEvent.get(), System::make_eventhandler(&XUI::UI::Window::__OnTitle_Click, shared_from_base_static<XUI::UI::Window>()));
            _titleBar->AddHandler(Controls::InputElement::PointerLeaveEvent.get(), System::make_eventhandler(&XUI::UI::Window::__OnTitle_PointerLeave, shared_from_base_static<XUI::UI::Window>()));

            //AddLogicalChild(control);
        }
        auto bt_close = _template->FindName("PART_CLOSE", this);
        if (bt_close)
        {
            auto control = bt_close->Static_As<Controls::Control>();
            control->As<Button>()->Command = Window::CloseWindowCommand;
            control->As<Button>()->CommandParameter = this;
        }
        auto bt_maximalize = _template->FindName("PART_MAXIMIZE_RESTORE", this);
        if (bt_maximalize)
        {
            auto control = bt_maximalize->Static_As<Controls::Control>();
            control->As<Button>()->Command = Window::MaximizeWindowCommand;
            control->As<Button>()->CommandParameter = this;
        }
        auto bt_minimalize = _template->FindName("PART_MINIMIZE", this);
        if (bt_minimalize)
        {
            auto control = bt_minimalize->Static_As<Controls::Control>();
            control->As<Button>()->Command = Window::MinimizeWindowCommand;
            control->As<Button>()->CommandParameter = this;
        }
        EndInit();
    }
}

void XUI::UI::Window::OnExecutedMinimizeWindow(void * sender, Core::Observer::ExecutedRoutedEventArgs & e)
{
    //GENERIC_SAFE_AS(Window, sender)->OnExecutedBeginEdit(e);
}

void XUI::UI::Window::OnExecutedMaximizeWindow(void * sender, Core::Observer::ExecutedRoutedEventArgs & e)
{
}

void XUI::UI::Window::OnExecutedCloseWindow(void * sender, Core::Observer::ExecutedRoutedEventArgs & e)
{
}

void XUI::UI::Window::OnExecutedRestoreWindow(void * sender, Core::Observer::ExecutedRoutedEventArgs & e)
{
}

void XUI::UI::Window::OnWindowInitialized()
{
}

void XUI::UI::Window::HandlePaint(Core::Media::Rect const & rect)
{
    //Renderer->Paint(rect);
}

void XUI::UI::Window::HandleClosed()
{
    _windowImpl = nullptr;
    TopLevel::HandleClosed();
}

void XUI::UI::Window::OnPointerMoved(Interfaces::PointerEventArgs& e)
{
    TopLevel::OnPointerMoved(e);

    if (_inDragMove)
    {
        Interfaces::PointerEventArgs* moveArg = e.As<Interfaces::PointerEventArgs>();
        Core::Media::Point pointerPos = moveArg->GetPosition(VisualRoot->ToVisual());
        Core::Media::Point delta = (pointerPos - _lastMousePosition);

        if (_windowImpl)
        {
            auto pos = _windowImpl->GetPosition();
            pos.x += delta.x;
            pos.y += delta.y;
            _windowImpl->SetPosition(pos);
            _lastMousePosition = moveArg->GetPosition(VisualRoot->ToVisual());
        }
        else
        {
            Core::Media::Thickness pos = Margin;
            pos.Left += delta.x;
            pos.Top += delta.y;
            Margin = pos;
            InvalidateArrange();
            _lastMousePosition = pointerPos;
        }
    }
    else if (_inResizing && (!_windowImpl || WindowStyle == UI::WindowStyle::None))
    {
        Interfaces::PointerEventArgs& moveArg = (Interfaces::PointerEventArgs&)e;
        Core::Media::Point pointerPos = moveArg.GetPosition(VisualRoot->ToVisual());

        if (_windowImpl)
        {
            float left = Left;
            float top = Top;

            if (HasActionModeFlags(EGUI_ACTION_MODE_FLAGS::EAMF_RESIZE_T))
            {
                Height = Height - pointerPos.y;
                top = top + pointerPos.y;
            }
            if (HasActionModeFlags(EGUI_ACTION_MODE_FLAGS::EAMF_RESIZE_B))
            {
                Height = pointerPos.y;
            }
            if (HasActionModeFlags(EGUI_ACTION_MODE_FLAGS::EAMF_RESIZE_L))
            {
                Width = Width - pointerPos.x;
                left = left + pointerPos.x;
            }
            if (HasActionModeFlags(EGUI_ACTION_MODE_FLAGS::EAMF_RESIZE_R))
            {
                Width = pointerPos.x;
            }

            _windowImpl->SetPosition(Core::Media::Point(left, top));
            _windowImpl->Resize(Core::Media::Size(Width, Height));
        }
        else
        {
            Core::Media::Thickness pos = Margin;

            if (HasActionModeFlags(EGUI_ACTION_MODE_FLAGS::EAMF_RESIZE_T))
            {
                Height = Height + (pos.Top - pointerPos.y);
                pos.Top = pointerPos.y;
            }
            if (HasActionModeFlags(EGUI_ACTION_MODE_FLAGS::EAMF_RESIZE_B))
            {
                Height = pointerPos.y - Top;
            }
            if (HasActionModeFlags(EGUI_ACTION_MODE_FLAGS::EAMF_RESIZE_L))
            {
                Width = Width + (pos.Left - pointerPos.x);
                pos.Left = pointerPos.x;
            }
            if (HasActionModeFlags(EGUI_ACTION_MODE_FLAGS::EAMF_RESIZE_R))
            {
                Width = pointerPos.x - Left;
            }

            Margin = pos;
            InvalidateMeasure();
        }
    }
    else if (!_isMinialized && (!_windowImpl || WindowStyle == UI::WindowStyle::None))
    {
        Interfaces::PointerEventArgs& moveArg = (Interfaces::PointerEventArgs&)e;
        Core::Media::Point pointerPos = moveArg.GetPosition(VisualRoot->ToVisual());

        float _left = _windowImpl ? 0 : Left;
        float _top = _windowImpl ? 0 : Top;
        float top = _top - pointerPos.y;
        float bottom = _top + Height - pointerPos.y;
        float left = _left - pointerPos.x;
        float right = _left + Width - pointerPos.x;
        float maxdelta = 4;

        if (top < maxdelta && top > -maxdelta && right < maxdelta && right > -maxdelta)
        {
            if (!HasActionModeFlags(EGUI_ACTION_MODE_FLAGS::EAMF_CAN_RESIZE_TR))
            {
                SetValue(CursorProperty.get(), std::make_shared<Core::Input::Cursor>(Core::Input::StandardCursorType::TopRightCorner), Core::Dependency::BindingLevel::TemplatedParent);
                UpdateCursor();
                m_actionModeFlags |= EGUI_ACTION_MODE_FLAGS::EAMF_CAN_RESIZE_TR;
            }
        }
        else if (top < maxdelta && top > -maxdelta && left < maxdelta && left > -maxdelta)
        {
            if (!HasActionModeFlags(EGUI_ACTION_MODE_FLAGS::EAMF_CAN_RESIZE_TL))
            {
                SetValue(CursorProperty.get(), std::make_shared<Core::Input::Cursor>(Core::Input::StandardCursorType::TopLeftCorner), Core::Dependency::BindingLevel::TemplatedParent);
                UpdateCursor();
                m_actionModeFlags |= EGUI_ACTION_MODE_FLAGS::EAMF_CAN_RESIZE_TL;
            }
        }
        else if (bottom < maxdelta && bottom > -maxdelta && right < maxdelta && right > -maxdelta)
        {
            if (!HasActionModeFlags(EGUI_ACTION_MODE_FLAGS::EAMF_CAN_RESIZE_BR))
            {
                SetValue(CursorProperty.get(), std::make_shared<Core::Input::Cursor>(Core::Input::StandardCursorType::BottomRightCorner), Core::Dependency::BindingLevel::TemplatedParent);
                UpdateCursor();
                m_actionModeFlags |= EGUI_ACTION_MODE_FLAGS::EAMF_CAN_RESIZE_BR;
            }
        }
        else if (bottom < maxdelta && bottom > -maxdelta && left < maxdelta && left > -maxdelta)
        {
            if (!HasActionModeFlags(EGUI_ACTION_MODE_FLAGS::EAMF_CAN_RESIZE_BL))
            {
                SetValue(CursorProperty.get(), std::make_shared<Core::Input::Cursor>(Core::Input::StandardCursorType::BottomLeftCorner), Core::Dependency::BindingLevel::TemplatedParent);
                UpdateCursor();
                m_actionModeFlags |= EGUI_ACTION_MODE_FLAGS::EAMF_CAN_RESIZE_BL;
            }
        }
        else if (top < maxdelta && top > -maxdelta)
        {
            if (!HasActionModeFlags(EGUI_ACTION_MODE_FLAGS::EAMF_CAN_RESIZE_T))
            {
                SetValue(CursorProperty.get(), std::make_shared<Core::Input::Cursor>(Core::Input::StandardCursorType::SizeNorthSouth), Core::Dependency::BindingLevel::TemplatedParent);
                UpdateCursor();
                m_actionModeFlags |= EGUI_ACTION_MODE_FLAGS::EAMF_CAN_RESIZE_T;
            }
        }
        else if (bottom < maxdelta && bottom > -maxdelta)
        {
            if (!HasActionModeFlags(EGUI_ACTION_MODE_FLAGS::EAMF_CAN_RESIZE_B))
            {
                SetValue(CursorProperty.get(), std::make_shared<Core::Input::Cursor>(Core::Input::StandardCursorType::SizeNorthSouth), Core::Dependency::BindingLevel::TemplatedParent);
                UpdateCursor();
                m_actionModeFlags |= EGUI_ACTION_MODE_FLAGS::EAMF_CAN_RESIZE_B;
            }
        }
        else if (left < maxdelta && left > -maxdelta)
        {
            if (!HasActionModeFlags(EGUI_ACTION_MODE_FLAGS::EAMF_CAN_RESIZE_L))
            {
                SetValue(CursorProperty.get(), std::make_shared<Core::Input::Cursor>(Core::Input::StandardCursorType::SizeWestEast), Core::Dependency::BindingLevel::TemplatedParent);
                UpdateCursor();
                m_actionModeFlags |= EGUI_ACTION_MODE_FLAGS::EAMF_CAN_RESIZE_L;
            }
        }
        else if (right < maxdelta && right > -maxdelta)
        {
            if (!HasActionModeFlags(EGUI_ACTION_MODE_FLAGS::EAMF_CAN_RESIZE_R))
            {
                SetValue(CursorProperty.get(), std::make_shared<Core::Input::Cursor>(Core::Input::StandardCursorType::SizeWestEast), Core::Dependency::BindingLevel::TemplatedParent);
                UpdateCursor();
                m_actionModeFlags |= EGUI_ACTION_MODE_FLAGS::EAMF_CAN_RESIZE_R;
            }
        }
        else if (HasActionModeFlags(EGUI_ACTION_MODE_FLAGS::EAMF_CAN_RESIZE_ALL))
        {
            SetValue(CursorProperty.get(), Core::Dependency::DependencyPropertyMgr::UnsetValue, Core::Dependency::BindingLevel::TemplatedParent);
            UpdateCursor();
            m_actionModeFlags &= ~EGUI_ACTION_MODE_FLAGS::EAMF_CAN_RESIZE_ALL;
        }
    }
}

void XUI::UI::Window::OnPointerPressed(Interfaces::PointerPressedEventArgs& e)
{
    TopLevel::OnPointerPressed(e);

    if (HasActionModeFlags(EGUI_ACTION_MODE_FLAGS::EAMF_CAN_RESIZE_ALL))
    {
        e.Handled = true;
        _inDragMove = false;
        _inResizing = true;

        if (HasActionModeFlags(EGUI_ACTION_MODE_FLAGS::EAMF_CAN_RESIZE_T) || HasActionModeFlags(EGUI_ACTION_MODE_FLAGS::EAMF_CAN_RESIZE_TR) || HasActionModeFlags(EGUI_ACTION_MODE_FLAGS::EAMF_CAN_RESIZE_TL))
        {
            m_actionModeFlags |= EGUI_ACTION_MODE_FLAGS::EAMF_RESIZE_T;
        }
        if (HasActionModeFlags(EGUI_ACTION_MODE_FLAGS::EAMF_CAN_RESIZE_B) || HasActionModeFlags(EGUI_ACTION_MODE_FLAGS::EAMF_CAN_RESIZE_BR) || HasActionModeFlags(EGUI_ACTION_MODE_FLAGS::EAMF_CAN_RESIZE_BL))
        {
            m_actionModeFlags |= EGUI_ACTION_MODE_FLAGS::EAMF_RESIZE_B;
        }
        if (HasActionModeFlags(EGUI_ACTION_MODE_FLAGS::EAMF_CAN_RESIZE_L) || HasActionModeFlags(EGUI_ACTION_MODE_FLAGS::EAMF_CAN_RESIZE_TL) || HasActionModeFlags(EGUI_ACTION_MODE_FLAGS::EAMF_CAN_RESIZE_BL))
        {
            m_actionModeFlags |= EGUI_ACTION_MODE_FLAGS::EAMF_RESIZE_L;
        }
        if (HasActionModeFlags(EGUI_ACTION_MODE_FLAGS::EAMF_CAN_RESIZE_R) || HasActionModeFlags(EGUI_ACTION_MODE_FLAGS::EAMF_CAN_RESIZE_TR) || HasActionModeFlags(EGUI_ACTION_MODE_FLAGS::EAMF_CAN_RESIZE_BR))
        {
            m_actionModeFlags |= EGUI_ACTION_MODE_FLAGS::EAMF_RESIZE_R;
        }

        auto mouseDevice = XUI::Platform::UIService::Instance()->Get<Interfaces::IMouseDevice>();
        mouseDevice->Capture(this);
    }
}

void XUI::UI::Window::OnPointerReleased(Interfaces::PointerReleasedEventArgs& e)
{
    TopLevel::OnPointerReleased(e);

    if (_inDragMove || _inResizing)
    {
        m_actionModeFlags &= ~EGUI_ACTION_MODE_FLAGS::EAMF_RESIZE_ALL;
        _inDragMove = false;
        _inResizing = false;

        auto mouseDevice = XUI::Platform::UIService::Instance()->Get<Interfaces::IMouseDevice>();
        mouseDevice->Capture(nullptr);
    }
}

void XUI::UI::Window::OnPreviewPointerPressed(Interfaces::PointerPressedEventArgs & e)
{
    if (_windowImpl && e.Source)
    {
        auto renderService = Platform::UIService::Instance()->Get<Platform::UIRender>();
        auto& ev = (Interfaces::PointerPressedEventArgs&)e;
        UI::Controls::InputElement* element = ev.Device ? ev.Device->Captured : nullptr;
        if (!element)
            element = e.Source;

        if (element && !renderService->CanFocus(element))
            element = nullptr;

        if (element == nullptr || !renderService->CanFocus(element))
        {
            UI::Controls::Control* control = e.Source->Static_As<UI::Controls::Control>();
            while (control != nullptr)
            {
                if (control && renderService->CanFocus(control))
                    break;
                control = control->Parent;
            }

            if (control && renderService->CanFocus(control))
                element = control;
        }

        if (element != nullptr && element != renderService->GetFocusControl().lock().get())
        {
            renderService->Focus(element, Interfaces::NavigationMethod::Pointer, ev.InputModifiers);
            if (_windowImpl)
                _windowImpl->Focus();
        }
    }
}

/// <summary>
/// Handles a deactivated notification from <see cref="IWindowBaseImpl.Deactivated"/>.
/// </summary>
void XUI::UI::Window::HandleDeactivated()
{
    _isActive = false;

    if (Deactivated)
    {
        System::Events::EventArg e;
        Deactivated(static_cast<System::Reflection::Object*>(this), e);
    }
}

Core::Media::Size XUI::UI::Window::MeasureOverride(Core::Media::Size const & availableSize)
{
    auto sizeToContent = SizeToContent;
    if (!_windowImpl)
        _clientSize = Core::Media::Size(Width, Height);
    auto size = ClientSize;
    auto desired = TopLevel::MeasureOverride(availableSize.Constrain(ClientSize));

    switch (sizeToContent)
    {
        case SizeToContent::Width:
        size = Core::Media::Size(desired.Width, ClientSize.Height);
        break;
        case SizeToContent::Height:
        size = Core::Media::Size(ClientSize.Width, desired.Height);
        break;
        case SizeToContent::WidthAndHeight:
        size = Core::Media::Size(desired.Width, desired.Height);
        break;
        case SizeToContent::Manual:
        size = ClientSize;
        break;
        default:
        throw System::InvalidOperationException("Invalid value for SizeToContent.");
    }

    return size;
}

void XUI::UI::Window::HandleResized(Core::Media::Size const & clientSize)
{
    Width = clientSize.Width;
    Height = clientSize.Height;
    _clientSize = Core::Media::Size(clientSize.Width, clientSize.Height);

    InvalidateMeasure();
}

/// <summary>
/// Handles input from <see cref="ITopLevelImpl.Input"/>.
/// </summary>
/// <param name="e">The event args.</param>
void XUI::UI::Window::HandleInput(Interfaces::RawInputEventArgs * e)
{
    TopLevel::HandleInput(e);
}

/// <summary>
/// Handles a window scaling change notification from 
/// <see cref="ITopLevelImpl.ScalingChanged"/>.
/// </summary>
/// <param name="scaling">The window scaling.</param>
void XUI::UI::Window::HandleScalingChanged(double scaling)
{
    InvalidateMeasure();
}

/// <summary>
/// Handles a window position change notification from 
/// <see cref="IWindowBaseImpl.PositionChanged"/>.
/// </summary>
/// <param name="pos">The window position.</param>
void XUI::UI::Window::HandlePositionChanged(Core::Media::Point const & pos)
{
    if (PositionChanged)
    {
        XUI::UI::PointEventArgs e;
        e.Point = pos;
        PositionChanged(static_cast<System::Reflection::Object*>(this), e);
    }
}

/// <summary>
/// Handles an activated notification from <see cref="IWindowBaseImpl.Activated"/>.
/// </summary>
void XUI::UI::Window::HandleActivated()
{
    if (Activated)
    {
        System::Events::EventArg e;
        Activated(static_cast<System::Reflection::Object*>(this), e);
    }

    _isActive = true;
}

void XUI::UI::Window::EnsureInitialized()
{
    if (!IsInitialized)
    {
        BeginInit();
        EndInit();
    }
}

