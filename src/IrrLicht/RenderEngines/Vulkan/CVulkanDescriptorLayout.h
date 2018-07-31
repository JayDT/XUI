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

         /** Wrapper for the Vulkan descriptor layout object. */
        class VulkanDescriptorLayout
        {
        public:
            VulkanDescriptorLayout(VulkanDevice& device, VkDescriptorSetLayoutBinding* bindings, u32 numBindings);
            ~VulkanDescriptorLayout();

            /** Returns a handle to the Vulkan set layout object. */
            VkDescriptorSetLayout* getHandle() { return &mLayout; }

            /** Returns a hash value for the descriptor layout. */
            size_t getHash() const { return mHash; }

            /** Calculates a has value for the provided descriptor set layout bindings. */
            static size_t calculateHash(VkDescriptorSetLayoutBinding* bindings, u32 numBindings);

        protected:
            VulkanDevice & mDevice;
            VkDescriptorSetLayout mLayout;
            size_t mHash;
        };

        /** @} */
    }
}