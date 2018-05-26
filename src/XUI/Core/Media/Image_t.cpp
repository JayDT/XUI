#include "Image_t.h"
#include "Platform/UIRenderer.h"
#include "Platform/UIService.h"

using namespace XUI;
using namespace XUI::Core::Media;

XUI::Core::Media::Image_t::Image_t()
{
}

XUI::Core::Media::Image_t::~Image_t()
{
}

std::shared_ptr<Interfaces::IBitmapImpl> XUI::Core::Media::Image_t::CreateBitmapFromImageSource(System::Imaging::ImageSource * src)
{
    Interfaces::IPlatformRenderInterface* platformService = Platform::UIService::Instance()->Get<Interfaces::IPlatformRenderInterface>();

    auto bitmap = platformService->CreateWritableBitmap(src->Width, src->Height, src->PixelEntry.Format);
    Interfaces::IFramebuffer* buffer = bitmap->Lock();
    if (!buffer)
        return nullptr;

    
    System::ThrowIfFailed<System::InvalidOperationException>(size_t(buffer->RowBytes() * buffer->Height()) == size_t(src->RowBytes * src->Height));

    byte* data = buffer->Address();
    memcpy(data, src->Data.data(), size_t(src->RowBytes * src->Height));
    bitmap->Unlock();
    return bitmap;
}
