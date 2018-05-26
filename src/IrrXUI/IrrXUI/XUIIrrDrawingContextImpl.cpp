#include "XUIIrrDrawingContextImpl.h"
#include "XUIIrrRenderer.h"
#include "XUIIrrGeometry.h"
#include "XUIIrrBitMap.h"
#include "XUIIrrFormatedText.h"
#include "XUIIrrWindowImpl.h"

#include "XUI/Platform/UIService.h"
#include "XUI/Core/Media/Pen.h"
#include "XUI/Core/Media/Brush.h"
#include "XUI/Core/Media/VisualBrush.h"
#include "XUI/Core/Input/KeyboardDevice.h"
#include "XUI/Core/Input/MouseDevice.h"
#include "XUI/Core/Input/Cursors.h"
#include "XUI/Interfaces/IFramebuffer.h"
#include "XUI/Interfaces/IBitmap.h"

#include "IrrLicht/include/irrlicht.h"
#include "IrrLicht/CImage.h"

#include "XUI/Core/GlobalInitializerScheduler.h"
#include "XUI/Interfaces/IInputDevices.h"

#include "XUI/Interfaces/IBitmap.h"
#include "XUI/Core/Media/Pen.h"
#include "XUI/Platform/UIRenderer.h"

#include <glm/simd/common.h>

using namespace XUI;
using namespace XUI::Interfaces;

irr::video::SColor GetIrrColorFrom(XUI::Interfaces::IBrush* bursh, IrrBackend::XUIIrrDrawingContext* context)
{
    irr::video::SColor color(bursh ? 255 * bursh->GetOpacity() : 255, 0, 0, 0);

    if (bursh)
    {
        Interfaces::BurshTypes type = bursh->GetTypeId();
        switch (type)
        {
            case Interfaces::BurshTypes::SOLID_COLOR:
            {
                XUI::Core::Media::SolidColorBrush* scb = static_cast<Core::Media::SolidColorBrush*>(bursh);
                color.setRed(scb->Color.color.r);
                color.setGreen(scb->Color.color.g);
                color.setBlue(scb->Color.color.b);
                break;
            }
            case Interfaces::BurshTypes::VISUAL_BRUSH:
            {
                XUI::Core::Media::VisualBrush* scb = static_cast<Core::Media::VisualBrush*>(bursh);
                scb->Render(context);
                color.setAlpha(0x0);
                color.setRed(0x0);
                color.setGreen(0x0);
                color.setBlue(0x00);
                break;
            }
        }
    }
    else
    {
        color.setRed(0xFF);
        color.setGreen(0xFF);
        color.setBlue(0xFF);
    }

    return color;
}

IrrBackend::XUIIrrDrawingContext::XUIIrrDrawingContext(irr::IrrlichtDevice * context)
    : m_transform(XUI::Core::Media::Matrix::Identity())
    , m_context(context)
{
}

IrrBackend::XUIIrrDrawingContext::~XUIIrrDrawingContext()
{
}

void IrrBackend::XUIIrrDrawingContext::Clear(Core::Media::Color const & color)
{
    //m_context->getVideoDriver()->beginScene();
}

void IrrBackend::XUIIrrDrawingContext::BeginScene()
{
    bool success = m_context->run();
    if (!success)
        throw System::TargetInvocationException("");

    // This can be cache to framebuffer
    m_context->getVideoDriver()->beginScene();
}

void IrrBackend::XUIIrrDrawingContext::EndScene()
{
    // This can be cache to framebuffer
    m_context->getVideoDriver()->endScene();
}

void IrrBackend::XUIIrrDrawingContext::SetCursor(XUI::Interfaces::PlatformHandle const & cursor)
{
    GetContext()->getCursorControl()->setActiveIcon((irr::gui::ECURSOR_ICON)cursor.Handle);
}

void IrrBackend::XUIIrrDrawingContext::DrawImage(IBitmapImpl * source, double opacity, Core::Media::Rect const & sourceRect, Core::Media::Rect const & destRect)
{
    XUIIrrBitMap* bitmap = static_cast<XUIIrrBitMap*>(source);
    irr::core::recti dst(irr::core::vector2di(destRect.x0(), destRect.y0()), irr::core::vector2di(destRect.x1(), destRect.y1()));
    irr::core::recti src(irr::core::vector2di(sourceRect.x0(), sourceRect.y0()), irr::core::vector2di(sourceRect.x1(), sourceRect.y1()));
    
    irr::video::ITexture* texture = bitmap->GetRenderContext(this);
    if (texture)
    {
        irr::core::recti* clip = nullptr;
        if (!m_clipRect.empty())
            clip = &m_clipRect.top();
        m_context->getVideoDriver()->draw2DImage(texture, dst, src, clip, nullptr, true);
    }
}

void IrrBackend::XUIIrrDrawingContext::DrawLine(Core::Media::Pen const & pen, Core::Media::Point const & p1, Core::Media::Point const & p2)
{
    irr::core::vector2di start(p1.x, p1.y);
    irr::core::vector2di end(p2.x, p2.y);
    irr::video::SColor color(GetIrrColorFrom(const_cast<Core::Media::Pen&>(pen).Brush, this));
    irr::core::recti* clip = nullptr;
    if (!m_clipRect.empty())
    {
        clip = &m_clipRect.top();

        if (start.X > clip->LowerRightCorner.X)
            start.X = clip->LowerRightCorner.X;
        if (start.Y > clip->LowerRightCorner.Y)
            start.Y = clip->LowerRightCorner.Y;

        if (end.X > clip->LowerRightCorner.X)
            end.X = clip->LowerRightCorner.X;
        if (end.Y > clip->LowerRightCorner.Y)
            end.Y = clip->LowerRightCorner.Y;

        if (start.X < clip->UpperLeftCorner.X)
            start.X = clip->UpperLeftCorner.X;
        if (start.Y < clip->UpperLeftCorner.Y)
            start.Y = clip->UpperLeftCorner.Y;

        if (end.X < clip->UpperLeftCorner.X)
            end.X = clip->UpperLeftCorner.X;
        if (end.Y < clip->UpperLeftCorner.Y)
            end.Y = clip->UpperLeftCorner.Y;
    }

    //irr::video::SMaterial m;
    //m.AmbientColor = color;
    //m.AnimateColor = m.AmbientColor;
    //m.FrontfaceCulling = false;
    //m.BackfaceCulling = false;
    //m.AntiAliasing = 1;
    //m.Thickness = 1;
    //m_context->getVideoDriver()->setMaterial(m);

    m_context->getVideoDriver()->draw2DLine(start, end, color);
}

void IrrBackend::XUIIrrDrawingContext::DrawGeometry(IBrush * brush, Core::Media::Pen const & pen, IGeometry * geometry)
{
    XUIIrrGeometry* _geometry = static_cast<XUIIrrGeometry*>(geometry->GetStreamGeometryContext());
    
    if (!_geometry->GetBuffer().GetHWBuffer())
        m_context->getVideoDriver()->CreateHardwareBuffer(&_geometry->GetBuffer());
    
    auto color = GetIrrColorFrom(brush, this);

    if (color.getAlpha() == 0)
        return;
    
    if (_geometry->GetBuffer().GetSubBufferCount() == 0)
    {
        irr::video::SMaterial& m = const_cast<irr::video::SMaterial&>(_geometry->GetBuffer().getMaterial());
        m.MaterialType = irr::video::E_MATERIAL_TYPE::EMT_SOLID_2_LAYER;
        m.AmbientColor = color;
        //m.Wireframe = true;
        m_context->getVideoDriver()->setMaterial(m);
    
        m_context->getVideoDriver()->drawMeshBuffer(&_geometry->GetBuffer());
    }
    else
    {
        for (int i = 0; i != _geometry->GetBuffer().GetSubBufferCount(); ++i)
        {
            irr::video::SMaterial& m = const_cast<irr::video::SMaterial&>(_geometry->GetBuffer().getMaterial());
            m.MaterialType = irr::video::E_MATERIAL_TYPE::EMT_SOLID_2_LAYER;
            m.AmbientColor = color;
            //m.Wireframe = true;
            m_context->getVideoDriver()->setMaterial(m);
    
            const_cast<irr::scene::CMeshBufferEx<irr::video::S3DVertex>&>(_geometry->GetBuffer()).SetActiveSubBuffer(i);
            m_context->getVideoDriver()->drawMeshBuffer(&_geometry->GetBuffer());
        }
    
    }
}

void IrrBackend::XUIIrrDrawingContext::DrawRectangle(Core::Media::Pen const & pen, Core::Media::Rect const & rect, float cornerRadius /*= 0.0f*/)
{
    irr::core::recti _rect(irr::core::vector2di(rect.x0(), rect.y0()), irr::core::vector2di(rect.x1(), rect.y1()));
    irr::video::SColor _color(GetIrrColorFrom(const_cast<Core::Media::Pen&>(pen).Brush, this));
    irr::core::recti* clip = nullptr;
    if (!m_clipRect.empty())
        clip = &m_clipRect.top();

#ifdef _DEBUG
    assert(isfinite(rect.x0()));
    assert(isfinite(rect.x1()));
    assert(isfinite(rect.y0()));
    assert(isfinite(rect.y1()));
#endif

    // cornerRadius != 0.0f implement rounded rectangle
    m_context->getVideoDriver()->draw2DRectangle(_color, _rect, clip, false);
}

void IrrBackend::XUIIrrDrawingContext::DrawText(IBrush * foreground, Core::Media::Point const & origin, IFormattedText * text)
{
    irr::core::recti* clip = nullptr;
    if (!m_clipRect.empty())
        clip = &m_clipRect.top();

    //irr::core::matrix4& view_mat = const_cast<irr::core::matrix4&>(m_context->getVideoDriver()->getTransform(irr::video::E_TRANSFORMATION_STATE::ETS_VIEW_2D));
    //auto old_trans
    static_cast<XUIIrrFormatedText*>(text)->Draw(this, foreground, clip);
}

void IrrBackend::XUIIrrDrawingContext::FillRectangle(IBrush * brush, Core::Media::Rect const & rect, float cornerRadius /*= 0.0f*/)
{
    irr::core::recti _rect(irr::core::vector2di(rect.x0(), rect.y0()), irr::core::vector2di(rect.x1(), rect.y1()));
    irr::video::SColor _color(GetIrrColorFrom(brush, this));
    irr::core::recti* clip = nullptr;
    if (!m_clipRect.empty())
        clip = &m_clipRect.top();

#ifdef _DEBUG
    assert(isfinite(rect.x0()));
    assert(isfinite(rect.x1()));
    assert(isfinite(rect.y0()));
    assert(isfinite(rect.y1()));
#endif

    // cornerRadius != 0.0f implement rounded rectangle
    m_context->getVideoDriver()->draw2DRectangle(_color, _rect, clip, true);
}

void IrrBackend::XUIIrrDrawingContext::PushClip(Core::Media::Rect const & clip)
{
    auto clipRect = irr::core::recti(irr::core::vector2di(clip.x0(), clip.y0()), irr::core::vector2di(clip.x1(), clip.y1()));
    m_clipRect.push(clipRect);
}

void IrrBackend::XUIIrrDrawingContext::PopClip()
{
    m_clipRect.pop();
}

void IrrBackend::XUIIrrDrawingContext::PushOpacity(double opacity)
{
    m_opacity.push(opacity);
}

void IrrBackend::XUIIrrDrawingContext::PopOpacity()
{
    m_opacity.pop();
}

void IrrBackend::XUIIrrDrawingContext::PushOpacityMask(IBrush * mask, Core::Media::Rect const & bounds)
{
    //Not implement yet
}

void IrrBackend::XUIIrrDrawingContext::PopOpacityMask()
{
}

void IrrBackend::XUIIrrDrawingContext::PushGeometryClip(IGeometry * clip)
{
    m_geometry.push(clip);
}

void IrrBackend::XUIIrrDrawingContext::PopGeometryClip()
{
    m_geometry.pop();
}

const XUI::Core::Media::Matrix & IrrBackend::XUIIrrDrawingContext::GetTransform() const
{
    return m_transform;
}

void IrrBackend::XUIIrrDrawingContext::SetTransform(const XUI::Core::Media::Matrix & value)
{
    m_transform = value;
#ifdef _DEBUG
    for (int i = 0; i < 16; ++i)
        assert(isfinite((&value[0].x)[i]));
#endif
    memcpy(m_irr_transform.pointer(), &value[0].x, sizeof(value[0].x) * 4 * 4);
    m_context->getVideoDriver()->setTransform(irr::video::E_TRANSFORMATION_STATE::ETS_VIEW_2D, m_irr_transform);
}

void IrrBackend::XUIIrrDrawingContext::PushTransform(Core::Media::Matrix const& mat, bool relative)
{
    m_transformStack.push(Transform);
    if (relative)
        Transform = Core::Media::Matrix(Transform * mat);
    else
        Transform = mat;
}

void IrrBackend::XUIIrrDrawingContext::PopTransform()
{
    Transform = m_transformStack.top();
    m_transformStack.pop();
}

std::shared_ptr<XUI::Interfaces::IBitmapImpl> IrrBackend::XUIIrrDrawingContext::LoadBitmap(const std::string &fileName)
{
    irr::video::IImage* image = GetContext()->getVideoDriver()->createImageFromFile(fileName.c_str());
    auto bitmap = std::make_shared<XUIIrrBitMap>(image);
    image->drop();
    return bitmap;
}

std::shared_ptr<XUI::Interfaces::IBitmapImpl> IrrBackend::XUIIrrDrawingContext::LoadBitmap(const std::vector<byte, std::allocator<byte>> &stream)
{
    //TODO: not implement yet
    return nullptr;
}

std::shared_ptr<XUI::Interfaces::IBitmapImpl> IrrBackend::XUIIrrDrawingContext::LoadBitmap(System::Imaging::PixelFormat format, byte *data, int width, int height, int stride)
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
    }

    irr::video::CImage* image = new irr::video::CImage(irr_format, irr::core::dimension2du(width, height));
    void* image_data = image->lock();
    memcpy(image_data, data, image->getImageDataSizeInBytes());
    auto bitmap = std::make_shared<XUIIrrBitMap>(image);
    image->drop();

    return bitmap;
}
