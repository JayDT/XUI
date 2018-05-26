#include "XUIIrrBitMap.h"
#include "XUIIrrWindowImpl.h"

#include "irrlicht.h"
#include "IrrLicht/CImage.h"
#include "ITexture.h"

/// ToDo: implement this

IrrBackend::XUIIrrBitMap::XUIIrrBitMap(irr::video::IImage * image)
    : m_image(image)
{
    if (m_image)
        m_image->grab();
}

IrrBackend::XUIIrrBitMap::~XUIIrrBitMap()
{
    if (m_image)
        m_image->drop();
}

irr::video::ITexture * IrrBackend::XUIIrrBitMap::GetRenderContext(XUIIrrDrawingContext* context)
{
    auto _GPUDriver = context->GetContext()->getVideoDriver();
    if (GPUDriver != _GPUDriver)
    {
        GPUDriver = _GPUDriver;
        if (m_renderTarget)
            m_renderTarget->drop();
        m_renderTarget = _GPUDriver->createTexture(m_image, "");
    }
    return m_renderTarget;
}

void IrrBackend::XUIIrrBitMap::Unlock()
{
}

int IrrBackend::XUIIrrBitMap::PixelWidth()
{
    return m_image->getDimension().Width;
}

int IrrBackend::XUIIrrBitMap::PixelHeight()
{
    return m_image->getDimension().Height;
}

void IrrBackend::XUIIrrBitMap::Save(std::string const & fileName)
{
}

void IrrBackend::XUIIrrBitMap::Save(std::vector<uint8_t> const & stream)
{
}

XUI::Interfaces::IFramebuffer * IrrBackend::XUIIrrBitMap::Lock()
{
    return this;
}

uint8_t * IrrBackend::XUIIrrBitMap::Address()
{
    return m_image->GetData();
}

int IrrBackend::XUIIrrBitMap::Width()
{
    return PixelWidth();
}

int IrrBackend::XUIIrrBitMap::Height()
{
    return PixelHeight();
}

int IrrBackend::XUIIrrBitMap::RowBytes()
{
    return m_image->getPitch();
}

XUI::Core::Media::Size IrrBackend::XUIIrrBitMap::Dpi()
{
    return XUI::Core::Media::Size(96, 96);
}

System::Imaging::PixelFormat IrrBackend::XUIIrrBitMap::Format()
{
    switch (m_image->getColorFormat())
    {
        case irr::video::ECOLOR_FORMAT::ECF_R5G6B5:
            return System::Imaging::PixelFormat::Rgb565;
        case irr::video::ECOLOR_FORMAT::ECF_RGBA8:
            return System::Imaging::PixelFormat::Rgba8888;
        //case ??:
        //    return System::Imaging::PixelFormat::Bgra8888;
    }
    return System::Imaging::PixelFormat::Bgra8888;
}
