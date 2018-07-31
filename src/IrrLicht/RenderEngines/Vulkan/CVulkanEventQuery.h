#pragma once

#include "CVulkanUtility.h"
#include "CVulkanSharedDefines.h"
#include "CVulkanResources.h"
#include "RenderEngines/General/CEventQuery.h"

#include <mutex>

namespace irr
{
    namespace video
    {
        class CVulkanDriver;
        class VulkanCmdBuffer;
        class VulkanQueryPool;
        class VulkanCommandBuffer;

        /** @addtogroup Vulkan
         *  @{
         */

         /** Wrapper around a Vulkan event object that manages its usage and lifetime. */
        class VulkanEvent : public CVulkanDeviceResource
        {
        public:
            VulkanEvent(const VulkanCommandBuffer* owner);
            virtual ~VulkanEvent();

            /** Returns the internal handle to the Vulkan object. */
            VkEvent getHandle() const { return mEvent; }

            /** Checks if the event has been signaled on the device. */
            bool isSignaled() const;

            /** Resets an event back to unsignaled state, making it re-usable. */
            void reset();

            // Inherited via CVulkanDeviceResource
            virtual void OnDeviceLost(CVulkanDriver * device) override;
            virtual void OnDeviceRestored(CVulkanDriver * device) override;
        private:
            VkEvent mEvent;
        };

        /** @copydoc EventQuery */
        class VulkanEventQuery : public EventQuery
        {
        public:
            VulkanEventQuery(CVulkanDriver& device);
            virtual ~VulkanEventQuery();

            /** @copydoc EventQuery::begin */
            void begin(const CommandBuffer* cb) override;

            /** @copydoc EventQuery::isReady */
            bool isReady() const override;

        private:
            CVulkanDriver & mDevice;
            VulkanEvent* mEvent;
        };

        /** Wrapper around a single query in a Vulkan query pool object. */
        class VulkanQuery : public CVulkanDeviceResource
        {
        public:
            VulkanQuery(CVulkanDriver* owner, VkQueryPool pool, uint32_t queryIdx);
            virtual ~VulkanQuery() {}
            /**
            * Attempts to retrieve the result from the query. The result is only valid if the query stopped executing on the
            * GPU (otherwise previous query results could be accessed, if the reset command hasn't executed yet).
            *
            * @param[out]	result	Value of the query, if the method return true. Undefined otherwise.
            * @return				True if the result is ready, false otherwise.
            */
            bool getResult(UINT64& result) const;

            /** Queues a command for the query reset, on the provided command buffer. */
            void reset(VkCommandBuffer cmdBuf);

            // Inherited via CVulkanDeviceResource
            virtual void OnDeviceLost(CVulkanDriver * device) override;
            virtual void OnDeviceRestored(CVulkanDriver * device) override;

        private:
            friend class VulkanQueryPool;

            VkQueryPool mPool;
            uint32_t mQueryIdx;
            bool mFree = true;
        };

        /**
        * Pool that allocates and distributes Vulkan queries.
        *
        * @note	Thread safe.
        */
        class VulkanQueryPool
        {
        public:
            VulkanQueryPool(VulkanDevice& device);
            ~VulkanQueryPool();

            /**
            * Begins a timer query on the provided command buffer.
            *
            * @param[in]	cb			Command buffer to begin the query on.
            * @return					Relevant query object that was queued. It must be released via releaseQuery() once the
            *							caller is done accessing it.
            */
            VulkanQuery* beginTimerQuery(VulkanCmdBuffer* cb);

            /**
            * Begins an occlusion query on the provided command buffer. Must be followed with a call to endOcclusionQuery
            * on the same command buffer, before the command buffer gets submitted.
            *
            * @param[in]	cb			Command buffer to begin the query on.
            * @param[in]	precise		When true the query will be able to return the precise number of processed samples,
            *							otherwise it just returns a boolean value if anything was drawn.
            * @return					Relevant query object that was queued. It must be released via releaseQuery() once the
            *							caller is done accessing it.
            */
            VulkanQuery* beginOcclusionQuery(VulkanCmdBuffer* cb, bool precise);

            /**
            * End am occlusion query query on the provided command buffer.
            *
            * @param[in]	query		Query previously begun with beginOcclusionQuery().
            * @param[in]	cb			Command buffer to end the query on.
            */
            void endOcclusionQuery(VulkanQuery* query, VulkanCmdBuffer* cb);

            /** Releases a previously retrieved query, ensuring it can be re-used. */
            void releaseQuery(VulkanQuery* query);

        private:
            /** Query buffer pool and related information. */
            struct PoolInfo
            {
                VkQueryPool pool = VK_NULL_HANDLE;
                UINT32 startIdx;
            };

            /** Attempts to find a free query of the specified type, or allocates a new one. Creates new pools as needed. */
            VulkanQuery* getQuery(VkQueryType type);

            /** Creates a new Vulkan query pool object. */
            PoolInfo& allocatePool(VkQueryType type);

            static const UINT32 NUM_QUERIES_PER_POOL = 16;

            VulkanDevice& mDevice;

            std::vector<VulkanQuery*> mTimerQueries;
            std::vector<VulkanQuery*> mOcclusionQueries;

            std::vector<PoolInfo> mTimerPools;
            std::vector<PoolInfo> mOcclusionPools;

            std::mutex mMutex;
        };

        /** @} */
    }
}