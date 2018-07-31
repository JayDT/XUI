#include "RenderEngines/General/CCommandBuffer.h"

using namespace irr;
using namespace irr::video;

void CommandSyncMask::addDependency(const CommandBuffer* buffer)
{
    if (buffer == nullptr)
        return;

    mMask |= getGlobalQueueMask(buffer->getType(), buffer->getQueueIdx());
}

uint32_t CommandSyncMask::getGlobalQueueMask(GpuQueueType type, uint32_t queueIdx)
{
    uint32_t bitShift = 0;
    switch (type)
    {
        case GQT_GRAPHICS:
            break;
        case GQT_COMPUTE:
            bitShift = _MAX_QUEUES_PER_TYPE;
            break;
        case GQT_UPLOAD:
            bitShift = _MAX_BOUND_VERTEX_BUFFERS;
            break;
        default:
            break;
    }

    return (1 << queueIdx) << bitShift;
}

uint32_t CommandSyncMask::getGlobalQueueIdx(GpuQueueType type, uint32_t queueIdx)
{
    switch (type)
    {
        case GQT_COMPUTE:
            return _MAX_QUEUES_PER_TYPE + queueIdx;
        case GQT_UPLOAD:
            return _MAX_BOUND_VERTEX_BUFFERS + queueIdx;
        default:
            return queueIdx;
    }
}

uint32_t CommandSyncMask::getQueueIdxAndType(uint32_t globalQueueIdx, GpuQueueType& type)
{
    if (globalQueueIdx >= _MAX_BOUND_VERTEX_BUFFERS)
    {
        type = GQT_UPLOAD;
        return globalQueueIdx - _MAX_BOUND_VERTEX_BUFFERS;
    }

    if (globalQueueIdx >= _MAX_QUEUES_PER_TYPE)
    {
        type = GQT_COMPUTE;
        return globalQueueIdx - _MAX_QUEUES_PER_TYPE;
    }

    type = GQT_GRAPHICS;
    return globalQueueIdx;
}

CommandBuffer::CommandBuffer(GpuQueueType type, uint32_t deviceIdx, uint32_t queueIdx, bool secondary)
    :mType(type), mDeviceIdx(deviceIdx), mQueueIdx(queueIdx), mIsSecondary(secondary)
{

}

//std::shared_ptr<CommandBuffer> CommandBuffer::create(GpuQueueType type, uint32_t deviceIdx, uint32_t queueIdx, bool secondary)
//{
//    return CommandBufferManager::instance().create(type, deviceIdx, queueIdx, secondary);
//}
