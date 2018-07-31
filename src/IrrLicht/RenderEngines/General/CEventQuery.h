#pragma once

#include "irrlicht.h"
#include "standard/events.h"

namespace irr
{
    namespace video
    {
        class CommandBuffer;

        /** @addtogroup RenderAPI-Internal
         *  @{
         */

         /**
          * Represents a GPU query that gets triggered when GPU starts processing the query.
          *
          * @note
          * Normally GPU will have many commands in its command buffer. When begin() is called it is placed in that command
          * buffer. Once the buffer empties and GPU reaches the EventQuery command, the query callback is triggered.
          * @note
          * Core thread only.
          */
        class IRRLICHT_API EventQuery
        {
        public:
            EventQuery()
                :mActive(false) {}
            virtual ~EventQuery() {}

            /**
             * Starts the query.
             *
             * @param[in]	cb		Optional command buffer to queue the operation on. If not provided operation
             *						is executed on the main command buffer. Otherwise it is executed when
             *						RenderAPI::executeCommands() is called. Buffer must support graphics or compute operations.
             *
             * @note
             * Once the query is started you may poll isReady() method to check when query has finished, or you may hook up
             * an #onTriggered callback and be notified that way.
             */
            virtual void begin(const CommandBuffer* cb = nullptr) = 0;

            /** Check if GPU has processed the query. */
            virtual bool isReady() const = 0;

            /**	Triggered when GPU starts processing the query. */
            System::Async::InvokeHandler onTriggered;

        protected:
            friend class QueryManager;

            /**	Returns true if the has still not been completed by the GPU. */
            bool isActive() const { return mActive; }
            void setActive(bool active) { mActive = active; }

        protected:
            bool mActive;
        };

        /** @} */
    }
}