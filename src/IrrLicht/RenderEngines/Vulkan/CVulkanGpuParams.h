#pragma once

#include "CVulkanUtility.h"
#include "CVulkanResources.h"

namespace irr
{
    namespace video
    {
        /** @addtogroup Vulkan
         *  @{
         */

        class VulkanBuffer;
        class VulkanCmdBuffer;
        class VulkanDescriptorSet;
        class CVulkanHardwareBuffer;
        class CVulkanGLSLProgram;

         /** Vulkan implementation of GpuParams, containing resource descriptors for all shader stages. */
        class VulkanGpuParams : public CVulkanDeviceResource
        {
        public:
            VulkanGpuParams(CVulkanDriver* driver, CVulkanGLSLProgram* shader, GpuDeviceFlags deviceMask);
            virtual ~VulkanGpuParams();

            /** @copydoc GpuParams::initialize */
            void initialize();
            void reset();

            /** @copydoc GpuParams::setParamBlockBuffer(u32, u32, const ParamsBufferType&) */
            void setParamBlockBuffer(u32 set, u32 slot, const CVulkanHardwareBuffer* paramBlockBuffer);

            /** @copydoc GpuParams::setTexture */
            void setTexture(u32 set, u32 slot, const ITexture* texture, const TextureSurface& surface);

            /** @copydoc GpuParams::setLoadStoreTexture */
            void setLoadStoreTexture(u32 set, u32 slot, const ITexture* texture, const TextureSurface& surface);

            /** @copydoc GpuParams::setBuffer */
            void setBuffer(u32 set, u32 slot, const CVulkanHardwareBuffer* buffer);

            /** @copydoc GpuParams::setSamplerState */
            void setSamplerState(u32 set, u32 slot, const VkSampler& sampler);

            /** Returns the total number of descriptor sets used by this object. */
            u32 getNumSets() const;

            /**
             * Prepares the internal descriptor sets for a bind operation on the provided command buffer. It generates and/or
             * updates and descriptor sets, and registers the relevant resources with the command buffer.
             *
             * Caller must perform external locking if some other thread could write to this object while it is being bound.
             * The same applies to any resources held by this object.
             *
             * @param[in]	buffer	Buffer on which the parameters will be bound to.
             * @param[out]	sets	Pre-allocated buffer in which the descriptor set handled will be written. Must be of
             *						getNumSets() size.
             *
             * @note	Thread safe.
             */
            void UpdateDescriptors(VulkanCmdBuffer& buffer, VkDescriptorSet* sets);

            // Inherited via CVulkanDeviceResource
            virtual void OnDeviceLost(CVulkanDriver * device) override;

            virtual void OnDeviceRestored(CVulkanDriver * device) override;

        protected:
            /** Contains data about writing to either buffer or a texture descriptor. */
            union WriteInfo
            {
                VkDescriptorImageInfo image;
                VkDescriptorBufferInfo buffer;
                VkBufferView bufferView;
            };

            /** All GPU param data related to a single descriptor set. */
            struct PerSetData
            {
                VulkanDescriptorSet* latestSet = nullptr;
                std::vector<VulkanDescriptorSet*> sets;

                VkWriteDescriptorSet* writeSetInfos = nullptr;
                WriteInfo* writeInfos = nullptr;

                u32 numElements = 0;
                bool mSetsDirty = true;
            };

            /** All GPU param data beloning to a single device. */
            struct PerDeviceData
            {
                std::vector<PerSetData> perSetData;

                std::vector<VkImage> sampledImages;
                std::vector<VkImage> storageImages;
            };

            std::vector<u32> bufferBindingSlot;
            std::vector<u32> TextureBindingSlot;

            PerDeviceData mPerDeviceData[_MAX_DEVICES];
            GpuDeviceFlags mDeviceMask;

            CVulkanGLSLProgram* mShader = nullptr;

            //std::mutex mMutex;
        };

        /** @} */
    }
}