#ifndef __SYSTEM_IMAGE_PNGBITMAPDECODER_H__
#define __SYSTEM_IMAGE_PNGBITMAPDECODER_H__

#include "System/Image/ImageSource.h"
#include "System/Uri.h"

namespace System::Imaging
{
    class TC_CFRAMEWORK_API PngBitmapDecoder : public ImageSource
    {
    private:

    public:

        PngBitmapDecoder();
        virtual ~PngBitmapDecoder();

        bool Decode(System::URI const& uri);
        bool Decode(std::vector<byte> const& stream);
    };
}

#endif // !__SYSTEM_IMAGE_PNGBITMAPDECODER_H__