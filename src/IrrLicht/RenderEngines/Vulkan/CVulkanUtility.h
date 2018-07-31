#pragma once

// Glslang
#ifndef AMD_EXTENSIONS
#   define AMD_EXTENSIONS
#   define NV_EXTENSIONS
#   define ENABLE_HLSL
#endif

#include "IrrCompileConfig.h"
#include <IContextManager.h>

#include "SIrrCreationParameters.h"
#include "SExposedVideoData.h"
#include "IContextManager.h"
#include "SColor.h"
#include "SMaterial.h"
#include "SVertexIndex.h"
#include "EPrimitiveTypes.h"

#if defined(_IRR_COMPILE_WITH_WINDOWS_DEVICE_)
#   define WIN32_LEAN_AND_MEAN
#   if !defined(NOMINMAX) && defined(_MSC_VER)
#   	define NOMINMAX // Required to stop windows.h messing up std::min
#   endif
#   define VK_USE_PLATFORM_WIN32_KHR
#elif defined(_IRR_COMPILE_WITH_X11_DEVICE_)
#   define VK_USE_PLATFORM_XLIB_KHR
#elif defined(_IRR_COMPILE_WITH_OSX_DEVICE_)
#   define VK_USE_PLATFORM_MACOS_MVK
#elif defined(_IRR_COMPILE_WITH_ANDROID_DEVICE_)
#   define VK_USE_PLATFORM_ANDROID_KHR
#endif

#define VULKAN_HPP_NO_EXCEPTIONS
#include <Vulkan/vulkan.h>
#include <Vulkan/vulkan.hpp>

#include "CCommonDefines.h"

#include <vector>
#include "vk_mem_alloc.h"

namespace irr
{
    namespace video
    {
        class VulkanDevice;
        class CVulkanDriver;

        /** @addtogroup Vulkan
         *  @{
         */

         /** Contains lists of images and buffers that require pipeline barrier transitions. */
        struct TransitionInfo
        {
            std::vector<VkImageMemoryBarrier> imageBarriers;
            std::vector<VkBufferMemoryBarrier> bufferBarriers;
        };

         /** Contains various helper methods for dealing with Vulkan. */
        class VulkanUtility
        {
        public:
            ///**	Finds the closest pixel format that a specific Vulkan device supports. */
            static ECOLOR_FORMAT getClosestSupportedPixelFormat(VulkanDevice& device, ECOLOR_FORMAT format, TextureType texType,
            	int usage, bool optimalTiling, bool hwGamma);
            
            ///** Converts between Irrlicht and Vulkan pixel format. */
            static VkFormat getPixelFormat(ECOLOR_FORMAT format, bool sRGB = false);

            static ECOLOR_FORMAT getPixelFormat(VkFormat format);

            static u32 getIndexAmount(scene::E_PRIMITIVE_TYPE primType, u32 primitiveCount);

            /**	Converts between Irrlicht and Vulkan texture addressing mode. */
            static VkSamplerAddressMode getAddressingMode(E_TEXTURE_CLAMP mode);
            
            /**	Converts between Irrlicht and Vulkan blend factor. */
            static VkBlendFactor getBlendFactor(E_BLEND_FACTOR factor);
            
            /**	Converts between Irrlicht and Vulkan blend operation. */
            static VkBlendOp getBlendOp(E_BLEND_OPERATION op);
            
            /**	Converts between Irrlicht and Vulkan comparison operation. */
            static VkCompareOp getCompareOp(E_COMPARISON_FUNC op);
            
            /**	Converts between Irrlicht and Vulkan cull mode. */
            static VkCullModeFlagBits getCullMode(const irr::video::SMaterial& material);
            
            /**	Converts between Irrlicht and Vulkan polygon mode. */
            static VkPolygonMode getPolygonMode(bool fill);
            
            /**	Converts between Irrlicht and Vulkan stencil op. */
            static VkStencilOp getStencilOp(E_STENCIL_OPERATION op);
            
            /**	Converts between Irrlicht and Vulkan index type. */
            static VkIndexType getIndexType(E_INDEX_TYPE op);
            
            /**	Converts between Irrlicht and Vulkan draw operation (i.e. primitive topology). */
            static VkPrimitiveTopology getDrawOp(scene::E_PRIMITIVE_TYPE op);
            
            /**	Converts between Irrlicht and Vulkan texture filtering modes. */
            static VkFilter getFilter(bool linear);
            
            ///**	Converts between Irrlicht and Vulkan texture filtering modes. */
            static VkSamplerMipmapMode getMipFilter(bool linear);
            
            ///** Gets Vulkan flags representing the number of samples in an image. Sample count must be a power of 2. */
            static VkSampleCountFlagBits getSampleFlags(uint32_t numSamples);
            
            ///** 
            // * Populates the provided array with Vulkan devices that correspond to provided flags. Sets null in unused slots. 
            // * Each device is placed at its own index in the output array.
            // */
            static void getDevices(CVulkanDriver* driver, GpuDeviceFlags flags, VulkanDevice* (&devices)[_MAX_DEVICES]);
            
            ///** Checks is a flag for a particular device enabled. */
            static bool isDeviceIdxSet(CVulkanDriver* driver, uint32_t idx, GpuDeviceFlags flags);
            
            ///** 
            // * Subdivides an image subresource range by cutting it with another range. If the ranges don't overlap, or the
            // * @p cutWith range completely overs the @p toCut range, the original @p toCut range is output. 
            // * 
            // * @param[in]	toCut		Range to cut.
            // * @param[in]	cutWith		Range to cut with.
            // * @param[out]	output		Pieces of the range that was cut.
            // * @param[out]	numAreas	Number of pieces in the @p output array.
            // */
            static void cutRange(const VkImageSubresourceRange& toCut, const VkImageSubresourceRange& cutWith, std::array<VkImageSubresourceRange, 5>& output, uint32_t& numAreas);
            
            ///** Checks if the two image subresource ranges have any overlapping subresources. */
            static bool rangeOverlaps(const VkImageSubresourceRange& a, const VkImageSubresourceRange& b);
        };

        /** @} */
    }
}