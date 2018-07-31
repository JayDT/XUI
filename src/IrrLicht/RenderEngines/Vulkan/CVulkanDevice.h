#pragma once

#include "CVulkanUtility.h"
#include "CVulkanSharedDefines.h"
#include "CVulkanCommandBuffer.h"

#include "vk_mem_alloc.h"

namespace irr
{
    namespace video
    {
        class VulkanQueue;
        class VulkanQueryPool;
        class VulkanTransferBuffer;
        class VulkanDescriptorPool;
        class VulkanDescriptorSet;
        class VulkanDescriptorLayout;

        /** @addtogroup Vulkan
         *  @{
         */

         /** Contains format describing a Vulkan surface. */
        struct SurfaceFormat
        {
            VkFormat colorFormat;
            VkFormat depthFormat;
            VkColorSpaceKHR colorSpace;
        };

        /** Represents a single GPU device usable by Vulkan. */
        class VulkanDevice
        {
        public:
            VulkanDevice(CVulkanDriver* driver, VkPhysicalDevice device, uint32_t deviceIdx);
            ~VulkanDevice();

            /** Returns an object describing the physical properties of the device. */
            VkPhysicalDevice getPhysical() const { return mPhysicalDevice; }

            /** Returns an object describing the logical properties of the device. */
            VkDevice getLogical() const { return mLogicalDevice; }

            /** Returns true if the device is one of the primary GPU's. */
            bool isPrimary() const { return mIsPrimary; }

            /** Returns the unique index of the device. */
            uint32_t getIndex() const { return mDeviceIdx; }

            /** Blocks the calling thread until all operations on the device finish. */
            void waitIdle() const;

            /** Returns a set of properties describing the physical device. */
            const VkPhysicalDeviceProperties& getDeviceProperties() const { return mDeviceProperties; }

            /** Returns a set of features that the application can use to check if a specific feature is supported. */
            const VkPhysicalDeviceFeatures& getDeviceFeatures() const { return mDeviceFeatures; }

            /** Returns a set of properties describing the memory of the physical device. */
            const VkPhysicalDeviceMemoryProperties& getMemoryProperties() const { return mMemoryProperties; }

            /** Returns the number of queue supported on the device, per type. */
            uint32_t getNumQueues(GpuQueueType type) const { return (uint32_t)mQueueInfos[(int)type].queues.size(); }

            /** Returns queue of the specified type at the specified index. Index must be in range [0, getNumQueues()). */
            VulkanQueue* getQueue(GpuQueueType type, uint32_t idx) const { return mQueueInfos[(int)type].queues[idx]; }

            /**
             * Returns index of the queue family for the specified queue type. Returns -1 if no queues for the specified type
             * exist. There will always be a queue family for the graphics type.
             */
            uint32_t getQueueFamily(GpuQueueType type) const { return mQueueInfos[(int)type].familyIdx; }

            /**
             * Fills out a mask that has bits set for every queue index that maps to the same physical queue as the provided
             * index. This is useful as different queue indices will sometimes map to the same physical queue.
             */
            uint32_t getQueueMask(GpuQueueType type, uint32_t queueIdx) const;

            /** Returns the best matching surface format according to the provided parameters. */
            SurfaceFormat getSurfaceFormat(const VkSurfaceKHR& surface, bool gamma) const;

            /** Returns a pool that can be used for allocating command buffers for all queues on this device. */
            VulkanCmdBufferPool& getCmdBufferPool() const { return *mCommandBufferPool; }

            /** Returns a pool that can be used for allocating queries on this device. */
            VulkanQueryPool& getQueryPool() const { return *mQueryPool; }

            /** Returns a manager that can be used for allocating descriptor layouts and sets. */
            CVulkanDriver* getDriver() const { return mDriver; }
            VkPhysicalDevice getDevice() const { return mPhysicalDevice; }

            /**
             * Allocates memory for the provided image, and binds it to the image. Returns null if it cannot find memory
             * with the specified flags.
             */
            VmaAllocation allocateMemory(VkImage image, VkMemoryPropertyFlags flags);

            /**
             * Allocates memory for the provided buffer, and binds it to the buffer. Returns null if it cannot find memory
             * with the specified flags.
             */
            VmaAllocation allocateMemory(VkBuffer buffer, VkMemoryPropertyFlags flags);

            /** Frees a previously allocated block of memory. */
            void freeMemory(VmaAllocation allocation);

            /** Returns the device memory block and offset into the block for a specific memory allocation. */
            void getAllocationInfo(VmaAllocation allocation, VkDeviceMemory& memory, VkDeviceSize& offset);

            VulkanTransferBuffer* getTransferBuffer(GpuQueueType type, u32 queueIdx);
            void flushTransferBuffers();

            VulkanDescriptorPool* GetDescriptorPool() { return mDescriptorPool; }

            /** Allocates a new empty descriptor set matching the provided layout. */
            VulkanDescriptorSet* createSet(VulkanDescriptorLayout* layout);

            static VkAllocationCallbacks* gVulkanAllocator;

        private:
            friend class CVulkanDriver;

            /** Attempts to find a memory type that matches the requirements bits and the requested flags. */
            uint32_t findMemoryType(uint32_t requirementBits, VkMemoryPropertyFlags wantedFlags);

            /** Marks the device as a primary device. */
            void setIsPrimary() { mIsPrimary = true; }

            VkPhysicalDevice mPhysicalDevice;
            VkDevice mLogicalDevice;
            bool mIsPrimary;
            uint32_t mDeviceIdx;

            CVulkanDriver* mDriver;
            VulkanCmdBufferPool* mCommandBufferPool;
            VulkanQueryPool* mQueryPool;
            VmaAllocator mAllocator;

            VkPhysicalDeviceProperties mDeviceProperties;
            VkPhysicalDeviceFeatures mDeviceFeatures;
            VkPhysicalDeviceMemoryProperties mMemoryProperties;

            /** Contains data about a set of queues of a specific type. */
            struct QueueInfo
            {
                uint32_t familyIdx;
                std::vector<VulkanQueue*> queues;
            };

            QueueInfo mQueueInfos[GQT_COUNT];
            VulkanTransferBuffer* transferBuffers[GQT_COUNT][_MAX_QUEUES_PER_TYPE];
            VulkanDescriptorPool* mDescriptorPool;
        };

        /** @} */
    }
}