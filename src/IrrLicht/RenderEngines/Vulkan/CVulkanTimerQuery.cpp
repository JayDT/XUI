#include "IrrCompileConfig.h"

#define _IRR_DONT_DO_MEMORY_DEBUGGING_HERE

#include "CVulkanTimerQuery.h"
#include "CVulkanDriver.h"
#include "CVulkanHardwareBuffer.h"
#include "CVulkanShader.h"
#include "CVulkanDevice.h"
#include "CVulkanTexture.h"
#include "CVulkanGpuPipelineState.h"
#include "CVulkanGpuParams.h"
#include "CVulkanEventQuery.h"
#include "os.h"

using namespace irr;
using namespace video;

VulkanTimerQuery::VulkanTimerQuery(VulkanDevice& device)
    : CVulkanDeviceResource(device.getDriver()), mDevice(device), mTimeDelta(0.0f), mQueryEndCalled(false), mQueryFinalized(false), mQueryInterrupted(false)
{
}

VulkanTimerQuery::~VulkanTimerQuery()
{
    for (auto& query : mQueries)
    {
        if (query.first != nullptr)
            mDevice.getQueryPool().releaseQuery(query.first);

        if (query.second != nullptr)
            mDevice.getQueryPool().releaseQuery(query.second);
    }

    mQueries.clear();
}

void VulkanTimerQuery::begin(CommandBuffer* cb)
{
    if (!cb)
        return;

    VulkanQueryPool& queryPool = mDevice.getQueryPool();

    // Clear any existing queries
    for (auto& query : mQueries)
    {
        if (query.first != nullptr)
            queryPool.releaseQuery(query.first);

        if (query.second != nullptr)
            queryPool.releaseQuery(query.second);
    }

    mQueries.clear();

    mQueryEndCalled = false;
    mTimeDelta = 0.0f;

    // Retrieve and queue new query
    VulkanCommandBuffer* vulkanCB = static_cast<VulkanCommandBuffer*>(cb);

    VulkanCmdBuffer* internalCB = vulkanCB->getInternal();
    VulkanQuery* beginQuery = queryPool.beginTimerQuery(internalCB);
    internalCB->registerQuery(this);

    mQueries.push_back(std::make_pair(beginQuery, nullptr));

    setActive(true);
}

void VulkanTimerQuery::end(CommandBuffer* cb)
{
    if (!cb)
        return;

    assert(!mQueryInterrupted);

    if (mQueries.empty())
    {
        os::Printer::log("end() called but query was never started.");
        return;
    }

    mQueryEndCalled = true;
    mQueryFinalized = false;

    VulkanCommandBuffer* vulkanCB = static_cast<VulkanCommandBuffer*>(cb);

    VulkanQueryPool& queryPool = mDevice.getQueryPool();
    VulkanCmdBuffer* internalCB = vulkanCB->getInternal();
    VulkanQuery* endQuery = queryPool.beginTimerQuery(internalCB);
    internalCB->registerQuery(this);

    mQueries.back().second = endQuery;
}

bool VulkanTimerQuery::_isInProgress() const
{
    return !mQueries.empty() && !mQueryEndCalled;
}

void VulkanTimerQuery::_interrupt(VulkanCmdBuffer& cb)
{
    assert(mQueries.size() != 0 && !mQueryEndCalled);

    VulkanQueryPool& queryPool = mDevice.getQueryPool();
    VulkanQuery* endQuery = queryPool.beginTimerQuery(&cb);
    cb.registerQuery(this);

    mQueries.back().second = endQuery;
    mQueryInterrupted = true;
}

void VulkanTimerQuery::_resume(VulkanCmdBuffer& cb)
{
    assert(mQueryInterrupted);

    VulkanQueryPool& queryPool = mDevice.getQueryPool();
    VulkanQuery* beginQuery = queryPool.beginTimerQuery(&cb);
    cb.registerQuery(this);

    mQueries.push_back(std::make_pair(beginQuery, nullptr));
    mQueryInterrupted = false;
}

void irr::video::VulkanTimerQuery::OnDeviceLost(CVulkanDriver * device)
{
}

void irr::video::VulkanTimerQuery::OnDeviceRestored(CVulkanDriver * device)
{
}

bool VulkanTimerQuery::isReady() const
{
    if (!mQueryEndCalled)
        return false;

    if (mQueryFinalized)
        return true;

    UINT64 timeBegin, timeEnd;
    bool ready = true;
    for (auto& entry : mQueries)
    {
        ready &= !entry.first->isBound() && entry.first->getResult(timeBegin);
        ready &= !entry.second->isBound() && entry.second->getResult(timeEnd);
    }

    return ready;
}

float VulkanTimerQuery::getTimeMs()
{
    if (!mQueryFinalized)
    {
        UINT64 totalTimeDiff = 0;
        bool ready = true;
        for (auto& entry : mQueries)
        {
            UINT64 timeBegin = 0;
            UINT64 timeEnd = 0;
            ready &= !entry.first->isBound() && entry.first->getResult(timeBegin);
            ready &= !entry.second->isBound() && entry.second->getResult(timeEnd);

            totalTimeDiff += (timeEnd - timeBegin);
        }

        if (ready)
        {
            mQueryFinalized = true;

            double timestampToMs = (double)mDevice.getDeviceProperties().limits.timestampPeriod / 1e6; // Nano to milli
            mTimeDelta = (float)((double)totalTimeDiff * timestampToMs);

            VulkanQueryPool& queryPool = mDevice.getQueryPool();
            for (auto& query : mQueries)
            {
                if (query.first != nullptr)
                    queryPool.releaseQuery(query.first);

                if (query.second != nullptr)
                    queryPool.releaseQuery(query.second);
            }

            mQueries.clear();
        }
    }

    return mTimeDelta;
}
