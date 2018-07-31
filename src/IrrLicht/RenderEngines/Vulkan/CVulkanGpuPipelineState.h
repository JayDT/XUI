#pragma once

#include "CVulkanUtility.h"
#include "CVulkanSharedDefines.h"
#include "CVulkanResources.h"
#include "EPrimitiveTypes.h"

#include <map>
#include <mutex>

namespace irr
{
    namespace video
    {
        class CVulkanVertexDeclaration;
        class CVulkanGLSLProgram;
        class VulkanCmdBuffer;
        class VulkanFramebuffer;

        /** @addtogroup Vulkan
         *  @{
         */

         /** Wrapper around a Vulkan graphics pipeline that manages its usage and lifetime. */
        class VulkanPipeline : public CVulkanDeviceResource
        {
        public:
            VulkanPipeline(CVulkanDriver* owner, VkPipeline pipeline,
                const std::array<bool, _MAX_MULTIPLE_RENDER_TARGETS>& colorReadOnly, bool depthStencilReadOnly);
            VulkanPipeline(CVulkanDriver* owner, VkPipeline pipeline);
            ~VulkanPipeline();

            /** Returns the internal handle to the Vulkan object. */
            VkPipeline getHandle() const { return mPipeline; }

            /** Checks is the specified color attachment read-only. Only relevant for graphics pipelines. */
            bool isColorReadOnly(uint32_t colorIdx) const { return mReadOnlyColor[colorIdx]; }

            /** Checks is the depth attachment read-only. Only relevant for graphics pipelines. */
            bool isDepthReadOnly() const { return mReadOnlyDepth; }

            // Inherited via CVulkanDeviceResource
            virtual void OnDeviceLost(CVulkanDriver * device) override;
            virtual void OnDeviceRestored(CVulkanDriver * device) override;

        private:
            VkPipeline mPipeline;

            std::array<bool, _MAX_MULTIPLE_RENDER_TARGETS> mReadOnlyColor;
            bool mReadOnlyDepth;
        };

        /**	Vulkan implementation of a graphics pipeline state. */
        class VulkanGraphicsPipelineState
        {
        public:
            VulkanGraphicsPipelineState(const irr::video::SMaterial& desc, CVulkanGLSLProgram* shader, CVulkanVertexDeclaration* inputLayout, GpuDeviceFlags deviceMask);
            ~VulkanGraphicsPipelineState();

            /**	@copydoc GraphicsPipelineState::initialize */
            void initialize();
            void update(u8 deviceIdx, const irr::video::SMaterial& desc, scene::E_PRIMITIVE_TYPE drawOp);

            /** Checks does the pipeline enable scissor tests. */
            bool isScissorEnabled() const { return mScissorEnabled; }

            /** Returns the vertex input declaration from the vertex GPU program bound on the pipeline. */
            CVulkanVertexDeclaration* getInputDeclaration() const { return mVertexDecl; }

            /**
             * Attempts to find an existing pipeline matching the provided parameters, or creates a new one if one cannot be
             * found.
             *
             * @param[in]	deviceIdx			Index of the device to retrieve the pipeline for.
             * @param[in]	framebuffer			Framebuffer object that defines the surfaces this pipeline will render to.
             * @param[in]	readOnlyFlags		Flags that control which portion of the framebuffer is read-only. Accepts
             *									combinations of FrameBufferType enum.
             * @param[in]	drawOp				Type of geometry that will be drawn using the pipeline.
             * @param[in]	vertexInput			State describing inputs to the vertex program.
             * @return							Vulkan graphics pipeline object.
             *
             * @note	Thread safe.
             */
            VulkanPipeline* getPipeline(uint32_t deviceIdx, VulkanFramebuffer* framebuffer, uint32_t readOnlyFlags, scene::E_PRIMITIVE_TYPE drawOp, CVulkanVertexDeclaration* vertexInput);

            /**
             * Returns a pipeline layout object for the specified device index. If the device index doesn't match a bit in the
             * device mask provided on pipeline creation, null is returned.
             */
            VkPipelineLayout getPipelineLayout(uint32_t deviceIdx) const;

            /**
             * Registers any resources used by the pipeline with the provided command buffer. This should be called whenever
             * a pipeline is bound to a command buffer.
             */
            void registerPipelineResources(VulkanCmdBuffer* cmdBuffer);

            /**
             * Create a new Vulkan graphics pipeline.
             *
             * @param[in]	deviceIdx			Index of the device to create the pipeline for.
             * @param[in]	framebuffer			Framebuffer object that defines the surfaces this pipeline will render to.
             * @param[in]	readOnlyFlags		Flags that control which portion of the framebuffer is read-only. Accepts
             *									combinations of FrameBufferType enum.
             * @param[in]	drawOp				Type of geometry that will be drawn using the pipeline.
             * @param[in]	vertexInput			State describing inputs to the vertex program.
             * @return							Vulkan graphics pipeline object.
             *
             * @note	Thread safe.
             */
            VulkanPipeline* createPipeline(uint32_t deviceIdx, VulkanFramebuffer* framebuffer, uint32_t readOnlyFlags, scene::E_PRIMITIVE_TYPE drawOp);

            VkSampler getSampler(u8 deviceIdx, u8 idx) { return mPerDeviceData[deviceIdx].mSampler[idx]; }

            bool IsDirty() const { return mIsDirty; }
            scene::E_PRIMITIVE_TYPE GetDrawOp() const { return mDrawOp; }

        protected:

            /** Contains pipeline data specific to a single Vulkan device. */
            struct PerDeviceData
            {
                u64 pipelineGuid = 0;
                VulkanDevice* device = nullptr;
                VkPipelineLayout pipelineLayout = nullptr;
                VulkanPipeline* pipeline = nullptr;
                VkSampler mSampler[MATERIAL_MAX_TEXTURES];
                std::map<u64, VulkanPipeline*> pipelines;
            };

            VkPipelineShaderStageCreateInfo mShaderStageInfos[5];
            VkPipelineInputAssemblyStateCreateInfo mInputAssemblyInfo;
            VkPipelineTessellationStateCreateInfo mTesselationInfo;
            VkPipelineViewportStateCreateInfo mViewportInfo;
            VkPipelineRasterizationStateCreateInfo mRasterizationInfo;
            VkPipelineMultisampleStateCreateInfo mMultiSampleInfo;
            VkPipelineDepthStencilStateCreateInfo mDepthStencilInfo;
            VkPipelineColorBlendAttachmentState mAttachmentBlendStates[_MAX_MULTIPLE_RENDER_TARGETS];
            VkPipelineColorBlendStateCreateInfo mColorBlendStateInfo;
            VkPipelineDynamicStateCreateInfo mDynamicStateInfo;
            VkDynamicState mDynamicStates[3];
            VkGraphicsPipelineCreateInfo mPipelineInfo;
            VkSamplerCreateInfo mSamplerInfo[MATERIAL_MAX_TEXTURES];
            CVulkanVertexDeclaration* mVertexDecl;
            CVulkanGLSLProgram* mShader;
            irr::video::SMaterial mMaterial;
            u64 mRequestPipelineGuid = 0;

            GpuDeviceFlags mDeviceMask;
            PerDeviceData mPerDeviceData[_MAX_DEVICES];
            scene::E_PRIMITIVE_TYPE mDrawOp;

            bool mScissorEnabled;
            bool mIsDirty;

            //std::mutex mMutex;
        };

        /**	Vulkan implementation of a compute pipeline state. */
        //class VulkanComputePipelineState
        //{
        //public:
        //    ~VulkanComputePipelineState();
        //
        //    /**
        //     * Returns a pipeline object for the specified device index. If the device index doesn't match a bit in the
        //     * device mask provided on pipeline creation, null is returned.
        //     */
        //    VulkanPipeline* getPipeline(uint32_t deviceIdx) const;
        //
        //    /**
        //     * Returns a pipeline layout object for the specified device index. If the device index doesn't match a bit in the
        //     * device mask provided on pipeline creation, null is returned.
        //     */
        //    VkPipelineLayout getPipelineLayout(uint32_t deviceIdx) const;
        //
        //    /**
        //     * Registers any resources used by the pipeline with the provided command buffer. This should be called whenever
        //     * a pipeline is bound to a command buffer.
        //     */
        //    void registerPipelineResources(VulkanCmdBuffer* cmdBuffer);
        //
        //protected:
        //    friend class VulkanRenderStateManager;
        //
        //    VulkanComputePipelineState(CVulkanGLSLProgram* program, GpuDeviceFlags deviceMask);
        //
        //    /**	@copydoc ComputePipelineState::initialize */
        //    void initialize();
        //
        //    /** Contains pipeline data specific to a single Vulkan device. */
        //    struct PerDeviceData
        //    {
        //        CVulkanDriver* device;
        //        VulkanPipeline* pipeline;
        //        VkPipelineLayout pipelineLayout;
        //    };
        //
        //    GpuDeviceFlags mDeviceMask;
        //    PerDeviceData mPerDeviceData[_MAX_DEVICES];
        //};

        /** @} */
    }
}