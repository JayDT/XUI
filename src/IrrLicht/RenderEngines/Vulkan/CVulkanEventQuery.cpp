#include "CVulkanEventQuery.h"
#include "CVulkanDriver.h"
#include "CVulkanDevice.h"
#include "CVulkanCommandBuffer.h"

using namespace irr;
using namespace irr::video;

VulkanEvent::VulkanEvent(const VulkanCommandBuffer* owner)
    : CVulkanDeviceResource(const_cast<VulkanCommandBuffer*>(owner)->GetDriver())
{
    VkDevice vkDevice = Driver->_getPrimaryDevice()->getLogical();

    VkEventCreateInfo eventCI;
    eventCI.sType = VK_STRUCTURE_TYPE_EVENT_CREATE_INFO;
    eventCI.pNext = nullptr;
    eventCI.flags = 0;

    VkResult result = vkCreateEvent(vkDevice, &eventCI, VulkanDevice::gVulkanAllocator, &mEvent);
    assert(result == VK_SUCCESS);
}

VulkanEvent::~VulkanEvent()
{
    VkDevice vkDevice = Driver->_getPrimaryDevice()->getLogical();
    vkDestroyEvent(vkDevice, mEvent, VulkanDevice::gVulkanAllocator);
}

bool VulkanEvent::isSignaled() const
{
    VkDevice vkDevice = Driver->_getPrimaryDevice()->getLogical();
    return vkGetEventStatus(vkDevice, mEvent) == VK_EVENT_SET;
}

void VulkanEvent::reset()
{
    VkDevice vkDevice = Driver->_getPrimaryDevice()->getLogical();

    VkResult result = vkResetEvent(vkDevice, mEvent);
    assert(result == VK_SUCCESS);
}

void irr::video::VulkanEvent::OnDeviceLost(CVulkanDriver * device)
{
}

void irr::video::VulkanEvent::OnDeviceRestored(CVulkanDriver * device)
{
}

VulkanEventQuery::VulkanEventQuery(CVulkanDriver& device)
    : mDevice(device)
    , mEvent(nullptr)
{
}

VulkanEventQuery::~VulkanEventQuery()
{
    if (mEvent != nullptr)
        mEvent->drop();
}

void VulkanEventQuery::begin(const CommandBuffer* cb)
{
    if (!cb)
        return;

    if (mEvent != nullptr)
    {
        if (mEvent->isBound())
        {
            // Clear current event and create a new one
            mEvent->drop();
            mEvent = new VulkanEvent(static_cast<const VulkanCommandBuffer*>(cb));
        }
        else
        {
            // Re-use existing event
            mEvent->reset();
        }
    }
    else // Create new event
        mEvent = new VulkanEvent(static_cast<const VulkanCommandBuffer*>(cb));
    
    const VulkanCommandBuffer* vulkanCB = static_cast<const VulkanCommandBuffer*>(cb);
    
    VulkanCmdBuffer* internalCB = vulkanCB->getInternal();
    internalCB->registerResource(mEvent, VulkanUseFlag::Read);
    
    internalCB->setEvent(mEvent);
    
    setActive(true);
}

bool VulkanEventQuery::isReady() const
{
    if (mEvent == nullptr)
        return false;

    return mEvent->isSignaled();
}

VulkanQuery::VulkanQuery(CVulkanDriver* owner, VkQueryPool pool, uint32_t queryIdx)
    : CVulkanDeviceResource(owner)
    , mPool(pool)
    , mQueryIdx(queryIdx)
{
}

bool VulkanQuery::getResult(UINT64& result) const
{
    // Note: A potentially better approach to get results is to make the query pool a VulkanResource, which we attach
    // to a command buffer upon use. Then when CB finishes executing we perform vkGetQueryPoolResults on all queries
    // in the pool at once.

    VkDevice vkDevice = Driver->_getPrimaryDevice()->getLogical();
    VkResult vkResult = vkGetQueryPoolResults(vkDevice, mPool, mQueryIdx, 1, sizeof(result), &result, sizeof(result), VK_QUERY_RESULT_64_BIT);
    assert(vkResult == VK_SUCCESS || vkResult == VK_NOT_READY);
    
    return vkResult == VK_SUCCESS;
}

void VulkanQuery::reset(VkCommandBuffer cmdBuf)
{
    vkCmdResetQueryPool(cmdBuf, mPool, mQueryIdx, 1);
}

void irr::video::VulkanQuery::OnDeviceLost(CVulkanDriver * device)
{
}

void irr::video::VulkanQuery::OnDeviceRestored(CVulkanDriver * device)
{
}

VulkanQueryPool::VulkanQueryPool(VulkanDevice& device)
    :mDevice(device)
{
    std::unique_lock<std::mutex> lock(mMutex);

    allocatePool(VK_QUERY_TYPE_TIMESTAMP);
    allocatePool(VK_QUERY_TYPE_OCCLUSION);
}

VulkanQueryPool::~VulkanQueryPool()
{
    std::unique_lock<std::mutex> lock(mMutex);

    for (auto& entry : mTimerQueries)
    {
        if (entry != nullptr)
            entry->drop();
    }

    for (auto& entry : mOcclusionQueries)
    {
        if (entry != nullptr)
            entry->drop();
    }

    for (auto& entry : mTimerPools)
        vkDestroyQueryPool(mDevice.getLogical(), entry.pool, VulkanDevice::gVulkanAllocator);

    for (auto& entry : mOcclusionPools)
        vkDestroyQueryPool(mDevice.getLogical(), entry.pool, VulkanDevice::gVulkanAllocator);
}

VulkanQueryPool::PoolInfo& VulkanQueryPool::allocatePool(VkQueryType type)
{
    VkQueryPoolCreateInfo queryPoolCI;
    queryPoolCI.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
    queryPoolCI.pNext = nullptr;
    queryPoolCI.flags = 0;
    queryPoolCI.pipelineStatistics = 0;
    queryPoolCI.queryCount = NUM_QUERIES_PER_POOL;
    queryPoolCI.queryType = type;

    PoolInfo poolInfo;
    VkResult result = vkCreateQueryPool(mDevice.getLogical(), &queryPoolCI, VulkanDevice::gVulkanAllocator, &poolInfo.pool);
    assert(result == VK_SUCCESS);

    auto& poolInfos = type == VK_QUERY_TYPE_TIMESTAMP ? mTimerPools : mOcclusionPools;
    poolInfo.startIdx = (UINT32)poolInfos.size() * NUM_QUERIES_PER_POOL;

    poolInfos.push_back(poolInfo);

    auto& queries = type == VK_QUERY_TYPE_TIMESTAMP ? mTimerQueries : mOcclusionQueries;
    for (UINT32 i = 0; i < NUM_QUERIES_PER_POOL; i++)
        queries.push_back(nullptr);

    return poolInfos.back();
}

VulkanQuery* VulkanQueryPool::getQuery(VkQueryType type)
{
    auto& queries = type == VK_QUERY_TYPE_TIMESTAMP ? mTimerQueries : mOcclusionQueries;
    auto& poolInfos = type == VK_QUERY_TYPE_TIMESTAMP ? mTimerPools : mOcclusionPools;

    for (UINT32 i = 0; i < (UINT32)queries.size(); i++)
    {
        VulkanQuery* curQuery = queries[i];
        if (curQuery == nullptr)
        {
            div_t divResult = std::div(i, (INT32)NUM_QUERIES_PER_POOL);
            UINT32 poolIdx = (UINT32)divResult.quot;
            UINT32 queryIdx = (UINT32)divResult.rem;

            curQuery = new VulkanQuery(mDevice.getDriver(), poolInfos[poolIdx].pool, queryIdx);
            queries[i] = curQuery;

            return curQuery;
        }
        else if (!curQuery->isBound() && curQuery->mFree)
            return curQuery;
    }

    PoolInfo& poolInfo = allocatePool(type);
    UINT32 queryIdx = poolInfo.startIdx % NUM_QUERIES_PER_POOL;

    VulkanQuery* query = new VulkanQuery(mDevice.getDriver(), poolInfo.pool, queryIdx);
    queries[poolInfo.startIdx] = query;

    return query;
}

VulkanQuery* VulkanQueryPool::beginTimerQuery(VulkanCmdBuffer* cb)
{
    std::unique_lock<std::mutex> lock(mMutex);

    VulkanQuery* query = getQuery(VK_QUERY_TYPE_TIMESTAMP);
    query->mFree = false;

    VkCommandBuffer vkCmdBuf = cb->getHandle();
    cb->resetQuery(query);
    vkCmdWriteTimestamp(vkCmdBuf, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, query->mPool, query->mQueryIdx);

    // Note: Must happen only here because we need to check VulkanResource::isBound under the same mutex
    cb->registerResource(query, VulkanUseFlag::Write);

    return query;
}

VulkanQuery* VulkanQueryPool::beginOcclusionQuery(VulkanCmdBuffer* cb, bool precise)
{
    std::unique_lock<std::mutex> lock(mMutex);

    VulkanQuery* query = getQuery(VK_QUERY_TYPE_TIMESTAMP);
    query->mFree = false;

    VkCommandBuffer vkCmdBuf = cb->getHandle();
    cb->resetQuery(query);
    vkCmdBeginQuery(vkCmdBuf, query->mPool, query->mQueryIdx, precise ? VK_QUERY_CONTROL_PRECISE_BIT : 0);

    // Note: Must happen only here because we need to check VulkanResource::isBound under the same mutex
    cb->registerResource(query, VulkanUseFlag::Write);

    return query;
}

void VulkanQueryPool::endOcclusionQuery(VulkanQuery* query, VulkanCmdBuffer* cb)
{
    std::unique_lock<std::mutex> lock(mMutex);

    VkCommandBuffer vkCmdBuf = cb->getHandle();
    vkCmdEndQuery(vkCmdBuf, query->mPool, query->mQueryIdx);
}

void VulkanQueryPool::releaseQuery(VulkanQuery* query)
{
    std::unique_lock<std::mutex> lock(mMutex);

    query->mFree = true;
}