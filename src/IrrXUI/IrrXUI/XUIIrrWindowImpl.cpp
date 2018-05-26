#include "XUIIrrWindowImpl.h"

#include "XUI/Platform/UIService.h"
#include "XUI/Interfaces/IInputDevices.h"
#include "XUI/Core/Media/Pen.h"
#include "XUI/Core/Media/Brush.h"
#include "XUI/Core/Input/KeyboardDevice.h"
#include "XUI/Core/Input/MouseDevice.h"
#include "XUI/Core/Input/Cursors.h"
#include "XUI/UI/Window.h"
#include "IrrXUI/XUIIrrRenderer.h"

#include "IrrLicht/include/irrlicht.h"
#include "IrrLicht/CIrrDeviceStub.h"
#include "IrrLicht/CImage.h"

using namespace XUI;
using namespace XUI::Interfaces;

IrrBackend::IrrWindowImpl::IrrWindowImpl(irr::IrrlichtDevice * contextDevice, irr::video::IVideoDriver * contextDriver)
    : m_contextDevice(contextDevice)
    , m_contextDriver(contextDriver)
    , m_drawingContext(contextDevice)
{
    m_contextDevice->setEventReceiver(static_cast<irr::IEventReceiver*>(this));

    m_handle.Handle = m_contextDevice->getHandle();
    m_handle.HandleDescriptor = "IrrXUI Window native handle";
}

IrrBackend::IrrWindowImpl::~IrrWindowImpl()
{
    auto serviceUI = static_cast<IrrBackend::XUIIrrPlatformDriver*>(XUI::Platform::UIService::Instance()->Get<XUI::Platform::UIRender>());
    if (serviceUI)
        serviceUI->RemoveActiveWindow(this);
}

Core::Media::Size IrrBackend::IrrWindowImpl::GetClientSize() const
{
    auto windowSize = ((irr::CIrrDeviceStub*)m_contextDevice)->getClientSize();
    return Core::Media::Size(windowSize.Width, windowSize.Height);
}

void IrrBackend::IrrWindowImpl::SetBackgroundColor(XUI::Core::Media::Color const& color)
{
    static_cast<irr::CIrrDeviceStub*>(m_contextDevice)->setWindowBackgroundLayer(irr::video::SColor(color.ToUInt32()));
}

double IrrBackend::IrrWindowImpl::GetScaling() const
{
    return 1.0;
}

std::vector<System::Reflection::RefObject> IrrBackend::IrrWindowImpl::GetSurfaces() const
{
    return std::vector<System::Reflection::RefObject>();
}

std::function<void(RawInputEventArgs*)> IrrBackend::IrrWindowImpl::GetInput() const
{
    return m_inputFn;
}

void IrrBackend::IrrWindowImpl::SetInput(const std::function<void(RawInputEventArgs*)>& value)
{
    m_inputFn = value;
}

std::function<void(Core::Media::Rect)> IrrBackend::IrrWindowImpl::GetPaint() const
{
    return m_paintFn;
}

void IrrBackend::IrrWindowImpl::SetPaint(const std::function<void(Core::Media::Rect)>& value)
{
    m_paintFn = value;
    ((irr::CIrrDeviceStub*)m_contextDevice)->OnPaintFn = [&](irr::core::recti const& rect)
    {
        if (m_paintFn)
            m_paintFn(Core::Media::Rect::xyxy(rect.UpperLeftCorner.X, rect.UpperLeftCorner.Y, rect.LowerRightCorner.X, rect.LowerRightCorner.Y));
    };
}

std::function<void(Core::Media::Size)> IrrBackend::IrrWindowImpl::GetResized() const
{
    return m_resizedFn;
}

void IrrBackend::IrrWindowImpl::SetResized(const std::function<void(Core::Media::Size)>& value)
{
    m_resizedFn = value;
    ((irr::CIrrDeviceStub*)m_contextDevice)->OnResizeFn = [&](irr::core::dimension2du const& size)
    {
        if (m_resizedFn)
            m_resizedFn(Core::Media::Size(size.Width, size.Height));
    };
}

std::function<void(double)> IrrBackend::IrrWindowImpl::GetScalingChanged() const
{
    return m_scalingChangedFn;
}

void IrrBackend::IrrWindowImpl::SetScalingChanged(const std::function<void(double)>& value)
{
    m_scalingChangedFn = value;
}

void IrrBackend::IrrWindowImpl::Invalidate(Core::Media::Rect const & rect)
{
    ((irr::CIrrDeviceStub*)m_contextDevice)->invalidate(irr::core::recti(rect.x0(), rect.y0(), rect.x1(), rect.y1()));
}

void IrrBackend::IrrWindowImpl::SetInputRoot(Interfaces::UIControls::IInputRoot * inputRoot)
{
    m_inputRoot = inputRoot;
}

Core::Media::Point IrrBackend::IrrWindowImpl::PointToClient(Core::Media::Point const & point)
{
    irr::core::vector2df absPoint(point.x, point.y);
    irr::core::vector2df clientPoint = m_drawingContext.GetContext()->GetPostionToClient(absPoint);
    return Core::Media::Point(clientPoint.X, clientPoint.Y);
}

Core::Media::Point IrrBackend::IrrWindowImpl::PointToScreen(Core::Media::Point const & point)
{
    irr::core::vector2df relPoint(point.x, point.y);
    irr::core::vector2df screenPoint = m_drawingContext.GetContext()->GetPostionToScreen(relPoint);
    return Core::Media::Point(screenPoint.X, screenPoint.Y);
}

void IrrBackend::IrrWindowImpl::SetCursor(PlatformHandle const & cursor)
{
    m_drawingContext.GetContext()->getCursorControl()->setActiveIcon((irr::gui::ECURSOR_ICON)cursor.Handle);
}

std::function<void()> IrrBackend::IrrWindowImpl::GetClosed() const
{
    return m_closedFn;
}

void IrrBackend::IrrWindowImpl::SetClosed(const std::function<void()>& value)
{
    m_closedFn = value;
    ((irr::CIrrDeviceStub*)m_contextDevice)->OnClosedFn = value;
}

IMouseDevice * IrrBackend::IrrWindowImpl::GetMouseDevice() const
{
    return XUI::Platform::UIService::Instance()->Get<XUI::Interfaces::IMouseDevice>();
}

void IrrBackend::IrrWindowImpl::Show()
{
    ((irr::CIrrDeviceStub*)m_contextDevice)->show();
}

void IrrBackend::IrrWindowImpl::Hide()
{
    ((irr::CIrrDeviceStub*)m_contextDevice)->hide();
}

void IrrBackend::IrrWindowImpl::BeginMoveDrag()
{
}

void IrrBackend::IrrWindowImpl::BeginResizeDrag(UI::WindowEdge edge)
{
}

Core::Media::Point IrrBackend::IrrWindowImpl::GetPosition() const
{
    auto pos = ((irr::CIrrDeviceStub*)m_contextDevice)->getWindowPosition();
    return Core::Media::Point(pos.X, pos.Y);
}

void IrrBackend::IrrWindowImpl::SetPosition(Core::Media::Point const & value)
{
    ((irr::CIrrDeviceStub*)m_contextDevice)->setWindowPosition(irr::core::position2di(value.x, value.y));
}

std::function<void(Core::Media::Point const&)> IrrBackend::IrrWindowImpl::GetPositionChanged() const
{
    return m_positionChangedFn;
}

void IrrBackend::IrrWindowImpl::SetPositionChanged(const std::function<void(Core::Media::Point const&)>& value)
{
    m_positionChangedFn = value;
    ((irr::CIrrDeviceStub*)m_contextDevice)->OnPositionChangedFn = [&](irr::core::position2di const& point)
    {
        if (m_positionChangedFn)
            m_positionChangedFn(Core::Media::Point(point.X, point.Y));
    };
}

void IrrBackend::IrrWindowImpl::Activate()
{
    if (m_activateFn)
        m_activateFn();
}

std::function<void()> IrrBackend::IrrWindowImpl::GetDeactivated() const
{
    return m_deActivateFn;
}

void IrrBackend::IrrWindowImpl::SetDeactivated(const std::function<void()>& value)
{
    m_deActivateFn = value;
    ((irr::CIrrDeviceStub*)m_contextDevice)->OnDeactivatedFn = m_deActivateFn;
}

std::function<void()> IrrBackend::IrrWindowImpl::GetActivated() const
{
    return m_activateFn;
}

void IrrBackend::IrrWindowImpl::SetActivated(const std::function<void()>& value)
{
    m_activateFn = value;
    ((irr::CIrrDeviceStub*)m_contextDevice)->OnActivatedFn = m_deActivateFn;
}

PlatformHandle const & IrrBackend::IrrWindowImpl::GetHandle() const
{
    return m_handle;
}

Core::Media::Size IrrBackend::IrrWindowImpl::GetMaxClientSize() const
{
    auto windowSize = ((irr::CIrrDeviceStub*)m_contextDevice)->getWindowSize();
    return Core::Media::Size(windowSize.Width, windowSize.Height);
}

void IrrBackend::IrrWindowImpl::Resize(Core::Media::Size const & clientSize)
{
    ((irr::CIrrDeviceStub*)m_contextDevice)->setWindowSize(irr::core::dimension2du(clientSize.Width, clientSize.Height));
}

UI::WindowState IrrBackend::IrrWindowImpl::GetWindowState() const
{
    if (m_contextDevice->isWindowMinimized())
        return UI::WindowState::Minimized;
    if (((irr::CIrrDeviceStub*)m_contextDevice)->isWindowMaximized())
        return UI::WindowState::Maximized;
    return UI::WindowState::Normal;
}

void IrrBackend::IrrWindowImpl::SetWindowState(UI::WindowState value)
{
    if (value == UI::WindowState::Minimized)
        ((irr::CIrrDeviceStub*)m_contextDevice)->minimizeWindow();
    if (value == UI::WindowState::Maximized)
        ((irr::CIrrDeviceStub*)m_contextDevice)->maximizeWindow();
    if (value == UI::WindowState::Normal)
        ((irr::CIrrDeviceStub*)m_contextDevice)->restoreWindow();
}

void IrrBackend::IrrWindowImpl::SetTitle(const std::string & title)
{
    System::String _title(title);
    std::wstring wtitle = _title.ToWString();
    m_contextDevice->setWindowCaption(wtitle.c_str());
}

void IrrBackend::IrrWindowImpl::SetTitle(const std::wstring & wtitle)
{
    m_contextDevice->setWindowCaption(wtitle.c_str());
}

rxcpp::subscription IrrBackend::IrrWindowImpl::ShowDialog()
{
    return rxcpp::subscription();
}

void IrrBackend::IrrWindowImpl::SetSystemDecorations(bool enabled)
{
}

void IrrBackend::IrrWindowImpl::SetIcon(std::shared_ptr<IWindowIconImpl> const & icon)
{
    //m_contextDevice->
}

void IrrBackend::IrrWindowImpl::ShowTaskbarIcon(bool value)
{
}

bool IrrBackend::IrrWindowImpl::OnEvent(const irr::SEvent & event)
{
    using namespace irr;
    extern InputModifiers GetMouseModifiers(const irr::SEvent::SMouseInput & input);
    extern XUI::Interfaces::Key GetXUIKey(irr::EKEY_CODE key);

    switch (event.EventType)
    {
        case EET_MOUSE_INPUT_EVENT:
        {
            XUI::Interfaces::RawMouseEventType eventType = XUI::Interfaces::RawMouseEventType::Move;
            XUI::Interfaces::InputModifiers inputMods = GetMouseModifiers(event.MouseInput);
            XUI::Core::Media::Point p(event.MouseInput.X, event.MouseInput.Y);

            if (event.MouseInput.Event == EMOUSE_INPUT_EVENT::EMIE_MOUSE_WHEEL)
            {
                eventType = XUI::Interfaces::RawMouseEventType::Wheel;
                XUI::Interfaces::RawMouseWheelEventArgs e(XUI::Platform::UIService::Instance()->Get<XUI::Interfaces::IMouseDevice>(), std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count(), m_inputRoot, eventType, p, inputMods);
                e.Delta.y = event.MouseInput.Wheel;
                if (m_inputFn)
                    m_inputFn(&e);
            }
            else
            {
                XUI::Interfaces::RawMouseEventArgs e(XUI::Platform::UIService::Instance()->Get<XUI::Interfaces::IMouseDevice>(), std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count(), m_inputRoot, eventType, p, inputMods);
                switch (event.MouseInput.Event)
                {
                    case EMOUSE_INPUT_EVENT::EMIE_MOUSE_MOVED: eventType = XUI::Interfaces::RawMouseEventType::Move; break;
                    case EMOUSE_INPUT_EVENT::EMIE_LMOUSE_PRESSED_DOWN: eventType = XUI::Interfaces::RawMouseEventType::LeftButtonDown; break;
                    case EMOUSE_INPUT_EVENT::EMIE_LMOUSE_LEFT_UP: eventType = XUI::Interfaces::RawMouseEventType::LeftButtonUp; break;
                    case EMOUSE_INPUT_EVENT::EMIE_RMOUSE_PRESSED_DOWN: eventType = XUI::Interfaces::RawMouseEventType::RightButtonDown; break;
                    case EMOUSE_INPUT_EVENT::EMIE_RMOUSE_LEFT_UP: eventType = XUI::Interfaces::RawMouseEventType::RightButtonUp; break;
                    case EMOUSE_INPUT_EVENT::EMIE_MMOUSE_PRESSED_DOWN: eventType = XUI::Interfaces::RawMouseEventType::MiddleButtonDown; break;
                    case EMOUSE_INPUT_EVENT::EMIE_MMOUSE_LEFT_UP: eventType = XUI::Interfaces::RawMouseEventType::MiddleButtonUp; break;
                }

                e.Type = eventType;
                if (m_inputFn)
                    m_inputFn(&e);
            }
            break;
        }
        case EET_KEY_INPUT_EVENT:
        {
            uint32 modifiers = InputModifiers::None;
            if (event.KeyInput.Control)
                modifiers |= InputModifiers::Control;
            if (event.KeyInput.Shift)
                modifiers |= InputModifiers::Shift;
            if (event.KeyInput.Alt)
                modifiers |= InputModifiers::Alt;

            XUI::Interfaces::InputModifiers inputMods = XUI::Interfaces::InputModifiers(modifiers);

            XUI::Interfaces::RawKeyEventArgs e(XUI::Platform::UIService::Instance()->Get<XUI::Interfaces::IKeyboardDevice>(),
                std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count(),
                event.KeyInput.PressedDown ? RawKeyEventType::KeyDown : RawKeyEventType::KeyUp,
                GetXUIKey(event.KeyInput.Key),
                inputMods);

            e.Char = event.KeyInput.Char;
            if (m_inputFn)
                m_inputFn(&e);
            break;
        }
        case EET_TEXT_INPUT_EVENT:
        {
            if (event.KeyInput.Char < 32)
                break;

            std::wstring text;
            text += event.KeyInput.Char;
            XUI::Interfaces::RawTextInputEventArgs e(
                XUI::Platform::UIService::Instance()->Get<XUI::Interfaces::IKeyboardDevice>(),
                std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count(), text);

            if (m_inputFn)
                m_inputFn(&e);
            break;
        }
        break;
        default:
            break;
    } // end switch

    return false;
}

void IrrBackend::IrrWindowImpl::Focus() const
{
    ((irr::CIrrDeviceStub*)m_contextDevice)->focus();
}

void IrrBackend::IrrWindowImpl::Unfocus() const
{
    ((irr::CIrrDeviceStub*)m_contextDevice)->unfocus();
}

void IrrBackend::IrrWindowImpl::Capture() const
{
    ((irr::CIrrDeviceStub*)m_contextDevice)->capture();
}

void IrrBackend::IrrWindowImpl::Uncapture() const
{
    ((irr::CIrrDeviceStub*)m_contextDevice)->uncapture();
}

void IrrBackend::IrrWindowImpl::SetActiveWindow() const
{
    ((irr::CIrrDeviceStub*)m_contextDevice)->setActiveWindow();
}

void IrrBackend::IrrWindowImpl::SetParent(IWindowImpl * parent) const
{
    ((irr::CIrrDeviceStub*)m_contextDevice)->setParent(((IrrBackend::IrrWindowImpl*)parent)->GetContext());
}

void IrrBackend::IrrWindowImpl::SetTimer(uint32 id, uint interval, std::function<void()> const & callback)
{
    ((irr::CIrrDeviceStub*)m_contextDevice)->setTimer(id, interval, callback);
}

void IrrBackend::IrrWindowImpl::UnsetTimer(uint32 id)
{
    ((irr::CIrrDeviceStub*)m_contextDevice)->unsetTimer(id);
}
