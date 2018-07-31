#include "IrrCompileConfig.h"

#define _IRR_DONT_DO_MEMORY_DEBUGGING_HERE

#include "irrTypes.h"
#include "CVulkanDriver.h"
#include "CVulkanTexture.h"
#include "CVulkanDevice.h"
#include "CVulkanHardwareBuffer.h"
#include "os.h"
#include "CColorConverter.h"

#include <memory>

using namespace irr;
using namespace irr::video;

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

//! rendertarget constructor
CVulkanTexture::CVulkanTexture(CVulkanDriver* driver, const core::dimension2d<u32>& size, const io::path& name,
    const ECOLOR_FORMAT format, u32 arraySlices, u32 sampleCount, u32 sampleQuality, TextureType type)
    : ITexture(name)
    , CVulkanDeviceResource(static_cast<CVulkanDriver*>(driver))
    , TextureSize(size)
    , ImageSize(size)
    , Pitch(0)
    , UsageFlags(0)
    , QualityOfSample(sampleQuality)
    , NumberOfSamples(sampleCount)
    , NumberOfMipLevels(0)
    , NumberOfArraySlices(arraySlices)
    , Image(nullptr)
{
}

//! constructor
CVulkanTexture::CVulkanTexture(IImage* image, CVulkanDriver* driver, u32 flags, const io::path& name,
    u32 arraySlices, void* mipmapData, u32 sampleCount, u32 sampleQuality, TextureType type)
    : ITexture(name)
    , CVulkanDeviceResource(static_cast<CVulkanDriver*>(driver))
    , TextureSize(0, 0)
    , ImageSize(0, 0)
    , Pitch(0)
    , UsageFlags(flags)
    , QualityOfSample(sampleQuality)
    , NumberOfSamples(sampleCount)
    , NumberOfMipLevels(0)
    , NumberOfArraySlices(arraySlices)
    , Image(nullptr)
{
    memset(mImages, 0, sizeof(mImages));
    memset(mInternalFormats, 0, sizeof(mInternalFormats));
    mTextureType = type;
    mDeviceMask = GpuDeviceFlags::GDF_PRIMARY;
    
    bool success = InitializeColorFormat(flags, image->getColorFormat());
    _IRR_DEBUG_BREAK_IF(!success);
    
    ImageSize = image->getDimension();
    
    switch (image->getColorFormat())
    {
        case ECF_RGBA8:
        case ECF_DXT1:
        case ECF_DXT3:
        case ECF_DXT5:
            TextureSize = image->getDimension();
            mInternalFormats[0] = image->getColorFormat();
            Image = image;
            image->grab();
            break;
        default:
            if (getColorFormat() != image->getColorFormat())
            {
                TextureSize = ImageSize.getOptimalSize(!Driver->queryFeature(EVDF_TEXTURE_NPOT));
                Image = Driver->createImage(getColorFormat(), image->getDimension());
                image->copyTo(Image);
            }
            else
            {
                TextureSize = image->getDimension();
                mInternalFormats[0] = image->getColorFormat();
                Image = image;
                image->grab();
            }
            break;
    }
    
    NumberOfMipLevels = 1;
    if (Driver->getTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS))
    {
        if (image->GetMipLevelCount() > 0)
            NumberOfMipLevels = image->GetMipLevelCount() + 1;
        else
            NumberOfMipLevels = mostSignificantBitSet(std::max(image->getDimension().Width, image->getDimension().Height));
    }

    Pitch = (IImage::getBitsPerPixelFromFormat(getColorFormat()) / 8.0) * TextureSize.Width;

    Device = driver->_getPrimaryDevice();

    if (image)
    {
        if (createTexture(flags, Image, type))
        {
            void* imageData = Image->lock();
            void* data = lock(E_TEXTURE_LOCK_MODE::ETLM_WRITE_ONLY, 0, 0);
            memcpy(data, imageData, Image->getImageDataSizeInBytes());
            Image->unlock();
            unlock();

            regenerateMipMapLevels();
        }
        else
            os::Printer::log("Could not create DIRECT3D11 Texture.", ELL_WARNING);
    }
    
    clearImage();
}


//! destructor
CVulkanTexture::~CVulkanTexture()
{
    for (int i = 0; i < _MAX_DEVICES; ++i)
    {
        if (mImages[i])
        {
            mImages[i]->drop();
            mImages[i] = nullptr;
        }
    }

    if (mStagingBuffer)
        mStagingBuffer->drop();
    mStagingBuffer = nullptr;

    clearImage();
}

void CVulkanTexture::clearImage()
{
    if (Image)
        Image->drop();
    Image = nullptr;
}

void * irr::video::CVulkanTexture::lock(E_TEXTURE_LOCK_MODE mode, u32 mipmapLevel, u32 arraySlice)
{
    if (!mImages[0])
        return nullptr;

    if (NumberOfSamples > 1)
    {
        os::Printer::log("Multisampled textures cannot be accessed from the CPU directly.", ELL_WARNING);
        return nullptr;
    }
    
    mIsMapped = true;
    MappedMode = mode;
    mMappedDeviceIdx = 0;
    mMappedGlobalQueueIdx = 0;
    mMappedMipLevelIdx = mipmapLevel;
    mMappedArraySliceIdx = arraySlice;
    
    VulkanImage* image = mImages[mMappedDeviceIdx];
    VulkanDevice* device = Device;
    GpuQueueType queueType;
    u32 localQueueIdx = CommandSyncMask::getQueueIdxAndType(mMappedGlobalQueueIdx, queueType);

    MappedImageData result;
    result.format = getColorFormat();

    VulkanImageSubresource* subresource = image->getSubresource(mMappedArraySliceIdx, mMappedMipLevelIdx);

    if (mDirectlyMappable)
    {
        // Initially the texture will be in preinitialized layout, and it will transition to general layout on first
        // use in shader. No further transitions are allowed for directly mappable textures.
        assert(subresource->getLayout() == VK_IMAGE_LAYOUT_PREINITIALIZED || subresource->getLayout() == VK_IMAGE_LAYOUT_GENERAL);

        // GPU should never be allowed to write to a directly mappable texture, since only linear tiling is supported
        // for direct mapping, and we don't support using it with either storage textures or render targets.
        assert(!mSupportsGPUWrites);

        // Check is the GPU currently reading from the image
        u32 useMask = subresource->getUseInfo(VulkanUseFlag::Read);
        bool isUsedOnGPU = useMask != 0;

        // We're safe to map directly since GPU isn't using the subresource
        if (!isUsedOnGPU)
        {
            // If some CB has an operation queued that will be using the current contents of the image, create a new 
            // image so we don't modify the previous use of the image
            if (subresource->isBound())
            {
                VulkanImage* newImage = createImage(*device, UsageFlags, mInternalFormats[mMappedDeviceIdx]);

                // Copy contents of the current image to the new one, unless caller explicitly specifies he doesn't
                // care about the current contents
                if (mode != ETLM_WRITE_ONLY)
                {
                    VkMemoryRequirements memReqs;
                    vkGetImageMemoryRequirements(device->getLogical(), image->getHandle(), &memReqs);

                    UINT8* src = image->map(0, (u32)memReqs.size);
                    UINT8* dst = newImage->map(0, (u32)memReqs.size);

                    memcpy(dst, src, memReqs.size);

                    image->unmap();
                    newImage->unmap();
                }

                image->drop();
                image = newImage;
                mImages[mMappedDeviceIdx] = image;
            }

            image->map(mMappedArraySliceIdx, mMappedMipLevelIdx, result);
            return result.data;
        }

        // Caller guarantees he won't touch the same data as the GPU, so just map even though the GPU is using the
        // subresource
        if (mode == ETLM_WRITE_ONLY)
        {
            image->map(mMappedArraySliceIdx, mMappedMipLevelIdx, result);
            return result.data;
        }

        //// Caller doesn't care about buffer contents, so just discard the existing buffer and create a new one
        //if (options == GBL_WRITE_ONLY_DISCARD)
        //{
        //    // We need to discard the entire image, even though we're only writing to a single sub-resource
        //    image->destroy();
        //
        //    image = createImage(device, mInternalFormats[deviceIdx]);
        //    mImages[deviceIdx] = image;
        //
        //    image->map(face, mipLevel, lockedArea);
        //    return lockedArea;
        //}

        // We need to read the buffer contents
        if (mode == ETLM_READ_ONLY || mode == ETLM_READ_WRITE)
        {
            VulkanTransferBuffer* transferCB = device->getTransferBuffer(queueType, localQueueIdx);

            // Ensure flush() will wait for all queues currently using to the texture (if any) to finish
            // If only reading, wait for all writes to complete, otherwise wait on both writes and reads
            if (mode == ETLM_READ_ONLY)
                useMask = subresource->getUseInfo(VulkanUseFlag::Write);
            else
                useMask = subresource->getUseInfo(VulkanUseFlag::Read | VulkanUseFlag::Write);

            transferCB->appendMask(useMask);

            // Submit the command buffer and wait until it finishes
            transferCB->flush(true);

            // If writing and some CB has an operation queued that will be using the current contents of the image, 
            // create a new image so we don't modify the previous use of the image
            if (mode == ETLM_READ_WRITE && subresource->isBound())
            {
                VulkanImage* newImage = createImage(*device, UsageFlags, mInternalFormats[mMappedDeviceIdx]);

                VkMemoryRequirements memReqs;
                vkGetImageMemoryRequirements(device->getLogical(), image->getHandle(), &memReqs);

                UINT8* src = image->map(0, (u32)memReqs.size);
                UINT8* dst = newImage->map(0, (u32)memReqs.size);

                memcpy(dst, src, memReqs.size);

                image->unmap();
                newImage->unmap();

                image->drop();
                image = newImage;
                mImages[mMappedDeviceIdx] = image;
            }

            image->map(mMappedArraySliceIdx, mMappedMipLevelIdx, result);
            return result.data;
        }

        // Otherwise, we're doing write only, in which case it's best to use the staging buffer to avoid waiting
        // and blocking, so fall through
    }

    // Can't use direct mapping, so use a staging buffer

    // We might need to copy the current contents of the image to the staging buffer. Even if the user doesn't plan on
    // reading, it is still required as we will eventually copy all of the contents back to the original image,
    // and we can't write potentially uninitialized data. The only exception is when the caller specifies the image
    // contents should be discarded in which he guarantees he will overwrite the entire locked area with his own
    // contents.
    bool needRead = mode != ETLM_WRITE_ONLY;

    // Allocate a staging buffer
    if (!createTextureBuffer(needRead))
        return nullptr;

    if (needRead) // If reading, we need to copy the current contents of the image to the staging buffer
    {
        VulkanTransferBuffer* transferCB = device->getTransferBuffer(queueType, localQueueIdx);

        // Similar to above, if image supports GPU writes or is currently being written to, we need to wait on any
        // potential writes to complete
        u32 writeUseMask = subresource->getUseInfo(VulkanUseFlag::Write);

        if (mSupportsGPUWrites || writeUseMask != 0)
        {
            // Ensure flush() will wait for all queues currently writing to the image (if any) to finish
            transferCB->appendMask(writeUseMask);
        }

        VkImageSubresourceRange range;
        range.aspectMask = image->getAspectFlags();
        range.baseArrayLayer = mMappedArraySliceIdx;
        range.layerCount = 1;
        range.baseMipLevel = mMappedMipLevelIdx;
        range.levelCount = 1;

        VkImageSubresourceLayers rangeLayers;
        if ((UsageFlags & ETCF_USAGE_DEPTHSTENCIL) != 0)
            rangeLayers.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        else
            rangeLayers.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

        rangeLayers.baseArrayLayer = range.baseArrayLayer;
        rangeLayers.layerCount = range.layerCount;
        rangeLayers.mipLevel = range.baseMipLevel;

        VkExtent3D extent;
        ITexture::getSizeForMipLevel(TextureSize.Width, TextureSize.Height, 1, mMappedMipLevelIdx, extent.width, extent.height, extent.depth);

        // Transfer texture to a valid layout
        VkAccessFlags currentAccessMask = image->getAccessFlags(subresource->getLayout());
        transferCB->setLayout(image->getHandle(), currentAccessMask, VK_ACCESS_TRANSFER_READ_BIT, subresource->getLayout(),
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, range);

        // Queue copy command
        image->copy(transferCB->getCB(), mStagingBuffer, extent, rangeLayers, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

        // Transfer back to original layout
        VkImageLayout dstLayout = image->getOptimalLayout();
        currentAccessMask = image->getAccessFlags(dstLayout);

        transferCB->setLayout(image->getHandle(), VK_ACCESS_TRANSFER_READ_BIT, currentAccessMask,
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dstLayout, range);
        transferCB->getCB()->registerResource(image, range, VulkanUseFlag::Read, ResourceUsage::Transfer);

        // Ensure data written to the staging buffer is visible
        VkAccessFlags stagingAccessFlags;
        if (mode == ETLM_READ_ONLY)
            stagingAccessFlags = VK_ACCESS_HOST_READ_BIT;
        else // Must be read/write
            stagingAccessFlags = VK_ACCESS_HOST_READ_BIT | VK_ACCESS_HOST_WRITE_BIT;

        transferCB->memoryBarrier(mStagingBuffer->getHandle(),
            VK_ACCESS_TRANSFER_WRITE_BIT,
            stagingAccessFlags,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_HOST_BIT);

        // Submit the command buffer and wait until it finishes
        transferCB->flush(true);
    }

    u32 mipWidth = std::max(1u, TextureSize.Width >> mipmapLevel);
    u32 mipHeight = std::max(1u, TextureSize.Height >> mipmapLevel);
    u32 mipDepth = std::max(1u, 1u >> mipmapLevel);

    UINT8* data = mStagingBuffer->map(0, IImage::getMemorySize(mipWidth, mipHeight, mipDepth, mInternalFormats[mMappedDeviceIdx]));
    return data;
}

void irr::video::CVulkanTexture::unlock()
{
    // Possibly map() failed with some error
    if (!mIsMapped)
        return;

    // Note: If we did any writes they need to be made visible to the GPU. However there is no need to execute 
    // a pipeline barrier because (as per spec) host writes are implicitly visible to the device.

    if (mStagingBuffer == nullptr)
        mImages[mMappedDeviceIdx]->unmap();
    else
    {
        mStagingBuffer->unmap();

        bool isWrite = MappedMode != ETLM_READ_ONLY;

        // If the caller wrote anything to the staging buffer, we need to upload it back to the main buffer
        if (isWrite)
        {
            VulkanDevice& device = *Driver->_getDevice(mMappedDeviceIdx);

            GpuQueueType queueType;
            u32 localQueueIdx = CommandSyncMask::getQueueIdxAndType(mMappedGlobalQueueIdx, queueType);

            VulkanImage* image = mImages[mMappedDeviceIdx];
            VulkanTransferBuffer* transferCB = device.getTransferBuffer(queueType, localQueueIdx);

            VulkanImageSubresource* subresource = image->getSubresource(mMappedArraySliceIdx, mMappedMipLevelIdx);
            VkImageLayout curLayout = subresource->getLayout();

            // If the subresource is used in any way on the GPU, we need to wait for that use to finish before
            // we issue our copy
            u32 useMask = subresource->getUseInfo(VulkanUseFlag::Read | VulkanUseFlag::Write);
            bool isNormalWrite = false;
            if (useMask != 0) // Subresource is currently used on the GPU
            {
                // Try to avoid the wait by checking for special write conditions

                // Caller guarantees he won't touch the same data as the GPU, so just copy
                if (MappedMode == ETLM_WRITE_ONLY)
                {
                    // Fall through to copy()
                }
                //// Caller doesn't care about buffer contents, so just discard the existing buffer and create a new one
                //else if (mMappedLockOptions == GBL_WRITE_ONLY_DISCARD)
                //{
                //    // We need to discard the entire image, even though we're only writing to a single sub-resource
                //    image->destroy();
                //
                //    image = createImage(device, mInternalFormats[mMappedDeviceIdx]);
                //    mImages[mMappedDeviceIdx] = image;
                //
                //    subresource = image->getSubresource(mMappedFace, mMappedMip);
                //}
                else // Otherwise we have no choice but to issue a dependency between the queues
                {
                    transferCB->appendMask(useMask);
                    isNormalWrite = true;
                }
            }
            else
                isNormalWrite = true;

            // Check if the subresource will still be bound somewhere after the CBs using it finish
            if (isNormalWrite)
            {
                u32 useCount = subresource->getUseCount();
                u32 boundCount = subresource->getBoundCount();

                bool isBoundWithoutUse = boundCount > useCount;

                // If image is queued for some operation on a CB, then we need to make a copy of the subresource to
                // avoid modifying its use in the previous operation
                if (isBoundWithoutUse)
                {
                    VulkanImage* newImage = createImage(device, UsageFlags, mInternalFormats[mMappedDeviceIdx]);

                    // Avoid copying original contents if the image only has one sub-resource, which we'll overwrite anyway
                    if (NumberOfMipLevels > 1 || NumberOfArraySlices > 1)
                    {
                        VkImageLayout oldImgLayout = image->getOptimalLayout();

                        curLayout = newImage->getOptimalLayout();
                        copyImage(transferCB, image, newImage, oldImgLayout, curLayout);
                    }

                    image->drop();
                    image = newImage;
                    mImages[mMappedDeviceIdx] = image;
                }
            }

            VkImageSubresourceRange range;
            range.aspectMask = image->getAspectFlags();
            range.baseArrayLayer = mMappedArraySliceIdx;
            range.layerCount = 1;
            range.baseMipLevel = mMappedMipLevelIdx;
            range.levelCount = 1;

            VkImageSubresourceLayers rangeLayers;
            rangeLayers.aspectMask = range.aspectMask;
            rangeLayers.baseArrayLayer = range.baseArrayLayer;
            rangeLayers.layerCount = range.layerCount;
            rangeLayers.mipLevel = range.baseMipLevel;

            VkExtent3D extent;
            ITexture::getSizeForMipLevel(TextureSize.Width, TextureSize.Height, 1, mMappedMipLevelIdx, extent.width, extent.height, extent.depth);

            VkImageLayout transferLayout;
            if (mDirectlyMappable)
                transferLayout = VK_IMAGE_LAYOUT_GENERAL;
            else
                transferLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

            // Transfer texture to a valid layout
            VkAccessFlags currentAccessMask = image->getAccessFlags(curLayout);
            transferCB->setLayout(image->getHandle(), currentAccessMask, VK_ACCESS_TRANSFER_WRITE_BIT,
                curLayout, transferLayout, range);

            // Queue copy command
            mStagingBuffer->copy(transferCB->getCB(), image, extent, rangeLayers, transferLayout);

            // Transfer back to original  (or optimal if initial layout was undefined/preinitialized)
            VkImageLayout dstLayout = image->getOptimalLayout();

            currentAccessMask = image->getAccessFlags(dstLayout);
            transferCB->setLayout(image->getHandle(), VK_ACCESS_TRANSFER_WRITE_BIT, currentAccessMask, transferLayout, dstLayout, range);

            // Notify the command buffer that these resources are being used on it
            transferCB->getCB()->registerResource(mStagingBuffer, VK_ACCESS_TRANSFER_READ_BIT, VulkanUseFlag::Read);
            transferCB->getCB()->registerResource(image, range, VulkanUseFlag::Write, ResourceUsage::Transfer);

            // We don't actually flush the transfer buffer here since it's an expensive operation, but it's instead
            // done automatically before next "normal" command buffer submission.
        }

        mStagingBuffer->drop();
        mStagingBuffer = nullptr;
    }

    mIsMapped = false;
}

const core::dimension2d<u32>& irr::video::CVulkanTexture::getOriginalSize() const
{
    return TextureSize;
}

const core::dimension2d<u32>& irr::video::CVulkanTexture::getSize() const
{
    return ImageSize;
}

E_DRIVER_TYPE irr::video::CVulkanTexture::getDriverType() const
{
    return E_DRIVER_TYPE::EDT_VULKAN;
}

ECOLOR_FORMAT irr::video::CVulkanTexture::getColorFormat() const
{
    return mInternalFormats[0];
}

u32 irr::video::CVulkanTexture::getPitch() const
{
    return Pitch;
}

bool irr::video::CVulkanTexture::hasMipMaps() const
{
    return NumberOfMipLevels > 1;
}

u32 irr::video::CVulkanTexture::getNumberOfArraySlices() const
{
    return NumberOfArraySlices;
}

bool irr::video::CVulkanTexture::createMipMaps(u32 level)
{
    if (level == 0)
        return true;

    VkFormatProperties formatProperties;

    // Get device properites for the requested texture format
    vkGetPhysicalDeviceFormatProperties(Device->getPhysical(), VulkanUtility::getPixelFormat(getColorFormat(), false), &formatProperties);

    // Hardware mode
    if ((formatProperties.optimalTilingFeatures & (VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT)) == (VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT))
    {
        // Generate the mip chain
        // ---------------------------------------------------------------
        // We copy down the whole mip chain doing a blit from mip-1 to mip
        // An alternative way would be to always blit from the first mip level and sample that one down
        VulkanDevice* device = Device;
        GpuQueueType queueType;
        u32 localQueueIdx = CommandSyncMask::getQueueIdxAndType(mMappedGlobalQueueIdx, queueType);

        VulkanTransferBuffer* transferCB = Driver->_getPrimaryDevice()->getTransferBuffer(queueType, localQueueIdx);

        VulkanImageSubresource* subresource = GetVkImages(0)->getSubresource(0, 0);
        VkImageLayout curLayout = subresource->getLayout();

        VkImageSubresourceRange mipSubRange = {};
        mipSubRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        mipSubRange.baseMipLevel = 0;
        mipSubRange.levelCount = 1;
        mipSubRange.layerCount = 1;

        transferCB->setLayout(GetVkImages(0)->getHandle(), GetVkImages(0)->getAccessFlags(VK_IMAGE_LAYOUT_UNDEFINED), GetVkImages(0)->getAccessFlags(VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL),
            VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, mipSubRange);

        // Copy down mips from n-1 to n
        for (int32_t i = 1; i < NumberOfMipLevels; i++)
        {
            VkImageBlit imageBlit{};

            // Source
            imageBlit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            imageBlit.srcSubresource.layerCount = 1;
            imageBlit.srcSubresource.mipLevel = i - 1;
            imageBlit.srcOffsets[1].x = std::max(1, int32_t(TextureSize.Width >> (i - 1)));
            imageBlit.srcOffsets[1].y = std::max(1, int32_t(TextureSize.Height >> (i - 1)));
            imageBlit.srcOffsets[1].z = 1;

            // Destination
            imageBlit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            imageBlit.dstSubresource.layerCount = 1;
            imageBlit.dstSubresource.mipLevel = i;
            imageBlit.dstOffsets[1].x = std::max(1, int32_t(TextureSize.Width >> i));
            imageBlit.dstOffsets[1].y = std::max(1, int32_t(TextureSize.Height >> i));
            imageBlit.dstOffsets[1].z = 1;

            VkImageSubresourceRange mipSubRange = {};
            mipSubRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            mipSubRange.baseMipLevel = i;
            mipSubRange.levelCount = 1;
            mipSubRange.layerCount = 1;

            // Transfer texture to a valid layout
            VkAccessFlags currentAccessMask = GetVkImages(0)->getAccessFlags(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            transferCB->setLayout(GetVkImages(0)->getHandle(), VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_TRANSFER_WRITE_BIT,
                VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipSubRange);

            // Blit from previous level
            vkCmdBlitImage(
                transferCB->getCB()->getHandle(),
                GetVkImages(0)->getHandle(),
                VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                GetVkImages(0)->getHandle(),
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1,
                &imageBlit,
                VK_FILTER_LINEAR);

            // Transfer texture to a valid layout
            currentAccessMask = GetVkImages(0)->getAccessFlags(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
            transferCB->setLayout(GetVkImages(0)->getHandle(), currentAccessMask, VK_ACCESS_TRANSFER_READ_BIT,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, mipSubRange);
        }

        VkImageSubresourceRange subresourceRange = {};
        subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        subresourceRange.levelCount = NumberOfMipLevels;
        subresourceRange.layerCount = 1;

        VkAccessFlags currentAccessMask = GetVkImages(0)->getAccessFlags(VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
        VkAccessFlags dstAccessMask = GetVkImages(0)->getAccessFlags(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        transferCB->setLayout(GetVkImages(0)->getHandle(), currentAccessMask, dstAccessMask,
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, subresourceRange);

        transferCB->appendMask(subresource->getUseInfo(VulkanUseFlag::Write));
        transferCB->getCB()->registerResource(GetVkImages(0), subresourceRange, VulkanUseFlag::Read, ResourceUsage::Transfer);

        // Submit the command buffer and wait until it finishes
        transferCB->flush(true);
    }
    else // Software mode (Experimental)
    {
        core::dimension2du size = TextureSize;
        u32 level = 0;
        u32 pixelSize = (IImage::getBitsPerPixelFromFormat(getColorFormat()) / 8.0);
        do
        {
            if ( size.Width>1 )
                size.Width /= 2;
            if ( size.Height>1 )
                size.Height /= 2;

            ++level;

            if ( NumberOfMipLevels <= level )
                break;

            void* data = lock(E_TEXTURE_LOCK_MODE::ETLM_WRITE_ONLY, level, 0);
            Image->copyToScaling(data, size.Width, size.Height, getColorFormat(), pixelSize * size.Width * size.Height);
            unlock();

        } while ( size.Width != 1 || size.Height != 1 );
    }

    return true;
}

bool irr::video::CVulkanTexture::isRenderTarget() const
{
    return false;
}

bool irr::video::CVulkanTexture::InitializeColorFormat(u32 flags, ECOLOR_FORMAT colorFormat)
{
    // get color format
    mInternalFormats[0] = colorFormat;

    return mInternalFormats[0] != ECF_UNKNOWN;
}

void irr::video::CVulkanTexture::createRenderTarget(const ECOLOR_FORMAT format)
{
}


VulkanImage* irr::video::CVulkanTexture::createImage(VulkanDevice& device, u32 usage, ECOLOR_FORMAT format)
{
    bool directlyMappable = mImageCI.tiling == VK_IMAGE_TILING_LINEAR;
    VkMemoryPropertyFlags flags = directlyMappable ? (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) : // Note: Try using cached memory
                                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    VkDevice vkDevice = device.getLogical();

    mImageCI.format = VulkanUtility::getPixelFormat(format, false);

    VULKAN_IMAGE_DESC imgDesc;
    VkResult result = vkCreateImage(vkDevice, &mImageCI, VulkanDevice::gVulkanAllocator, &imgDesc.image);
    assert(result == VK_SUCCESS);

    imgDesc.allocation = device.allocateMemory(imgDesc.image, flags);
    imgDesc.layout = mImageCI.initialLayout;
    imgDesc.type = mTextureType;
    imgDesc.format = VulkanUtility::getPixelFormat(getColorFormat());
    imgDesc.numFaces = NumberOfArraySlices;
    imgDesc.numMipLevels = NumberOfMipLevels;
    imgDesc.usage = usage;

    return new VulkanImage(Driver, imgDesc);
}

bool irr::video::CVulkanTexture::createTexture(u32 flags, IImage * image, TextureType type)
{
    mTextureType = type;

    mImageCI.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    mImageCI.pNext = nullptr;
    mImageCI.flags = 0;

    switch (type)
    {
        case TextureType::e1D:
            mImageCI.imageType = VK_IMAGE_TYPE_1D;
            break;
        case TextureType::e2D:
            mImageCI.imageType = VK_IMAGE_TYPE_2D;
            break;
        case TextureType::e3D:
            mImageCI.imageType = VK_IMAGE_TYPE_3D;
            break;
        case TextureType::eCube:
            mImageCI.imageType = VK_IMAGE_TYPE_2D;
            mImageCI.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
            break;
    }


    // Note: I force rendertarget and depthstencil types to be readable in shader. Depending on performance impact
    // it might be beneficial to allow the user to enable this explicitly only when needed.

    mImageCI.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

    if ((flags & ETCF_USAGE_RENDERTARGET) != 0)
    {
        mImageCI.usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        mSupportsGPUWrites = true;
    }
    else if ((flags & ETCF_USAGE_DEPTHSTENCIL) != 0)
    {
        mImageCI.usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        mSupportsGPUWrites = true;
    }

    if ((flags & ETCF_USAGE_LOADSTORE) != 0)
    {
        mImageCI.usage |= VK_IMAGE_USAGE_STORAGE_BIT;
        mSupportsGPUWrites = true;
    }

    VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;
    VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;
    if ((flags & ETCF_USAGE_DYNAMIC) != 0) // Attempt to use linear tiling for dynamic textures, so we can directly map and modify them
    {
        // Only support 2D textures, with one sample and one mip level, only used for shader reads
        // (Optionally check vkGetPhysicalDeviceFormatProperties & vkGetPhysicalDeviceImageFormatProperties for
        // additional supported configs, but right now there doesn't seem to be any additional support)
        if (type == TextureType::e2D && QualityOfSample <= 1 && NumberOfMipLevels <= 1 &&
            NumberOfArraySlices == 1 && (mImageCI.usage & VK_IMAGE_USAGE_SAMPLED_BIT) != 0)
        {
            // Also, only support normal textures, not render targets or storage textures
            if (!mSupportsGPUWrites)
            {
                mDirectlyMappable = true;
                tiling = VK_IMAGE_TILING_LINEAR;
                layout = VK_IMAGE_LAYOUT_PREINITIALIZED;
            }
        }
    }

    mImageCI.extent = { TextureSize.Width, TextureSize.Height, /*TextureSize.Depth*/ 1 };
    mImageCI.mipLevels = NumberOfMipLevels;
    mImageCI.arrayLayers = NumberOfArraySlices;
    mImageCI.samples = VulkanUtility::getSampleFlags(NumberOfSamples);
    mImageCI.tiling = tiling;
    mImageCI.initialLayout = layout;
    mImageCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    mImageCI.queueFamilyIndexCount = 0;
    mImageCI.pQueueFamilyIndices = nullptr;

    VulkanDevice* devices[_MAX_DEVICES];
    VulkanUtility::getDevices(Driver, mDeviceMask, devices);

    // Allocate buffers per-device
    for (u32 i = 0; i < _MAX_DEVICES; i++)
    {
        if (devices[i] == nullptr)
            continue;

        bool optimalTiling = tiling == VK_IMAGE_TILING_OPTIMAL;

        mInternalFormats[i] = VulkanUtility::getClosestSupportedPixelFormat(*devices[i], getColorFormat(), type, flags, optimalTiling, false);

        mImages[i] = createImage(*devices[i], flags, mInternalFormats[i]);
    }

    return true;
}

void irr::video::CVulkanTexture::copyImage(VulkanTransferBuffer * cb, VulkanImage * srcImage, VulkanImage * dstImage, VkImageLayout srcFinalLayout, VkImageLayout dstFinalLayout)
{
    u32 numFaces = NumberOfArraySlices;
    u32 numMipmaps = NumberOfMipLevels;

    u32 mipWidth = TextureSize.Width;
    u32 mipHeight = TextureSize.Height;
    u32 mipDepth = 1;

    VkImageCopy* imageRegions = new VkImageCopy[numMipmaps];

    for (u32 i = 0; i < numMipmaps; i++)
    {
        VkImageCopy& imageRegion = imageRegions[i];

        imageRegion.srcOffset = { 0, 0, 0 };
        imageRegion.dstOffset = { 0, 0, 0 };
        imageRegion.extent = { mipWidth, mipHeight, mipDepth };
        imageRegion.srcSubresource.baseArrayLayer = 0;
        imageRegion.srcSubresource.layerCount = numFaces;
        imageRegion.srcSubresource.mipLevel = i;
        imageRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageRegion.dstSubresource.baseArrayLayer = 0;
        imageRegion.dstSubresource.layerCount = numFaces;
        imageRegion.dstSubresource.mipLevel = i;
        imageRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

        if (mipWidth != 1) mipWidth /= 2;
        if (mipHeight != 1) mipHeight /= 2;
        if (mipDepth != 1) mipDepth /= 2;
    }

    VkImageSubresourceRange range;
    range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    range.baseArrayLayer = 0;
    range.layerCount = numFaces;
    range.baseMipLevel = 0;
    range.levelCount = numMipmaps;

    VkImageLayout transferSrcLayout, transferDstLayout;
    if (mDirectlyMappable)
    {
        transferSrcLayout = VK_IMAGE_LAYOUT_GENERAL;
        transferDstLayout = VK_IMAGE_LAYOUT_GENERAL;
    }
    else
    {
        transferSrcLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        transferDstLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    }

    // Transfer textures to a valid layout
    cb->setLayout(srcImage, range, VK_ACCESS_TRANSFER_READ_BIT, transferSrcLayout);
    cb->setLayout(dstImage, range, VK_ACCESS_TRANSFER_WRITE_BIT, transferDstLayout);

    vkCmdCopyImage(cb->getCB()->getHandle(), srcImage->getHandle(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        dstImage->getHandle(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, numMipmaps, imageRegions);

    // Transfer back to final layouts
    VkAccessFlags srcAccessMask = srcImage->getAccessFlags(srcFinalLayout);
    cb->setLayout(srcImage->getHandle(), VK_ACCESS_TRANSFER_READ_BIT, srcAccessMask,
        transferSrcLayout, srcFinalLayout, range);

    VkAccessFlags dstAccessMask = dstImage->getAccessFlags(dstFinalLayout);
    cb->setLayout(dstImage->getHandle(), VK_ACCESS_TRANSFER_WRITE_BIT, dstAccessMask,
        transferDstLayout, dstFinalLayout, range);

    cb->getCB()->registerResource(srcImage, range, VulkanUseFlag::Read, ResourceUsage::Transfer);
    cb->getCB()->registerResource(dstImage, range, VulkanUseFlag::Write, ResourceUsage::Transfer);

    delete [](imageRegions);
}

bool irr::video::CVulkanTexture::copyTexture(IImage * image)
{
    return true;
}

bool irr::video::CVulkanTexture::createTextureBuffer(bool readable)
{
    if (mStagingBuffer)
    {
        if (mStagingBufferReadable == readable)
            return true;

        mStagingBuffer->drop();
        mStagingBuffer = nullptr;
    }

    mStagingBufferReadable = readable;

    VkBufferCreateInfo bufferCI;
    bufferCI.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCI.pNext = nullptr;
    bufferCI.flags = 0;
    bufferCI.size = Pitch * TextureSize.Height;
    bufferCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferCI.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    bufferCI.queueFamilyIndexCount = 0;
    bufferCI.pQueueFamilyIndices = nullptr;

    if (readable)
        bufferCI.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;

    VkDevice vkDevice = Device->getLogical();

    VkBuffer buffer;
    VkResult result = vkCreateBuffer(vkDevice, &bufferCI, VulkanDevice::gVulkanAllocator, &buffer);
    assert(result == VK_SUCCESS);

    VkMemoryPropertyFlags flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    VmaAllocation allocation = Device->allocateMemory(buffer, flags);

    mStagingBuffer = new VulkanBuffer(Driver, buffer, VK_NULL_HANDLE, allocation, 0, 0);
    return true;
}

bool irr::video::CVulkanTexture::createViews()
{
    return false;
}

void irr::video::CVulkanTexture::OnDeviceLost(CVulkanDriver * device)
{
}

void irr::video::CVulkanTexture::OnDeviceRestored(CVulkanDriver * device)
{
}

void irr::video::CVulkanTexture::regenerateMipMapLevels(void * mipmapData)
{
    if (Image && Image->GetMipLevelCount() > 0)
    {
        for (u8 level = 1; level <= Image->GetMipLevelCount(); ++level)
        {
            void* data = lock(E_TEXTURE_LOCK_MODE::ETLM_WRITE_ONLY, level, 0);
            memcpy(data, Image->lock(level), Image->getImageDataSizeInBytes(level));
            Image->unlock();
            unlock();
        }
    }
    else
    {
        createMipMaps(1);
    }
}

VulkanImage::VulkanImage(CVulkanDriver* owner, const VULKAN_IMAGE_DESC& desc, bool ownsImage)
    : CVulkanDeviceResource(owner), mImage(desc.image), mAllocation(desc.allocation)
    , mFramebufferMainView(VK_NULL_HANDLE), mUsage(desc.usage), mOwnsImage(ownsImage), mNumFaces(desc.numFaces)
    , mNumMipLevels(desc.numMipLevels)
{
    mImageViewCI = {};
    mImageViewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    mImageViewCI.pNext = nullptr;
    mImageViewCI.flags = 0;
    mImageViewCI.image = desc.image;
    mImageViewCI.format = desc.format;
    mImageViewCI.components = {
        VK_COMPONENT_SWIZZLE_R,
        VK_COMPONENT_SWIZZLE_G,
        VK_COMPONENT_SWIZZLE_B,
        VK_COMPONENT_SWIZZLE_A
    };

    switch (desc.type)
    {
        default:
        case TextureType::e2D:
            mImageViewCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
            break;
        case TextureType::e3D:
            mImageViewCI.viewType = VK_IMAGE_VIEW_TYPE_3D;
            break;
        case TextureType::eCube:
            mImageViewCI.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
            break;
    }

    TextureSurface completeSurface(0, desc.numMipLevels, 0, desc.numFaces);
    if ((mUsage & ETCF_USAGE_DEPTHSTENCIL) != 0)
    {
        mFramebufferMainView = createView(completeSurface, getAspectFlags());
        mMainView = createView(completeSurface, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);
    }
    else
        mMainView = createView(completeSurface, VK_IMAGE_ASPECT_COLOR_BIT);

    ImageViewInfo mainViewInfo;
    mainViewInfo.surface = completeSurface;
    mainViewInfo.framebuffer = false;
    mainViewInfo.view = mMainView;

    mImageInfos.push_back(mainViewInfo);

    if (mFramebufferMainView != VK_NULL_HANDLE)
    {
        ImageViewInfo fbMainViewInfo;
        fbMainViewInfo.surface = completeSurface;
        fbMainViewInfo.framebuffer = true;
        fbMainViewInfo.view = mFramebufferMainView;

        mImageInfos.push_back(fbMainViewInfo);
    }

    uint32_t numSubresources = mNumFaces * mNumMipLevels;
    mSubresources = new VulkanImageSubresource*[numSubresources];
    for (uint32_t i = 0; i < numSubresources; i++)
        mSubresources[i] = new VulkanImageSubresource(Driver, desc.layout);
}

VulkanImage::~VulkanImage()
{
    VulkanDevice* device = Driver->_getPrimaryDevice();
    VkDevice vkDevice = device->getLogical();

    uint32_t numSubresources = mNumFaces * mNumMipLevels;
    for (uint32_t i = 0; i < numSubresources; i++)
    {
        assert(!mSubresources[i]->isBound()); // Image beeing freed but its subresources are still bound somewhere

        mSubresources[i]->drop();
    }

    for (auto& entry : mImageInfos)
        vkDestroyImageView(vkDevice, entry.view, VulkanDevice::gVulkanAllocator);

    if (mOwnsImage)
    {
        vkDestroyImage(vkDevice, mImage, VulkanDevice::gVulkanAllocator);
        device->freeMemory(mAllocation);
    }
}

VkImageView VulkanImage::getView(bool framebuffer) const
{
    if (framebuffer && (mUsage & ETCF_USAGE_DEPTHSTENCIL) != 0)
        return mFramebufferMainView;

    return mMainView;
}

VkImageView VulkanImage::getView(const TextureSurface& surface, bool framebuffer) const
{
    for (auto& entry : mImageInfos)
    {
        if (surface.mipLevel == entry.surface.mipLevel &&
            surface.numMipLevels == entry.surface.numMipLevels &&
            surface.face == entry.surface.face &&
            surface.numFaces == entry.surface.numFaces)
        {
            if ((mUsage & ETCF_USAGE_DEPTHSTENCIL) == 0)
                return entry.view;
            else
            {
                if (framebuffer == entry.framebuffer)
                    return entry.view;
            }
        }
    }

    ImageViewInfo info;
    info.surface = surface;
    info.framebuffer = framebuffer;

    if ((mUsage & ETCF_USAGE_DEPTHSTENCIL) != 0)
    {
        if (framebuffer)
            info.view = createView(surface, getAspectFlags());
        else
            info.view = createView(surface, VK_IMAGE_ASPECT_DEPTH_BIT);
    }
    else
        info.view = createView(surface, VK_IMAGE_ASPECT_COLOR_BIT);

    mImageInfos.push_back(info);

    return info.view;
}

VkImageView VulkanImage::createView(const TextureSurface& surface, VkImageAspectFlags aspectMask) const
{
    VkImageViewType oldViewType = mImageViewCI.viewType;

    const uint32_t numFaces = surface.numFaces == 0 ? mNumFaces : surface.numFaces;

    switch (oldViewType)
    {
        case VK_IMAGE_VIEW_TYPE_CUBE:
            if (numFaces == 1)
                mImageViewCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
            else if (numFaces % 6 == 0)
            {
                if (mNumFaces > 6)
                    mImageViewCI.viewType = VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
            }
            else
                mImageViewCI.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
            break;
        case VK_IMAGE_VIEW_TYPE_1D:
            if (numFaces > 1)
                mImageViewCI.viewType = VK_IMAGE_VIEW_TYPE_1D_ARRAY;
            break;
        case VK_IMAGE_VIEW_TYPE_2D:
        case VK_IMAGE_VIEW_TYPE_3D:
            if (numFaces > 1)
                mImageViewCI.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
            break;
        default:
            break;
    }

    mImageViewCI.subresourceRange.aspectMask = aspectMask;
    mImageViewCI.subresourceRange.baseMipLevel = surface.mipLevel;
    mImageViewCI.subresourceRange.levelCount = surface.numMipLevels == 0 ? VK_REMAINING_MIP_LEVELS : surface.numMipLevels;
    mImageViewCI.subresourceRange.baseArrayLayer = surface.face;
    mImageViewCI.subresourceRange.layerCount = surface.numFaces == 0 ? VK_REMAINING_ARRAY_LAYERS : surface.numFaces;

    VkImageView view = {};
    VkResult result = vkCreateImageView(Driver->_getPrimaryDevice()->getLogical(), &mImageViewCI, VulkanDevice::gVulkanAllocator, &view);
    assert(result == VK_SUCCESS);

    mImageViewCI.viewType = oldViewType;
    return view;
}

void irr::video::VulkanImage::OnDeviceLost(CVulkanDriver * device)
{
}

void irr::video::VulkanImage::OnDeviceRestored(CVulkanDriver * device)
{
}

VkImageLayout VulkanImage::getOptimalLayout() const
{
    // If it's load-store, no other flags matter, it must be in general layout
    if ((mUsage & ETCF_USAGE_LOADSTORE) != 0)
        return VK_IMAGE_LAYOUT_GENERAL;

    if ((mUsage & ETCF_USAGE_RENDERTARGET) != 0)
        return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    else if ((mUsage & ETCF_USAGE_DEPTHSTENCIL) != 0)
        return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    else
    {
        if ((mUsage & ETCF_USAGE_DYNAMIC) != 0)
            return VK_IMAGE_LAYOUT_GENERAL;

        return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    }
}

VkImageAspectFlags VulkanImage::getAspectFlags() const
{
    if ((mUsage & ETCF_USAGE_DEPTHSTENCIL) != 0)
    {
        bool hasStencil = mImageViewCI.format == VK_FORMAT_D16_UNORM_S8_UINT ||
            mImageViewCI.format == VK_FORMAT_D24_UNORM_S8_UINT ||
            mImageViewCI.format == VK_FORMAT_D32_SFLOAT_S8_UINT;

        if (hasStencil)
            return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;

        return VK_IMAGE_ASPECT_DEPTH_BIT;
    }

    return VK_IMAGE_ASPECT_COLOR_BIT;
}

VkImageSubresourceRange VulkanImage::getRange() const
{
    VkImageSubresourceRange range;
    range.baseArrayLayer = 0;
    range.layerCount = mNumFaces;
    range.baseMipLevel = 0;
    range.levelCount = mNumMipLevels;
    range.aspectMask = getAspectFlags();

    return range;
}

VkImageSubresourceRange VulkanImage::getRange(const TextureSurface& surface) const
{
    VkImageSubresourceRange range;
    range.baseArrayLayer = surface.face;
    range.layerCount = surface.numFaces == 0 ? mNumFaces : surface.numFaces;
    range.baseMipLevel = surface.mipLevel;
    range.levelCount = surface.numMipLevels == 0 ? mNumMipLevels : surface.numMipLevels;
    range.aspectMask = getAspectFlags();

    return range;
}

VulkanImageSubresource* VulkanImage::getSubresource(uint32_t face, uint32_t mipLevel)
{
    return mSubresources[mipLevel * mNumFaces + face];
}

void VulkanImage::map(uint32_t face, uint32_t mipLevel, MappedImageData& output) const
{
    VulkanDevice* device = Driver->_getPrimaryDevice();

    VkImageSubresource range;
    range.mipLevel = mipLevel;
    range.arrayLayer = face;

    if (mImageViewCI.subresourceRange.aspectMask == VK_IMAGE_ASPECT_COLOR_BIT)
        range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    else // Depth stencil, but we only map depth
        range.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

    VkSubresourceLayout layout;
    vkGetImageSubresourceLayout(device->getLogical(), mImage, &range, &layout);

    const uint32_t pixelSize = IImage::getBitsPerPixelFromFormat(output.format) / 8.0;
    assert((uint32_t)layout.rowPitch % pixelSize == 0);
    assert((uint32_t)layout.depthPitch % pixelSize == 0);

    output.rowPitch = (uint32_t)layout.rowPitch / pixelSize;
    output.slicePitch = (uint32_t)layout.depthPitch / pixelSize;

    VkDeviceMemory memory;
    VkDeviceSize memoryOffset;
    device->getAllocationInfo(mAllocation, memory, memoryOffset);

    UINT8* data;
    VkResult result = vkMapMemory(device->getLogical(), memory, memoryOffset + layout.offset, layout.size, 0, (void**)&data);
    assert(result == VK_SUCCESS);

    output.data = data;
    output.size = layout.size;
}

UINT8* VulkanImage::map(uint32_t offset, uint32_t size) const
{
    VulkanDevice* device = Driver->_getPrimaryDevice();

    VkDeviceMemory memory;
    VkDeviceSize memoryOffset;
    device->getAllocationInfo(mAllocation, memory, memoryOffset);

    UINT8* data;
    VkResult result = vkMapMemory(device->getLogical(), memory, memoryOffset + offset, size, 0, (void**)&data);
    assert(result == VK_SUCCESS);

    return data;
}

void VulkanImage::unmap()
{
    VulkanDevice* device = Driver->_getPrimaryDevice();

    VkDeviceMemory memory;
    VkDeviceSize memoryOffset;
    device->getAllocationInfo(mAllocation, memory, memoryOffset);

    vkUnmapMemory(device->getLogical(), memory);
}

void VulkanImage::copy(VulkanCmdBuffer* cb, VulkanBuffer* destination, const VkExtent3D& extent,
    const VkImageSubresourceLayers& range, VkImageLayout layout)
{
    VkBufferImageCopy region;
    region.bufferRowLength = destination->getRowPitch();
    region.bufferImageHeight = destination->getSliceHeight();
    region.bufferOffset = 0;
    region.imageOffset.x = 0;
    region.imageOffset.y = 0;
    region.imageOffset.z = 0;
    region.imageExtent = extent;
    region.imageSubresource = range;

    vkCmdCopyImageToBuffer(cb->getHandle(), mImage, layout, destination->getHandle(), 1, &region);
}

VkAccessFlags VulkanImage::getAccessFlags(VkImageLayout layout, bool readOnly)
{
    VkAccessFlags accessFlags;

    switch (layout)
    {
        case VK_IMAGE_LAYOUT_GENERAL:
        {
            accessFlags = VK_ACCESS_SHADER_READ_BIT;
            if ((mUsage & ETCF_USAGE_LOADSTORE) != 0)
            {
                if (!readOnly)
                    accessFlags |= VK_ACCESS_SHADER_WRITE_BIT;
            }

            if ((mUsage & ETCF_USAGE_RENDERTARGET) != 0)
            {
                accessFlags |= VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;

                if (!readOnly)
                    accessFlags |= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            }
            else if ((mUsage & ETCF_USAGE_DEPTHSTENCIL) != 0)
            {
                accessFlags |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;

                if (!readOnly)
                    accessFlags |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            }
        }

        break;
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            accessFlags = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
            break;
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            accessFlags = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
            break;
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
        case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL_KHR:
        case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL_KHR:
            accessFlags = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_SHADER_READ_BIT;
            break;
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            accessFlags = VK_ACCESS_SHADER_READ_BIT;
            break;
        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            accessFlags = VK_ACCESS_TRANSFER_READ_BIT;
            break;
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            accessFlags = VK_ACCESS_TRANSFER_WRITE_BIT;
            break;
        case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
            accessFlags = VK_ACCESS_MEMORY_READ_BIT;
            break;
        case VK_IMAGE_LAYOUT_UNDEFINED:
        case VK_IMAGE_LAYOUT_PREINITIALIZED:
            accessFlags = 0;
            break;
        default:
            accessFlags = 0;
            os::Printer::log("Unsupported source layout for Vulkan image.");
            break;
    }

    return accessFlags;
}

void VulkanImage::getBarriers(const VkImageSubresourceRange& range, std::vector<VkImageMemoryBarrier>& barriers)
{
    uint32_t numSubresources = range.levelCount * range.layerCount;

    // Nothing to do
    if (numSubresources == 0)
        return;

    uint32_t mip = range.baseMipLevel;
    uint32_t face = range.baseArrayLayer;
    uint32_t lastMip = range.baseMipLevel + range.levelCount - 1;
    uint32_t lastFace = range.baseArrayLayer + range.layerCount - 1;

    VkImageMemoryBarrier defaultBarrier;
    defaultBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    defaultBarrier.pNext = nullptr;
    defaultBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    defaultBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    defaultBarrier.image = getHandle();
    defaultBarrier.subresourceRange.aspectMask = range.aspectMask;
    defaultBarrier.subresourceRange.layerCount = 1;
    defaultBarrier.subresourceRange.levelCount = 1;
    defaultBarrier.subresourceRange.baseArrayLayer = 0;
    defaultBarrier.subresourceRange.baseMipLevel = 0;

    auto addNewBarrier = [&](VulkanImageSubresource* subresource, uint32_t face, uint32_t mip)
    {
        barriers.push_back(defaultBarrier);
        VkImageMemoryBarrier* barrier = &barriers.back();

        barrier->subresourceRange.baseArrayLayer = face;
        barrier->subresourceRange.baseMipLevel = mip;
        barrier->srcAccessMask = getAccessFlags(subresource->getLayout());
        barrier->oldLayout = subresource->getLayout();

        return barrier;
    };

    //bs_frame_mark();
    {
        std::vector<bool> processed(numSubresources, false);

        // Add first subresource
        VulkanImageSubresource* subresource = getSubresource(face, mip);
        addNewBarrier(subresource, face, mip);
        numSubresources--;
        processed[0] = true;

        while (numSubresources > 0)
        {
            // Try to expand the barrier as much as possible
            VkImageMemoryBarrier* barrier = &barriers.back();

            while (true)
            {
                // Expand by one in the X direction
                bool expandedFace = true;
                if (face < lastFace)
                {
                    for (uint32_t i = 0; i < barrier->subresourceRange.levelCount; i++)
                    {
                        uint32_t curMip = barrier->subresourceRange.baseMipLevel + i;
                        VulkanImageSubresource* subresource = getSubresource(face + 1, curMip);
                        if (barrier->oldLayout != subresource->getLayout())
                        {
                            expandedFace = false;
                            break;
                        }
                    }

                    if (expandedFace)
                    {
                        barrier->subresourceRange.layerCount++;
                        numSubresources -= barrier->subresourceRange.levelCount;
                        face++;

                        for (uint32_t i = 0; i < barrier->subresourceRange.levelCount; i++)
                        {
                            uint32_t curMip = (barrier->subresourceRange.baseMipLevel + i) - range.baseMipLevel;
                            uint32_t idx = curMip * range.layerCount + (face - range.baseArrayLayer);
                            processed[idx] = true;
                        }
                    }
                }
                else
                    expandedFace = false;

                // Expand by one in the Y direction
                bool expandedMip = true;
                if (mip < lastMip)
                {
                    for (uint32_t i = 0; i < barrier->subresourceRange.layerCount; i++)
                    {
                        uint32_t curFace = barrier->subresourceRange.baseArrayLayer + i;
                        VulkanImageSubresource* subresource = getSubresource(curFace, mip + 1);
                        if (barrier->oldLayout != subresource->getLayout())
                        {
                            expandedMip = false;
                            break;
                        }
                    }

                    if (expandedMip)
                    {
                        barrier->subresourceRange.levelCount++;
                        numSubresources -= barrier->subresourceRange.layerCount;
                        mip++;

                        for (uint32_t i = 0; i < barrier->subresourceRange.layerCount; i++)
                        {
                            uint32_t curFace = (barrier->subresourceRange.baseArrayLayer + i) - range.baseArrayLayer;
                            uint32_t idx = (mip - range.baseMipLevel) * range.layerCount + curFace;
                            processed[idx] = true;
                        }
                    }
                }
                else
                    expandedMip = false;

                // If we can't grow no more, we're done with this square
                if (!expandedMip && !expandedFace)
                    break;
            }

            // Look for a new starting point (sub-resource we haven't processed yet)
            for (uint32_t i = 0; i < range.levelCount; i++)
            {
                bool found = false;
                for (uint32_t j = 0; j < range.layerCount; j++)
                {
                    uint32_t idx = i * range.layerCount + j;
                    if (!processed[idx])
                    {
                        mip = range.baseMipLevel + i;
                        face = range.baseArrayLayer + j;

                        found = true;
                        processed[idx] = true;
                        break;
                    }
                }

                if (found)
                {
                    VulkanImageSubresource* subresource = getSubresource(face, mip);
                    addNewBarrier(subresource, face, mip);
                    numSubresources--;
                    break;
                }
            }
        }
    }
    //bs_frame_clear();
}

VulkanImageSubresource::VulkanImageSubresource(CVulkanDriver* owner, VkImageLayout layout)
    : CVulkanDeviceResource(owner)
    , mLayout(layout)
{ }

void irr::video::VulkanImageSubresource::OnDeviceLost(CVulkanDriver * device)
{
}

void irr::video::VulkanImageSubresource::OnDeviceRestored(CVulkanDriver * device)
{
}
