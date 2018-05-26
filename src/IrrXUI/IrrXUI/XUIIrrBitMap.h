#ifndef __WCLIENT_XUI_PLATFORM_BITMAP_H__
#define __WCLIENT_XUI_PLATFORM_BITMAP_H__

#include "Reflection/Runtime/CXXReflection.h"
#include "Reflection/Runtime/Variant.h"
#include "Reflection/Runtime/Object.h"

#include "XUI/Core/Media/MediaTypes.h"
#include "XUI/Core/Media/Geometries/PathGeometry.h"
#include "XUI/Interfaces/IBitmap.h"

#include "IrrLicht/include/IVideoDriver.h"

namespace irr
{
    class IrrlichtDevice;

    namespace video
    {
        class IImage;
        class ITexture;
        class IVideoDriver;
    }
}

namespace IrrBackend
{
    class XUIIrrDrawingContext;

    class XUIIrrBitMap : public XUI::Interfaces::IBitmapImpl, public XUI::Interfaces::IFramebuffer
    {
	private:

        irr::video::IImage* m_image = nullptr;
        irr::video::ITexture* m_renderTarget = nullptr;
        irr::video::IVideoDriver* GPUDriver = nullptr;

    public:
        XUIIrrBitMap(irr::video::IImage* image);
        virtual ~XUIIrrBitMap();

        irr::video::IImage* GetImageContext() const { return m_image; }
        irr::video::ITexture* GetRenderContext(XUIIrrDrawingContext* context);

        void Unlock() override;

        // Inherited via IBitmapImpl
        virtual int PixelWidth() override;
        virtual int PixelHeight() override;
        virtual void Save(std::string const & fileName) override;
        virtual void Save(std::vector<uint8_t> const & stream) override;
        virtual XUI::Interfaces::IFramebuffer * Lock() override;

        // Inherited via IFramebuffer
        virtual uint8_t * Address() override;
        virtual int Width() override;
        virtual int Height() override;
        virtual int RowBytes() override;
        virtual XUI::Core::Media::Size Dpi() override;
        virtual System::Imaging::PixelFormat Format() override;
    };
}

#endif //!__WCLIENT_XUI_PLATFORM_BITMAP_H__