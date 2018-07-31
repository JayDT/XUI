
#ifndef __C_CVULKAN_HARDWARE_BUFFER_H_INCLUDED__
#define __C_CVULKAN_HARDWARE_BUFFER_H_INCLUDED__

#include "IrrCompileConfig.h"

#include "IHardwareBuffer.h"
#include "CVulkanUtility.h"
#include "CVulkanResources.h"

#include <vector>

namespace irr
{
    namespace video
    {
        struct IShaderDataBuffer;
        class CVulkanDriver;
        class VulkanImage;
        class VulkanCommandBuffer;
        class VulkanCmdBuffer;
        class VulkanGraphicsPipelineState;
        class VulkanGpuParams;

        /** Wrapper around a Vulkan buffer object that manages its usage and lifetime. */
        class VulkanBuffer : public CVulkanDeviceResource
        {
        public:
            /**
            * @param[in]	owner		Manager that takes care of tracking and releasing of this object.
            * @param[in]	buffer		Actual low-level Vulkan buffer handle.
            * @param[in]	view		Optional handle to the buffer view.
            * @param[in]	allocation	Information about memory mapped to the buffer.
            * @param[in]	rowPitch	If buffer maps to an image sub-resource, length of a single row (in elements).
            * @param[in]	slicePitch	If buffer maps to an image sub-resource, size of a single 2D surface (in elements).
            */
            VulkanBuffer(CVulkanDriver* owner, VkBuffer buffer, VkBufferView view, VmaAllocation allocation, uint32_t rowPitch = 0, uint32_t slicePitch = 0);
            virtual ~VulkanBuffer();

            /** Returns the internal handle to the Vulkan object. */
            VkBuffer getHandle() const { return mBuffer; }

            /** Returns a buffer view that covers the entire buffer. */
            VkBufferView getView() const { return mView; }

            /**
            * If buffer represents an image sub-resource, this is the number of elements that separate one row of the
            * sub-resource from another (if no padding, it is equal to image width).
            */
            uint32_t getRowPitch() const { return mRowPitch; }

            /**
            * If buffer represents an image sub-resource, this is the number of elements that separate one column of the
            * sub-resource from another (if no padding, it is equal to image height). Only relevant for 3D images.
            */
            uint32_t getSliceHeight() const { return mSliceHeight; }

            /**
            * Returns a pointer to internal buffer memory. Must be followed by unmap(). Caller must ensure the buffer was
            * created in CPU readable memory, and that buffer isn't currently being written to by the GPU.
            */
            UINT8* map(VkDeviceSize offset, VkDeviceSize length) const;

            /** Unmaps a buffer previously mapped with map(). */
            void unmap();

            /**
            * Queues a command on the provided command buffer. The command copies the contents of the current buffer to
            * the destination buffer. Caller must ensure the provided offsets and length are within valid bounds of
            * both buffers.
            */
            void copy(VulkanCmdBuffer* cb, VulkanBuffer* destination, VkDeviceSize srcOffset, VkDeviceSize dstOffset,
                VkDeviceSize length);

            /**
            * Queues a command on the provided command buffer. The command copies the contents of the current buffer to
            * the destination image subresource.
            */
            void copy(VulkanCmdBuffer* cb, VulkanImage* destination, const VkExtent3D& extent,
                const VkImageSubresourceLayers& range, VkImageLayout layout);

            /**
            * Queues a command on the provided command buffer. The command copies the contents of the provided memory location
            * the destination buffer. Caller must ensure the provided offset and length are within valid bounds of
            * both buffers. Caller must ensure the offset and size is a multiple of 4, and size is equal to or less then 65536.
            */
            void update(VulkanCmdBuffer* cb, UINT8* data, VkDeviceSize offset, VkDeviceSize length);

        private:
            VkBuffer mBuffer;
            VkBufferView mView;
            VmaAllocation mAllocation;

            uint32_t mRowPitch;
            uint32_t mSliceHeight;

            // Inherited via CVulkanDeviceResource
            virtual void OnDeviceLost(CVulkanDriver * device) override;
            virtual void OnDeviceRestored(CVulkanDriver * device) override;
        };

        class CVulkanHardwareBuffer : public IHardwareBuffer, protected CVulkanDeviceResource
        {
        private:
            friend class CVulkanDriver;

            struct BufferCacheDesc
            {
                u32 Stride;
                u32 Offset;

                VkBufferCreateInfo bufferCI;
                VulkanBuffer* Buffer;
            };

            struct BufferDesc
            {
                BufferDesc();

                u32 Stride;
                u32 Element;
                u32 ChangeId;
                u32 Offset;
                VkBufferCreateInfo bufferCI;
                VkDescriptorBufferInfo Descriptor;
                VulkanBuffer* Buffer;
                std::vector<BufferCacheDesc>* BufferCache; // For Stream mode
                VulkanBuffer* mStagingBuffer;
                u8* mStagingMemory;
                E_HARDWARE_BUFFER_TYPE Type : 8;
                E_HARDWARE_BUFFER_ACCESS AccessType : 8;
                bool UseTempStagingBuffer : 1;
                bool mDirectlyMappable : 1;
                bool mSupportsGPUWrites : 1;
                bool initialize : 1;
            };

            VulkanCommandBuffer* CommandBuffer = nullptr;
            std::vector<BufferDesc> VertexBufferStreams;
            std::vector<VulkanGraphicsPipelineState*> Pipelines;
            VulkanGpuParams * mGpuParams = nullptr;

            u32 Flags;
            u32 mMappedDeviceIdx = 0;
            u32 mMappedGlobalQueueIdx = 0;
            u32 mMappedSize = 0;
            bool mMappedReadOnly = false;
            bool mIsMapped = true;

            VulkanBuffer* GetCacheBuffer(VulkanDevice & device, CVulkanHardwareBuffer::BufferDesc& descriptor, VkBufferCreateInfo& bufferCI, E_HARDWARE_BUFFER_ACCESS AccessType, u32 stride, bool readable);
            VulkanBuffer* CreateBuffer(VulkanDevice& device, VkBufferCreateInfo& bufferCI, E_HARDWARE_BUFFER_ACCESS AccessType, u32 stride, bool readable, bool staging = false);
            bool UpdateBuffer(E_HARDWARE_BUFFER_TYPE Type, E_HARDWARE_BUFFER_ACCESS AccessType, const void* initialData, u32 size, u32 offset = 0, u32 endoffset = 0);
            u32 getVertexDeclarationStride(u8 inputSlot) const;

        public:
            // Implementation of public methods
            CVulkanHardwareBuffer(CVulkanDriver* driver, scene::IMeshBuffer *meshBuffer, video::IShaderDataBuffer* instanceBuffer, u32 flags, E_VERTEX_TYPE vtype);
            CVulkanHardwareBuffer(CVulkanDriver* driver, E_HARDWARE_BUFFER_TYPE type, E_HARDWARE_BUFFER_ACCESS accessType, u32 size, u32 flags = 0, const void* initialData = 0);

            virtual ~CVulkanHardwareBuffer();

            //! Lock function.
            virtual void* lock(E_HARDWARE_BUFFER_TYPE type, u32 size, bool readOnly = false);

            //! Unlock function. Must be called after a lock() to the buffer.
            virtual void unlock(E_HARDWARE_BUFFER_TYPE type);

            //! Copy data from system memory
            virtual void copyFromMemory(E_HARDWARE_BUFFER_TYPE type, const void* sysData, u32 offset, u32 length);

            //! Copy data from another buffer
            virtual void copyFromBuffer(E_HARDWARE_BUFFER_TYPE type, IHardwareBuffer* buffer, u32 srcOffset, u32 descOffset, u32 length);

            //! Get size of buffer in bytes
            virtual u32 size(E_HARDWARE_BUFFER_TYPE type) const;

            virtual u32 GetMemoryAccessType(E_HARDWARE_BUFFER_ACCESS access);

            virtual u32 getChangeID(E_HARDWARE_BUFFER_TYPE type) const
            {
                if (VertexBufferStreams.size() <= (u32)type)
                    return 0;
                return VertexBufferStreams[(u32)type].ChangeId;
            }

            virtual void setChangeID(E_HARDWARE_BUFFER_TYPE type, u32 id)
            {
                if (VertexBufferStreams.size() <= (u32)type)
                    return;
                VertexBufferStreams[(u32)type].ChangeId = id;
            }

            const BufferDesc* GetBufferDesc(E_HARDWARE_BUFFER_TYPE type) const
            {
                if (VertexBufferStreams.size() <= (u32)type)
                    return nullptr ;
                return &VertexBufferStreams[(u32)type];
            }

            virtual u32 GetInstanceCount()
            {
                if (VertexBufferStreams.size() <= (u32)E_HARDWARE_BUFFER_TYPE::EHBT_VERTEX_INSTANCE_STREAM)
                    return 1;
                return (u32)VertexBufferStreams[(u32)E_HARDWARE_BUFFER_TYPE::EHBT_VERTEX_INSTANCE_STREAM].Element;
            }

            VulkanCommandBuffer* GetCommandBuffer() { return CommandBuffer; }
            void SetCommandBuffer(VulkanCommandBuffer* cb);
            VulkanGraphicsPipelineState* GetPipeline(u8 idx);
            void SetPipeline(u8 idx, VulkanGraphicsPipelineState* pl)
            {
                if (Pipelines.size() <= idx)
                    Pipelines.resize(idx + 1);

                Pipelines[idx] = pl;
            }
            VulkanGpuParams * GetGpuParams() { return mGpuParams; }
            void SetGpuParams(VulkanGpuParams * param);

            //! Get driver type of buffer.
            virtual E_DRIVER_TYPE getDriverType() const;

            //! Get type of buffer.
            virtual E_HARDWARE_BUFFER_TYPE getType(E_HARDWARE_BUFFER_TYPE type) const;

            //! Get flags
            virtual u32 getFlags() const;

            // Methods for Vulkan implementation

            virtual void Bind();
            virtual void Unbind();
            virtual void Initialize();
            virtual void Finalize();

            // Inherited via D3D11DeviceResource
            virtual void OnDeviceLost(CVulkanDriver * device) override;
            virtual void OnDeviceRestored(CVulkanDriver * device) override;
        };
    }
}

#endif