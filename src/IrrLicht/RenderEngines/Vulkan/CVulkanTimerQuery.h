//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "CVulkanUtility.h"
#include "CVulkanSharedDefines.h"
#include "CVulkanResources.h"
#include "RenderEngines/General/CIrrTimerQuery.h"

namespace irr
{
    namespace video
    {
        class CommandBuffer;
        class CVulkanDriver;
        class VulkanCommandBuffer;
        class VulkanCmdBuffer;
        class VulkanQuery;

        /** @addtogroup Vulkan
         *  @{
         */

         /** @copydoc TimerQuery */
        class VulkanTimerQuery : public TimerQuery, public CVulkanDeviceResource
        {
        public:
            VulkanTimerQuery(VulkanDevice& device);
            virtual ~VulkanTimerQuery();

            /** @copydoc TimerQuery::begin */
            void begin(CommandBuffer* cb) override;

            /** @copydoc TimerQuery::end */
            void end(CommandBuffer* cb) override;

            /** @copydoc TimerQuery::isReady */
            bool isReady() const override;

            /** @copydoc TimerQuery::getTimeMs */
            float getTimeMs() override;

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
            VulkanDevice & mDevice;
            std::vector<std::pair<VulkanQuery*, VulkanQuery*>> mQueries;

            float mTimeDelta;
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