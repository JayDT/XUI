#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_DIRECT3D_11_

#define _IRR_DONT_DO_MEMORY_DEBUGGING_HERE

#include "irrTypes.h"
#include "CD3D11Driver.h"
#include "CD3D11Texture.h"
#include "os.h"
#include "CColorConverter.h"

#include <memory>

#define SAFE_RELEASE(x) \
    if(x)				\
        x->Release(); 	\
    x = 0;
#include <assert.h>

static DWORD STATHREADiD = 0;

namespace irr
{
namespace video
{

static void GetSurfaceInfo(_In_ CD3D11Driver* driver, _In_ size_t width,
        _In_ size_t height,
        _In_ DXGI_FORMAT fmt,
        _Out_opt_ size_t* outNumBytes,
        _Out_opt_ size_t* outRowBytes,
        _Out_opt_ size_t* outNumRows);

//! rendertarget constructor
CD3D11Texture::CD3D11Texture(CD3D11Driver* driver, const core::dimension2d<u32>& size,
                           const io::path& name, const ECOLOR_FORMAT format, u32 arraySlices,
                           u32 sampleCount, u32 sampleQuality)
: ITexture(name)
, Device(0)
, ImmediateContext(0)
, Texture(0)
, TextureBuffer(0)
, RTView(0)
, SRView(0)
, TextureDimension(D3D11_RESOURCE_DIMENSION_TEXTURE2D)
, LastMapDirection((D3D11_MAP)0)
, Driver(driver)
, DepthSurface(0)
, TextureSize(size)
, ImageSize(size)
, Pitch(0)
, MappedPitch(0)
, MipLevelLocked(0)
, ArraySliceLocked(0)
, NumberOfMipLevels(0)
, NumberOfArraySlices(arraySlices)
, SampleCount(sampleCount)
, SampleQuality(sampleQuality)
, ColorFormat(ECF_UNKNOWN)
, Image(nullptr)
, HasMipMaps(false)
, HardwareMipMaps(false)
, IsRenderTarget(true)
{
    if (!STATHREADiD)
        STATHREADiD = GetCurrentThreadId();
    assert(STATHREADiD == GetCurrentThreadId());

    #ifdef _DEBUG
    setDebugName("CD3D11Texture");
    #endif

    bool success = InitializeColorFormat(0, format);
    _IRR_DEBUG_BREAK_IF(!success);

    Device=driver->getExposedVideoData().D3D11.D3DDev11;
    if (Device)
    {
        Device->AddRef();
    }

    Device->GetImmediateContext( &ImmediateContext );

    createRenderTarget(format);
}


//! constructor
CD3D11Texture::CD3D11Texture(IImage* image, CD3D11Driver* driver,
               u32 flags, const io::path& name, u32 arraySlices, void* mipmapData, u32 sampleCount, u32 sampleQuality)
: ITexture(name)
, Device(0)
, ImmediateContext(0)
, Texture(0)
, TextureBuffer(0)
, RTView(0)
, SRView(0)
, TextureDimension(D3D11_RESOURCE_DIMENSION_TEXTURE2D)
, LastMapDirection((D3D11_MAP)0)
, Driver(driver)
, DepthSurface(0)
, TextureSize(0,0)
, ImageSize(0,0)
, Pitch(0)
, MappedPitch(0)
, MipLevelLocked(0)
, ArraySliceLocked(0)
, NumberOfMipLevels(0)
, NumberOfArraySlices(arraySlices)
, SampleCount(sampleCount)
, SampleQuality(sampleQuality)
, ColorFormat(ECF_UNKNOWN)
, Image(nullptr)
, HasMipMaps(false)
, HardwareMipMaps(false)
, IsRenderTarget(false)
{
    #ifdef _DEBUG
    setDebugName("CD3D11Texture");
    #endif

    if (!STATHREADiD)
        STATHREADiD = GetCurrentThreadId();
    assert(STATHREADiD == GetCurrentThreadId());

    bool success = InitializeColorFormat(flags, image->getColorFormat());
    _IRR_DEBUG_BREAK_IF(!success);

    switch (image->getColorFormat())
    {
    case ECF_RGBA8:
    case ECF_RGBA_S3TC_DXT1:
    case ECF_RGBA_S3TC_DXT3:
    case ECF_RGBA_S3TC_DXT5:
        ColorFormat = image->getColorFormat();
        Image = image;
        image->grab();
        break;
    default:
        Image = Driver->createImage(ColorFormat, image->getDimension());
        image->copyTo(Image);
        break;
    }

    HasMipMaps = Driver->getTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS);

    Device=driver->getExposedVideoData().D3D11.D3DDev11;
    if (Device)
    {
        Device->AddRef();
    }

    Device->GetImmediateContext( &ImmediateContext );

    // Load a dds file
    //if (core::hasFileExtension(name, "dds"))
    //{
    //    
    //}

    if (image)
    {
        if (createTexture(flags, Image, mipmapData))
        {
            copyTexture(Image);
            regenerateMipMapLevels(mipmapData);
        }
        else
            os::Printer::log("Could not create DIRECT3D11 Texture.", ELL_WARNING);
    }

    if ( Image )
    {
        Image->drop();
        Image = nullptr;
    }

}


//! destructor
CD3D11Texture::~CD3D11Texture()
{	
    if (DepthSurface)
    {
        if (DepthSurface->drop())
            Driver->removeDepthSurface(DepthSurface);
    }

    SAFE_RELEASE(RTView);
    SAFE_RELEASE(SRView);
    SAFE_RELEASE(Texture);
    SAFE_RELEASE(TextureBuffer);

    SAFE_RELEASE(ImmediateContext);
    SAFE_RELEASE(Device);
}

//! return texture resource
ID3D11Resource* CD3D11Texture::getTextureResource() const
{
    return Texture;
}

//! return render target view
ID3D11RenderTargetView* CD3D11Texture::getRenderTargetView() const
{
    return RTView;
}

//! return shader resource view
ID3D11ShaderResourceView* CD3D11Texture::getShaderResourceView() const
{
    // Emulate "auto" mipmap generation
    if( IsRenderTarget && SRView )
        ImmediateContext->GenerateMips( SRView );

    return SRView;
}

//! lock function
void* CD3D11Texture::lock(E_TEXTURE_LOCK_MODE mode /*= ETLM_READ_WRITE*/, u32 mipmapLevel /*= 0*/, u32 arraySlice /*= 0*/) //bool readOnly, u32 mipmapLevel, u32 arraySlice)
{
    if(!Texture || !createTextureBuffer())
        return 0;

    if (!STATHREADiD)
        STATHREADiD = GetCurrentThreadId();
    assert(STATHREADiD == GetCurrentThreadId());

    HRESULT hr = S_OK;

    switch ( mode )
    {
        case ETLM_READ_WRITE:
        case ETLM_WRITE_ONLY:
        {
            //if (!dynamic)
            //    break;

            if ( !Image )
                Image = Driver->createImage(ColorFormat, TextureSize);
            return Image->GetData();
        }
        case ETLM_READ_ONLY:
        {
            // Record mip level locked to use in unlock
            MipLevelLocked = mipmapLevel;
            ArraySliceLocked = arraySlice;

            // set map direction
            if ( mode == ETLM_READ_ONLY )
                LastMapDirection = D3D11_MAP_READ;
            else
                LastMapDirection = (D3D11_MAP)(D3D11_MAP_READ | D3D11_MAP_WRITE);

            // if read, and this is a render target texture (i.ex.: GPU will write data to texture)
            // shall sync data from main texture to texture buffer
            if ( (IsRenderTarget == true) && (LastMapDirection & D3D11_MAP_READ) )
            {
                ImmediateContext->CopyResource(TextureBuffer, Texture);
            }

            UINT subresource = D3D11CalcSubresource(MipLevelLocked,		// mip level to lock
                ArraySliceLocked,		// array slice (only 1 slice for now)
                NumberOfMipLevels); 	// number of mip levels

            // Map texture buffer
            D3D11_MAPPED_SUBRESOURCE mappedData;
            hr = ImmediateContext->Map(TextureBuffer,
                subresource,
                LastMapDirection, 							// direction to map
                0,
                &mappedData);								// mapped result

            if ( FAILED(hr) )
                return 0;

            MappedPitch = mappedData.RowPitch;

            return mappedData.pData;
        }
    }

    return nullptr;
}

//! unlock function
void CD3D11Texture::unlock()
{
    if (!Texture)
        return;

    if (!STATHREADiD)
        STATHREADiD = GetCurrentThreadId();
    assert(STATHREADiD == GetCurrentThreadId());

    switch ( LastMapDirection )
    {
        case ETLM_READ_WRITE:
        case ETLM_WRITE_ONLY:
        {
            if ( Image )
            {
                size_t numBytes, RowBytes, numRows;
                GetSurfaceInfo(Driver, Image->getDimension().Width, Image->getDimension().Height, Driver->getD3DFormatFromColorFormat(ColorFormat), &numBytes, &RowBytes, &numRows);

                ArraySliceLocked = 0;
                UINT res = D3D11CalcSubresource(0, ArraySliceLocked, NumberOfMipLevels);
                ImmediateContext->UpdateSubresource(Texture, res, nullptr, Image->GetData(), static_cast<UINT>(RowBytes), static_cast<UINT>(numBytes));
            }
            break;
        }
        case ETLM_READ_ONLY:
        {
            // unlock texture buffer
            ImmediateContext->Unmap( TextureBuffer, D3D11CalcSubresource(MipLevelLocked, ArraySliceLocked, NumberOfMipLevels) );
            
            // copy texture buffer to main texture ONLY if buffer was write
            if (LastMapDirection & D3D11_MAP_WRITE)
            {
                ImmediateContext->CopyResource( Texture, TextureBuffer );
            }
        }
    }
}

//! Returns original size of the texture.
const core::dimension2d<u32>& CD3D11Texture::getOriginalSize() const
{
    return ImageSize;
}

//! Returns (=size) of the texture.
const core::dimension2d<u32>& CD3D11Texture::getSize() const
{
    return TextureSize;
}

//! returns driver type of texture (=the driver, who created the texture)
E_DRIVER_TYPE CD3D11Texture::getDriverType() const
{
    return EDT_DIRECT3D11;
}

//! returns color format of texture
ECOLOR_FORMAT CD3D11Texture::getColorFormat() const
{
    return ColorFormat;
}

u32 CD3D11Texture::getPitch() const
{
    return Pitch;
}

//! returns if texture has mipmap levels
bool CD3D11Texture::hasMipMaps() const
{
    return HasMipMaps;
}

u32 CD3D11Texture::getNumberOfArraySlices() const
{
    return NumberOfArraySlices;
}

//! Regenerates the mip map levels of the texture. Useful after locking and
//! modifying the texture
void CD3D11Texture::regenerateMipMapLevels(void* mipmapData)
{
    if ( hasMipMaps() && !HardwareMipMaps && mipmapData)
    {
        core::dimension2du size = TextureSize;
        u32 level = 0;
        DXGI_FORMAT d3dformat = Driver->getD3DFormatFromColorFormat(ColorFormat);

        u8* target = static_cast<u8*>(mipmapData);
        do
        {
            if (size.Width>1)
                size.Width /= 2;
            if (size.Height>1)
                size.Height /= 2;
            ++level;

            //Texture->

            u32 imageSize = *(u32*)target;//width*height*Image->getBytesPerPixel();
            target += sizeof(u32);

            size_t numBytes, RowBytes, numRows;
            GetSurfaceInfo(Driver, size.Width, size.Height, Driver->getD3DFormatFromColorFormat(ColorFormat), &numBytes, &RowBytes, &numRows);

            //_IRR_DEBUG_BREAK_IF(imageSize != numBytes);

            ArraySliceLocked = 0;
            UINT res = D3D11CalcSubresource(level, ArraySliceLocked, NumberOfMipLevels);
            ImmediateContext->UpdateSubresource(Texture, res, nullptr, target, static_cast<UINT>(RowBytes), static_cast<UINT>(imageSize));

            // get next prepared mipmap data if available
            if (mipmapData)
            {
                target += imageSize;
                if (!*(u32*)target)
                    break;
            }
        } while (size.Width != 1 || size.Height != 1);
    }
    else if ( hasMipMaps() )
    {
        // create mip maps.
        createMipMaps();
    }
}

bool CD3D11Texture::createMipMaps(u32 level)
{
    if (level == 0)
        return true;

    if (SRView && HardwareMipMaps)
    {
        // generate mipmaps in hardware
        ImmediateContext->GenerateMips(SRView);
        return true;
    }
    else
    // manual mipmap generation
    {
        core::dimension2du size = TextureSize;
        u32 level = 0;
        DXGI_FORMAT d3dformat = Driver->getD3DFormatFromColorFormat(ColorFormat);
        core::array<char> buffer;
        do
        {
            if ( size.Width>1 )
                size.Width /= 2;
            if ( size.Height>1 )
                size.Height /= 2;

            ++level;

            if ( NumberOfMipLevels <= level )
                break;

            size_t numBytes, RowBytes, numRows;
            GetSurfaceInfo(Driver, size.Width, size.Height, Driver->getD3DFormatFromColorFormat(ColorFormat), &numBytes, &RowBytes, &numRows);
            buffer.set_used(numBytes);

            Image->copyToScaling(buffer.pointer(), size.Width, size.Height, ColorFormat, RowBytes);

            ArraySliceLocked = 0;
            UINT res = D3D11CalcSubresource(level, ArraySliceLocked, NumberOfMipLevels);
            ImmediateContext->UpdateSubresource(Texture, res, nullptr, buffer.pointer(), static_cast<UINT>(RowBytes), static_cast<UINT>(numBytes));

        } while ( size.Width != 1 || size.Height != 1 );
    }

    // generate next level
    return true;
}

//! returns if it is a render target
bool CD3D11Texture::isRenderTarget() const
{
    return IsRenderTarget;
}

void CD3D11Texture::createRenderTarget(const ECOLOR_FORMAT format)
{
    HRESULT hr = S_OK;

    // are texture size restrictions there ?
    if(!Driver->queryFeature(EVDF_TEXTURE_NPOT))
    {
        if (TextureSize != ImageSize)
            os::Printer::log("RenderTarget size has to be a power of two", ELL_INFORMATION);
    }
    TextureSize = TextureSize.getOptimalSize(!Driver->queryFeature(EVDF_TEXTURE_NPOT), !Driver->queryFeature(EVDF_TEXTURE_NSQUARE), true, Driver->getMaxTextureSize().Width);

    DXGI_FORMAT d3dformat = Driver->getD3DColorFormat();

    if(ColorFormat == ECF_UNKNOWN)
    {
        // get irrlicht format from backbuffer
        // (This will get overwritten by the custom format if it is provided, else kept.)
        ColorFormat = Driver->getColorFormat();
        setPitch(d3dformat);

        // Use color format if provided.
        if(format != ECF_UNKNOWN)
        {
            ColorFormat = format;
            d3dformat = Driver->getD3DFormatFromColorFormat(format);
            setPitch(d3dformat); // This will likely set pitch to 0 for now.
        }
    }
    else
    {
        d3dformat = Driver->getD3DFormatFromColorFormat(ColorFormat);
    }

    // creating texture
    D3D11_TEXTURE2D_DESC desc;
    ZeroMemory( &desc, sizeof(D3D11_TEXTURE2D_DESC) );
    desc.ArraySize = NumberOfArraySlices;
    desc.CPUAccessFlags = 0;
    desc.Format = d3dformat;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.SampleDesc.Count = SampleCount;
    desc.SampleDesc.Quality = SampleQuality;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;

    // test if Direct3D support automatic mip map generation
    if ( hasMipMaps() &&
        Driver->querySupportForColorFormat(d3dformat, D3D11_FORMAT_SUPPORT_MIP_AUTOGEN))
    {
        desc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
        desc.MipLevels = 0;
    }
    else
    {
        //desc.MiscFlags = 0;
        desc.MipLevels = 1;
    }

    // If array size == 6, force cube texture
    if (desc.ArraySize == 6)
    {
        desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
    }

    // If multisampled, mip levels shall be 1
    if (desc.SampleDesc.Count > 1)
    {
        desc.MiscFlags &= ~D3D11_RESOURCE_MISC_GENERATE_MIPS;
        desc.MipLevels = 1;
    }
    
    // Texture size
    desc.Width = TextureSize.Width;
    desc.Height = TextureSize.Height;

    // create texture
    hr = Device->CreateTexture2D( &desc, NULL, &Texture );
    if (FAILED(hr))
    {
        if (E_INVALIDARG == hr)
            os::Printer::log("Could not create render target texture", "Invalid Arguments");
        else if (E_OUTOFMEMORY == hr)
            os::Printer::log("Could not create render target texture", "Out of Video Memory");
        else
            os::Printer::log("Could not create render target texture");

        return;
    }

    // Get texture description to update some fields
    Texture->GetDesc( &desc );
    NumberOfMipLevels = desc.MipLevels;
    TextureSize.Width = desc.Width;
    TextureSize.Height = desc.Height;

    // create views
    createViews();
}

//! creates the hardware texture
bool CD3D11Texture::createTexture(u32 flags, IImage* image, void* mipmapData)
{
    if (!STATHREADiD)
        STATHREADiD = GetCurrentThreadId();
    assert(STATHREADiD == GetCurrentThreadId());

    HRESULT hr = S_OK;
    ImageSize = image->getDimension();

    core::dimension2d<u32> optSize = ImageSize.getOptimalSize(!Driver->queryFeature(EVDF_TEXTURE_NPOT), 
                                                              !Driver->queryFeature(EVDF_TEXTURE_NSQUARE), 
                                                              true,
                                                              Driver->getMaxTextureSize().Width);

    TextureSize.Width = optSize.Width;
    TextureSize.Height = optSize.Height;

    DXGI_FORMAT format = Driver->getD3DFormatFromColorFormat(ColorFormat);

    // Check hardware support for automatic mipmap support
    if( hasMipMaps() &&
       Driver->queryFeature(EVDF_MIP_MAP_AUTO_UPDATE))
    {
        UINT support = 0;
        hr = Device->CheckFormatSupport(format, &support);
        if( SUCCEEDED(hr) && support & D3D11_FORMAT_SUPPORT_MIP_AUTOGEN )
            HardwareMipMaps = true;
    }

    D3D11_TEXTURE2D_DESC desc;
    ZeroMemory( &desc, sizeof(D3D11_TEXTURE2D_DESC) );
    desc.ArraySize          = NumberOfArraySlices;
    desc.CPUAccessFlags     = 0;
    desc.Format             = format;
    desc.Usage              = D3D11_USAGE_DEFAULT;
    desc.SampleDesc.Count   = SampleCount;
    desc.SampleDesc.Quality = SampleQuality;
    desc.Width              = optSize.Width;
    desc.Height             = optSize.Height;
    desc.BindFlags          = D3D11_BIND_SHADER_RESOURCE; // | D3D11_BIND_RENDER_TARGET;
    desc.MiscFlags          = 0;

    // test if Direct3D support automatic mip map generation
    // AND creation flag is true
    if ( hasMipMaps() && HardwareMipMaps
        && Driver->querySupportForColorFormat(format, D3D11_FORMAT_SUPPORT_MIP_AUTOGEN))
    {
        desc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
        desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
        desc.MipLevels = 0;
    }
    else if ( hasMipMaps() && mipmapData)
    {
        core::dimension2du size = TextureSize;
        u32 level = 1;
        u8* target = static_cast<u8*>(mipmapData);
        do
        {
            if (size.Width>1)
                size.Width /= 2;
            if (size.Height>1)
                size.Height /= 2;
            ++level;
            u32 imageSize = *(u32*)target;//width*height*Image->getBytesPerPixel();
            target += sizeof(u32);
            if (mipmapData)
            {
                target += imageSize;
                if (!*(u32*)target)
                    break;
            }
        } while (size.Width != 1 || size.Height != 1);
        desc.MipLevels = level;
    }
    else
        desc.MipLevels = 1;		// Set only one mip level if do not support auto mip generation

    // If array size == 6, force cube texture
    if (desc.ArraySize == 6)
    {
        desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
    }

    // If multisampled, mip levels shall be 1
    if (desc.SampleDesc.Count > 1)
    {
        desc.MiscFlags &= ~D3D11_RESOURCE_MISC_GENERATE_MIPS;
        desc.MipLevels = 1;
    }

    size_t numBytes, RowBytes, numRows;
    GetSurfaceInfo(Driver, image->getDimension().Width, image->getDimension().Height, format, &numBytes, &RowBytes, &numRows);

    // create texture
    hr = Device->CreateTexture2D( &desc, nullptr, &Texture );
    if (FAILED(hr))
    {
        if (E_INVALIDARG == hr)
            os::Printer::log("Could not create texture", "Invalid Arguments");
        else if (E_OUTOFMEMORY == hr)
            os::Printer::log("Could not create texture", "Out of Video Memory");
        else
            os::Printer::log("Could not create texture");

        return false;
    }

    // Get texture description to update number of mipmaps
    Texture->GetDesc( &desc );
    NumberOfMipLevels  = desc.MipLevels;
    TextureSize.Width  = desc.Width;
    TextureSize.Height = desc.Height;
    setPitch(format);

    // create views to bound texture to pipeline
    return createViews();
}

//! copies the image to the texture
bool CD3D11Texture::copyTexture(IImage* image)
{
    size_t numBytes, RowBytes, numRows;
    GetSurfaceInfo(Driver, Image->getDimension().Width, Image->getDimension().Height, Driver->getD3DFormatFromColorFormat(ColorFormat), &numBytes, &RowBytes, &numRows);

    ArraySliceLocked = 0;
    UINT res = D3D11CalcSubresource(0, ArraySliceLocked, NumberOfMipLevels);
    ImmediateContext->UpdateSubresource(Texture, res, nullptr, Image->GetData(), static_cast<UINT>(RowBytes), static_cast<UINT>(numBytes));

    return true;
}

void CD3D11Texture::setPitch(DXGI_FORMAT d3dformat)
{
    Pitch = ((float)Driver->getBitsPerPixel(d3dformat) / 8.0) * TextureSize.Width;
}

bool CD3D11Texture::createTextureBuffer()
{
    if (!Texture)
    {
        os::Printer::log("Error creating texture buffer: main texture is null", ELL_ERROR);
        return false;
    }

    if ( !TextureBuffer )
    {
        D3D11_TEXTURE2D_DESC desc;
        ::ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
        Texture->GetDesc(&desc);

        desc.BindFlags = 0;
        desc.Usage = D3D11_USAGE_STAGING;
        desc.MiscFlags = 0;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
        //desc.MipLevels = 0;
        ID3D11Texture2D* tex = NULL;
        HRESULT hr = Device->CreateTexture2D(&desc, NULL, &TextureBuffer);
        if ( FAILED(hr) )
        {
            os::Printer::log("Error creating texture buffer", ELL_ERROR);
            return false;
        }
    }

    // sync main texture contents with texture buffer
    ImmediateContext->CopyResource(TextureBuffer, Texture );

    return true;
}

bool CD3D11Texture::createViews()
{
    if (!Texture)
        return false;

    HRESULT hr = S_OK;
    DXGI_FORMAT format = Driver->getD3DFormatFromColorFormat( ColorFormat );

    // set final tex. attributes from tex. description
    // they may differ from the source image !!!
    D3D11_TEXTURE2D_DESC desc;
    Texture->GetDesc(&desc);

    // create render target view only if needed
    if(IsRenderTarget)
    {
        SAFE_RELEASE( RTView );

        D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
        ::ZeroMemory( &rtvDesc, sizeof( rtvDesc ) );
        rtvDesc.Format = format;

        // check if texture is array and/or multisampled
        if (SampleCount > 1 && NumberOfArraySlices > 1)		// multisampled array
        {
            rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY;
            rtvDesc.Texture2DMSArray.ArraySize = desc.ArraySize;
            rtvDesc.Texture2DMSArray.FirstArraySlice = 0;
        }
        else if (SampleCount > 1)	// only multisampled
        {
            rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
        }
        else if (NumberOfArraySlices > 1)	// only array
        {
            rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
            rtvDesc.Texture2DArray.ArraySize = desc.ArraySize;
            rtvDesc.Texture2DArray.FirstArraySlice = 0;
            rtvDesc.Texture2DArray.MipSlice = 0;
        }
        else	// simple texture
        {
            rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
            rtvDesc.Texture2D.MipSlice = 0;
        }

        hr = Device->CreateRenderTargetView( Texture, &rtvDesc, &RTView );
        if (FAILED(hr))
        {
            os::Printer::log("Could not create render target view", ELL_ERROR);
            return false;
        }
    }

    // create shader resource view
    SAFE_RELEASE( SRView );
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ::ZeroMemory( &srvDesc, sizeof( srvDesc ) );
    srvDesc.Format = format;

    // check if texture is array and/or multisampled
    if (SampleCount > 1 && NumberOfArraySlices > 1)		// multisampled array
    {
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY;
        srvDesc.Texture2DMSArray.ArraySize = desc.ArraySize;
    }
    else if (SampleCount > 1)	// only multisampled
    {
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
    }
    else if (NumberOfArraySlices > 1)	// only array
    {
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
        srvDesc.Texture2DArray.ArraySize = desc.ArraySize;
        srvDesc.Texture2DArray.FirstArraySlice = 0;
        srvDesc.Texture2DArray.MipLevels = desc.MipLevels;
        srvDesc.Texture2DArray.MostDetailedMip = 0;
    }
    else	// simple texture
    {
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = desc.MipLevels;
        srvDesc.Texture2D.MostDetailedMip = 0;	
    }
        
    hr = Device->CreateShaderResourceView( Texture, &srvDesc, &SRView );
    if (FAILED(hr))
    {
        os::Printer::log("Could not create shader resource view", ELL_ERROR);
        return false;
    }

    return true;
}

bool CD3D11Texture::InitializeColorFormat(u32 flags, ECOLOR_FORMAT colorFormat)
{
    DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
    // Color format for DX 10 driver shall be different that for DX 9
    // - B5G5R5A1 family is deprecated in DXGI, and doesn't exists in DX 10
    // - Irrlicht color format follows DX 9 (alpha first), and DX 10 is alpha last
    switch ( colorFormat )
    {
        case ECF_RGBA_S3TC_DXT1:
            format = DXGI_FORMAT_BC1_UNORM;
            break;
        case ECF_RGBA_S3TC_DXT3:
            format = DXGI_FORMAT_BC2_UNORM;
            break;
        case ECF_RGBA_S3TC_DXT5:
            format = DXGI_FORMAT_BC3_UNORM;
            break;
        default:
        {
            switch ( getTextureFormatFromFlags(flags) )
            {
                case ETCF_ALWAYS_16_BIT:
                    format = DXGI_FORMAT_B5G5R5A1_UNORM; break;
                case ETCF_ALWAYS_32_BIT:
                    format = DXGI_FORMAT_R8G8B8A8_UNORM; break;
                case ETCF_OPTIMIZED_FOR_QUALITY:
                {
                    switch ( colorFormat )
                    {
                        case ECF_R16F:
                            format = DXGI_FORMAT_R16_FLOAT;
                            break;

                        case ECF_R32F:
                            format = DXGI_FORMAT_R32_FLOAT;
                            break;

                        case ECF_G16R16F:
                            format = DXGI_FORMAT_R16G16_FLOAT;
                            break;

                        case ECF_G32R32F:
                            format = DXGI_FORMAT_R32G32_FLOAT;
                            break;

                        case ECF_A16B16G16R16F:
                            format = DXGI_FORMAT_R16G16B16A16_FLOAT;
                            break;

                        case ECF_A32B32G32R32F:
                            format = DXGI_FORMAT_R32G32B32A32_FLOAT;
                            break;

                        case ECF_A1R5G5B5:
                            format = DXGI_FORMAT_B5G5R5A1_UNORM;
                            break;
                        case ECF_R5G6B5:
                        case ECF_R8G8B8:
                        case ECF_A8R8G8B8:
                        default:
                            format = DXGI_FORMAT_R8G8B8A8_UNORM;
                            break;
                    }
                }
                break;
                case ETCF_OPTIMIZED_FOR_SPEED:
                    format = DXGI_FORMAT_B5G5R5A1_UNORM; break;
                    break;
                default:
                    break;
            }
        }
    }

    // get color format
    ColorFormat = Driver->getColorFormatFromD3DFormat(format);

    return ColorFormat != ECF_UNKNOWN;
}

//--------------------------------------------------------------------------------------
// Get surface information for a particular format
//--------------------------------------------------------------------------------------
static void GetSurfaceInfo(_In_ CD3D11Driver* driver, _In_ size_t width,
    _In_ size_t height,
    _In_ DXGI_FORMAT fmt,
    _Out_opt_ size_t* outNumBytes,
    _Out_opt_ size_t* outRowBytes,
    _Out_opt_ size_t* outNumRows)
{
    size_t numBytes = 0;
    size_t rowBytes = 0;
    size_t numRows = 0;

    bool bc = false;
    bool packed = false;
    bool planar = false;
    size_t bpe = 0;
    switch ( fmt )
    {
        case DXGI_FORMAT_BC1_TYPELESS:
        case DXGI_FORMAT_BC1_UNORM:
        case DXGI_FORMAT_BC1_UNORM_SRGB:
        case DXGI_FORMAT_BC4_TYPELESS:
        case DXGI_FORMAT_BC4_UNORM:
        case DXGI_FORMAT_BC4_SNORM:
            bc = true;
            bpe = 8;
            break;

        case DXGI_FORMAT_BC2_TYPELESS:
        case DXGI_FORMAT_BC2_UNORM:
        case DXGI_FORMAT_BC2_UNORM_SRGB:
        case DXGI_FORMAT_BC3_TYPELESS:
        case DXGI_FORMAT_BC3_UNORM:
        case DXGI_FORMAT_BC3_UNORM_SRGB:
        case DXGI_FORMAT_BC5_TYPELESS:
        case DXGI_FORMAT_BC5_UNORM:
        case DXGI_FORMAT_BC5_SNORM:
        case DXGI_FORMAT_BC6H_TYPELESS:
        case DXGI_FORMAT_BC6H_UF16:
        case DXGI_FORMAT_BC6H_SF16:
        case DXGI_FORMAT_BC7_TYPELESS:
        case DXGI_FORMAT_BC7_UNORM:
        case DXGI_FORMAT_BC7_UNORM_SRGB:
            bc = true;
            bpe = 16;
            break;

        case DXGI_FORMAT_R8G8_B8G8_UNORM:
        case DXGI_FORMAT_G8R8_G8B8_UNORM:
        case DXGI_FORMAT_YUY2:
            packed = true;
            bpe = 4;
            break;

        case DXGI_FORMAT_Y210:
        case DXGI_FORMAT_Y216:
            packed = true;
            bpe = 8;
            break;

        case DXGI_FORMAT_NV12:
        case DXGI_FORMAT_420_OPAQUE:
            planar = true;
            bpe = 2;
            break;

        case DXGI_FORMAT_P010:
        case DXGI_FORMAT_P016:
            planar = true;
            bpe = 4;
            break;

#if defined(_XBOX_ONE) && defined(_TITLE)

        case DXGI_FORMAT_D16_UNORM_S8_UINT:
        case DXGI_FORMAT_R16_UNORM_X8_TYPELESS:
        case DXGI_FORMAT_X16_TYPELESS_G8_UINT:
            planar = true;
            bpe = 4;
            break;

#endif
    }

    if ( bc )
    {
        size_t numBlocksWide = 0;
        if ( width > 0 )
        {
            numBlocksWide = std::max<size_t>(1, (width + 3) / 4);
        }
        size_t numBlocksHigh = 0;
        if ( height > 0 )
        {
            numBlocksHigh = std::max<size_t>(1, (height + 3) / 4);
        }
        rowBytes = numBlocksWide * bpe;
        numRows = numBlocksHigh;
        numBytes = rowBytes * numBlocksHigh;
    }
    else if ( packed )
    {
        rowBytes = ((width + 1) >> 1) * bpe;
        numRows = height;
        numBytes = rowBytes * height;
    }
    else if ( fmt == DXGI_FORMAT_NV11 )
    {
        rowBytes = ((width + 3) >> 2) * 4;
        numRows = height * 2; // Direct3D makes this simplifying assumption, although it is larger than the 4:1:1 data
        numBytes = rowBytes * numRows;
    }
    else if ( planar )
    {
        rowBytes = ((width + 1) >> 1) * bpe;
        numBytes = (rowBytes * height) + ((rowBytes * height + 1) >> 1);
        numRows = height + ((height + 1) >> 1);
    }
    else
    {
        size_t bpp = driver->getBitsPerPixel(fmt);
        rowBytes = (width * bpp + 7) / 8; // round up to nearest byte
        numRows = height;
        numBytes = rowBytes * height;
    }

    if ( outNumBytes )
    {
        *outNumBytes = numBytes;
    }
    if ( outRowBytes )
    {
        *outRowBytes = rowBytes;
    }
    if ( outNumRows )
    {
        *outNumRows = numRows;
    }
}

}
}

#endif;