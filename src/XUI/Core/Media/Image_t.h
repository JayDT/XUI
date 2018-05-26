#ifndef __SYSTEM_IMAGE_IMAGE_T_H__
#define __SYSTEM_IMAGE_IMAGE_T_H__

#include <System/Image/ImageSource.h>
#include "Interfaces/IBitmap.h"

namespace XUI::Core::Media
{
    class TC_XUI_API Image_t : public System::Reflection::Object
    {
    private:

        std::shared_ptr<System::Imaging::ImageSource> _source;

    public:

        Image_t();
        virtual ~Image_t();

        void GetSource(std::shared_ptr<System::Imaging::ImageSource> const& src) { _source = src; }
        std::shared_ptr<System::Imaging::ImageSource>& GetSource() { return _source; }

        static std::shared_ptr<Interfaces::IBitmapImpl> CreateBitmapFromImageSource(System::Imaging::ImageSource* src);
    };
}

#endif // !__SYSTEM_IMAGE_IMAGE_T_H__
