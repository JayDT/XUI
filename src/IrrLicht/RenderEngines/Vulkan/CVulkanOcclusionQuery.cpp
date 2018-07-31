#include "IrrCompileConfig.h"

#define _IRR_DONT_DO_MEMORY_DEBUGGING_HERE

#include "CVulkanOcclusionQuery.h"
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

VulkanOcclusionQuery::VulkanOcclusionQuery(VulkanDevice& device, bool binary)
    : SOccQuery(binary), CVulkanDeviceResource(device.getDriver()), mDevice(device), mNumSamples(0), mQueryEndCalled(false), mQueryFinalized(false)
    , mQueryInterrupted(false)
{
}

VulkanOcclusionQuery::~VulkanOcclusionQuery()
{
    for (auto& query : mQueries)
        mDevice.getQueryPool().releaseQuery(query);

    mQueries.clear();
}

void VulkanOcclusionQuery::begin(CommandBuffer* cb)
{
    if (!cb)
        return;

    VulkanQueryPool& queryPool = mDevice.getQueryPool();

    // Clear any existing queries
    for (auto& query : mQueries)
        mDevice.getQueryPool().releaseQuery(query);

    mQueries.clear();

    mQueryEndCalled = false;
    mNumSamples = 0;

    // Retrieve and queue new query
    VulkanCommandBuffer* vulkanCB =  static_cast<VulkanCommandBuffer*>(cb);

    VulkanCmdBuffer* internalCB = vulkanCB->getInternal();
    mQueries.push_back(queryPool.beginOcclusionQuery(internalCB, !mBinary));
    internalCB->registerQuery(this);

    setActive(true);
}

void VulkanOcclusionQuery::end(CommandBuffer* cb)
{
    if (!cb)
        return;

    assert(!mQueryInterrupted);

    if (mQueries.size() == 0)
    {
        os::Printer::log("end() called but query was never started.");
        return;
    }

    mQueryEndCalled = true;
    mQueryFinalized = false;

    VulkanCommandBuffer* vulkanCB = static_cast<VulkanCommandBuffer*>(cb);

    VulkanQueryPool& queryPool = mDevice.getQueryPool();
    VulkanCmdBuffer* internalCB = vulkanCB->getInternal();
    queryPool.endOcclusionQuery(mQueries.back(), internalCB);
}

bool VulkanOcclusionQuery::_isInProgress() const
{
    return !mQueries.empty() && !mQueryEndCalled;
}

void VulkanOcclusionQuery::_interrupt(VulkanCmdBuffer& cb)
{
    assert(mQueries.size() != 0 && !mQueryEndCalled);

    VulkanQueryPool& queryPool = mDevice.getQueryPool();
    queryPool.endOcclusionQuery(mQueries.back(), &cb);

    mQueryInterrupted = true;
}

void VulkanOcclusionQuery::_resume(VulkanCmdBuffer& cb)
{
    assert(mQueryInterrupted);

    VulkanQueryPool& queryPool = mDevice.getQueryPool();
    mQueries.push_back(queryPool.beginOcclusionQuery(&cb, !mBinary));
    cb.registerQuery(this);

    mQueryInterrupted = false;
}

void irr::video::VulkanOcclusionQuery::OnDeviceLost(CVulkanDriver * device)
{
}

void irr::video::VulkanOcclusionQuery::OnDeviceRestored(CVulkanDriver * device)
{
}

bool VulkanOcclusionQuery::isReady() const
{
    if (!mQueryEndCalled)
        return false;

    if (mQueryFinalized)
        return true;

    UINT64 numSamples;
    bool ready = true;
    for (auto& query : mQueries)
        ready &= !query->isBound() && query->getResult(numSamples);

    return ready;
}

UINT32 VulkanOcclusionQuery::getNumSamples()
{
    if (!mQueryFinalized)
    {
        UINT64 totalNumSamples = 0;
        bool ready = true;
        for (auto& query : mQueries)
        {
            UINT64 numSamples = 0;
            ready &= !query->isBound() && query->getResult(numSamples);

            totalNumSamples += numSamples;
        }

        if (ready)
        {
            mQueryFinalized = true;
            mNumSamples = totalNumSamples;

            VulkanQueryPool& queryPool = mDevice.getQueryPool();
            for (auto& query : mQueries)
                queryPool.releaseQuery(query);

            mQueries.clear();
        }
    }

    if (mBinary)
        return mNumSamples == 0 ? 0 : 1;

    return (UINT32)mNumSamples;
}
