#include "CVulkanDevice.h"
#include "CVulkanQueue.h"
#include "CVulkanCommandBuffer.h"
#include "CVulkanDriver.h"
#include "CVulkanDescriptorPool.h"
#include "CVulkanDescriptorLayout.h"
#include "CVulkanDescriptorSet.h"

#include "os.h"

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

using namespace irr;
using namespace irr::video;

VkAllocationCallbacks* VulkanDevice::gVulkanAllocator = nullptr;

VulkanDevice::VulkanDevice(CVulkanDriver* driver, VkPhysicalDevice device, uint32_t deviceIdx)
    : mDriver(driver)
    , mPhysicalDevice(device)
    , mLogicalDevice(nullptr)
    , mIsPrimary(false)
    , mDeviceIdx(deviceIdx)
    , mQueueInfos()
{
    // Set to default
    for (uint32_t i = 0; i < GQT_COUNT; i++)
        mQueueInfos[i].familyIdx = (uint32_t)-1;

    vkGetPhysicalDeviceProperties(device, &mDeviceProperties);
    vkGetPhysicalDeviceFeatures(device, &mDeviceFeatures);
    vkGetPhysicalDeviceMemoryProperties(device, &mMemoryProperties);

    uint32_t numQueueFamilies;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &numQueueFamilies, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilyProperties(numQueueFamilies);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &numQueueFamilies, queueFamilyProperties.data());

    // Create queues
    const float defaultQueuePriorities[_MAX_QUEUES_PER_TYPE] = { 0.0f };
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

    auto populateQueueInfo = [&](GpuQueueType type, uint32_t familyIdx)
    {
        queueCreateInfos.push_back(VkDeviceQueueCreateInfo());

        VkDeviceQueueCreateInfo& createInfo = queueCreateInfos.back();
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.queueFamilyIndex = familyIdx;
        createInfo.queueCount = std::min(queueFamilyProperties[familyIdx].queueCount, (uint32_t)_MAX_QUEUES_PER_TYPE);
        createInfo.pQueuePriorities = defaultQueuePriorities;

        mQueueInfos[type].familyIdx = familyIdx;
        mQueueInfos[type].queues.resize(createInfo.queueCount, nullptr);
    };

    // Look for dedicated compute queues
    for (uint32_t i = 0; i < (uint32_t)queueFamilyProperties.size(); i++)
    {
        if ((queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) && (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0)
        {
            populateQueueInfo(GQT_COMPUTE, i);
            break;
        }
    }

    // Look for dedicated upload queues
    for (uint32_t i = 0; i < (uint32_t)queueFamilyProperties.size(); i++)
    {
        if ((queueFamilyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT) &&
            ((queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0) &&
            ((queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) == 0))
        {
            populateQueueInfo(GQT_UPLOAD, i);
            break;
        }
    }

    // Looks for graphics queues
    for (uint32_t i = 0; i < (uint32_t)queueFamilyProperties.size(); i++)
    {
        if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            populateQueueInfo(GQT_GRAPHICS, i);
            break;
        }
    }

    // Set up extensions
    const char* extensions[5];
    uint32_t numExtensions = 0;

    extensions[numExtensions++] = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
    extensions[numExtensions++] = VK_KHR_MAINTENANCE1_EXTENSION_NAME;
    extensions[numExtensions++] = VK_KHR_MAINTENANCE2_EXTENSION_NAME;

    // Enumerate supported extensions
    bool dedicatedAllocExt = false;
    bool getMemReqExt = false;

    uint32_t numAvailableExtensions = 0;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &numAvailableExtensions, nullptr);
    if (numAvailableExtensions > 0)
    {
        std::vector<VkExtensionProperties> availableExtensions(numAvailableExtensions);
        if (vkEnumerateDeviceExtensionProperties(device, nullptr, &numAvailableExtensions, availableExtensions.data()) == VK_SUCCESS)
        {
            for (auto entry : extensions)
            {
                if (entry == VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME)
                {
                    extensions[numExtensions++] = VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME;
                    dedicatedAllocExt = true;
                }
                else if (entry == VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME)
                {
                    extensions[numExtensions++] = VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME;
                    getMemReqExt = true;
                }
            }
        }
    }

    VkDeviceCreateInfo deviceInfo = {};
    deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceInfo.pNext = nullptr;
    deviceInfo.flags = 0;
    deviceInfo.queueCreateInfoCount = (uint32_t)queueCreateInfos.size();
    deviceInfo.pQueueCreateInfos = queueCreateInfos.data();
    deviceInfo.pEnabledFeatures = &mDeviceFeatures;
    deviceInfo.enabledExtensionCount = numExtensions;
    deviceInfo.ppEnabledExtensionNames = extensions;
    deviceInfo.enabledLayerCount = 0;
    deviceInfo.ppEnabledLayerNames = nullptr;

    VkResult result = vkCreateDevice(device, &deviceInfo, gVulkanAllocator, &mLogicalDevice);
    assert(result == VK_SUCCESS);

    // Retrieve queues
    for (uint32_t i = 0; i < GQT_COUNT; i++)
    {
        uint32_t numQueues = (uint32_t)mQueueInfos[i].queues.size();
        for (uint32_t j = 0; j < numQueues; j++)
        {
            VkQueue queue;
            vkGetDeviceQueue(mLogicalDevice, mQueueInfos[i].familyIdx, j, &queue);

            mQueueInfos[i].queues[j] = new VulkanQueue(*this, queue, (GpuQueueType)i, j);
        }
    }

    // Set up the memory allocator
    VmaAllocatorCreateInfo allocatorCI = {};
    allocatorCI.physicalDevice = device;
    allocatorCI.device = mLogicalDevice;
    allocatorCI.pAllocationCallbacks = gVulkanAllocator;

    if (dedicatedAllocExt && getMemReqExt)
        allocatorCI.flags |= VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT;

    vmaCreateAllocator(&allocatorCI, &mAllocator);

    mCommandBufferPool = new VulkanCmdBufferPool(*this);

    for (u32 j = 0; j < GQT_COUNT; j++)
    {
        GpuQueueType queueType = (GpuQueueType)j;

        for (u32 k = 0; k < _MAX_QUEUES_PER_TYPE; k++)
            transferBuffers[j][k] = new VulkanTransferBuffer(this, queueType, k);
    }

    mDescriptorPool = new VulkanDescriptorPool(*this);
}

VulkanDevice::~VulkanDevice()
{
    waitIdle();

    for (uint32_t i = 0; i < GQT_COUNT; i++)
    {
        uint32_t numQueues = (uint32_t)mQueueInfos[i].queues.size();
        for (uint32_t j = 0; j < numQueues; j++)
        {
            mQueueInfos[i].queues[j]->refreshStates(true, true);
            delete (mQueueInfos[i].queues[j]);
        }
    }

    //delete (mDescriptorManager);
    //delete (mQueryPool);
    delete (mCommandBufferPool);
    delete (mDescriptorPool);

    // Needs to happen after query pool & command buffer pool shutdown, to ensure their resources are destroyed
    //delete (mResourceManager);

    vmaDestroyAllocator(mAllocator);
    vkDestroyDevice(mLogicalDevice, gVulkanAllocator);

    for (u32 i = 0; i < GQT_COUNT; i++)
    {
        for (u32 j = 0; j < _MAX_QUEUES_PER_TYPE; j++)
        {
            delete transferBuffers[i][j];
            transferBuffers[i][j] = nullptr;
        }
    }
}

VulkanTransferBuffer* VulkanDevice::getTransferBuffer(GpuQueueType type, u32 queueIdx)
{
    VulkanTransferBuffer* transferBuffer = transferBuffers[type][queueIdx];
    transferBuffer->allocate();
    return transferBuffer;
}

void VulkanDevice::flushTransferBuffers()
{
    for (u32 i = 0; i < GQT_COUNT; i++)
    {
        for (u32 j = 0; j < _MAX_QUEUES_PER_TYPE; j++)
            transferBuffers[i][j]->flush(false);
    }
}

VulkanDescriptorSet * irr::video::VulkanDevice::createSet(VulkanDescriptorLayout * layout)
{
    // Note: We always retrieve the last created pool, even though there could be free room in earlier pools. However
    // that requires additional tracking. Since the assumption is that the first pool will be large enough for all
    // descriptors, and the only reason to create a second pool is fragmentation, this approach should not result in
    // a major resource waste.
    VkDescriptorSetLayout setLayout = *layout->getHandle();

    VkDescriptorSetAllocateInfo allocateInfo;
    allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocateInfo.pNext = nullptr;
    allocateInfo.descriptorPool = mDescriptorPool->getHandle();
    allocateInfo.descriptorSetCount = 1;
    allocateInfo.pSetLayouts = &setLayout;

    VkDescriptorSet set;
    VkResult result = vkAllocateDescriptorSets(getLogical(), &allocateInfo, &set);
    assert(result == VK_SUCCESS);

    return new VulkanDescriptorSet(getDriver(), set, allocateInfo.descriptorPool);
}

void VulkanDevice::waitIdle() const
{
    VkResult result = vkDeviceWaitIdle(mLogicalDevice);
    assert(result == VK_SUCCESS);
}

uint32_t VulkanDevice::getQueueMask(GpuQueueType type, uint32_t queueIdx) const
{
    uint32_t numQueues = getNumQueues(type);
    if (numQueues == 0)
        return 0;

    uint32_t idMask = 0;
    uint32_t curIdx = queueIdx % numQueues;
    while (curIdx < _MAX_QUEUES_PER_TYPE)
    {
        idMask |= CommandSyncMask::getGlobalQueueMask(type, curIdx);
        curIdx += numQueues;
    }

    return idMask;
}

SurfaceFormat VulkanDevice::getSurfaceFormat(const VkSurfaceKHR& surface, bool gamma) const
{
    uint32_t numFormats;
    VkResult result = vkGetPhysicalDeviceSurfaceFormatsKHR(mPhysicalDevice, surface, &numFormats, nullptr);
    assert(result == VK_SUCCESS);
    assert(numFormats > 0);

    VkSurfaceFormatKHR* surfaceFormats = new VkSurfaceFormatKHR[numFormats];
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(mPhysicalDevice, surface, &numFormats, surfaceFormats);
    assert(result == VK_SUCCESS);

    SurfaceFormat output;
    output.colorFormat = VK_FORMAT_R8G8B8A8_UNORM;
    output.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    output.depthFormat = VK_FORMAT_D24_UNORM_S8_UINT;

    // If there is no preferred format, use standard RGBA
    if ((numFormats == 1) && (surfaceFormats[0].format == VK_FORMAT_UNDEFINED))
    {
        if (gamma)
            output.colorFormat = VK_FORMAT_R8G8B8A8_SRGB;
        else
            output.colorFormat = VK_FORMAT_B8G8R8A8_UNORM;

        output.colorSpace = surfaceFormats[0].colorSpace;
    }
    else
    {
        bool foundFormat = false;

        VkFormat wantedFormatsUNORM[] =
        {
            VK_FORMAT_R8G8B8A8_UNORM,
            VK_FORMAT_B8G8R8A8_UNORM,
            VK_FORMAT_A8B8G8R8_UNORM_PACK32,
            VK_FORMAT_A8B8G8R8_UNORM_PACK32,
            VK_FORMAT_R8G8B8_UNORM,
            VK_FORMAT_B8G8R8_UNORM
        };

        VkFormat wantedFormatsSRGB[] =
        {
            VK_FORMAT_R8G8B8A8_SRGB,
            VK_FORMAT_B8G8R8A8_SRGB,
            VK_FORMAT_A8B8G8R8_SRGB_PACK32,
            VK_FORMAT_A8B8G8R8_SRGB_PACK32,
            VK_FORMAT_R8G8B8_SRGB,
            VK_FORMAT_B8G8R8_SRGB
        };

        uint32_t numWantedFormats;
        VkFormat* wantedFormats;
        if (gamma)
        {
            numWantedFormats = sizeof(wantedFormatsSRGB) / sizeof(wantedFormatsSRGB[0]);
            wantedFormats = wantedFormatsSRGB;
        }
        else
        {
            numWantedFormats = sizeof(wantedFormatsUNORM) / sizeof(wantedFormatsUNORM[0]);
            wantedFormats = wantedFormatsUNORM;
        }

        for (uint32_t i = 0; i < numWantedFormats; i++)
        {
            for (uint32_t j = 0; j < numFormats; j++)
            {
                if (surfaceFormats[j].format == wantedFormats[i])
                {
                    output.colorFormat = surfaceFormats[j].format;
                    output.colorSpace = surfaceFormats[j].colorSpace;

                    foundFormat = true;
                    break;
                }
            }

            if (foundFormat)
                break;
        }

        // If we haven't found anything, fall back to first available
        if (!foundFormat)
        {
            output.colorFormat = surfaceFormats[0].format;
            output.colorSpace = surfaceFormats[0].colorSpace;

            if (gamma)
                os::Printer::log("Cannot find a valid sRGB format for a render window surface, falling back to a default format.");
        }
    }

    delete [](surfaceFormats);
    return output;
}

VmaAllocation VulkanDevice::allocateMemory(VkImage image, VkMemoryPropertyFlags flags)
{
    VmaAllocationCreateInfo allocCI = {};
    allocCI.requiredFlags = flags;

    VmaAllocationInfo allocInfo;
    VmaAllocation allocation;
    VkResult result = vmaAllocateMemoryForImage(mAllocator, image, &allocCI, &allocation, &allocInfo);
    assert(result == VK_SUCCESS);

    result = vkBindImageMemory(mLogicalDevice, image, allocInfo.deviceMemory, allocInfo.offset);
    assert(result == VK_SUCCESS);

    return allocation;
}

VmaAllocation VulkanDevice::allocateMemory(VkBuffer buffer, VkMemoryPropertyFlags flags)
{
    VmaAllocationCreateInfo allocCI = {};
    allocCI.requiredFlags = flags;

    VmaAllocationInfo allocInfo;
    VmaAllocation memory;
    VkResult result = vmaAllocateMemoryForBuffer(mAllocator, buffer, &allocCI, &memory, &allocInfo);
    assert(result == VK_SUCCESS);

    result = vkBindBufferMemory(mLogicalDevice, buffer, allocInfo.deviceMemory, allocInfo.offset);
    assert(result == VK_SUCCESS);

    return memory;
}

void VulkanDevice::freeMemory(VmaAllocation allocation)
{
    vmaFreeMemory(mAllocator, allocation);
}

void VulkanDevice::getAllocationInfo(VmaAllocation allocation, VkDeviceMemory& memory, VkDeviceSize& offset)
{
    VmaAllocationInfo allocInfo;
    vmaGetAllocationInfo(mAllocator, allocation, &allocInfo);

    memory = allocInfo.deviceMemory;
    offset = allocInfo.offset;
}

uint32_t VulkanDevice::findMemoryType(uint32_t requirementBits, VkMemoryPropertyFlags wantedFlags)
{
    for (uint32_t i = 0; i < mMemoryProperties.memoryTypeCount; i++)
    {
        if (requirementBits & (1 << i))
        {
            if ((mMemoryProperties.memoryTypes[i].propertyFlags & wantedFlags) == wantedFlags)
                return i;
        }
    }

    return -1;
}
