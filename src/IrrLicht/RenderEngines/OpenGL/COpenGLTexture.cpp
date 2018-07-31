// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_OPENGL_

#include "irrTypes.h"
#include "COpenGLTexture.h"
#include "COpenGLDriver.h"
#include "os.h"
#include "CColorConverter.h"

#include "irrString.h"

inline unsigned int mostSignificantBitSet(unsigned int value)
{
    //                                     0, 1, 2, 3, 4, 5, 6, 7, 8, 9, A, B, C, D, E, F
    static const unsigned char msb[16] = { 0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4 };

    unsigned int result = 0;
    if (value & 0xFFFF0000) { result += 16; value >>= 16; }
    if (value & 0x0000FF00) { result += 8; value >>= 8; }
    if (value & 0x000000F0) { result += 4; value >>= 4; }
    result += msb[value];
    return result - 1;
}

namespace irr
{
namespace video
{

//! constructor for usual textures
COpenGLTexture::COpenGLTexture(IImage* origImage, const io::path& name, void* mipmapData, COpenGLDriver* driver)
	: ITexture(name), ColorFormat(ECF_A8R8G8B8), Driver(driver)
    , NumberOfMipLevels(0)
    , NumberOfArraySlices(0)
    , Image(0)
    , Pitch(0)
	, TextureName(0)
    , TextureBuffer(0), InternalFormat(GL_RGBA), PixelFormat(GL_BGRA_EXT),
	PixelType(GL_UNSIGNED_BYTE), MipLevelStored(0), MipmapLegacyMode(true),
	IsRenderTarget(false), AutomaticMipmapUpdate(false),
	ReadOnlyLock(false), KeepImage(false)
{
	#ifdef _DEBUG
	setDebugName("COpenGLTexture");
	#endif
    
	HasMipMaps = Driver->getTextureCreationFlag(ETCF_CREATE_MIP_MAPS);
	getImageValues(origImage);

	glGenTextures(1, &TextureName);

	uploadTexture(true, mipmapData);

    //if (Image && !KeepImage)
    //    clearImage();
}


//! constructor for basic setup (only for derived classes)
COpenGLTexture::COpenGLTexture(const io::path& name, COpenGLDriver* driver)
	: ITexture(name), ColorFormat(ECF_A8R8G8B8), Driver(driver)
    , NumberOfMipLevels(0)
    , NumberOfArraySlices(0)
    , Image(0)
    , Pitch(0)
	, TextureName(0)
    , TextureBuffer(0), InternalFormat(GL_RGBA), PixelFormat(GL_BGRA_EXT),
	PixelType(GL_UNSIGNED_BYTE), MipLevelStored(0), HasMipMaps(true),
	MipmapLegacyMode(true), IsRenderTarget(false), AutomaticMipmapUpdate(false),
	ReadOnlyLock(false), KeepImage(true)
{
	#ifdef _DEBUG
	setDebugName("COpenGLTexture");
	#endif
}


//! destructor
COpenGLTexture::~COpenGLTexture()
{
	if (TextureName)
		glDeleteTextures(1, &TextureName);

    if (TextureBuffer)
        glDeleteBuffers(1, &TextureBuffer);

    clearImage();
}


void COpenGLTexture::clearImage()
{
    if (Image)
        Image->drop();
    Image = nullptr;
}

//! Choose best matching color format, based on texture creation flags
ECOLOR_FORMAT COpenGLTexture::getBestColorFormat(ECOLOR_FORMAT format)
{
	ECOLOR_FORMAT destFormat = ECF_A8R8G8B8;
	switch (format)
	{
        case ECF_ALPHA:
            destFormat = ECF_ALPHA;
            break;
		case ECF_A1R5G5B5:
			if (!Driver->getTextureCreationFlag(ETCF_ALWAYS_32_BIT))
				destFormat = ECF_A1R5G5B5;
		break;
		case ECF_R5G6B5:
			if (!Driver->getTextureCreationFlag(ETCF_ALWAYS_32_BIT))
				destFormat = ECF_A1R5G5B5;
		break;
		case ECF_A8R8G8B8:
			if (Driver->getTextureCreationFlag(ETCF_ALWAYS_16_BIT) ||
					Driver->getTextureCreationFlag(ETCF_OPTIMIZED_FOR_SPEED))
				destFormat = ECF_A1R5G5B5;
		break;
		case ECF_R8G8B8:
			if (Driver->getTextureCreationFlag(ETCF_ALWAYS_16_BIT) ||
					Driver->getTextureCreationFlag(ETCF_OPTIMIZED_FOR_SPEED))
				destFormat = ECF_A1R5G5B5;
		default:
		break;
	}
	if (Driver->getTextureCreationFlag(ETCF_NO_ALPHA_CHANNEL))
	{
		switch (destFormat)
		{
			case ECF_A1R5G5B5:
				destFormat = ECF_R5G6B5;
			break;
			case ECF_A8R8G8B8:
				destFormat = ECF_R8G8B8;
			break;
			default:
			break;
		}
	}
	return destFormat;
}


//! Get opengl values for the GPU texture storage
GLint COpenGLTexture::getOpenGLFormatAndParametersFromColorFormat(ECOLOR_FORMAT format,
				GLint& filtering,
				GLenum& colorformat,
				GLenum& type)
{
	// default
	filtering = GL_LINEAR;
	colorformat = GL_RGBA;
	type = GL_UNSIGNED_BYTE;
	GLenum internalformat = GL_RGBA;

	switch(format)
	{
        case ECF_DXT1:
            internalformat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
            break;
        case ECF_DXT3:
            internalformat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
            break;
        case ECF_DXT5:
            internalformat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
            break;
        case ECF_ALPHA:
            colorformat = GL_ALPHA;
            internalformat = GL_ALPHA;
            break;
		case ECF_A1R5G5B5:
			colorformat=GL_BGRA_EXT;
			type=GL_UNSIGNED_SHORT_1_5_5_5_REV;
			internalformat =  GL_RGBA;
			break;
		case ECF_R5G6B5:
			colorformat=GL_RGB;
			type=GL_UNSIGNED_SHORT_5_6_5;
			internalformat =  GL_RGB;
			break;
		case ECF_R8G8B8:
			colorformat=GL_BGR;
			type=GL_UNSIGNED_BYTE;
			internalformat =  GL_RGB;
			break;
        case ECF_RGBA8:
            colorformat = GL_RGBA;
            if (Driver->Version > 101)
                type = GL_UNSIGNED_INT_8_8_8_8_REV;
            internalformat = GL_RGBA;
            break;
		case ECF_A8R8G8B8:
			colorformat=GL_BGRA_EXT;
            if (Driver->Version > 101)
				type=GL_UNSIGNED_INT_8_8_8_8_REV;
            internalformat =  GL_RGBA;
			break;
		// Floating Point texture formats. Thanks to Patryk "Nadro" Nadrowski.
		case ECF_R16F:
		{
#ifdef GL_ARB_texture_rg
			filtering = GL_NEAREST;
			colorformat = GL_RED;
			type = GL_FLOAT;

			internalformat =  GL_R16F;
#else
			ColorFormat = ECF_A8R8G8B8;
			internalformat =  GL_RGB8;
#endif
		}
			break;
		case ECF_G16R16F:
		{
#ifdef GL_ARB_texture_rg
			filtering = GL_NEAREST;
			colorformat = GL_RG;
			type = GL_FLOAT;

			internalformat =  GL_RG16F;
#else
			ColorFormat = ECF_A8R8G8B8;
			internalformat =  GL_RGB8;
#endif
		}
			break;
		case ECF_A16B16G16R16F:
		{
#ifdef GL_ARB_texture_rg
			filtering = GL_NEAREST;
			colorformat = GL_RGBA;
			type = GL_FLOAT;

			internalformat =  GL_RGBA16F_ARB;
#else
			ColorFormat = ECF_A8R8G8B8;
			internalformat =  GL_RGBA8;
#endif
		}
			break;
		case ECF_R32F:
		{
#ifdef GL_ARB_texture_rg
			filtering = GL_NEAREST;
			colorformat = GL_RED;
			type = GL_FLOAT;

			internalformat =  GL_R32F;
#else
			ColorFormat = ECF_A8R8G8B8;
			internalformat =  GL_RGB8;
#endif
		}
			break;
		case ECF_G32R32F:
		{
#ifdef GL_ARB_texture_rg
			filtering = GL_NEAREST;
			colorformat = GL_RG;
			type = GL_FLOAT;

			internalformat =  GL_RG32F;
#else
			ColorFormat = ECF_A8R8G8B8;
			internalformat =  GL_RGB8;
#endif
		}
			break;
		case ECF_A32B32G32R32F:
		{
#ifdef GL_ARB_texture_float
			filtering = GL_NEAREST;
			colorformat = GL_RGBA;
			type = GL_FLOAT;

			internalformat =  GL_RGBA32F_ARB;
#else
			ColorFormat = ECF_A8R8G8B8;
			internalformat =  GL_RGBA8;
#endif
		}
			break;
		default:
		{
			os::Printer::log("Unsupported texture format", ELL_ERROR);
			internalformat =  GL_RGBA8;
		}
	}
#if defined(GL_ARB_framebuffer_sRGB) || defined(GL_EXT_framebuffer_sRGB)
	if (Driver->Params.HandleSRGB)
	{
		if (internalformat==GL_RGBA)
			internalformat=GL_SRGB_ALPHA_EXT;
		else if (internalformat==GL_RGB)
			internalformat=GL_SRGB_EXT;
	}
#endif
	return internalformat;
}


// prepare values ImageSize, TextureSize, and ColorFormat based on image
void COpenGLTexture::getImageValues(IImage* image)
{
	if (!image)
	{
		os::Printer::log("No image for OpenGL texture.", ELL_ERROR);
		return;
	}

	ImageSize = image->getDimension();

	if ( !ImageSize.Width || !ImageSize.Height)
	{
		os::Printer::log("Invalid size of image for OpenGL Texture.", ELL_ERROR);
		return;
	}

    switch (image->getColorFormat())
    {
        case ECF_RGBA8:
        case ECF_DXT1:
        case ECF_DXT3:
        case ECF_DXT5:
            TextureSize = ImageSize;
            ColorFormat = image->getColorFormat();
            Image = image;
            image->grab();
            break;
        default:
        {
            TextureSize = ImageSize.getOptimalSize(!Driver->queryFeature(EVDF_TEXTURE_NPOT));
            Image = Driver->createImage(ColorFormat, image->getDimension());
            image->copyTo(Image);
            break;
        }
    }

    //if (origImage->getColorFormat() == ColorFormat && ImageSize == TextureSize)
    //{
    //    Image = origImage;
    //    Image->grab();
    //}
    //else if (ImageSize == TextureSize)
    //{
    //    Image = Driver->createImage(ColorFormat, ImageSize);
    //    origImage->copyTo(Image);
    //}
    //else
    //{
    //    Image = Driver->createImage(ColorFormat, TextureSize);
    //    // scale texture
    //    origImage->copyToScaling(Image);
    //}

    Pitch = (IImage::getBitsPerPixelFromFormat(ColorFormat) / 8.0) * TextureSize.Width;

	//const f32 ratio = (f32)ImageSize.Width/(f32)ImageSize.Height;
	//if ((ImageSize.Width>Driver->MaxTextureSize) && (ratio >= 1.0f))
	//{
	//	ImageSize.Width = Driver->MaxTextureSize;
	//	ImageSize.Height = (u32)(Driver->MaxTextureSize/ratio);
	//}
	//else if (ImageSize.Height>Driver->MaxTextureSize)
	//{
	//	ImageSize.Height = Driver->MaxTextureSize;
	//	ImageSize.Width = (u32)(Driver->MaxTextureSize*ratio);
	//}
	//TextureSize=ImageSize.getOptimalSize(!Driver->queryFeature(EVDF_TEXTURE_NPOT));

    //ColorFormat = image->getColorFormat(); // getBestColorFormat(image->getColorFormat());
}


//! copies the the texture into an open gl texture.
void COpenGLTexture::uploadTexture(bool newTexture, void* mipmapData, u32 level)
{
	// check which image needs to be uploaded
	IImage* image = Image;
	if (!image)
	{
		os::Printer::log("No image for OpenGL texture to upload", ELL_ERROR);
		return;
	}

	// get correct opengl color data values
	GLenum oldInternalFormat = InternalFormat;
	GLint filtering;
	InternalFormat = getOpenGLFormatAndParametersFromColorFormat(ColorFormat, filtering, PixelFormat, PixelType);
	// make sure we don't change the internal format of existing images
	if (!newTexture)
		InternalFormat=oldInternalFormat;

	if (Driver->testGLError())
		os::Printer::log("Could not bind Texture", ELL_ERROR);

    // now get image data and upload to GPU
    void* source = image->lock();

    Driver->GetStateCache()->setEnabled(OGLStateEntries::OGL_TEXTURE_2D, true);
    //Driver->GetStateCache()->bindGLTexture(0, 0);
    //Driver->setActiveTexture(0, nullptr);
    Driver->GetStateCache()->activateGLTextureUnit(0);
    glBindTexture(GL_TEXTURE_2D, TextureName);


    createTextureBuffer();

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, TextureBuffer);
    void* buffer = glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, image->getImageDataSizeInBytes(), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
    memcpy(buffer, source, image->getImageDataSizeInBytes());
    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);


    bool isCompressed = false;
    if (IImage::isCompressedFormat(ColorFormat))
    {
        //lock(E_TEXTURE_LOCK_MODE::ETLM_WRITE_ONLY);
        glCompressedTexImage2D(GL_TEXTURE_2D, level, InternalFormat, image->getDimension().Width, image->getDimension().Height, 0, image->getImageDataSizeInBytes(), 0);
    }
    else
    {
        if (newTexture)
            glTexImage2D(GL_TEXTURE_2D, level, InternalFormat, image->getDimension().Width,
                image->getDimension().Height, 0, PixelFormat, PixelType, 0);
        else
            glTexSubImage2D(GL_TEXTURE_2D, level, 0, 0, image->getDimension().Width,
                image->getDimension().Height, PixelFormat, PixelType, 0);
    }

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    image->unlock();

	// mipmap handling for main texture
    if (!level && newTexture)
    {
        // auto generate if possible and no mipmap data is given
        if (HasMipMaps && !mipmapData && Driver->queryFeature(EVDF_MIP_MAP_AUTO_UPDATE))
        {
            if (Driver->getTextureCreationFlag(ETCF_OPTIMIZED_FOR_SPEED))
                glHint(GL_GENERATE_MIPMAP_HINT_SGIS, GL_FASTEST);
            else if (Driver->getTextureCreationFlag(ETCF_OPTIMIZED_FOR_QUALITY))
                glHint(GL_GENERATE_MIPMAP_HINT_SGIS, GL_NICEST);
            else
                glHint(GL_GENERATE_MIPMAP_HINT_SGIS, GL_DONT_CARE);
                //glHint(GL_GENERATE_MIPMAP_HINT_SGIS, GL_FASTEST);
        
            AutomaticMipmapUpdate = true;

            u32 maxLevel = mostSignificantBitSet(std::max(image->getDimension().Width, image->getDimension().Height));
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, maxLevel);

            if (!Driver->queryFeature(EVDF_FRAMEBUFFER_OBJECT))
            {
                glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE );
            	MipmapLegacyMode=true;
            }
            else
                MipmapLegacyMode = false;
        }
        else
        {
            // Either generate manually due to missing capability
            // or use predefined mipmap data
            AutomaticMipmapUpdate = false;
            regenerateMipMapLevels(mipmapData);
        }

        if (HasMipMaps) // might have changed in regenerateMipMapLevels
        {
            // enable bilinear mipmap filter
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
        else
        {
            // enable bilinear filter without mipmaps
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        if (!MipmapLegacyMode && AutomaticMipmapUpdate)
        {
            glGenerateMipmap(GL_TEXTURE_2D);
        }
    }

    //Driver->GetStateCache()->bindGLTexture(0, 0);

	if (Driver->testGLError())
		os::Printer::log("Could not glTexImage2D", ELL_ERROR);

    if (Driver->testGLError())
        os::Printer::log("Could not glTexImage2D", ELL_ERROR);
}

bool COpenGLTexture::createTextureBuffer()
{
    if (!TextureBuffer)
    {
        glCreateBuffers(1, &TextureBuffer);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, TextureBuffer);
        glBufferData(GL_PIXEL_UNPACK_BUFFER, getPitch() * TextureSize.Height, NULL, GL_STATIC_COPY);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    }
    return TextureBuffer != 0;
}

//! lock function
void* COpenGLTexture::lock(E_TEXTURE_LOCK_MODE mode, u32 mipmapLevel, u32 arraySlice)
{
    if (!TextureName || !createTextureBuffer())
        return nullptr;

    LastMapDirection = mode;

    // Record mip level locked to use in unlock
    MipLevelLocked = mipmapLevel;
    ArraySliceLocked = arraySlice;

    switch (mode)
    {
        case ETLM_WRITE_ONLY:
        {
            glBindBuffer(GL_PIXEL_PACK_BUFFER, TextureBuffer);
            return glMapBufferRange(GL_PIXEL_PACK_BUFFER, 0, getPitch() * TextureSize.Height, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
        }
        case ETLM_READ_WRITE:
        case ETLM_READ_ONLY:
        {
            glBindBuffer(GL_PIXEL_PACK_BUFFER, TextureBuffer);

            // bind texture
            Driver->GetStateCache()->setEnabled(OGLStateEntries::OGL_TEXTURE_2D, true);
            Driver->GetStateCache()->bindGLTexture(0, 0);
            Driver->GetStateCache()->activateGLTextureUnit(0);
            glBindTexture(GL_TEXTURE_2D, TextureName);

            // transfer texture into PBO
            glGetnTexImage(GL_TEXTURE_2D, mipmapLevel, PixelFormat, PixelType, getPitch() * TextureSize.Height, 0);

            void* mappedPtr = nullptr;
            if (mode == ETLM_READ_WRITE)
                mappedPtr = glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_WRITE);
            else
                mappedPtr = glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);

            //glGetTexImage(GL_TEXTURE_2D, mipmapLevel, PixelFormat, PixelType, (GLvoid*)0);
            return mappedPtr;
        }
    }

    return nullptr;
}


//! unlock function
void COpenGLTexture::unlock()
{
    switch (LastMapDirection)
    {
        case ETLM_WRITE_ONLY:
        {
            glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, TextureBuffer);
            Driver->GetStateCache()->setEnabled(OGLStateEntries::OGL_TEXTURE_2D, true);
            Driver->GetStateCache()->bindGLTexture(0, 0);
            Driver->GetStateCache()->activateGLTextureUnit(0);
            glBindTexture(GL_TEXTURE_2D, TextureName);

            //Driver->setActiveTexture(0, nullptr);
            //glBindBuffer(GL_PIXEL_UNPACK_BUFFER, TextureBuffer);
            //glBindTexture(GL_TEXTURE_2D, TextureName);
            //Driver->GetStateCache()->setEnabled(OGLStateEntries::OGL_TEXTURE_2D, true);
            if (IImage::isCompressedFormat(ColorFormat))
            {
                glCompressedTexSubImage2D(GL_TEXTURE_2D, MipLevelLocked, 0, 0, TextureSize.Width, TextureSize.Height, InternalFormat, getPitch() * TextureSize.Height, 0);
            }
            else
            {
                glTexSubImage2D(GL_TEXTURE_2D, MipLevelLocked, 0, 0, TextureSize.Width, TextureSize.Height, PixelFormat, PixelType, 0);
            }
            //glGenerateTextureMipmap(TextureName);
            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
            glBindTexture(GL_TEXTURE_2D, 0);
            //Driver->setActiveTexture(0, nullptr); // invalidate driver cache
            break;
        }
        case ETLM_READ_WRITE:
        {
            glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, TextureBuffer);
            Driver->GetStateCache()->setEnabled(OGLStateEntries::OGL_TEXTURE_2D, true);
            Driver->GetStateCache()->bindGLTexture(0, 0);
            Driver->GetStateCache()->activateGLTextureUnit(0);
            glBindTexture(GL_TEXTURE_2D, TextureName);

            //glBindTexture(GL_TEXTURE_2D, TextureName);
            //Driver->GetStateCache()->setEnabled(OGLStateEntries::OGL_TEXTURE_2D, true);
            if (IImage::isCompressedFormat(ColorFormat))
            {
                glCompressedTexSubImage2D(GL_TEXTURE_2D, MipLevelLocked, 0, 0, TextureSize.Width, TextureSize.Height, InternalFormat, getPitch() * TextureSize.Height, 0);
            }
            else
            {
                glTexSubImage2D(GL_TEXTURE_2D, MipLevelLocked, 0, 0, TextureSize.Width, TextureSize.Height, PixelFormat, PixelType, 0);
            }
            //glGenerateTextureMipmap(TextureName);
            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
            glBindTexture(GL_TEXTURE_2D, 0);
            //Driver->setActiveTexture(0, nullptr); // invalidate driver cache
            break;
        }
        case ETLM_READ_ONLY:
        {
            glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
            break;
        }
    }
}


//! Returns size of the original image.
const core::dimension2d<u32>& COpenGLTexture::getOriginalSize() const
{
	return ImageSize;
}


//! Returns size of the texture.
const core::dimension2d<u32>& COpenGLTexture::getSize() const
{
	return TextureSize;
}


//! returns driver type of texture, i.e. the driver, which created the texture
E_DRIVER_TYPE COpenGLTexture::getDriverType() const
{
	return EDT_OPENGL;
}


//! returns color format of texture
ECOLOR_FORMAT COpenGLTexture::getColorFormat() const
{
	return ColorFormat;
}


//! returns pitch of texture (in bytes)
u32 COpenGLTexture::getPitch() const
{
	return Pitch;
}


//! return open gl texture name
GLuint COpenGLTexture::getOpenGLTextureName() const
{
	return TextureName;
}


//! Returns whether this texture has mipmaps
bool COpenGLTexture::hasMipMaps() const
{
	return HasMipMaps;
}


//! Regenerates the mip map levels of the texture. Useful after locking and
//! modifying the texture
void COpenGLTexture::regenerateMipMapLevels(void* mipmapData)
{
	if (AutomaticMipmapUpdate || !HasMipMaps || !Image)
		return;

	if ((Image->getDimension().Width==1) && (Image->getDimension().Height==1))
		return;


	// Manually create mipmaps or use prepared version
	u32 width=Image->getDimension().Width;
	u32 height=Image->getDimension().Height;
	u32 level=0;
	u8* target = static_cast<u8*>(mipmapData);
    if (!target)
        target = new u8[Pitch * height];

    do
	{
		if (width>1)
			width>>=1;
		if (height>1)
			height>>=1;
		++level;

        GLuint buffer;
        glGenBuffers(1, &buffer);

        u32 imageSize;

		// create scaled version if no mipdata available
        if (!mipmapData)
        {
            Image->copyToScaling(target, width, height, Image->getColorFormat());

            //glBufferData(GL_PIXEL_UNPACK_BUFFER, width * height * (IImage::getBitsPerPixelFromFormat(ColorFormat) / 8.0), NULL, GL_STATIC_COPY);
            //void* bufferPtr = glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
            //memcpy(bufferPtr, target, width * height * (IImage::getBitsPerPixelFromFormat(ColorFormat) / 8.0));
            //glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);

            glTexImage2D(GL_TEXTURE_2D, level, InternalFormat, width, height,
                0, PixelFormat, PixelType, target);
        }
        else
        {
            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, buffer);

            imageSize = *(u32*)target;
            target += sizeof(u32);

            if (IImage::isCompressedFormat(ColorFormat))
            {
                if (width > 4)
                    width = (width + 3) & ~3;
                if (height > 4)
                    height = (height + 3) & ~3;

                // Ref: http://stackoverflow.com/questions/1715027/dds-dxt1-loading-in-opengl-crashes
                GLint nBlockSize = 16;
                if (InternalFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT)
                    nBlockSize = 8;
                GLint nSize = ((width + 3) / 4) * ((height + 3) / 4) * nBlockSize;

                glBufferData(GL_PIXEL_UNPACK_BUFFER, nSize, NULL, GL_STATIC_COPY);

                void* bufferPtr = glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, nSize, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
                memcpy(bufferPtr, target, nSize);
                glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);


                glCompressedTexImage2D(GL_TEXTURE_2D, level, InternalFormat, width, height, 0, nSize /*imageSize*/, 0);
            }
            else
            {
                glBufferData(GL_PIXEL_UNPACK_BUFFER, imageSize, NULL, GL_STATIC_COPY);

                void* bufferPtr = glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, imageSize, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
                memcpy(bufferPtr, target, imageSize);
                glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);

                glTexImage2D(GL_TEXTURE_2D, level, InternalFormat, width, height, 0, PixelFormat, PixelType, 0);
            }

            Driver->testGLError();
        }

        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
        glDeleteBuffers(1, &buffer);

		// get next prepared mipmap data if available
		if (mipmapData)
		{
			target += imageSize;
            if (!*(u32*)target)
                break;
		}
    }
	while (width!=1 || height!=1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, level);

    // cleanup
    if (!mipmapData)
        delete[] target;
}


bool COpenGLTexture::isRenderTarget() const
{
	return IsRenderTarget;
}


void COpenGLTexture::setIsRenderTarget(bool isTarget)
{
	IsRenderTarget = isTarget;
}


bool COpenGLTexture::isFrameBufferObject() const
{
	return false;
}


//! Bind Render Target Texture
void COpenGLTexture::bindRTT()
{
}


//! Unbind Render Target Texture
void COpenGLTexture::unbindRTT()
{
    Driver->GetStateCache()->setEnabled(OGLStateEntries::OGL_TEXTURE_2D, true);
    Driver->GetStateCache()->bindGLTexture(0, 0);
    Driver->GetStateCache()->activateGLTextureUnit(0);
    glBindTexture(GL_TEXTURE_2D, TextureName);

    //Driver->setActiveTexture(0, nullptr);

	// Copy Our ViewPort To The Texture
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, getSize().Width, getSize().Height);

    glBindTexture(GL_TEXTURE_2D, 0);
}


/* FBO Textures */

// helper function for render to texture
static bool checkFBOStatus(COpenGLDriver* Driver);

//! RTT ColorFrameBuffer constructor
COpenGLFBOTexture::COpenGLFBOTexture(const core::dimension2d<u32>& size,
					const io::path& name, COpenGLDriver* driver,
					ECOLOR_FORMAT format)
	: COpenGLTexture(name, driver), DepthTexture(0), ColorFrameBuffer(0)
{
	#ifdef _DEBUG
	setDebugName("COpenGLTexture_FBO");
	#endif

	ImageSize = size;
	TextureSize = size;

	if (ECF_UNKNOWN == format)
		format = getBestColorFormat(driver->getColorFormat());

	ColorFormat = format;

	GLint FilteringType;
	InternalFormat = getOpenGLFormatAndParametersFromColorFormat(format, FilteringType, PixelFormat, PixelType);

	HasMipMaps = false;
	IsRenderTarget = true;

#ifdef GL_EXT_framebuffer_object
	// generate frame buffer
	Driver->extGlGenFramebuffers(1, &ColorFrameBuffer);
	bindRTT();

	// generate color texture
	glGenTextures(1, &TextureName);

	Driver->setActiveTexture(0, this);
    Driver->GetStateCache()->setEnabled(OGLStateEntries::OGL_TEXTURE_2D, true);
    Driver->GetStateCache()->bindGLTexture(GL_TEXTURE0_ARB + 0, TextureName);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, FilteringType);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, ImageSize.Width,
		ImageSize.Height, 0, PixelFormat, PixelType, 0);
#ifdef _DEBUG
	driver->testGLError();
#endif

	// attach color texture to frame buffer
	Driver->extGlFramebufferTexture2D(GL_FRAMEBUFFER_EXT,
						GL_COLOR_ATTACHMENT0_EXT,
						GL_TEXTURE_2D,
						TextureName,
						0);
#ifdef _DEBUG
	checkFBOStatus(Driver);
#endif

#endif
	unbindRTT();
}


//! destructor
COpenGLFBOTexture::~COpenGLFBOTexture()
{
	if (DepthTexture)
		if (DepthTexture->drop())
			Driver->removeDepthTexture(DepthTexture);
	if (ColorFrameBuffer)
		Driver->extGlDeleteFramebuffers(1, &ColorFrameBuffer);
}


bool COpenGLFBOTexture::isFrameBufferObject() const
{
	return true;
}


//! Bind Render Target Texture
void COpenGLFBOTexture::bindRTT()
{
#ifdef GL_EXT_framebuffer_object
	if (ColorFrameBuffer != 0)
		Driver->extGlBindFramebuffer(GL_FRAMEBUFFER_EXT, ColorFrameBuffer);
	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
#endif
}


//! Unbind Render Target Texture
void COpenGLFBOTexture::unbindRTT()
{
#ifdef GL_EXT_framebuffer_object
	if (ColorFrameBuffer != 0)
		Driver->extGlBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
#endif
}


/* FBO Depth Textures */

//! RTT DepthBuffer constructor
COpenGLFBODepthTexture::COpenGLFBODepthTexture(
		const core::dimension2d<u32>& size,
		const io::path& name,
		COpenGLDriver* driver,
		bool useStencil)
	: COpenGLTexture(name, driver), DepthRenderBuffer(0),
	StencilRenderBuffer(0), UseStencil(useStencil)
{
#ifdef _DEBUG
	setDebugName("COpenGLTextureFBO_Depth");
#endif

	ImageSize = size;
	TextureSize = size;
	InternalFormat = GL_RGBA;
	PixelFormat = GL_RGBA;
	PixelType = GL_UNSIGNED_BYTE;
	HasMipMaps = false;

	if (useStencil)
	{
		glGenTextures(1, &DepthRenderBuffer);
		glBindTexture(GL_TEXTURE_2D, DepthRenderBuffer);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#ifdef GL_EXT_packed_depth_stencil
		if (Driver->queryOpenGLFeature(COpenGLExtensionHandler::IRR_EXT_packed_depth_stencil))
		{
			// generate packed depth stencil texture
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_STENCIL_EXT, ImageSize.Width,
				ImageSize.Height, 0, GL_DEPTH_STENCIL_EXT, GL_UNSIGNED_INT_24_8_EXT, 0);
			StencilRenderBuffer = DepthRenderBuffer; // stencil is packed with depth
		}
		else // generate separate stencil and depth textures
#endif
		{
			// generate depth texture
			glTexImage2D(GL_TEXTURE_2D, 0, Driver->getZBufferBits(), ImageSize.Width,
				ImageSize.Height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);

			// generate stencil texture
			glGenTextures(1, &StencilRenderBuffer);
			glBindTexture(GL_TEXTURE_2D, StencilRenderBuffer);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_STENCIL_INDEX, ImageSize.Width,
				ImageSize.Height, 0, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, 0);
		}
	}
#ifdef GL_EXT_framebuffer_object
	else
	{
		// generate depth buffer
		Driver->extGlGenRenderbuffers(1, &DepthRenderBuffer);
		Driver->extGlBindRenderbuffer(GL_RENDERBUFFER_EXT, DepthRenderBuffer);
		Driver->extGlRenderbufferStorage(GL_RENDERBUFFER_EXT,
				Driver->getZBufferBits(), ImageSize.Width,
				ImageSize.Height);
	}
#endif
}


//! destructor
COpenGLFBODepthTexture::~COpenGLFBODepthTexture()
{
	if (DepthRenderBuffer && UseStencil)
		glDeleteTextures(1, &DepthRenderBuffer);
	else
		Driver->extGlDeleteRenderbuffers(1, &DepthRenderBuffer);
	if (StencilRenderBuffer && StencilRenderBuffer != DepthRenderBuffer)
		glDeleteTextures(1, &StencilRenderBuffer);
}


//combine depth texture and rtt
bool COpenGLFBODepthTexture::attach(ITexture* renderTex)
{
	if (!renderTex)
		return false;
	video::COpenGLFBOTexture* rtt = static_cast<video::COpenGLFBOTexture*>(renderTex);
	rtt->bindRTT();
#ifdef GL_EXT_framebuffer_object
	if (UseStencil)
	{
		// attach stencil texture to stencil buffer
		Driver->extGlFramebufferTexture2D(GL_FRAMEBUFFER_EXT,
						GL_STENCIL_ATTACHMENT_EXT,
						GL_TEXTURE_2D,
						StencilRenderBuffer,
						0);

		// attach depth texture to depth buffer
		Driver->extGlFramebufferTexture2D(GL_FRAMEBUFFER_EXT,
						GL_DEPTH_ATTACHMENT_EXT,
						GL_TEXTURE_2D,
						DepthRenderBuffer,
						0);
	}
	else
	{
		// attach depth renderbuffer to depth buffer
		Driver->extGlFramebufferRenderbuffer(GL_FRAMEBUFFER_EXT,
						GL_DEPTH_ATTACHMENT_EXT,
						GL_RENDERBUFFER_EXT,
						DepthRenderBuffer);
	}
#endif
	// check the status
	if (!checkFBOStatus(Driver))
	{
		os::Printer::log("FBO incomplete");
		return false;
	}
	rtt->DepthTexture=this;
	grab(); // grab the depth buffer, not the RTT
	rtt->unbindRTT();
	return true;
}


//! Bind Render Target Texture
void COpenGLFBODepthTexture::bindRTT()
{
}


//! Unbind Render Target Texture
void COpenGLFBODepthTexture::unbindRTT()
{
}


bool checkFBOStatus(COpenGLDriver* Driver)
{
#ifdef GL_EXT_framebuffer_object
	GLenum status = Driver->extGlCheckFramebufferStatus(GL_FRAMEBUFFER_EXT);

	switch (status)
	{
		//Our FBO is perfect, return true
		case GL_FRAMEBUFFER_COMPLETE_EXT:
			return true;

		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
			os::Printer::log("FBO has invalid read buffer", ELL_ERROR);
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
			os::Printer::log("FBO has invalid draw buffer", ELL_ERROR);
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
			os::Printer::log("FBO has one or several incomplete image attachments", ELL_ERROR);
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
			os::Printer::log("FBO has one or several image attachments with different internal formats", ELL_ERROR);
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
			os::Printer::log("FBO has one or several image attachments with different dimensions", ELL_ERROR);
			break;

// not part of fbo_object anymore, but won't harm as it is just a return value
#ifdef GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT
		case GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT:
			os::Printer::log("FBO has a duplicate image attachment", ELL_ERROR);
			break;
#endif

		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
			os::Printer::log("FBO missing an image attachment", ELL_ERROR);
			break;

#ifdef GL_EXT_framebuffer_multisample
		case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE_EXT:
			os::Printer::log("FBO wrong multisample setup", ELL_ERROR);
			break;
#endif

		case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
			os::Printer::log("FBO format unsupported", ELL_ERROR);
			break;

		default:
			break;
	}
#endif
	os::Printer::log("FBO error", ELL_ERROR);
//	_IRR_DEBUG_BREAK_IF(true);
	return false;
}


} // end namespace video
} // end namespace irr

#endif // _IRR_COMPILE_WITH_OPENGL_

