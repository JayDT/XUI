#ifndef __C_CVulkan_RESOURCE_H_INCLUDED__
#define __C_CVulkan_RESOURCE_H_INCLUDED__

#include "IReferenceCounted.h"
#include "CVulkanSharedDefines.h"

#include <mutex>

namespace irr
{
    namespace video
    {
        class CVulkanDriver;

        struct CVulkanDeviceResource : irr::IReferenceCountedMT
        {
            CVulkanDeviceResource(CVulkanDriver* driver);

            virtual ~CVulkanDeviceResource();

            virtual void OnDeviceLost(CVulkanDriver* device) = 0;
            virtual void OnDeviceRestored(CVulkanDriver* device) = 0;

            CVulkanDriver* GetDriver() const { return Driver; }


            /**
            * Notifies the resource that it is currently bound to a command buffer. Buffer hasn't yet been submitted so the
            * resource isn't being used on the GPU yet.
            *
            * Must eventually be followed by a notifyUsed() or notifyUnbound().
            */
            void notifyBound();

            /**
            * Notifies the resource that it is currently being used on the provided command buffer. This means the command
            * buffer has actually been submitted to the queue and the resource is used by the GPU.
            *
            * A resource can only be used by a single command buffer at a time unless resource concurrency is enabled.
            *
            * Must follow a notifyBound(). Must eventually be followed by a notifyDone().
            *
            * @param[in]	globalQueueIdx	Global index of the queue the resource is being used in.
            * @param[in]	queueFamily		Family of the queue the resource is being used in.
            * @param[in]	useFlags		Flags that determine in what way is the resource being used.
            */
            void notifyUsed(u32 globalQueueIdx, u32 queueFamily, VulkanUseFlag useFlags);

            /**
            * Notifies the resource that it is no longer used by on the GPU. This makes the resource usable on other command
            * buffers again.
            *
            * Must follow a notifyUsed().
            *
            * @param[in]	globalQueueIdx	Global index of the queue that finished using the resource.
            * @param[in]	useFlags		Use flags that specify how was the resource being used.
            */
            void notifyDone(u32 globalQueueIdx, VulkanUseFlag useFlags);

            /**
            * Notifies the resource that it is no longer queued on the command buffer. This is similar to notifyDone(), but
            * should only be called if resource never got submitted to the GPU (e.g. command buffer was destroyed before
            * being submitted).
            *
            * Must follow a notifyBound() if notifyUsed() wasn't called.
            */
            void notifyUnbound();

            /**
            * Checks is the resource currently used on a device.
            *
            * @note	Resource usage is only checked at certain points of the program. This means the resource could be
            *			done on the device but this method may still report true. If you need to know the latest state
            *			call VulkanCommandBufferManager::refreshStates() before checking for usage.
            */
            bool isUsed() const { /*std::unique_lock<std::mutex> lock(mMutex);*/ return mNumUsedHandles > 0; }

            /**
            * Checks is the resource currently bound to any command buffer.
            *
            * @note	Resource usage is only checked at certain points of the program. This means the resource could be
            *			done on the device but this method may still report true. If you need to know the latest state
            *			call VulkanCommandBufferManager::refreshStates() before checking for usage.
            */
            bool isBound() const { /*std::unique_lock<std::mutex> lock(mMutex);*/ return mNumBoundHandles > 0; }

            /**
            * Returns the queue family the resource is currently owned by. Returns -1 if owned by no queue.
            *
            * @note	If resource concurrency is enabled, then this value has no meaning as the resource can be used on
            *			multiple queue families at once.
            */
            u32 getQueueFamily() const { /*std::unique_lock<std::mutex> lock(mMutex);*/ return mQueueFamily; }

            /**
            * Returns a mask that has bits set for every queue that the resource is currently used (read or written) by.
            *
            * @param[in]	useFlags	Flags for which to check use information (e.g. read only, write only, or both).
            * @return					Bitmask of which queues is the resource used on. This has the same format as sync mask
            *							created by CommandSyncMask.
            */
            u32 getUseInfo(VulkanUseFlag useFlags) const;

            /** Returns on how many command buffers is the buffer currently used on. */
            u32 getUseCount() const { return mNumUsedHandles; }

            /** Returns on how many command buffers is the buffer currently bound on. */
            u32 getBoundCount() const { return mNumBoundHandles; }

            /** Returns true if the resource is only allowed to be used by a single queue family at once. */
            bool isExclusive() const { /*std::unique_lock<std::mutex> lock(mMutex);*/ return mState != State::Shared; }

#ifdef _DEBUG
            void SetDebugName(const char* n) { mDebugName = n; }
#endif

        protected:
            /** Possible states of this object. */
            enum class State
            {
                Normal,
                Shared,
                Destroyed
            };

            static const u32 MAX_UNIQUE_QUEUES = _MAX_QUEUES_PER_TYPE * GQT_COUNT;

            CVulkanDriver* Driver;
            u32 mQueueFamily;
            State mState;

            u8 mReadUses[MAX_UNIQUE_QUEUES];
            u8 mWriteUses[MAX_UNIQUE_QUEUES];

            u32 mNumUsedHandles;
            u32 mNumBoundHandles;

#ifdef _DEBUG
            const char* mDebugName;
#endif

            //mutable std::mutex mMutex;
        };
    }
}

#endif //!__C_CVulkan_RESOURCE_H_INCLUDED__