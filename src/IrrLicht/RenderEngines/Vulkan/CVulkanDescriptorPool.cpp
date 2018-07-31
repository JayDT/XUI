#include "CVulkanDescriptorPool.h"
#include "CVulkanDevice.h"

using namespace irr;
using namespace irr::video;

VulkanDescriptorPool::VulkanDescriptorPool(VulkanDevice& device)
    :mDevice(device)
{
    VkDescriptorPoolSize poolSizes[6];
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[0].descriptorCount = sMaxSampledImages;

    poolSizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[1].descriptorCount = sMaxUniformBuffers;

    poolSizes[2].type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    poolSizes[2].descriptorCount = sMaxImages;

    poolSizes[3].type = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
    poolSizes[3].descriptorCount = sMaxSampledBuffers;

    poolSizes[4].type = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
    poolSizes[4].descriptorCount = sMaxBuffers;

    poolSizes[5].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    poolSizes[5].descriptorCount = sMaxBuffers;

    VkDescriptorPoolCreateInfo poolCI;
    poolCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolCI.pNext = nullptr;
    poolCI.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    poolCI.maxSets = sMaxSets;
    poolCI.poolSizeCount = sizeof(poolSizes) / sizeof(poolSizes[0]);
    poolCI.pPoolSizes = poolSizes;

    VkResult result = vkCreateDescriptorPool(mDevice.getLogical(), &poolCI, VulkanDevice::gVulkanAllocator, &mPool);
    assert(result == VK_SUCCESS);
}

VulkanDescriptorPool::~VulkanDescriptorPool()
{
    vkDestroyDescriptorPool(mDevice.getLogical(), mPool, VulkanDevice::gVulkanAllocator);
}
