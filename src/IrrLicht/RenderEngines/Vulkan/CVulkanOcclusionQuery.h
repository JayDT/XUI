#pragma once

#include "CVulkanUtility.h"
#include "CVulkanSharedDefines.h"
#include "CVulkanResources.h"
#include "RenderEngines/General/CIrrOcculisionQuery.h"

namespace irr
{
    namespace video
    {
        class CVulkanDriver;
        class VulkanCommandBuffer;
        class VulkanCmdBuffer;
        class VulkanQuery;

        /** @addtogroup Vulkan
     *  @{
     */

     /** @copydoc OcclusionQuery */
        class VulkanOcclusionQuery : public SOccQuery, public CVulkanDeviceResource
        {
        public:
            VulkanOcclusionQuery(VulkanDevice& device, bool binary);
            virtual ~VulkanOcclusionQuery();

            /** @copydoc OcclusionQuery::begin */
            void begin(CommandBuffer* cb) override;

            /** @copydoc OcclusionQuery::end */
            void end(CommandBuffer* cb) override;

            /** @copydoc OcclusionQuery::isReady */
            bool isReady() const override;

            /** @copydoc OcclusionQuery::getNumSamples */
            UINT32 getNumSamples() override;

            /** Returns true if the query begin() was called, but not end(). */
            bool _isInProgress() const;

            /**
             * Interrupts an in-progress query, allowing the command buffer to submitted. Interrupted queries must be resumed
             * using _resume().
             */
            void _interrupt(VulkanCmdBuffer& cb);

            /** Resumes an interrupted query, restoring it back to its original in-progress state. */
            void _resume(VulkanCmdBuffer& cb);

        private:
            friend class QueryManager;

            VulkanDevice& mDevice;
            std::vector<VulkanQuery*> mQueries;

            UINT64 mNumSamples;
            bool mQueryEndCalled : 1;
            bool mQueryFinalized : 1;
            bool mQueryInterrupted : 1;

            // Inherited via CVulkanDeviceResource
            virtual void OnDeviceLost(CVulkanDriver * device) override;
            virtual void OnDeviceRestored(CVulkanDriver * device) override;
        };

        /** @} */
    }
}