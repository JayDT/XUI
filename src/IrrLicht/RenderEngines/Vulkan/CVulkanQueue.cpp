#include "CVulkanQueue.h"
#include "CVulkanCommandBuffer.h"
#include "CVulkanSwapChain.h"

using namespace irr;
using namespace irr::video;

VulkanQueue::VulkanQueue(VulkanDevice& device, VkQueue queue, GpuQueueType type, uint32_t index)
    : mDevice(device), mQueue(queue), mType(type), mIndex(index), mLastCommandBuffer(nullptr)
    , mLastCBSemaphoreUsed(false), mNextSubmitIdx(1)
{
    for (uint32_t i = 0; i < _MAX_UNIQUE_QUEUES; i++)
        mSubmitDstWaitMask[i] = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
}

bool VulkanQueue::isExecuting() const
{
    if (mLastCommandBuffer == nullptr)
        return false;

    return mLastCommandBuffer->isSubmitted();
}

void VulkanQueue::submit(VulkanCmdBuffer* cmdBuffer, VulkanSemaphore** waitSemaphores, uint32_t semaphoresCount)
{
    VkSemaphore signalSemaphores[_MAX_VULKAN_CB_DEPENDENCIES + 1];
    cmdBuffer->allocateSemaphores(signalSemaphores);

    VkCommandBuffer vkCmdBuffer = cmdBuffer->getHandle();

    mSemaphoresTemp.resize(semaphoresCount + 1); // +1 for self semaphore
    prepareSemaphores(waitSemaphores, mSemaphoresTemp.data(), semaphoresCount);

    VkSubmitInfo submitInfo;
    getSubmitInfo(&vkCmdBuffer, signalSemaphores, _MAX_VULKAN_CB_DEPENDENCIES + 1,
        mSemaphoresTemp.data(), semaphoresCount, submitInfo);

    VkResult result = vkQueueSubmit(mQueue, 1, &submitInfo, cmdBuffer->getFence());
    assert(result == VK_SUCCESS);

    cmdBuffer->setIsSubmitted();
    mLastCommandBuffer = cmdBuffer;
    mLastCBSemaphoreUsed = false;

    mActiveSubmissions.push_back(SubmitInfo(cmdBuffer, mNextSubmitIdx++, semaphoresCount, 1));
    mActiveBuffers.push(cmdBuffer);
}

void VulkanQueue::queueSubmit(VulkanCmdBuffer* cmdBuffer, VulkanSemaphore** waitSemaphores, uint32_t semaphoresCount)
{
    mQueuedBuffers.push_back(SubmitInfo(cmdBuffer, 0, semaphoresCount, 1));

    for (uint32_t i = 0; i < semaphoresCount; i++)
        mQueuedSemaphores.push_back(waitSemaphores[i]);
}

void VulkanQueue::submitQueued()
{
    uint32_t numCBs = (uint32_t)mQueuedBuffers.size();
    if (numCBs == 0)
        return;

    uint32_t totalNumWaitSemaphores = (uint32_t)mQueuedSemaphores.size() + numCBs;
    uint32_t signalSemaphoresPerCB = (_MAX_VULKAN_CB_DEPENDENCIES + 1);

    UINT8* data = new u8[((sizeof(VkSubmitInfo) + sizeof(VkCommandBuffer)) *
        numCBs + sizeof(VkSemaphore) * signalSemaphoresPerCB * numCBs + sizeof(VkSemaphore) * totalNumWaitSemaphores)];

    UINT8* dataPtr = data;

    VkSubmitInfo* submitInfos = (VkSubmitInfo*)dataPtr;
    dataPtr += sizeof(VkSubmitInfo) * numCBs;

    VkCommandBuffer* commandBuffers = (VkCommandBuffer*)dataPtr;
    dataPtr += sizeof(VkCommandBuffer) * numCBs;

    VkSemaphore* signalSemaphores = (VkSemaphore*)dataPtr;
    dataPtr += sizeof(VkSemaphore) * signalSemaphoresPerCB * numCBs;

    VkSemaphore* waitSemaphores = (VkSemaphore*)dataPtr;
    dataPtr += sizeof(VkSemaphore) * totalNumWaitSemaphores;

    uint32_t readSemaphoreIdx = 0;
    uint32_t writeSemaphoreIdx = 0;
    uint32_t signalSemaphoreIdx = 0;
    for (uint32_t i = 0; i < numCBs; i++)
    {
        const SubmitInfo& entry = mQueuedBuffers[i];

        commandBuffers[i] = entry.cmdBuffer->getHandle();
        entry.cmdBuffer->allocateSemaphores(&signalSemaphores[signalSemaphoreIdx]);

        uint32_t semaphoresCount = entry.numSemaphores;
        prepareSemaphores(mQueuedSemaphores.data() + readSemaphoreIdx, &waitSemaphores[writeSemaphoreIdx], semaphoresCount);

        getSubmitInfo(&commandBuffers[i], &signalSemaphores[signalSemaphoreIdx], signalSemaphoresPerCB,
            &waitSemaphores[writeSemaphoreIdx], semaphoresCount, submitInfos[i]);

        entry.cmdBuffer->setIsSubmitted();
        mLastCommandBuffer = entry.cmdBuffer; // Needs to be set because getSubmitInfo depends on it
        mLastCBSemaphoreUsed = false;

        mActiveBuffers.push(entry.cmdBuffer);

        readSemaphoreIdx += entry.numSemaphores;
        writeSemaphoreIdx += semaphoresCount;
        signalSemaphoreIdx += signalSemaphoresPerCB;
    }

    VulkanCmdBuffer* lastCB = mQueuedBuffers[numCBs - 1].cmdBuffer;
    uint32_t totalNumSemaphores = writeSemaphoreIdx;
    mActiveSubmissions.push_back(SubmitInfo(lastCB, mNextSubmitIdx++, totalNumSemaphores, numCBs));

    VkResult result = vkQueueSubmit(mQueue, numCBs, submitInfos, mLastCommandBuffer->getFence());
    assert(result == VK_SUCCESS);

    mQueuedBuffers.clear();
    mQueuedSemaphores.clear();

    delete [](data);
}

void VulkanQueue::getSubmitInfo(VkCommandBuffer* cmdBuffer, VkSemaphore* signalSemaphores, uint32_t numSignalSemaphores,
    VkSemaphore* waitSemaphores, uint32_t numWaitSemaphores, VkSubmitInfo& submitInfo)
{
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = nullptr;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = cmdBuffer;
    submitInfo.signalSemaphoreCount = numSignalSemaphores;
    submitInfo.pSignalSemaphores = signalSemaphores;
    submitInfo.waitSemaphoreCount = numWaitSemaphores;

    if (numWaitSemaphores > 0)
    {
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = mSubmitDstWaitMask;
    }
    else
    {
        submitInfo.pWaitSemaphores = nullptr;
        submitInfo.pWaitDstStageMask = nullptr;
    }
}

void VulkanQueue::present(VulkanSwapChain* swapChain, VulkanSemaphore** waitSemaphores, uint32_t semaphoresCount)
{
    uint32_t backBufferIdx;
    if (!swapChain->prepareForPresent(backBufferIdx))
        return; // Nothing to present (back buffer wasn't even acquired)

    mSemaphoresTemp.resize(semaphoresCount + 1); // +1 for self semaphore
    prepareSemaphores(waitSemaphores, mSemaphoresTemp.data(), semaphoresCount);

    VkSwapchainKHR vkSwapChain = swapChain->getHandle();
    VkPresentInfoKHR presentInfo;
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext = nullptr;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &vkSwapChain;
    presentInfo.pImageIndices = &backBufferIdx;
    presentInfo.pResults = nullptr;

    // Wait before presenting, if required
    if (semaphoresCount > 0)
    {
        presentInfo.pWaitSemaphores = mSemaphoresTemp.data();
        presentInfo.waitSemaphoreCount = semaphoresCount;
    }
    else
    {
        presentInfo.pWaitSemaphores = nullptr;
        presentInfo.waitSemaphoreCount = 0;
    }

    VkResult result = vkQueuePresentKHR(mQueue, &presentInfo);
    assert(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR);

    mActiveSubmissions.push_back(SubmitInfo(nullptr, mNextSubmitIdx++, semaphoresCount, 0));
}

void VulkanQueue::waitIdle() const
{
    VkResult result = vkQueueWaitIdle(mQueue);
    assert(result == VK_SUCCESS);
}

void VulkanQueue::refreshStates(bool forceWait, bool queueEmpty)
{
    uint32_t lastFinishedSubmission = 0;

    auto iter = mActiveSubmissions.begin();
    while (iter != mActiveSubmissions.end())
    {
        VulkanCmdBuffer* cmdBuffer = iter->cmdBuffer;
        if (cmdBuffer == nullptr)
        {
            ++iter;
            continue;
        }

        if (!cmdBuffer->checkFenceStatus(forceWait))
        {
            assert(!forceWait);
            break; // No chance of any later CBs of being done either
        }

        lastFinishedSubmission = iter->submitIdx;
        ++iter;
    }

    // If last submission was a present() call, it won't be freed until a command buffer after it is done. However on
    // shutdown there might not be a CB following it. So we instead check this special flag and free everything when its
    // true.
    if (queueEmpty)
        lastFinishedSubmission = mNextSubmitIdx - 1;

    iter = mActiveSubmissions.begin();
    while (iter != mActiveSubmissions.end())
    {
        if (iter->submitIdx > lastFinishedSubmission)
            break;

        for (uint32_t i = 0; i < iter->numSemaphores; i++)
        {
            VulkanSemaphore* semaphore = mActiveSemaphores.front();
            mActiveSemaphores.pop();
        
            semaphore->notifyDone(0, VulkanUseFlag::Read | VulkanUseFlag::Write);
        }

        for (uint32_t i = 0; i < iter->numCommandBuffers; i++)
        {
            VulkanCmdBuffer* cb = mActiveBuffers.front();
            mActiveBuffers.pop();

            cb->reset();
        }

        iter = mActiveSubmissions.erase(iter);
    }
}

void VulkanQueue::prepareSemaphores(VulkanSemaphore** inSemaphores, VkSemaphore* outSemaphores, uint32_t& semaphoresCount)
{
    uint32_t semaphoreIdx = 0;
    for (uint32_t i = 0; i < semaphoresCount; i++)
    {
        VulkanSemaphore* semaphore = inSemaphores[i];

        semaphore->notifyBound();
        semaphore->notifyUsed(0, 0, VulkanUseFlag::Read | VulkanUseFlag::Write);

        outSemaphores[semaphoreIdx++] = semaphore->getHandle();
        mActiveSemaphores.push(semaphore);
    }

    // Wait on previous CB, as we want execution to proceed in order
    if (mLastCommandBuffer != nullptr && mLastCommandBuffer->isSubmitted() && !mLastCBSemaphoreUsed)
    {
        VulkanSemaphore* prevSemaphore = mLastCommandBuffer->getIntraQueueSemaphore();

        prevSemaphore->notifyBound();
        prevSemaphore->notifyUsed(0, 0, VulkanUseFlag::Read | VulkanUseFlag::Write);

        outSemaphores[semaphoreIdx++] = prevSemaphore->getHandle();
        mActiveSemaphores.push(prevSemaphore);

        // This will prevent command buffers submitted after present() to use the same semaphore. This also means that
        // there will be no intra-queue dependencies between commands for on the other ends of a present call
        // (Meaning those queue submissions could execute concurrently).
        mLastCBSemaphoreUsed = true;
    }

    semaphoresCount = semaphoreIdx;
}
