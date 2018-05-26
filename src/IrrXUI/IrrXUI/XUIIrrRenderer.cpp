#include "XUIIrrRenderer.h"
#include "XUIIrrGeometry.h"
#include "XUIIrrBitMap.h"
#include "XUIIrrFormatedText.h"
#include "XUIIrrWindowImpl.h"

#include "XUI/Platform/UIService.h"
#include "XUI/Core/Media/Pen.h"
#include "XUI/Core/Media/Brush.h"
#include "XUI/Core/Input/KeyboardDevice.h"
#include "XUI/Core/Input/MouseDevice.h"
#include "XUI/Core/Input/Cursors.h"
#include "XUI/UI/Window.h"

#include "IrrLicht/include/irrlicht.h"
#include "IrrLicht/CImage.h"

#include "XUI/Core/GlobalInitializerScheduler.h"
#include "XUI/Interfaces/IInputDevices.h"

#include "Log/Log.h"

#include "CLogger.h"

using namespace XUI;
using namespace XUI::Interfaces;

std::unordered_map<irr::EKEY_CODE, XUI::Interfaces::Key> s_translateKeyMap;
std::shared_ptr<IrrBackend::XUIIrrPlatformDriver> s_xuiDriverService = nullptr;
irr::IrrlichtDevice * m_mainDevice = nullptr;

std::set<IrrBackend::IrrWindowImpl*> m_activeWindows;

void CreateKeyMap();

void InitializeInlineComponents()
{
    irr::video::E_DRIVER_TYPE _driverType = irr::video::E_DRIVER_TYPE::EDT_DIRECT3D11;

    int _xres = 1024;
    int _yres = 768;

    int _colordepth = 32;

    bool _windowed = true,
        _shadows = true,
        _vsync = false;

    m_mainDevice = irr::createDevice(_driverType, irr::core::dimension2d<uint>(_xres, _yres), _colordepth, !_windowed, _shadows, _vsync, nullptr);

    irr::os::Printer::Logger->setLogLevel(irr::ELL_DEBUG);

    m_mainDevice->setWindowCaption(L"- Initializing");
    m_mainDevice->setResizable(true);

    // Clear Screen
    m_mainDevice->getVideoDriver()->beginScene();
    m_mainDevice->getVideoDriver()->endScene();
}

void IrrBackend::XUIIrrPlatformDriver::InitializeXUI()
{
    XUI::Core::GlobalInitializer::Finalize();
    s_xuiDriverService = std::make_shared<IrrBackend::XUIIrrPlatformDriver>();
    s_xuiDriverService->InstanceInitializer();
    XUI::Core::GlobalInitializer::FinalizeAfterService();
    XUI::Platform::UIRender::PlatformMainInitializer();
}

IrrBackend::XUIIrrPlatformDriver * IrrBackend::XUIIrrPlatformDriver::Instance()
{
    return s_xuiDriverService.get();
}

IrrBackend::XUIIrrPlatformDriver::XUIIrrPlatformDriver()
{
    XUI::Platform::UIService::Instance()->Add<XUI::Interfaces::IPlatformRenderInterface>(static_cast<XUI::Interfaces::IPlatformRenderInterface*>(this));
    XUI::Platform::UIService::Instance()->Add<XUI::Interfaces::IClipboard>(new XUIIrrClipboard(this));
    //XUI::Platform::UIService::Instance()->Add<XUI::Interfaces::IDrawingContextImpl>(static_cast<XUI::Interfaces::IDrawingContextImpl*>(m_renderWindow.GetDrawingContext()));

    CreateKeyMap();
}

IrrBackend::XUIIrrPlatformDriver::~XUIIrrPlatformDriver()
{
    XUI::Platform::UIService::Instance()->Remove<XUI::Interfaces::IPlatformRenderInterface>();
    XUI::Platform::UIService::Instance()->Remove<XUI::Interfaces::IDrawingContextImpl>();

    RemoveActiveWindow(m_renderWindow.get());
}

void IrrBackend::XUIIrrPlatformDriver::SetIrrMainHwnd(irr::IrrlichtDevice * context)
{
    RemoveActiveWindow(m_renderWindow.get());
    m_renderWindow = std::make_shared<IrrWindowImpl>(context, context->getVideoDriver());
    // Inital size
    XUI::Core::Media::Rect rect = XUI::Core::Media::Rect::xywh(0, 0, context->getVideoDriver()->getScreenSize().Width, context->getVideoDriver()->getScreenSize().Height);
    s_xuiDriverService->Width = rect.width();
    s_xuiDriverService->Height = rect.height();
    s_xuiDriverService->Bounds = rect;
    s_xuiDriverService->_visualRoot = s_xuiDriverService->shared_from_base_dynamic<Interfaces::IRenderRoot>();

    AddActiveWindow(m_renderWindow.get());
    SetWindowDevice(m_renderWindow);
}

void IrrBackend::XUIIrrPlatformDriver::InitializeInlineMainWindow()
{
    if (!m_mainDevice)
    {
        InitializeInlineComponents();
        SetIrrMainHwnd(m_mainDevice);
    }
}

IFormattedText * IrrBackend::XUIIrrPlatformDriver::CreateFormattedText(std::wstring const & text, Core::Media::Typeface const & typeface, Core::Media::TextAlignment textAlignment, Core::Media::TextWrapping wrapping, Core::Media::Size const & constraint, std::initializer_list<FormattedTextStyleSpan> spans)
{
    return new XUIIrrFormatedText(text, typeface, textAlignment, wrapping, constraint, spans);
}

std::shared_ptr<IStreamGeometry> IrrBackend::XUIIrrPlatformDriver::CreateStreamGeometry()
{
    auto geometry = std::make_shared<XUIIrrGeometry>();
    ((irr::scene::CMeshBufferEx<irr::video::S3DVertex>&)geometry->GetBuffer()).Set3DMode(false);
    return geometry;
}

std::shared_ptr<IRenderTarget> IrrBackend::XUIIrrPlatformDriver::CreateRenderTarget(std::vector<void*> surfaces)
{
    return nullptr;
}

std::shared_ptr<IRenderTarget> IrrBackend::XUIIrrPlatformDriver::CreateRenderTargetBitmap(int width, int height, double dpiX, double dpiY)
{
    return nullptr;
}

std::shared_ptr<IBitmapImpl> IrrBackend::XUIIrrPlatformDriver::CreateWritableBitmap(int width, int height, System::Imaging::PixelFormat format)
{
    irr::video::ECOLOR_FORMAT irr_format = irr::video::ECOLOR_FORMAT::ECF_UNKNOWN;
    switch (format)
    {
        case System::Imaging::PixelFormat::Rgb565:
        {
            irr_format = irr::video::ECOLOR_FORMAT::ECF_R5G6B5;
            break;
        }
        case System::Imaging::PixelFormat::Bgra8888:
        {
            //TODO: Implement or convert
            irr_format = irr::video::ECOLOR_FORMAT::ECF_RGBA8;
            break;
        }
        case System::Imaging::PixelFormat::Rgba8888:
        {
            irr_format = irr::video::ECOLOR_FORMAT::ECF_RGBA8;
            break;
        }
        case System::Imaging::PixelFormat::Rgb888:
        {
            irr_format = irr::video::ECOLOR_FORMAT::ECF_R8G8B8;
            break;
        }
        case System::Imaging::PixelFormat::Bgr888:
        {
            irr_format = irr::video::ECOLOR_FORMAT::ECF_R8G8B8;
            break;
        }
        default:
        {
            System::ThrowIfFailed<System::InvalidOperationException>(false);
            break;
        }
    }

    irr::video::CImage* image = new irr::video::CImage(irr_format, irr::core::dimension2du(width, height));
    auto bitmap = std::make_shared<XUIIrrBitMap>(image);
    image->drop();

    return bitmap;
}

Core::Media::Point IrrBackend::XUIIrrPlatformDriver::PointToClient(Core::Media::Point const & point)
{
    if (m_renderWindow)
    {
        irr::core::vector2df absPoint(point.x, point.y);
        irr::core::vector2df clientPoint = m_renderWindow->GetContext()->GetPostionToClient(absPoint);
        return Core::Media::Point(clientPoint.X, clientPoint.Y);
    }

    return Core::Media::Point::Zero();
}

Core::Media::Point IrrBackend::XUIIrrPlatformDriver::PointToScreen(Core::Media::Point const & point)
{
    if (m_renderWindow)
    {
        irr::core::vector2df relPoint(point.x, point.y);
        irr::core::vector2df screenPoint = m_renderWindow->GetContext()->GetPostionToScreen(relPoint);
        return Core::Media::Point(screenPoint.X, screenPoint.Y);
    }

    return Core::Media::Point::Zero();
}

void IrrBackend::XUIIrrPlatformDriver::SetClientSize(const XUI::Core::Media::Size &value)
{
}

void IrrBackend::XUIIrrPlatformDriver::Invalidate(Core::Media::Rect const & rect)
{
    _invalidatedRender = true;
    //[DllImport("user32.dll")]
    //public static extern bool InvalidateRect(IntPtr hWnd, ref RECT lpRect, bool bErase);
}

void IrrBackend::XUIIrrPlatformDriver::Render(Interfaces::IDrawingContextImpl * context)
{
    Platform::UIRender::Render(context);
}

InputModifiers GetMouseModifiers(const irr::SEvent::SMouseInput & input)
{
    uint32 modifiers = InputModifiers::None;
    if (input.Control)
        modifiers |= InputModifiers::Control;
    if (input.Shift)
        modifiers |= InputModifiers::Shift;
    if (input.ButtonStates & MK_LBUTTON)
        modifiers |= InputModifiers::LeftMouseButton;
    if (input.ButtonStates & MK_RBUTTON)
        modifiers |= InputModifiers::RightMouseButton;
    if (input.ButtonStates & MK_MBUTTON)
        modifiers |= InputModifiers::MiddleMouseButton;
    return InputModifiers(modifiers);
}

void CreateKeyMap()
{
    struct SKeyMap
    {
        SKeyMap() {}
        SKeyMap(XUI::Interfaces::Key x11, irr::EKEY_CODE win32)
            : X11Key(x11), Win32Key(win32)
        {
        }

        XUI::Interfaces::Key X11Key;
        irr::EKEY_CODE Win32Key;

        bool operator<(const SKeyMap& o) const
        {
            return Win32Key<o.Win32Key;
        }
    };

    irr::core::array<SKeyMap> KeyMap;

    KeyMap.clear();
    KeyMap.reallocate(84);
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::Back, irr::KEY_BACK));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::Tab, irr::KEY_TAB));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::OemBackTab, irr::KEY_TAB));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::LineFeed, 0)); // ???
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::Clear, irr::KEY_CLEAR));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::Return, irr::KEY_RETURN));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::Pause, irr::KEY_PAUSE));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::Scroll, irr::KEY_SCROLL));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::System, 0)); // ???
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::Escape, irr::KEY_ESCAPE));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::Insert, irr::KEY_INSERT));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::Delete, irr::KEY_DELETE));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::Home, irr::KEY_HOME));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::Left, irr::KEY_LEFT));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::Up, irr::KEY_UP));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::Right, irr::KEY_RIGHT));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::Down, irr::KEY_DOWN));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::Prior, irr::KEY_PRIOR));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::PageUp, irr::KEY_PRIOR));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::Next, irr::KEY_NEXT));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::PageDown, irr::KEY_NEXT));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::End, irr::KEY_END));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::BrowserHome, irr::KEY_HOME));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::NumLock, irr::KEY_NUMLOCK));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::Space, irr::KEY_SPACE));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::KP_Tab, irr::KEY_TAB));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::KP_Enter, irr::KEY_RETURN));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::KP_F1, irr::KEY_F1));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::KP_F2, irr::KEY_F2));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::KP_F3, irr::KEY_F3));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::KP_F4, irr::KEY_F4));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::KP_Home, irr::KEY_HOME));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::KP_Left, irr::KEY_LEFT));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::KP_Up, irr::KEY_UP));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::KP_Right, irr::KEY_RIGHT));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::KP_Down, irr::KEY_DOWN));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::Print, irr::KEY_PRINT));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::KP_Prior, irr::KEY_PRIOR));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::KP_Page_Up, irr::KEY_PRIOR));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::KP_Next, irr::KEY_NEXT));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::KP_Page_Down, irr::KEY_NEXT));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::KP_End, irr::KEY_END));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::KP_Begin, irr::KEY_HOME));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::KP_Insert, irr::KEY_INSERT));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::KP_Delete, irr::KEY_DELETE));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::KP_Equal, 0)); // ???
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::KP_Multiply, irr::KEY_MULTIPLY));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::KP_Add, irr::KEY_ADD));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::KP_Separator, irr::KEY_SEPARATOR));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::KP_Subtract, irr::KEY_SUBTRACT));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::KP_Decimal, irr::KEY_DECIMAL));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::KP_Divide, irr::KEY_DIVIDE));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::NumPad0, irr::KEY_KEY_0));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::NumPad1, irr::KEY_KEY_1));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::NumPad2, irr::KEY_KEY_2));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::NumPad3, irr::KEY_KEY_3));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::NumPad4, irr::KEY_KEY_4));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::NumPad5, irr::KEY_KEY_5));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::NumPad6, irr::KEY_KEY_6));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::NumPad7, irr::KEY_KEY_7));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::NumPad8, irr::KEY_KEY_8));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::NumPad9, irr::KEY_KEY_9));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::F1, irr::KEY_F1));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::F2, irr::KEY_F2));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::F3, irr::KEY_F3));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::F4, irr::KEY_F4));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::F5, irr::KEY_F5));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::F6, irr::KEY_F6));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::F7, irr::KEY_F7));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::F8, irr::KEY_F8));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::F9, irr::KEY_F9));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::F10, irr::KEY_F10));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::F11, irr::KEY_F11));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::F12, irr::KEY_F12));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::LeftShift, irr::KEY_LSHIFT));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::RightShift, irr::KEY_RSHIFT));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::LeftCtrl, irr::KEY_LCONTROL));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::RightCtrl, irr::KEY_RCONTROL));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::CapsLock, irr::KEY_CAPITAL));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::Shift_Lock, irr::KEY_CAPITAL));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::LWin, irr::KEY_LWIN));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::RWin, irr::KEY_RWIN));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::LeftAlt, irr::KEY_LMENU));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::RightAlt, irr::KEY_RMENU));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::ISO_Level3_Shift, irr::KEY_RMENU));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::Menu, irr::KEY_MENU));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::Space, irr::KEY_SPACE));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::exclam, 0)); //?
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::quotedbl, 0)); //?
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::section, 0)); //?
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::numbersign, irr::KEY_OEM_2));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::dollar, 0)); //?
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::percent, 0)); //?
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::ampersand, 0)); //?
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::apostrophe, irr::KEY_OEM_7));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::parenleft, 0)); //?
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::parenright, 0)); //?
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::asterisk, 0)); //?
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::OemPlus, irr::KEY_PLUS)); //?
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::OemComma, irr::KEY_COMMA)); //?
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::OemMinus, irr::KEY_MINUS)); //?
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::OemPeriod, irr::KEY_PERIOD)); //?
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::OemBackslash, irr::KEY_OEM_2)); //?
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::D0, irr::KEY_KEY_0));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::D1, irr::KEY_KEY_1));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::D2, irr::KEY_KEY_2));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::D3, irr::KEY_KEY_3));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::D4, irr::KEY_KEY_4));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::D5, irr::KEY_KEY_5));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::D6, irr::KEY_KEY_6));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::D7, irr::KEY_KEY_7));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::D8, irr::KEY_KEY_8));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::D9, irr::KEY_KEY_9));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::colon, 0)); //?
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::OemSemicolon, irr::KEY_OEM_1));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::less, irr::KEY_OEM_102));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::equal, irr::KEY_PLUS));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::greater, 0)); //?
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::question, 0)); //?
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::at, irr::KEY_KEY_2)); //?
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::mu, 0)); //?
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::EuroSign, 0)); //?
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::A, irr::KEY_KEY_A));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::B, irr::KEY_KEY_B));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::C, irr::KEY_KEY_C));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::D, irr::KEY_KEY_D));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::E, irr::KEY_KEY_E));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::F, irr::KEY_KEY_F));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::G, irr::KEY_KEY_G));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::H, irr::KEY_KEY_H));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::I, irr::KEY_KEY_I));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::J, irr::KEY_KEY_J));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::K, irr::KEY_KEY_K));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::L, irr::KEY_KEY_L));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::M, irr::KEY_KEY_M));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::N, irr::KEY_KEY_N));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::O, irr::KEY_KEY_O));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::P, irr::KEY_KEY_P));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::Q, irr::KEY_KEY_Q));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::R, irr::KEY_KEY_R));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::S, irr::KEY_KEY_S));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::T, irr::KEY_KEY_T));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::U, irr::KEY_KEY_U));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::V, irr::KEY_KEY_V));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::W, irr::KEY_KEY_W));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::X, irr::KEY_KEY_X));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::Y, irr::KEY_KEY_Y));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::Z, irr::KEY_KEY_Z));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::Oem4, irr::KEY_OEM_4));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::OemBackslash, irr::KEY_OEM_5));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::Oem6, irr::KEY_OEM_6));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::Oem5, irr::KEY_OEM_5));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::degree, 0)); //?
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::underscore, irr::KEY_MINUS)); //?
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::Oem3, irr::KEY_OEM_3));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::acute, irr::KEY_OEM_6));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::a, irr::KEY_KEY_A));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::b, irr::KEY_KEY_B));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::c, irr::KEY_KEY_C));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::d, irr::KEY_KEY_D));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::e, irr::KEY_KEY_E));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::f, irr::KEY_KEY_F));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::g, irr::KEY_KEY_G));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::h, irr::KEY_KEY_H));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::i, irr::KEY_KEY_I));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::j, irr::KEY_KEY_J));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::k, irr::KEY_KEY_K));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::l, irr::KEY_KEY_L));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::m, irr::KEY_KEY_M));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::n, irr::KEY_KEY_N));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::o, irr::KEY_KEY_O));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::p, irr::KEY_KEY_P));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::q, irr::KEY_KEY_Q));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::r, irr::KEY_KEY_R));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::s, irr::KEY_KEY_S));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::t, irr::KEY_KEY_T));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::u, irr::KEY_KEY_U));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::v, irr::KEY_KEY_V));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::w, irr::KEY_KEY_W));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::x, irr::KEY_KEY_X));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::y, irr::KEY_KEY_Y));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::z, irr::KEY_KEY_Z));
    //KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::ssharp, irr::KEY_OEM_4));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::Oem7, irr::KEY_OEM_7));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::Oem3, irr::KEY_OEM_3));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::Oem1, irr::KEY_OEM_1));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::LWin, irr::KEY_LWIN));
    KeyMap.push_back(SKeyMap(XUI::Interfaces::Key::RWin, irr::KEY_RWIN));

    for (auto k = 0; k != KeyMap.size(); ++k)
        s_translateKeyMap[KeyMap[k].Win32Key] = KeyMap[k].X11Key;
}

XUI::Interfaces::Key GetXUIKey(irr::EKEY_CODE key)
{
    auto ikey = s_translateKeyMap.find(key);
    if (ikey != s_translateKeyMap.end())
        return ikey->second;
    return XUI::Interfaces::Key::None;
}

//bool IrrBackend::XUIIrrPlatformDriver::OnEvent(const irr::SEvent & event)
//{
//    using namespace irr;
//
//    switch (event.EventType)
//    {
//        case EET_MOUSE_INPUT_EVENT:
//        {
//            XUI::Interfaces::RawMouseEventType eventType = XUI::Interfaces::RawMouseEventType::Move;
//            XUI::Interfaces::InputModifiers inputMods = GetMouseModifiers(event.MouseInput);
//            XUI::Core::Media::Point p(event.MouseInput.X, event.MouseInput.Y);
//
//            if (event.MouseInput.Event == EMOUSE_INPUT_EVENT::EMIE_MOUSE_WHEEL)
//            {
//                eventType = XUI::Interfaces::RawMouseEventType::Wheel;
//                XUI::Interfaces::RawMouseWheelEventArgs e(UIRender::_mouseDevice.get(), time(nullptr), static_cast<Interfaces::UIControls::IInputRoot*>(this), eventType, p, inputMods);
//                e.Delta.y = event.MouseInput.Wheel;
//                HandleInput(e);
//            }
//            else
//            {
//                XUI::Interfaces::RawMouseEventArgs e(UIRender::_mouseDevice.get(), time(nullptr), static_cast<Interfaces::UIControls::IInputRoot*>(this), eventType, p, inputMods);
//                switch (event.MouseInput.Event)
//                {
//                    case EMOUSE_INPUT_EVENT::EMIE_MOUSE_MOVED: eventType = XUI::Interfaces::RawMouseEventType::Move; break;
//                    case EMOUSE_INPUT_EVENT::EMIE_LMOUSE_PRESSED_DOWN: eventType = XUI::Interfaces::RawMouseEventType::LeftButtonDown; break;
//                    case EMOUSE_INPUT_EVENT::EMIE_LMOUSE_LEFT_UP: eventType = XUI::Interfaces::RawMouseEventType::LeftButtonUp; break;
//                    case EMOUSE_INPUT_EVENT::EMIE_RMOUSE_PRESSED_DOWN: eventType = XUI::Interfaces::RawMouseEventType::RightButtonDown; break;
//                    case EMOUSE_INPUT_EVENT::EMIE_RMOUSE_LEFT_UP: eventType = XUI::Interfaces::RawMouseEventType::RightButtonUp; break;
//                    case EMOUSE_INPUT_EVENT::EMIE_MMOUSE_PRESSED_DOWN: eventType = XUI::Interfaces::RawMouseEventType::MiddleButtonDown; break;
//                    case EMOUSE_INPUT_EVENT::EMIE_MMOUSE_LEFT_UP: eventType = XUI::Interfaces::RawMouseEventType::MiddleButtonUp; break;
//                }
//
//                e.Type = eventType;
//                HandleInput(e);
//            }
//            break;
//        }
//        case EET_KEY_INPUT_EVENT:
//        {
//            uint32 modifiers = InputModifiers::None;
//            if (event.KeyInput.Control)
//                modifiers |= InputModifiers::Control;
//            if (event.KeyInput.Shift)
//                modifiers |= InputModifiers::Shift;
//            if (event.KeyInput.Alt)
//                modifiers |= InputModifiers::Alt;
//
//            XUI::Interfaces::InputModifiers inputMods = XUI::Interfaces::InputModifiers(modifiers);
//
//            XUI::Interfaces::RawKeyEventArgs e(static_cast<Interfaces::IKeyboardDevice*>(UIRender::_keyBoardDevice.get()),
//                time(nullptr),
//                event.KeyInput.PressedDown ? RawKeyEventType::KeyDown : RawKeyEventType::KeyUp,
//                GetXUIKey(event.KeyInput.Key),
//                inputMods);
//
//            e.Char = event.KeyInput.Char;
//
//            HandleInput(e);
//            break;
//        }
//        break;
//        default:
//            break;
//    } // end switch
//
//    return false;
//}

PlatformHandle IrrBackend::XUIIrrPlatformDriver::GetCursor(XUI::Core::Input::StandardCursorType cursorType)
{
    switch (cursorType)
    {
        case XUI::Core::Input::StandardCursorType::Arrow: return PlatformHandle((std::intptr_t)irr::gui::ECURSOR_ICON::ECI_NORMAL, "Cursor"); break;
        case XUI::Core::Input::StandardCursorType::Ibeam: return PlatformHandle((std::intptr_t)irr::gui::ECURSOR_ICON::ECI_IBEAM, "Cursor"); break;
        case XUI::Core::Input::StandardCursorType::Wait: return PlatformHandle((std::intptr_t)irr::gui::ECURSOR_ICON::ECI_WAIT, "Cursor"); break;
        case XUI::Core::Input::StandardCursorType::Cross: return PlatformHandle((std::intptr_t)irr::gui::ECURSOR_ICON::ECI_CROSS, "Cursor"); break;
        case XUI::Core::Input::StandardCursorType::UpArrow: return PlatformHandle((std::intptr_t)irr::gui::ECURSOR_ICON::ECI_UP, "Cursor"); break;
        case XUI::Core::Input::StandardCursorType::SizeWestEast: return PlatformHandle((std::intptr_t)irr::gui::ECURSOR_ICON::ECI_SIZEWE, "Cursor"); break;
        case XUI::Core::Input::StandardCursorType::SizeNorthSouth: return PlatformHandle((std::intptr_t)irr::gui::ECURSOR_ICON::ECI_SIZENS, "Cursor"); break;
        case XUI::Core::Input::StandardCursorType::SizeAll: return PlatformHandle((std::intptr_t)irr::gui::ECURSOR_ICON::ECI_SIZEALL, "Cursor"); break;
        case XUI::Core::Input::StandardCursorType::No: return PlatformHandle((std::intptr_t)irr::gui::ECURSOR_ICON::ECI_NO, "Cursor"); break;
        case XUI::Core::Input::StandardCursorType::Hand: return PlatformHandle((std::intptr_t)irr::gui::ECURSOR_ICON::ECI_HAND, "Cursor"); break;
        case XUI::Core::Input::StandardCursorType::AppStarting: return PlatformHandle((std::intptr_t)irr::gui::ECURSOR_ICON::ECI_NORMAL, "Cursor"); break;
        case XUI::Core::Input::StandardCursorType::Help: return PlatformHandle((std::intptr_t)irr::gui::ECURSOR_ICON::ECI_HELP, "Cursor"); break;
        case XUI::Core::Input::StandardCursorType::TopSide: return PlatformHandle((std::intptr_t)irr::gui::ECURSOR_ICON::ECI_SIZENS, "Cursor"); break;
        case XUI::Core::Input::StandardCursorType::BottomSize: return PlatformHandle((std::intptr_t)irr::gui::ECURSOR_ICON::ECI_SIZENS, "Cursor"); break;
        case XUI::Core::Input::StandardCursorType::LeftSide: return PlatformHandle((std::intptr_t)irr::gui::ECURSOR_ICON::ECI_SIZEWE, "Cursor"); break;
        case XUI::Core::Input::StandardCursorType::RightSide: return PlatformHandle((std::intptr_t)irr::gui::ECURSOR_ICON::ECI_SIZEWE, "Cursor"); break;
        case XUI::Core::Input::StandardCursorType::TopLeftCorner: return PlatformHandle((std::intptr_t)irr::gui::ECURSOR_ICON::ECI_SIZENWSE, "Cursor"); break;
        case XUI::Core::Input::StandardCursorType::TopRightCorner: return PlatformHandle((std::intptr_t)irr::gui::ECURSOR_ICON::ECI_SIZENESW, "Cursor"); break;
        case XUI::Core::Input::StandardCursorType::BottomLeftCorner: return PlatformHandle((std::intptr_t)irr::gui::ECURSOR_ICON::ECI_SIZENWSE, "Cursor"); break;
        case XUI::Core::Input::StandardCursorType::BottomRightCorner: return PlatformHandle((std::intptr_t)irr::gui::ECURSOR_ICON::ECI_SIZENESW, "Cursor"); break;
    }
    return PlatformHandle((std::intptr_t)irr::gui::ECURSOR_ICON::ECI_NORMAL, "Cursor");
}

std::shared_ptr<IWindowImpl> IrrBackend::XUIIrrPlatformDriver::CreateWindow(XUI::UI::WindowStyle state)
{
    irr::video::E_DRIVER_TYPE _driverType = irr::video::E_DRIVER_TYPE::EDT_DIRECT3D11;

    int _xres = 300;
    int _yres = 200;

    int _colordepth = 32;

    bool _windowed = true,
        stencilbuffer = true,
        _vsync = false;

    irr::SIrrlichtCreationParameters p;
    p.DriverType = _driverType;
    p.WindowSize = irr::core::dimension2d<uint>(_xres, _yres);
    p.Bits = _colordepth;
    p.Fullscreen = !_windowed;
    p.Stencilbuffer = stencilbuffer;
    p.Vsync = _vsync;
    p.EventReceiver = nullptr;
    if (state == XUI::UI::WindowStyle::None)
        p.WStyle = irr::SIrrlichtCreationParameters::WindowStyle::WS_Overlapped | irr::SIrrlichtCreationParameters::WindowStyle::WS_Popup;
    else if (state == XUI::UI::WindowStyle::SingleBorderWindow)
        p.WStyle = irr::SIrrlichtCreationParameters::WindowStyle::WS_Border | irr::SIrrlichtCreationParameters::WindowStyle::WS_Caption | irr::SIrrlichtCreationParameters::WindowStyle::WS_Minimize | irr::SIrrlichtCreationParameters::WindowStyle::WS_Maximize | irr::SIrrlichtCreationParameters::WindowStyle::WS_Sysmenu;
    else if (state == XUI::UI::WindowStyle::ThreeDBorderWindow)
        p.WStyle = irr::SIrrlichtCreationParameters::WindowStyle::WS_Border | irr::SIrrlichtCreationParameters::WindowStyle::WS_Caption | irr::SIrrlichtCreationParameters::WindowStyle::WS_Minimize | irr::SIrrlichtCreationParameters::WindowStyle::WS_Maximize | irr::SIrrlichtCreationParameters::WindowStyle::WS_Sysmenu | irr::SIrrlichtCreationParameters::WindowStyle::WS_Popup;
    else if (state == XUI::UI::WindowStyle::ToolWindow)
        p.WStyle = irr::SIrrlichtCreationParameters::WindowStyle::WS_Border | irr::SIrrlichtCreationParameters::WindowStyle::WS_Caption | irr::SIrrlichtCreationParameters::WindowStyle::WS_Sysmenu;


    irr::IrrlichtDevice* _device = irr::createDeviceEx(p);
    irr::video::IVideoDriver* _graphics_device = _device->getVideoDriver();

    if (state != XUI::UI::WindowStyle::None)
        _device->setResizable(true);

    std::shared_ptr<IrrBackend::IrrWindowImpl> wnd = std::make_shared<IrrBackend::IrrWindowImpl>(_device, _graphics_device);

    if (state != XUI::UI::WindowStyle::None)
        wnd->SetTitle("- Initializing");

    AddActiveWindow(wnd.get());

    // Clear Screen
    _device->getVideoDriver()->beginScene();
    _device->getVideoDriver()->endScene();

    System::Console()->WriteDebugLine("[" __func__ "] _device=%X", _device);

    return wnd;
}

std::shared_ptr<IWindowImpl> IrrBackend::XUIIrrPlatformDriver::CreateEmbeddableWindow(XUI::UI::WindowStyle state, XUI::Interfaces::IWindowImpl* parent)
{
    irr::video::E_DRIVER_TYPE _driverType = irr::video::E_DRIVER_TYPE::EDT_DIRECT3D11;

    int _xres = 300;
    int _yres = 200;

    int _colordepth = 32;

    bool _windowed = true,
        stencilbuffer = true,
        _vsync = false;

    irr::SIrrlichtCreationParameters p;
    p.DriverType = _driverType;
    p.WindowSize = irr::core::dimension2d<uint>(_xres, _yres);
    p.Bits = _colordepth;
    p.Fullscreen = !_windowed;
    p.Stencilbuffer = stencilbuffer;
    p.Vsync = _vsync;
    p.EventReceiver = nullptr;

    p.WStyle = irr::SIrrlichtCreationParameters::WindowStyle::WS_Overlapped | irr::SIrrlichtCreationParameters::WindowStyle::WS_Popup;

    irr::IrrlichtDevice* _device = irr::createDeviceEx(p);
    irr::video::IVideoDriver* _graphics_device = _device->getVideoDriver();

    if (state != XUI::UI::WindowStyle::None)
        _device->setResizable(true);

    std::shared_ptr<IrrBackend::IrrWindowImpl> wnd = std::make_shared<IrrBackend::IrrWindowImpl>(_device, _graphics_device);

    if (parent)
        wnd->SetParent(parent);

    AddActiveWindow(wnd.get());

    // Clear Screen
    _device->getVideoDriver()->beginScene();
    _device->getVideoDriver()->endScene();

    System::Console()->WriteDebugLine("[" __func__ "] _device=%X", _device);

    return wnd;
}

std::shared_ptr<IWindowImpl> IrrBackend::XUIIrrPlatformDriver::CreatePopup(XUI::UI::WindowStyle state, XUI::Interfaces::IWindowImpl* parent)
{
    irr::video::E_DRIVER_TYPE _driverType = irr::video::E_DRIVER_TYPE::EDT_DIRECT3D11;

    int _xres = 300;
    int _yres = 200;

    int _colordepth = 32;

    bool _windowed = true,
        stencilbuffer = true,
        _vsync = false;

    irr::SIrrlichtCreationParameters p;
    p.DriverType = _driverType;
    p.WindowSize = irr::core::dimension2d<uint>(_xres, _yres);
    p.Bits = _colordepth;
    p.Fullscreen = !_windowed;
    p.Stencilbuffer = stencilbuffer;
    p.Vsync = _vsync;
    p.EventReceiver = nullptr;

    p.WStyle = irr::SIrrlichtCreationParameters::WindowStyle::WS_Overlapped | irr::SIrrlichtCreationParameters::WindowStyle::WS_Popup;

    irr::IrrlichtDevice* _device = irr::createDeviceEx(p);
    irr::video::IVideoDriver* _graphics_device = _device->getVideoDriver();

    if (state != XUI::UI::WindowStyle::None)
        _device->setResizable(true);

    std::shared_ptr<IrrBackend::IrrWindowImpl> wnd = std::make_shared<IrrBackend::IrrWindowImpl>(_device, _graphics_device);

    if (parent)
        wnd->SetParent(parent);

    AddActiveWindow(wnd.get());

    // Clear Screen
    _device->getVideoDriver()->beginScene();
    _device->getVideoDriver()->endScene();

    System::Console()->WriteDebugLine("[" __func__ "] _device=%X", _device);

    return wnd;
}

void IrrBackend::XUIIrrPlatformDriver::InstanceInitializer()
{
    XUI::Platform::UIRender::InstanceInitializer();
}

void IrrBackend::XUIIrrPlatformDriver::Update(const uint32 time_diff)
{
    m_lastUpdateTime = time_diff;

    ExecuteLayoutPass();
    Invalidate(IrrBackend::XUIIrrPlatformDriver::Instance()->Bounds);

    for (TopLevelList::iterator itr = _topLevelHnd.begin(), next; itr != _topLevelHnd.end();)
    {
        auto hnd = *itr;
        next = ++itr;

        auto context = static_cast<IrrBackend::IrrWindowImpl*>(hnd->GetWindowImpl())->GetContext();
        if (context->run())
        {
            context->getVideoDriver()->beginScene();
            context->getSceneManager()->drawAll();
            //context->getGUIEnvironment()->drawAll();                      // irr: draw gui elements
            //Invalidate(Bounds);
            Paint(hnd.get(), hnd->Bounds);
            context->getVideoDriver()->endScene();
        }
    }

    OnExecuteWndProgress(time_diff);

    // ToDo: separate per context
    Core::UIDispatcher::Update(Core::UIDispatcher::Levels::AfterRender);
}

IrrBackend::IrrWindowImpl * IrrBackend::XUIIrrPlatformDriver::GetFirstActiveWindow() const
{
    std::lock_guard<std::mutex> g(m_activeWindowMutex);
    if (m_activeWindows.empty())
        return nullptr;
    return *m_activeWindows.begin();
}

void IrrBackend::XUIIrrPlatformDriver::AddActiveWindow(IrrBackend::IrrWindowImpl * wnd)
{
    m_activeWindowMutex.lock();
    m_activeWindows.insert(wnd);
    m_activeWindowMutex.unlock();
}

void IrrBackend::XUIIrrPlatformDriver::RemoveActiveWindow(IrrBackend::IrrWindowImpl * wnd)
{
    m_activeWindowMutex.lock();
    m_activeWindows.erase(wnd);
    m_activeWindowMutex.unlock();
}

void IrrBackend::XUIIrrPlatformDriver::SetWindowDevice(std::shared_ptr<XUI::Interfaces::IWindowImpl> const & wnd)
{
    Platform::UIRender::SetWindowDevice(wnd);
}

void IrrBackend::XUIIrrPlatformDriver::OnExecuteWndProgress(const uint32 time_diff)
{
    if (m_renderWindow)
    {
        auto context = m_renderWindow->GetContext();
        if (context->run())
        {
            context->getVideoDriver()->beginScene();
            context->getSceneManager()->drawAll();
            //context->getGUIEnvironment()->drawAll();                      // irr: draw gui elements
            //Invalidate(Bounds);
            Paint(this, Bounds);
            context->getVideoDriver()->endScene();
        }
    }
}

void IrrBackend::XUIIrrPlatformDriver::RenderFps(XUI::Interfaces::IWindowImpl * context, XUI::Core::Media::Rect const & clientRect, bool incrementFrameCount)
{
    if (!m_renderWindow)
        return;

    context->_debugTitle = L"IrrLicht [";
    context->_debugTitle += m_renderWindow->GetContext()->getVideoDriver()->getName();
    context->_debugTitle += L"] FPS: [";

    auto fps = m_renderWindow->GetContext()->getVideoDriver()->getFPS();
    
    if (context->_param0 != fps)
    {
        context->_debugTitle += std::to_wstring(fps);
        context->_debugTitle += L"] Update Time: ";
        context->_debugTitle += std::to_wstring(m_lastUpdateTime);
        context->SetTitle(context->_debugTitle);
    
        context->_param0 = fps;
    }
}

IrrBackend::XUIIrrClipboard::XUIIrrClipboard(XUIIrrPlatformDriver * mgr)
    : _mgr(mgr)
{
}

IrrBackend::XUIIrrClipboard::~XUIIrrClipboard()
{
}

std::string IrrBackend::XUIIrrClipboard::GetText()
{
    auto wnd = _mgr->GetWindowImpl();
    if (!wnd)
        wnd = _mgr->GetFirstActiveWindowImpl();
    if (wnd)
    {
        IrrWindowImpl* handle = static_cast<IrrWindowImpl*>(wnd);
        return handle->GetContext()->getOSOperator()->getTextFromClipboard();
    }
    return std::string();
}

void IrrBackend::XUIIrrClipboard::SetText(const std::string & text)
{
    auto wnd = _mgr->GetWindowImpl();
    if (!wnd)
        wnd = _mgr->GetFirstActiveWindowImpl();
    if (wnd)
    {
        IrrWindowImpl* handle = static_cast<IrrWindowImpl*>(wnd);
        handle->GetContext()->getOSOperator()->copyToClipboard(text.c_str());
    }
}

void IrrBackend::XUIIrrClipboard::Clear()
{
}
