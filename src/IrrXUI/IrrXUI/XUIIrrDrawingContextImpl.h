#ifndef __WCLIENT_XUI_PLATFORM_DRAWING_CONTEXT
#define __WCLIENT_XUI_PLATFORM_DRAWING_CONTEXT

#include "Reflection/Runtime/CXXReflection.h"
#include "Reflection/Runtime/Variant.h"
#include "Reflection/Runtime/Object.h"

#include "XUI/Interfaces/IDrawingContext.h"

#include "IrrLicht/include/rect.h"
#include "IrrLicht/include/matrix4.h"

#include <stack>

#undef LoadBitmap

MetaAssemblyBegin
Meta(modul: ReflInclude("XUI/Core/Media/Pen.h"))
Meta(modul: ReflInclude("XUI/Interfaces/IBitmap.h"))
Meta(modul: ReflInclude("XUI/Interfaces/IStandardCursorFactory.h"))
MetaAssemblyEnd

namespace irr
{
    class IrrlichtDevice;
}

namespace XUI
{
    namespace Interfaces
    {
        struct IGeometry;
        struct IBitmapImpl;
    }
}

namespace IrrBackend
{
    class XUIIrrDrawingContext : public XUI::Interfaces::IDrawingContextImpl
    {
    private:
        Meta(Disable) irr::IrrlichtDevice* m_context;

        std::stack<irr::core::recti> m_clipRect;
        std::stack<double> m_opacity;
        std::stack<XUI::Interfaces::IGeometry*> m_geometry;
        std::stack<XUI::Core::Media::Matrix> m_transformStack;

        XUI::Core::Media::Matrix m_transform;
        irr::core::matrix4 m_irr_transform;

    public:

        XUIIrrDrawingContext(irr::IrrlichtDevice* context);
        virtual ~XUIIrrDrawingContext();

        Meta(Disable) irr::IrrlichtDevice* GetContext() const { return m_context; }

        // Inherited via IDrawingContextImpl
        virtual void BeginScene();
        virtual void EndScene();
        virtual void SetCursor(XUI::Interfaces::PlatformHandle const& cursor) override;
        virtual void Clear(XUI::Core::Media::Color const & color) override;
        virtual void DrawImage(XUI::Interfaces::IBitmapImpl * source, double opacity, XUI::Core::Media::Rect const & sourceRect, XUI::Core::Media::Rect const & destRect) override;
        virtual void DrawLine(XUI::Core::Media::Pen const & pen, XUI::Core::Media::Point const & p1, XUI::Core::Media::Point const & p2) override;
        virtual void DrawGeometry(XUI::Interfaces::IBrush * brush, XUI::Core::Media::Pen const & pen, XUI::Interfaces::IGeometry * geometry) override;
        virtual void DrawRectangle(XUI::Core::Media::Pen const & pen, XUI::Core::Media::Rect const & rect, float cornerRadius = 0.0f) override;
        virtual void DrawText(XUI::Interfaces::IBrush * foreground, XUI::Core::Media::Point const & origin, XUI::Interfaces::IFormattedText * text) override;
        virtual void FillRectangle(XUI::Interfaces::IBrush * brush, XUI::Core::Media::Rect const & rect, float cornerRadius = 0.0f) override;
        virtual void PushClip(XUI::Core::Media::Rect const & clip) override;
        virtual void PopClip() override;
        virtual void PushOpacity(double opacity) override;
        virtual void PopOpacity() override;
        virtual void PushOpacityMask(XUI::Interfaces::IBrush * mask, XUI::Core::Media::Rect const & bounds) override;
        virtual void PopOpacityMask() override;
        virtual void PushGeometryClip(XUI::Interfaces::IGeometry * clip) override;
        virtual void PopGeometryClip() override;
        virtual const XUI::Core::Media::Matrix & GetTransform() const override;
        virtual void SetTransform(const XUI::Core::Media::Matrix &) override;
        virtual void PushTransform(XUI::Core::Media::Matrix const& mat, bool relative) override;
        virtual void PopTransform() override;
        virtual std::shared_ptr<XUI::Interfaces::IBitmapImpl> LoadBitmap(const std::string &fileName) override;
        virtual std::shared_ptr<XUI::Interfaces::IBitmapImpl> LoadBitmap(const std::vector<byte> &stream) override;
        virtual std::shared_ptr<XUI::Interfaces::IBitmapImpl> LoadBitmap(System::Imaging::PixelFormat format, byte *data, int width, int height, int stride) override;
    };
}

#endif //!__WCLIENT_XUI_PLATFORM_DRAWING_CONTEXT