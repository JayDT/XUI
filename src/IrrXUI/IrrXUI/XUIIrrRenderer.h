#ifndef __WCLIENT_XUI_PLATFORM_RENDERER
#define __WCLIENT_XUI_PLATFORM_RENDERER

#include "IrrXUI/XUIIrrDrawingContextImpl.h"
#include "IrrXUI/XUIIrrWindowImpl.h"

#include "Reflection/Runtime/CXXReflection.h"
#include "Reflection/Runtime/Variant.h"
#include "Reflection/Runtime/Object.h"

#include "XUI/Interfaces/IPlatformRenderInterface.h"
#include "XUI/Interfaces/IDrawingContext.h"
#include "XUI/Core/Media/Pen.h"
#include "XUI/Platform/UIRenderer.h"
#include "XUI/Interfaces/IClipboard.h"

#include "IrrLicht/include/rect.h"
#include "IrrLicht/include/matrix4.h"
#include "IrrLicht/include/IEventReceiver.h"

#include <stack>

#undef LoadBitmap

namespace irr
{
    class IrrlichtDevice;
}

namespace XUI
{
    namespace Interfaces
    {
        struct IGeometry;
    }
}

namespace IrrBackend
{
    class XUIIrrPlatformDriver;

    class XUIIrrClipboard : public XUI::Interfaces::IClipboard
    {
    private:

        XUIIrrPlatformDriver * _mgr;

    public:
        XUIIrrClipboard(XUIIrrPlatformDriver* mgr);
        virtual ~XUIIrrClipboard();


        // Inherited via IClipboard
        virtual std::string GetText() override;

        virtual void SetText(const std::string & text) override;

        virtual void Clear() override;

    };

    class XUIIrrPlatformDriver 
        : public XUI::Interfaces::IPlatformRenderInterface
        , public XUI::Platform::UIRender
    {
    private:
        mutable std::shared_ptr<IrrWindowImpl> m_renderWindow;
        mutable uint32 m_lastUpdateTime = 0;

        mutable std::mutex m_activeWindowMutex;

    public:

        static void InitializeXUI();
        static XUIIrrPlatformDriver* Instance();

        XUIIrrPlatformDriver();
        virtual ~XUIIrrPlatformDriver();

        void SetIrrMainHwnd(irr::IrrlichtDevice* context);
        virtual void InitializeInlineMainWindow() override;

        // Inherited via Visual
        virtual XUI::Interfaces::IDrawingContextImpl* GetDrawingContext() override { return m_renderWindow ? m_renderWindow->GetDrawingContext() : nullptr; }
        virtual XUI::Interfaces::IWindowImpl* GetWindowImpl() const override { return m_renderWindow.get(); }

        // Inherited via IPlatformRenderInterface
        virtual XUI::Interfaces::IFormattedText * CreateFormattedText(std::wstring const & text, XUI::Core::Media::Typeface const & typeface, XUI::Core::Media::TextAlignment textAlignment, XUI::Core::Media::TextWrapping wrapping, XUI::Core::Media::Size const & constraint, std::initializer_list<XUI::Interfaces::FormattedTextStyleSpan> spans) override;
        virtual std::shared_ptr<XUI::Interfaces::IStreamGeometry> CreateStreamGeometry() override;
        virtual std::shared_ptr<XUI::Interfaces::IRenderTarget> CreateRenderTarget(std::vector<void*> surfaces) override;
        virtual std::shared_ptr<XUI::Interfaces::IRenderTarget> CreateRenderTargetBitmap(int width, int height, double dpiX, double dpiY) override;
        virtual std::shared_ptr<XUI::Interfaces::IBitmapImpl> CreateWritableBitmap(int width, int height, System::Imaging::PixelFormat format) override;

        // Inherited via UIRender
        virtual XUI::Core::Media::Point PointToClient(XUI::Core::Media::Point const & point) override;
        virtual XUI::Core::Media::Point PointToScreen(XUI::Core::Media::Point const & point) override;
        virtual void SetClientSize(const XUI::Core::Media::Size & value);

        // Inherited via UIRender
        virtual void Invalidate(XUI::Core::Media::Rect const & rect) override;

        // Inherited via UIRender
        virtual void Render(XUI::Interfaces::IDrawingContextImpl * context) override;

        // Inherited via IEventReceiver
        //virtual bool OnEvent(const irr::SEvent & event) override;

        // Inherited via UIRender
        virtual XUI::Interfaces::PlatformHandle GetCursor(XUI::Core::Input::StandardCursorType cursorType) override;

        // Inherited via IPlatformRenderInterface
        virtual std::shared_ptr<XUI::Interfaces::IWindowImpl> CreateWindow(XUI::UI::WindowStyle state) override;
        virtual std::shared_ptr<XUI::Interfaces::IWindowImpl> CreateEmbeddableWindow(XUI::UI::WindowStyle state, XUI::Interfaces::IWindowImpl* parent) override;
        virtual std::shared_ptr<XUI::Interfaces::IWindowImpl> CreatePopup(XUI::UI::WindowStyle state, XUI::Interfaces::IWindowImpl* parent) override;

        // Inherited via UIRender
        virtual void InstanceInitializer() override;

        void Update(const uint32 time_diff);

        IrrWindowImpl* GetFirstActiveWindow() const;
        XUI::Interfaces::IWindowImpl* GetFirstActiveWindowImpl() const override { return GetFirstActiveWindow(); }

        void AddActiveWindow(IrrWindowImpl* wnd);
        void RemoveActiveWindow(IrrWindowImpl* wnd);

    protected:
        virtual void SetWindowDevice(std::shared_ptr<XUI::Interfaces::IWindowImpl> const& wnd) override;

        void OnExecuteWndProgress(const uint32 time_diff);

        void RenderFps(XUI::Interfaces::IWindowImpl* context, XUI::Core::Media::Rect const& clientRect, bool incrementFrameCount) override;
    };
}

#endif // !__WCLIENT_XUI_PLATFORM_RENDERER
