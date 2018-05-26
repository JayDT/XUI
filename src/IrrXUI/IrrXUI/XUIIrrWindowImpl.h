#ifndef __IRRXUI_WINDOW_IMPL_H__
#define __IRRXUI_WINDOW_IMPL_H__

#include "IrrXUI/XUIIrrDrawingContextImpl.h"

#include "Reflection/Runtime/CXXReflection.h"
#include "Reflection/Runtime/Variant.h"
#include "Reflection/Runtime/Object.h"

#include "XUI/Core/Media/MediaTypes.h"
#include "XUI/Core/Media/GeometryContext.h"
#include "XUI/Interfaces/IGeometry.h"
#include "XUI/Interfaces/IWindowImpl.h"

#include "IrrLicht/include/CMeshBuffer.h"
#include "IrrLicht/include/IEventReceiver.h"

MetaAssemblyBegin
Meta(modul: ReflInclude("XUI/Core/Media/Pen.h"))
MetaAssemblyEnd

namespace irr
{
    class IrrlichtDevice;

    namespace video
    {
        class IVideoDriver;
    }
}

namespace IrrBackend
{
    class IrrWindowImpl
        : public XUI::Interfaces::IWindowImpl
        , public irr::IEventReceiver
    {
    private:
        XUIIrrDrawingContext m_drawingContext;

        Meta(Disable) irr::IrrlichtDevice* m_contextDevice = nullptr;
        Meta(Disable) irr::video::IVideoDriver* m_contextDriver = nullptr;
        Meta(Disable) XUI::Interfaces::UIControls::IInputRoot * m_inputRoot = nullptr;

        XUI::Interfaces::PlatformHandle m_handle;

        std::function<void()> m_activateFn;
        std::function<void(XUI::Interfaces::RawInputEventArgs*)> m_inputFn;
        std::function<void(XUI::Core::Media::Rect)> m_paintFn;
        std::function<void(XUI::Core::Media::Size)> m_resizedFn;
        std::function<void(double)> m_scalingChangedFn;
        std::function<void(XUI::Core::Media::Point)> m_positionChangedFn;
        std::function<void()> m_deActivateFn;
        std::function<void()> m_closedFn;

    public:

        IrrWindowImpl(irr::IrrlichtDevice* contextDevice, irr::video::IVideoDriver* contextDriver);
        virtual ~IrrWindowImpl();

        // Inherited via IWindowImpl
        XUI::Interfaces::IDrawingContextImpl* GetDrawingContext() override { return &m_drawingContext; }
        Meta(Disable) irr::IrrlichtDevice* GetContext() { return m_contextDevice; }
        virtual void SetBackgroundColor(XUI::Core::Media::Color const& color) override;
        virtual XUI::Core::Media::Size GetClientSize() const override;
        virtual double GetScaling() const override;
        virtual std::vector<System::Reflection::RefObject> GetSurfaces() const override;
        virtual std::function<void(XUI::Interfaces::RawInputEventArgs*)> GetInput() const override;
        virtual void SetInput(const std::function<void(XUI::Interfaces::RawInputEventArgs*)>& value) override;
        virtual std::function<void(XUI::Core::Media::Rect)> GetPaint() const override;
        virtual void SetPaint(const std::function<void(XUI::Core::Media::Rect)>& value) override;
        virtual std::function<void(XUI::Core::Media::Size)> GetResized() const override;
        virtual void SetResized(const std::function<void(XUI::Core::Media::Size)>& value) override;
        virtual std::function<void(double)> GetScalingChanged() const override;
        virtual void SetScalingChanged(const std::function<void(double)>& value) override;
        virtual void Invalidate(XUI::Core::Media::Rect const & rect) override;
        virtual void SetInputRoot(XUI::Interfaces::UIControls::IInputRoot * inputRoot) override;
        virtual XUI::Core::Media::Point PointToClient(XUI::Core::Media::Point const & point) override;
        virtual XUI::Core::Media::Point PointToScreen(XUI::Core::Media::Point const & point) override;
        virtual void SetCursor(XUI::Interfaces::PlatformHandle const & cursor) override;
        virtual std::function<void()> GetClosed() const override;
        virtual void SetClosed(const std::function<void()>& value) override;
        virtual XUI::Interfaces::IMouseDevice * GetMouseDevice() const override;
        virtual void Show() override;
        virtual void Hide() override;
        virtual void BeginMoveDrag() override;
        virtual void BeginResizeDrag(XUI::UI::WindowEdge edge) override;
        virtual XUI::Core::Media::Point GetPosition() const override;
        virtual void SetPosition(XUI::Core::Media::Point const & value) override;
        virtual std::function<void(XUI::Core::Media::Point const&)> GetPositionChanged() const override;
        virtual void SetPositionChanged(const std::function<void(XUI::Core::Media::Point const&)>& value) override;
        virtual void Activate() override;
        virtual std::function<void()> GetDeactivated() const override;
        virtual void SetDeactivated(const std::function<void()>& value) override;
        virtual std::function<void()> GetActivated() const override;
        virtual void SetActivated(const std::function<void()>& value) override;
        virtual XUI::Interfaces::PlatformHandle const & GetHandle() const override;
        virtual XUI::Core::Media::Size GetMaxClientSize() const override;
        virtual void Resize(XUI::Core::Media::Size const & clientSize) override;
        virtual XUI::UI::WindowState GetWindowState() const override;
        virtual void SetWindowState(XUI::UI::WindowState value) override;
        virtual void SetTitle(const std::string & title) override;
        virtual void SetTitle(const std::wstring & title) override;
        virtual rxcpp::subscription ShowDialog() override;
        virtual void SetSystemDecorations(bool enabled) override;
        virtual void SetIcon(std::shared_ptr<XUI::Interfaces::IWindowIconImpl> const & icon) override;
        virtual void ShowTaskbarIcon(bool value) override;

        // Inherited via IEventReceiver
        virtual bool OnEvent(const irr::SEvent & event) override;

        // Inherited via IWindowImpl
        virtual void Focus() const override;
        virtual void Unfocus() const override;
        virtual void Capture() const override;
        virtual void Uncapture() const override;
        virtual void SetActiveWindow() const override;
        virtual void SetParent(IWindowImpl *) const override;
        virtual void SetTimer(uint32 id, uint interval, std::function<void()> const & callback) override;
        virtual void UnsetTimer(uint32 id) override;
    };
}

#endif //!__IRRXUI_WINDOW_IMPL_H__