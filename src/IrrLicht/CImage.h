// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_IMAGE_H_INCLUDED__
#define __C_IMAGE_H_INCLUDED__

#include "IImage.h"
#include "rect.h"

namespace irr
{
namespace video
{

//! IImage implementation with a lot of special image operations for
//! 16 bit A1R5G5B5/32 Bit A8R8G8B8 images, which are used by the SoftwareDevice.
class IRRLICHT_API CImage : public IImage
{
public:

	//! constructor from raw image data
	/** \param useForeignMemory: If true, the image will use the data pointer
	directly and own it from now on, which means it will also try to delete [] the
	data when the image will be destructed. If false, the memory will by copied. */
	CImage(ECOLOR_FORMAT format, const core::dimension2d<u32>& size,
		void* data, bool ownForeignMemory=true, bool deleteMemory = true);

	//! constructor for empty image
	CImage(ECOLOR_FORMAT format, const core::dimension2d<u32>& size);

	//! destructor
	virtual ~CImage();

    virtual void initializeMipLevels(u8 levelCount, core::dimension2d<u32>* sizes) override;
    virtual void initializeMipLevels(u8 levelCount, core::dimension2d<u32>* sizes, u32* dataSizes) override;

	//! Lock function.
	virtual void* lock(u8 miplevel = 0) override
	{
        u8* pData = miplevel ? Levels && miplevel <= LevelCount ? Levels[miplevel - 1].Data : nullptr : Data;
        return pData;
	}

	//! Unlock function.
	virtual void unlock() override {}

	//! Returns width and height of image data.
	virtual const core::dimension2d<u32>& getDimension() const override;

	//! Returns bits per pixel.
	virtual f32 getBitsPerPixel() const override;

	//! Returns bytes per pixel
	virtual f32 getBytesPerPixel() const override;

	//! Returns image data size in bytes
	virtual u32 getImageDataSizeInBytes(u8 miplevel = 0) const override;

    virtual u32 getMemorySize() const override;

	//! Returns image data size in pixels
	virtual u32 getImageDataSizeInPixels() const override;

	//! returns mask for red value of a pixel
	virtual u32 getRedMask() const override;

	//! returns mask for green value of a pixel
	virtual u32 getGreenMask() const override;

	//! returns mask for blue value of a pixel
	virtual u32 getBlueMask() const override;

	//! returns mask for alpha value of a pixel
	virtual u32 getAlphaMask() const override;

	//! returns a pixel
	virtual SColor getPixel(u32 x, u32 y, u8 miplevel = 0) const override;

	//! sets a pixel
	virtual void setPixel(u32 x, u32 y, const SColor &color, bool blend = false, u8 miplevel = 0) override;

	//! returns the color format
	virtual ECOLOR_FORMAT getColorFormat() const override;

	//! returns pitch of image
	virtual u32 getPitch() const override { return Pitch; }

	//! copies this surface into another, scaling it to fit.
	virtual void copyToScaling(void* target, u32 width, u32 height, ECOLOR_FORMAT format, u32 pitch=0) override;

	//! copies this surface into another, scaling it to fit.
	virtual void copyToScaling(IImage* target) override;

	//! copies this surface into another
	virtual void copyTo(IImage* target, const core::position2d<s32>& pos=core::position2d<s32>(0,0)) override;

	//! copies this surface into another
	virtual void copyTo(IImage* target, const core::position2d<s32>& pos, const core::rect<s32>& sourceRect, const core::rect<s32>* clipRect=0) override;

	//! copies this surface into another, using the alpha mask, an cliprect and a color to add with
	virtual void copyToWithAlpha(IImage* target, const core::position2d<s32>& pos,
			const core::rect<s32>& sourceRect, const SColor &color,
			const core::rect<s32>* clipRect = 0) override;

	//! copies this surface into another, scaling it to fit, appyling a box filter
	virtual void copyToScalingBoxFilter(IImage* target, s32 bias = 0, bool blend = false) override;
         
	//! fills the surface with given color
	virtual void fill(const SColor &color) override;

    virtual u8* GetData(u8 miplevel = 0) override
    {
        u8* pData = miplevel ? Levels && miplevel <= LevelCount ? Levels[miplevel - 1].Data : nullptr : Data;
        return pData;
    }

    virtual u8 GetMipLevelCount() const override { return LevelCount; }

private:

	//! assumes format and size has been set and creates the rest
	void initData();

	inline SColor getPixelBox ( s32 x, s32 y, s32 fx, s32 fy, s32 bias ) const;

    struct MipLevel
    {
        u8* Data;
        u32 DataSize;
        core::dimension2d<u32> Size;
    };

	u8* Data;
	core::dimension2d<u32> Size;
	f32 BytesPerPixel;
	u32 Pitch;
	ECOLOR_FORMAT Format;
    MipLevel* Levels;
    u32 LevelCount;

	bool DeleteMemory;
};

} // end namespace video
} // end namespace irr


#endif

