#include "IrrCompileConfig.h"

#define _IRR_DONT_DO_MEMORY_DEBUGGING_HERE

#include "CVulkanDriver.h"
#include "CVulkanHardwareBuffer.h"
#include "CVulkanShader.h"
#include "CVulkanDevice.h"
#include "CVulkanTexture.h"
#include "CVulkanGpuPipelineState.h"
#include "CVulkanGpuParams.h"
#include "os.h"

using namespace irr;
using namespace video;

VulkanBuffer::VulkanBuffer(CVulkanDriver* owner, VkBuffer buffer, VkBufferView view, VmaAllocation allocation,
    uint32_t rowPitch, uint32_t slicePitch)
    : CVulkanDeviceResource(owner)
    , mBuffer(buffer)
    , mView(view)
    , mAllocation(allocation)
    , mRowPitch(rowPitch)
{
    if (rowPitch != 0)
        mSliceHeight = slicePitch / rowPitch;
    else
        mSliceHeight = 0;
}

VulkanBuffer::~VulkanBuffer()
{
    VulkanDevice* device = Driver->_getPrimaryDevice();

    if (mView != VK_NULL_HANDLE)
        vkDestroyBufferView(device->getLogical(), mView, VulkanDevice::gVulkanAllocator);

    vkDestroyBuffer(device->getLogical(), mBuffer, VulkanDevice::gVulkanAllocator);
    device->freeMemory(mAllocation);
}

UINT8* VulkanBuffer::map(VkDeviceSize offset, VkDeviceSize length) const
{
    VulkanDevice* device = Driver->_getPrimaryDevice();

    VkDeviceMemory memory;
    VkDeviceSize memoryOffset;
    device->getAllocationInfo(mAllocation, memory, memoryOffset);

    UINT8* data;
    VkResult result = vkMapMemory(device->getLogical(), memory, memoryOffset + offset, length, 0, (void**)&data);
    assert(result == VK_SUCCESS);

    return data;
}

void VulkanBuffer::unmap()
{
    VulkanDevice* device = Driver->_getPrimaryDevice();

    VkDeviceMemory memory;
    VkDeviceSize memoryOffset;
    device->getAllocationInfo(mAllocation, memory, memoryOffset);

    vkUnmapMemory(device->getLogical(), memory);
}

void VulkanBuffer::copy(VulkanCmdBuffer* cb, VulkanBuffer* destination, VkDeviceSize srcOffset,
    VkDeviceSize dstOffset, VkDeviceSize length)
{
    VkBufferCopy region;
    region.size = length;
    region.srcOffset = srcOffset;
    region.dstOffset = dstOffset;

    vkCmdCopyBuffer(cb->getHandle(), mBuffer, destination->getHandle(), 1, &region);
}

void VulkanBuffer::copy(VulkanCmdBuffer* cb, VulkanImage* destination, const VkExtent3D& extent,
    const VkImageSubresourceLayers& range, VkImageLayout layout)
{
    VkBufferImageCopy region;
    region.bufferRowLength = mRowPitch;
    region.bufferImageHeight = mSliceHeight;
    region.bufferOffset = 0;
    region.imageOffset.x = 0;
    region.imageOffset.y = 0;
    region.imageOffset.z = 0;
    region.imageExtent = extent;
    region.imageSubresource = range;

    vkCmdCopyBufferToImage(cb->getHandle(), mBuffer, destination->getHandle(), layout, 1, &region);
}

void VulkanBuffer::update(VulkanCmdBuffer* cb, UINT8* data, VkDeviceSize offset, VkDeviceSize length)
{
    vkCmdUpdateBuffer(cb->getHandle(), mBuffer, offset, length, (uint32_t*)data);
}

void irr::video::VulkanBuffer::OnDeviceLost(CVulkanDriver * device)
{
}

void irr::video::VulkanBuffer::OnDeviceRestored(CVulkanDriver * device)
{
}

CVulkanHardwareBuffer::CVulkanHardwareBuffer(CVulkanDriver* driver, scene::IMeshBuffer *meshBuffer, video::IShaderDataBuffer* instanceBuffer, u32 flags, E_VERTEX_TYPE vtype)
    : IHardwareBuffer(meshBuffer, instanceBuffer)
    , CVulkanDeviceResource(driver)
    , Flags(flags)
{
}

irr::video::CVulkanHardwareBuffer::CVulkanHardwareBuffer(CVulkanDriver * driver, E_HARDWARE_BUFFER_TYPE type, E_HARDWARE_BUFFER_ACCESS accessType, u32 size, u32 flags, const void * initialData)
    : IHardwareBuffer(nullptr, nullptr)
    , CVulkanDeviceResource(driver)
    , Flags(flags)
{
}

irr::video::CVulkanHardwareBuffer::~CVulkanHardwareBuffer()
{
    if (CommandBuffer)
        CommandBuffer->drop();

    if (mGpuParams)
        mGpuParams->drop();

    for (VulkanGraphicsPipelineState* pipeline : Pipelines)
        if (pipeline)
            delete pipeline;

    for (BufferDesc& buffer : VertexBufferStreams)
    {
        if (buffer.Buffer)
            buffer.Buffer->drop();
        if (buffer.mStagingBuffer)
            buffer.mStagingBuffer->drop();
        if (buffer.mStagingMemory)
            free(buffer.mStagingMemory);
    }

    Pipelines.clear();
    VertexBufferStreams.clear();
}

void * irr::video::CVulkanHardwareBuffer::lock(E_HARDWARE_BUFFER_TYPE type, u32 length, bool readOnly)
{
    if (VertexBufferStreams.size() <= (u32)type)
        return nullptr;

    CVulkanHardwareBuffer::BufferDesc& desc = VertexBufferStreams[(u32)type];

    VulkanBuffer* buffer = desc.Buffer;

    if (buffer == nullptr)
        return nullptr;

    if (length > desc.bufferCI.size)
    {
        //os::Printer::log("Provided offset(" + std::to_string(offset) + ") + length(" + toString(length) + ") "
        //    "is larger than the buffer " + toString(mSize) + ".");

        return nullptr;
    }

    if (length == 0)
        return nullptr;

    mIsMapped = true;
    mMappedDeviceIdx = 0;
    mMappedGlobalQueueIdx = 0;
    mMappedSize = length;
    mMappedReadOnly = readOnly;

    VulkanDevice* device = Driver->_getDevice(mMappedDeviceIdx);

    GpuQueueType queueType;
    UINT32 localQueueIdx = CommandSyncMask::getQueueIdxAndType(mMappedGlobalQueueIdx, queueType);

    VkAccessFlags accessFlags;
    if (mMappedReadOnly)
        accessFlags = VK_ACCESS_HOST_READ_BIT;
    //else if (options == GBL_READ_WRITE)
    //    accessFlags = VK_ACCESS_HOST_READ_BIT | VK_ACCESS_HOST_WRITE_BIT;
    else
        accessFlags = VK_ACCESS_HOST_WRITE_BIT;

    // If memory is host visible try mapping it directly
    if (desc.mDirectlyMappable)
    {
        // Check is the GPU currently reading or writing from the buffer
        UINT32 useMask = buffer->getUseInfo(VulkanUseFlag::Read | VulkanUseFlag::Write);

        // Note: Even if GPU isn't currently using the buffer, but the buffer supports GPU writes, we consider it as
        // being used because the write could have completed yet still not visible, so we need to issue a pipeline
        // barrier below.
        bool isUsedOnGPU = useMask != 0 || desc.mSupportsGPUWrites;

        // We're safe to map directly since GPU isn't using the buffer
        if (!isUsedOnGPU)
        {
            // If some CB has an operation queued that will be using the current contents of the buffer, create a new 
            // buffer so we don't modify the previous use of the buffer
            if (buffer->isBound())
            {
                VulkanBuffer* newBuffer;
                if (desc.AccessType == E_HARDWARE_BUFFER_ACCESS::EHBA_STREAM || desc.AccessType == E_HARDWARE_BUFFER_ACCESS::EHBA_DYNAMIC)
                    newBuffer = GetCacheBuffer(*device, desc, desc.bufferCI, desc.AccessType, desc.Stride, true);
                else
                    newBuffer = CreateBuffer(*device, desc.bufferCI, desc.AccessType, desc.Stride, true);

                // Copy contents of the current buffer to the new one, unless caller explicitly specifies he doesn't
                // care about the current contents
                if (accessFlags & VK_ACCESS_HOST_READ_BIT) //(options != GBL_WRITE_ONLY_DISCARD)
                {
                    UINT8* src = buffer->map(0, length);
                    UINT8* dst = newBuffer->map(0, length);
                
                    memcpy(dst, src, length);
                
                    buffer->unmap();
                    newBuffer->unmap();
                }

                buffer->drop();
                buffer = newBuffer;
                desc.Buffer = buffer;
            }

            return buffer->map(0, length);
        }

        // Caller guarantees he won't touch the same data as the GPU, so just map even though the GPU is using the buffer
        //if (accessFlags == VK_ACCESS_HOST_WRITE_BIT) // (options == GBL_WRITE_ONLY_NO_OVERWRITE)
        //    return buffer->map(0, length);

        // Caller doesn't care about buffer contents, so just discard the existing buffer and create a new one
        if (accessFlags == VK_ACCESS_HOST_WRITE_BIT) //options == GBL_WRITE_ONLY_DISCARD)
        {
            buffer->drop();
        
            if (desc.AccessType == E_HARDWARE_BUFFER_ACCESS::EHBA_STREAM || desc.AccessType == E_HARDWARE_BUFFER_ACCESS::EHBA_DYNAMIC)
                buffer = GetCacheBuffer(*device, desc, desc.bufferCI, desc.AccessType, desc.Stride, true);
            else
                buffer = CreateBuffer(*device, desc.bufferCI, desc.AccessType, desc.Stride, true);

            desc.Buffer = buffer;

            return buffer->map(0, length);
        }

        // We need to read the buffer contents
        if (accessFlags & (VK_ACCESS_HOST_READ_BIT | VK_ACCESS_HOST_WRITE_BIT)) //(options == GBL_READ_ONLY || options == GBL_READ_WRITE)
        {
            // We need to wait until (potential) read/write operations complete
            VulkanTransferBuffer* transferCB = device->getTransferBuffer(queueType, localQueueIdx);

            // Ensure flush() will wait for all queues currently using to the buffer (if any) to finish
            // If only reading, wait for all writes to complete, otherwise wait on both writes and reads
            if (readOnly)
                useMask = buffer->getUseInfo(VulkanUseFlag::Write);
            else
                useMask = buffer->getUseInfo(VulkanUseFlag::Read | VulkanUseFlag::Write);

            transferCB->appendMask(useMask);

            // Make any writes visible before mapping
            if (desc.mSupportsGPUWrites)
            {
                // Issue a barrier so :
                //  - If reading: the device makes the written memory available for read (read-after-write hazard)
                //  - If writing: ensures our writes properly overlap with GPU writes (write-after-write hazard)
                transferCB->memoryBarrier(buffer->getHandle(),
                    VK_ACCESS_SHADER_WRITE_BIT,
                    accessFlags,
                    // Last stages that could have written to the buffer:
                    VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                    VK_PIPELINE_STAGE_HOST_BIT
                );
            }

            // Submit the command buffer and wait until it finishes
            transferCB->flush(true);

            // If writing and some CB has an operation queued that will be using the current contents of the buffer, 
            // create a new  buffer so we don't modify the previous use of the buffer
            if (accessFlags == (VK_ACCESS_HOST_READ_BIT | VK_ACCESS_HOST_WRITE_BIT) && buffer->isBound()) //(options == GBL_READ_WRITE && buffer->isBound())
            {
                VulkanBuffer* newBuffer = CreateBuffer(*device, desc.bufferCI, desc.AccessType, desc.Stride, true);

                // Copy contents of the current buffer to the new one
                UINT8* src = buffer->map(0, length);
                UINT8* dst = newBuffer->map(0, length);

                memcpy(dst, src, length);

                buffer->unmap();
                newBuffer->unmap();

                buffer->drop();
                buffer = newBuffer;
                desc.Buffer = buffer;
            }

            return buffer->map(0, length);
        }

        // Otherwise, we're doing write only, in which case it's best to use the staging buffer to avoid waiting
        // and blocking, so fall through
    }

    // Can't use direct mapping, so use a staging buffer or memory

    // We might need to copy the current contents of the buffer to the staging buffer. Even if the user doesn't plan on
    // reading, it is still required as we will eventually copy all of the contents back to the original buffer,
    // and we can't write potentially uninitialized data. The only exception is when the caller specifies the buffer
    // contents should be discarded in which he guarantees he will overwrite the entire locked area with his own
    // contents.
    bool needRead = accessFlags & VK_ACCESS_HOST_READ_BIT; //options != GBL_WRITE_ONLY_DISCARD_RANGE && options != GBL_WRITE_ONLY_DISCARD;

    // See if we can use the cheaper staging memory, rather than a staging buffer
    if (!needRead && /*offset % 4 == 0 &&*/ length % 4 == 0 && length <= 65536)
    {
        desc.mStagingMemory = (UINT8*)malloc(length);
        return desc.mStagingMemory;
    }

    // Create a staging buffer
    VkBufferCreateInfo StagingBufferCI = desc.bufferCI;
    desc.mStagingBuffer = CreateBuffer(*device, StagingBufferCI, E_HARDWARE_BUFFER_ACCESS::EHBA_DYNAMIC, 0, needRead, true);

    if (needRead)
    {
        VulkanTransferBuffer* transferCB = device->getTransferBuffer(queueType, localQueueIdx);

        // Similar to above, if buffer supports GPU writes or is currently being written to, we need to wait on any
        // potential writes to complete
        UINT32 writeUseMask = buffer->getUseInfo(VulkanUseFlag::Write);
        if (desc.mSupportsGPUWrites || writeUseMask != 0)
        {
            // Ensure flush() will wait for all queues currently writing to the buffer (if any) to finish
            transferCB->appendMask(writeUseMask);
        }

        // Queue copy command
        buffer->copy(transferCB->getCB(), desc.mStagingBuffer, 0, 0, length);

        // Ensure data written to the staging buffer is visible
        transferCB->memoryBarrier(desc.mStagingBuffer->getHandle(),
            VK_ACCESS_TRANSFER_WRITE_BIT,
            accessFlags,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_HOST_BIT
        );

        // Submit the command buffer and wait until it finishes
        transferCB->flush(true);
        assert(!buffer->isUsed());
    }

    return desc.mStagingBuffer->map(0, length);
}

void irr::video::CVulkanHardwareBuffer::unlock(E_HARDWARE_BUFFER_TYPE type)
{
    // Possibly map() failed with some error
    if (!mIsMapped)
        return;

    if (VertexBufferStreams.size() <= (u32)type)
        return;

    CVulkanHardwareBuffer::BufferDesc& desc = VertexBufferStreams[(u32)type];

    VulkanBuffer* buffer = desc.Buffer;

    if (buffer == nullptr)
        return;

    // Note: If we did any writes they need to be made visible to the GPU. However there is no need to execute 
    // a pipeline barrier because (as per spec) host writes are implicitly visible to the device.

    if (desc.mStagingMemory == nullptr && desc.mStagingBuffer == nullptr) // We directly mapped the buffer
    {
        desc.Buffer->unmap();
    }
    else
    {
        if (desc.mStagingBuffer != nullptr)
            desc.mStagingBuffer->unmap();

        bool isWrite = !mMappedReadOnly; // != GBL_READ_ONLY;

        // We the caller wrote anything to the staging buffer, we need to upload it back to the main buffer
        if (isWrite)
        {
            VulkanDevice* device = Driver->_getDevice(mMappedDeviceIdx);

            GpuQueueType queueType;
            UINT32 localQueueIdx = CommandSyncMask::getQueueIdxAndType(mMappedGlobalQueueIdx, queueType);

            VulkanTransferBuffer* transferCB = device->getTransferBuffer(queueType, localQueueIdx);

            // If the buffer is used in any way on the GPU, we need to wait for that use to finish before
            // we issue our copy
            UINT32 useMask = buffer->getUseInfo(VulkanUseFlag::Read | VulkanUseFlag::Write);
            bool isNormalWrite = false;
            if (useMask != 0) // Buffer is currently used on the GPU
            {
                // Try to avoid the wait by checking for special write conditions

                // Caller guarantees he won't touch the same data as the GPU, so just copy
                //if (mMappedLockOptions == GBL_WRITE_ONLY_NO_OVERWRITE)
                //{
                //    // Fall through to copy()
                //}
                //// Caller doesn't care about buffer contents, so just discard the existing buffer and create a new one
                //else if (mMappedLockOptions == GBL_WRITE_ONLY_DISCARD)
                //{
                //    buffer->destroy();
                //
                //    buffer = createBuffer(device, mSize, false, true);
                //    mBuffers[mMappedDeviceIdx] = buffer;
                //}
                //else // Otherwise we have no choice but to issue a dependency between the queues
                //{
                //    transferCB->appendMask(useMask);
                //    isNormalWrite = true;
                //}
            }
            else
                isNormalWrite = true;

            // Check if the buffer will still be bound somewhere after the CBs using it finish
            if (isNormalWrite)
            {
                UINT32 useCount = buffer->getUseCount();
                UINT32 boundCount = buffer->getBoundCount();

                bool isBoundWithoutUse = boundCount > useCount;

                // If buffer is queued for some operation on a CB, then we need to make a copy of the buffer to
                // avoid modifying its use in the previous operation
                if (isBoundWithoutUse)
                {
                    VulkanBuffer* newBuffer = CreateBuffer(*device, desc.bufferCI, desc.AccessType, desc.Stride, true);

                    // Avoid copying original contents if the staging buffer completely covers it
                    if (/*mMappedOffset > 0 ||*/ mMappedSize != desc.bufferCI.size)
                    {
                        buffer->copy(transferCB->getCB(), newBuffer, 0, 0, desc.bufferCI.size);

                        transferCB->getCB()->registerResource(buffer, VK_ACCESS_TRANSFER_READ_BIT, VulkanUseFlag::Read);
                    }

                    buffer->drop();
                    buffer = newBuffer;
                    desc.Buffer = buffer;
                }
            }

            // Queue copy/update command
            if (desc.mStagingBuffer != nullptr)
            {
                desc.mStagingBuffer->copy(transferCB->getCB(), buffer, 0, 0, mMappedSize);
                transferCB->getCB()->registerResource(desc.mStagingBuffer, VK_ACCESS_TRANSFER_READ_BIT, VulkanUseFlag::Read);
            }
            else // Staging memory
            {
                buffer->update(transferCB->getCB(), desc.mStagingMemory, 0, mMappedSize);
            }

            transferCB->getCB()->registerResource(buffer, VK_ACCESS_TRANSFER_WRITE_BIT, VulkanUseFlag::Write);

            // We don't actually flush the transfer buffer here since it's an expensive operation, but it's instead
            // done automatically before next "normal" command buffer submission.
        }

        if (desc.mStagingBuffer != nullptr)
        {
            desc.mStagingBuffer->drop();
            desc.mStagingBuffer = nullptr;
        }

        if (desc.mStagingMemory != nullptr)
        {
            free(desc.mStagingMemory);
            desc.mStagingMemory = nullptr;
        }
    }

    mIsMapped = false;
}

void irr::video::CVulkanHardwareBuffer::copyFromMemory(E_HARDWARE_BUFFER_TYPE type, const void * sysData, u32 offset, u32 length)
{
}

void irr::video::CVulkanHardwareBuffer::copyFromBuffer(E_HARDWARE_BUFFER_TYPE type, IHardwareBuffer * buffer, u32 srcOffset, u32 descOffset, u32 length)
{
}

u32 irr::video::CVulkanHardwareBuffer::size(E_HARDWARE_BUFFER_TYPE type) const
{
    if (VertexBufferStreams.size() <= (u32)type)
        return 0;

    return VertexBufferStreams[(u32)type].bufferCI.size;
}

u32 irr::video::CVulkanHardwareBuffer::GetMemoryAccessType(E_HARDWARE_BUFFER_ACCESS AccessType)
{
    if (AccessType == E_HARDWARE_BUFFER_ACCESS::EHBA_IMMUTABLE || AccessType == E_HARDWARE_BUFFER_ACCESS::EHBA_DEFAULT)
        return VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    if (AccessType == E_HARDWARE_BUFFER_ACCESS::EHBA_DYNAMIC)
        return VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    return VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
}

void irr::video::CVulkanHardwareBuffer::SetCommandBuffer(VulkanCommandBuffer * cb)
{
    if (CommandBuffer)
        CommandBuffer->drop();

    CommandBuffer = cb;

    if (CommandBuffer)
        CommandBuffer->grab();
}

VulkanGraphicsPipelineState * irr::video::CVulkanHardwareBuffer::GetPipeline(u8 idx)
{
    return Pipelines[idx];
}

void irr::video::CVulkanHardwareBuffer::SetGpuParams(VulkanGpuParams * param)
{
    if (mGpuParams)
        mGpuParams->drop();

    mGpuParams = param;

    if (mGpuParams)
        mGpuParams->grab();
}

E_DRIVER_TYPE irr::video::CVulkanHardwareBuffer::getDriverType() const
{
    return E_DRIVER_TYPE::EDT_VULKAN;
}

E_HARDWARE_BUFFER_TYPE irr::video::CVulkanHardwareBuffer::getType(E_HARDWARE_BUFFER_TYPE type) const
{
    return type;
}

u32 irr::video::CVulkanHardwareBuffer::getFlags() const
{
    return Flags;
}

void irr::video::CVulkanHardwareBuffer::Bind()
{
    if (auto desc = GetBufferDesc(E_HARDWARE_BUFFER_TYPE::EHBT_INDEX))
    {
        VkBuffer buffer = desc->Buffer->getHandle();
        GetCommandBuffer()->getInternal()->setIndexBuffer(desc->Buffer, (this->Flags & (u32)E_HARDWARE_BUFFER_FLAGS::EHBF_INDEX_32_BITS) ? E_INDEX_TYPE::EIT_32BIT : E_INDEX_TYPE::EIT_16BIT);
    }

    u32 vertexBufferNum = 0;
    VkDeviceSize offsets[2] = { 0, 0 };
    // Store the vertex and instance buffers into an array
    VulkanBuffer* vertBuffers[2] = { nullptr, nullptr };

    if (auto desc = GetBufferDesc(E_HARDWARE_BUFFER_TYPE::EHBT_VERTEX))
    {
        ++vertexBufferNum;

        offsets[0] = desc->Offset;
        vertBuffers[0] = desc->Buffer;
    }

    if (auto desc = GetBufferDesc(E_HARDWARE_BUFFER_TYPE::EHBT_VERTEX_INSTANCE_STREAM))
    {
        ++vertexBufferNum;

        offsets[1] = desc->Offset;
        vertBuffers[1] = desc->Buffer;
    }

    // set vertex buffer
    if (vertexBufferNum)
    {
        GetCommandBuffer()->getInternal()->setVertexBuffers(0, vertBuffers, vertexBufferNum);
    }

    IHardwareBuffer::Bind();
}

void irr::video::CVulkanHardwareBuffer::Unbind()
{
    IHardwareBuffer::Unbind();
}

void irr::video::CVulkanHardwareBuffer::Initialize()
{
}

void irr::video::CVulkanHardwareBuffer::Finalize()
{
}

void irr::video::CVulkanHardwareBuffer::OnDeviceLost(CVulkanDriver * device)
{
}

void irr::video::CVulkanHardwareBuffer::OnDeviceRestored(CVulkanDriver * device)
{
}

VulkanBuffer * irr::video::CVulkanHardwareBuffer::GetCacheBuffer(VulkanDevice & device, CVulkanHardwareBuffer::BufferDesc& descriptor, VkBufferCreateInfo& bufferCI, E_HARDWARE_BUFFER_ACCESS AccessType, u32 stride, bool readable)
{
    if (!descriptor.BufferCache)
        descriptor.BufferCache = new std::vector<BufferCacheDesc>;

    for (std::vector<BufferCacheDesc>::const_iterator itr = descriptor.BufferCache->begin(); itr != descriptor.BufferCache->end(); ++itr)
    {
        const BufferCacheDesc& entry = *itr;
        if (!entry.Buffer->isBound() && entry.bufferCI.size >= bufferCI.size)
        {
            entry.Buffer->grab();
            return entry.Buffer;
        }
    }

    // Cannot find an empty set, allocate a new one
    VulkanBuffer* buffer = CreateBuffer(device, bufferCI, AccessType, stride, readable, false);
    BufferCacheDesc& cacheEntry = descriptor.BufferCache->emplace_back();
    cacheEntry.Offset = descriptor.Offset;
    cacheEntry.Stride = descriptor.Stride;
    cacheEntry.bufferCI = bufferCI;
    cacheEntry.Buffer = buffer;
    cacheEntry.Buffer->grab();
    return buffer;
}

VulkanBuffer * irr::video::CVulkanHardwareBuffer::CreateBuffer(VulkanDevice & device, VkBufferCreateInfo& bufferCI, E_HARDWARE_BUFFER_ACCESS AccessType, u32 stride, bool readable, bool staging)
{
    VulkanDevice* Device = Driver->_getPrimaryDevice();

    if (staging)
    {
        bufferCI.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

        // Staging buffers are used as a destination for reads
        if (readable)
            bufferCI.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    }
    else if (readable)
        bufferCI.usage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

    bufferCI.usage |= VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT;

    VkBuffer buffer;
    VkResult result = vkCreateBuffer(Device->getLogical(), &bufferCI, VulkanDevice::gVulkanAllocator, &buffer);
    assert(result == VK_SUCCESS);

    VkMemoryPropertyFlags flags = GetMemoryAccessType(AccessType);
    VmaAllocation allocation = Device->allocateMemory(buffer, flags);

    VulkanBuffer* vkbuffer =  new VulkanBuffer(Driver, buffer, VK_NULL_HANDLE, allocation, stride);
#ifdef _DEBUG
    vkbuffer->SetDebugName("CVulkanHardwareBuffer");
#endif
    return vkbuffer;
}

bool irr::video::CVulkanHardwareBuffer::UpdateBuffer(E_HARDWARE_BUFFER_TYPE Type, E_HARDWARE_BUFFER_ACCESS AccessType, const void * initialData, u32 size, u32 offset, u32 endoffset)
{
    if (VertexBufferStreams.size() <= (int)Type)
        VertexBufferStreams.resize(int(Type) + 1);

    CVulkanHardwareBuffer::BufferDesc& desc = VertexBufferStreams[(u32)Type];

    // set need of staging buffer
    if (/*AccessType == E_HARDWARE_BUFFER_ACCESS::EHBA_DYNAMIC ||*/ AccessType == E_HARDWARE_BUFFER_ACCESS::EHBA_IMMUTABLE)
        desc.UseTempStagingBuffer = true;

    if (Type == E_HARDWARE_BUFFER_TYPE::EHBT_VERTEX || Type == E_HARDWARE_BUFFER_TYPE::EHBT_VERTEX_INSTANCE_STREAM)
    {
        if (!desc.initialize)
        {
            desc.bufferCI.usage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

            desc.Stride = getVertexDeclarationStride((u32)Type - 1);
            desc.initialize = true;
            desc.Type = Type;
        }
    }
    else if (Type == E_HARDWARE_BUFFER_TYPE::EHBT_INDEX)
    {
        if (!desc.initialize)
        {
            desc.bufferCI.usage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

            desc.Stride = (this->Flags & (u32)E_HARDWARE_BUFFER_FLAGS::EHBF_INDEX_32_BITS) ? sizeof(u32) : sizeof(u16);
            desc.initialize = true;
            desc.Type = Type;
        }
    }
    else if (Type == E_HARDWARE_BUFFER_TYPE::EHBT_SHADER_RESOURCE)
    {
        if (!desc.initialize)
        {
            desc.bufferCI.usage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

            desc.Stride = size;
            desc.initialize = true;
            desc.Type = Type;
            desc.mSupportsGPUWrites = true;
        }
    }
    else if (Type == E_HARDWARE_BUFFER_TYPE::EHBT_CONSTANTS)
    {
        if (!desc.initialize)
        {
            desc.bufferCI.usage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;

            desc.Stride = size;
            desc.initialize = true;
            desc.Type = Type;
            desc.mSupportsGPUWrites = true;
        }
    }
    else
    {
        os::Printer::log("Buffer type not supported", ELL_ERROR);
        return false;
    }

    if (size > 0)
    {
        if (desc.bufferCI.size < size || desc.AccessType != AccessType)
        {
            VulkanDevice* Device = Driver->_getPrimaryDevice();

            desc.bufferCI.size = size;
            desc.AccessType = AccessType;

            if (desc.Buffer)
                desc.Buffer->drop();

            desc.mDirectlyMappable = (GetMemoryAccessType(AccessType) & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) != 0;
            desc.Buffer = CreateBuffer(*Device, desc.bufferCI, AccessType, desc.Stride, desc.mDirectlyMappable);

            desc.Descriptor.buffer = desc.Buffer->getHandle();
            desc.Descriptor.offset = desc.Offset;
            desc.Descriptor.range = size;
        }

        if (initialData)
        {
            void* mappedPtr = lock(Type, size);
            memcpy(mappedPtr, initialData, size);
            unlock(Type);
        }
    }

    return true;
}

u32 irr::video::CVulkanHardwareBuffer::getVertexDeclarationStride(u8 inputSlot) const
{
    return GetBuffer() ? static_cast<CVulkanVertexDeclaration*>(GetBuffer()->GetVertexDeclaration())->GetVertexPitch(inputSlot) : static_cast<CVulkanVertexDeclaration*>(Driver->GetVertexDeclaration(E_VERTEX_TYPE::EVT_STANDARD))->GetVertexPitch(inputSlot);
}

irr::video::CVulkanHardwareBuffer::BufferDesc::BufferDesc()
{
    memset(this, 0, sizeof(BufferDesc));

    bufferCI.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
}
