#include "CVulkanDescriptorSet.h"
#include "CVulkanDevice.h"
#include "CVulkanDriver.h"

using namespace irr;
using namespace irr::video;

VulkanDescriptorSet::VulkanDescriptorSet(CVulkanDriver* owner, VkDescriptorSet set, VkDescriptorPool pool)
    : CVulkanDeviceResource(owner), mSet(set), mPool(pool)
{ }

VulkanDescriptorSet::~VulkanDescriptorSet()
{
    VkResult result = vkFreeDescriptorSets(Driver->_getPrimaryDevice()->getLogical(), mPool, 1, &mSet);
    assert(result == VK_SUCCESS);
}

void VulkanDescriptorSet::write(VkWriteDescriptorSet* entries, u32 count)
{
    for (u32 i = 0; i < count; i++)
        entries[i].dstSet = mSet;

    vkUpdateDescriptorSets(Driver->_getPrimaryDevice()->getLogical(), count, entries, 0, nullptr);
}

void irr::video::VulkanDescriptorSet::OnDeviceLost(CVulkanDriver * device)
{
}

void irr::video::VulkanDescriptorSet::OnDeviceRestored(CVulkanDriver * device)
{
}
