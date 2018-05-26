#ifndef __SYSTEM_IMAGE_IMAGESOURCE_H__
#define __SYSTEM_IMAGE_IMAGESOURCE_H__

#include <System/System.h>

namespace System::Imaging
{
    enum class PixelFormat : uint8
    {
        Rgb565,
        Rgba8888,
        Bgra8888,
        Rgb888,
        Bgr888
    };

    struct PixelFormatDesc
    {
        PixelFormat Format = PixelFormat::Rgba8888;
        int BitsPerPixel;
        uint32 Mask;
    };

    class TC_CFRAMEWORK_API ImageSource : public System::Reflection::Object
    {
    private:

    public:

        double DpiX = 0.0;
        double DpiY = 0.0;
        double Width = 0.0;
        double Height = 0.0;
        int PixelHeight = 0;
        int PixelWidth = 0;
        int RowBytes = 0;
        int PixelBitSize = 0;
        PixelFormatDesc PixelEntry;
        std::vector<byte> Data;

        ImageSource();
        virtual ~ImageSource();
    };
}

#endif // !__SYSTEM_IMAGE_IMAGESOURCE_H__
