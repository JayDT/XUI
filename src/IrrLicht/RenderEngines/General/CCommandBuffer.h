#pragma once

#include "irrlicht.h"
#include "RenderEngines/General/CCommonDefines.h"

#include <memory>

namespace irr
{
    namespace video
    {
        /** @addtogroup RenderAPI
         *  @{
         */

        class CommandBuffer;

         /** Mask that determines synchronization between command buffers executing on different hardware queues. */
        class IRRLICHT_API CommandSyncMask
        {
        public:
            /**
             * Registers a dependency on a command buffer. Use getMask() to get the new mask value after registering all
             * dependencies.
             */
            void addDependency(const CommandBuffer* buffer);

            /** Returns a combined mask that contains all the required dependencies. */
            uint32_t getMask() const { return mMask; }

            /** Uses the queue type and index to generate a mask with a bit set for that queue's global index. */
            static uint32_t getGlobalQueueMask(GpuQueueType type, uint32_t queueIdx);

            /** Uses the queue type and index to generate a global queue index. */
            static uint32_t getGlobalQueueIdx(GpuQueueType type, uint32_t queueIdx);

            /** Uses the global queue index to retrieve local queue index and queue type. */
            static uint32_t getQueueIdxAndType(uint32_t globalQueueIdx, GpuQueueType& type);

        private:
            uint32_t mMask = 0;
        };

        /**
         * Contains a list of render API commands that can be queued for execution on the GPU. User is allowed to populate the
         * command buffer from any thread, ensuring render API command generation can be multi-threaded. Command buffers
         * must always be created on the core thread. Same command buffer cannot be used on multiple threads simulateously
         * without external synchronization.
         */
        class IRRLICHT_API CommandBuffer : public irr::IReferenceCountedMT
        {
        public:
            virtual ~CommandBuffer() { }

            /**
             * Creates a new CommandBuffer.
             *
             * @param[in]	type		Determines what type of commands can be added to the command buffer.
             * @param[in]	deviceIdx	Index of the GPU the command buffer will be used to queue commands on. 0 is always
             *							the primary available GPU.
             * @param[in]	queueIdx	Index of the GPU queue the command buffer will be used on. Command buffers with
             *							the same index will execute sequentially, but command buffers with different queue
             *							indices may execute in parallel, for a potential performance improvement.
             *
             *							Caller must ensure to synchronize operations executing on different queues via
             *							sync masks. Command buffer dependant on another command buffer should provide a sync
             *							mask when being submitted (see RenderAPI::executeCommands).
             *
             *							Queue indices are unique per buffer type (e.g. upload index 0 and graphics index 0 may
             *							map to different queues internally). Must be in range [0, 7].
             * @param[in]	secondary	If true the command buffer will not be allowed to execute on its own, but it can
             *							be appended to a primary command buffer.
             * @return					New CommandBuffer instance.
             */
            //static std::shared_ptr<CommandBuffer> create(GpuQueueType type, uint32_t deviceIdx = 0, uint32_t queueIdx = 0, bool secondary = false);

            /** Returns the type of queue the command buffer will execute on. */
            GpuQueueType getType() const { return mType; }

            /** Returns the index of the queue the command buffer will execute on. */
            uint32_t getQueueIdx() const { return mQueueIdx; }

            /** Returns the device index this buffer will execute on. */
            uint32_t getDeviceIdx() const { return mDeviceIdx; }

        protected:
            CommandBuffer(GpuQueueType type, uint32_t deviceIdx, uint32_t queueIdx, bool secondary);

            GpuQueueType mType;
            uint32_t mDeviceIdx;
            uint32_t mQueueIdx;
            bool mIsSecondary;
        };

        /** @} */
    }
}