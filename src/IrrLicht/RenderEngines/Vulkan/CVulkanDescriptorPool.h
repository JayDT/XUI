#pragma once

#include "CVulkanUtility.h"
#include "CVulkanResources.h"

namespace irr
{
    namespace video
    {
        /** @addtogroup Vulkan
         *  @{
         */

         /** Pool that allocates and distributes Vulkan descriptor sets. */
        class VulkanDescriptorPool
        {
        public:
            VulkanDescriptorPool(VulkanDevice& device);
            ~VulkanDescriptorPool();

            /** Returns a handle to the internal Vulkan descriptor pool. */
            VkDescriptorPool getHandle() const { return mPool; }

        private:
            static const u32 sMaxSets = 8192;
            static const u32 sMaxSampledImages = 4096;
            static const u32 sMaxImages = 2048;
            static const u32 sMaxSampledBuffers = 2048;
            static const u32 sMaxBuffers = 2048;
            static const u32 sMaxUniformBuffers = 2048;

            VulkanDevice& mDevice;
            VkDescriptorPool mPool;
        };

        /** @} */
    }
}