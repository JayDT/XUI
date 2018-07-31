#include "CVulkanDescriptorLayout.h"
#include "CVulkanDevice.h"

template <class T>
inline void hash_combine(std::size_t& seed, const T& v)
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

using namespace irr;
using namespace irr::video;

VulkanDescriptorLayout::VulkanDescriptorLayout(VulkanDevice& device, VkDescriptorSetLayoutBinding* bindings, u32 numBindings)
    : mDevice(device)
{
    //mHash = calculateHash(bindings, numBindings);

    VkDescriptorSetLayoutCreateInfo layoutCI;
    layoutCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutCI.pNext = nullptr;
    layoutCI.flags = 0;
    layoutCI.bindingCount = numBindings;
    layoutCI.pBindings = bindings;

    VkResult result = vkCreateDescriptorSetLayout(device.getLogical(), &layoutCI, VulkanDevice::gVulkanAllocator, &mLayout);
    assert(result == VK_SUCCESS);
}

VulkanDescriptorLayout::~VulkanDescriptorLayout()
{
    vkDestroyDescriptorSetLayout(mDevice.getLogical(), mLayout, VulkanDevice::gVulkanAllocator);
}

size_t VulkanDescriptorLayout::calculateHash(VkDescriptorSetLayoutBinding* bindings, u32 numBindings)
{
    size_t hash = 0;
    for (u32 i = 0; i < numBindings; i++)
    {
        size_t bindingHash = 0;
        hash_combine(bindingHash, bindings[i].binding);
        hash_combine(bindingHash, bindings[i].descriptorCount);
        hash_combine(bindingHash, bindings[i].descriptorType);
        hash_combine(bindingHash, bindings[i].stageFlags);
        assert(bindings[i].pImmutableSamplers == nullptr); // Not accounted for in hash, assumed always null

        hash_combine(hash, bindingHash);
    }

    return hash;
}
