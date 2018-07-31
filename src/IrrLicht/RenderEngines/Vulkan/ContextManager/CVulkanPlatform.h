// Copyright (C) 2013 Christian Stehno
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#ifndef __C_VK_IMANAGER_H_INCLUDED__
#define __C_VK_IMANAGER_H_INCLUDED__

#include "CVulkanSharedDefines.h"
#include "CVulkanUtility.h"

namespace irr
{
    namespace video
    {
        class CVulkanDriver;
        class VulkanSwapChain;
        class VulkanSemaphore;

        class CVulkanPlatform
        {
        public:
            CVulkanPlatform(CVulkanDriver* driver);
            virtual ~CVulkanPlatform();

            virtual void initialize() = 0;
            virtual void resizeSwapBuffers() = 0;

            /** Prepares the back buffer for rendering. Should be called before it is bound to the GPU. */
            void acquireBackBuffer();

            /** swapBuffers */
            void swapBuffers(UINT32 syncMask = 0xFFFFFFFF);

            VulkanSwapChain* GetSwapChain() { return mSwapChain; }

        protected:
            CVulkanDriver * mDriver = nullptr;

            VkSurfaceKHR mSurface;
            VkColorSpaceKHR mColorSpace;
            VkFormat mColorFormat;
            VkFormat mDepthFormat;
            uint32_t mPresentQueueFamily;
            VulkanSwapChain* mSwapChain = nullptr;
            VulkanSemaphore* mSemaphoresTemp[_MAX_UNIQUE_QUEUES + 1]; // +1 for present semaphore
            bool mRequiresNewBackBuffer = true;
            bool mShowOnSwap = false;
        };
    }
}

//        class CVulkanDriver;
//
//        struct SwapchainImageResources
//        {
//            vk::Image image;
//            vk::CommandBuffer cmd;
//            vk::CommandBuffer graphics_to_present_cmd;
//            vk::ImageView view;
//            vk::Buffer uniform_buffer;
//            vk::DeviceMemory uniform_memory;
//            vk::Framebuffer framebuffer;
//            vk::DescriptorSet descriptor_set;
//        };
//
//        struct BackBuffer
//        {
//            vk::Semaphore image_acquired_semaphores;
//            vk::Semaphore draw_complete_semaphores;
//            vk::Semaphore image_ownership_semaphores;
//            vk::Fence fences;
//        };
//
//        struct GenericVulkanContext : public IContextManager
//        {
//            virtual ~GenericVulkanContext() {}
//            virtual bool InitializeContextComponents(CVulkanDriver*);
//
//            // Initialize
//            bool initialize(const SIrrlichtCreationParameters& params, const SExposedVideoData& data) override;
//
//            //virtual bool generateSurface();
//            virtual bool generateContext() override;
//
//            bool activateContext(const SExposedVideoData& videoData) override;
//
//            // Swap buffers.
//            bool swapBuffers() override;
//
//            bool GetMemoryType(uint32_t typeBits, vk::MemoryPropertyFlags requirements_mask, uint32_t *typeIndex = nullptr);
//
//            const vk::DispatchLoaderDynamic* GetVulkanDispatcher() const { return &VulkanDispatcherExt; }
//
//            SwapchainImageResources* GetActiveContext() { return &swapchain_image_resources[frame_index]; }
//
//            /**
//            * Allocate a command buffer from the command pool
//            *
//            * @param level Level of the new command buffer (primary or secondary)
//            * @param (Optional) begin If true, recording on the new command buffer will be started (vkBeginCommandBuffer) (Defaults to false)
//            *
//            * @return A handle to the allocated command buffer
//            */
//            vk::CommandBuffer createCommandBuffer(vk::CommandBufferLevel level, bool begin = false);
//
//            /**
//            * Finish command buffer recording and submit it to a queue
//            *
//            * @param commandBuffer Command buffer to flush
//            * @param queue Queue to submit the command buffer to
//            * @param free (Optional) Free the command buffer once it has been submitted (Defaults to true)
//            *
//            * @note The queue that the command buffer is submitted to must be from the same family index as the pool it was allocated from
//            * @note Uses a fence to ensure command buffer has finished executing
//            */
//            void flushCommandBuffer(vk::CommandBuffer commandBuffer, vk::Queue queue, bool free = true);
//
//        protected:
//            virtual char ** InitEnvironment() = 0;
//            virtual bool InitializeExtensions();
//            virtual bool InitializeDevice();
//            bool InitializeBuffers();
//            bool InvalidateRenderPipeline();
//            bool InitializeRenderPipeline();
//            void InitializeDepth();
//            void SelectPhysicalDevice();
//            virtual bool InitializeDebugExtensions();
//            void CreateDevice();
//            //void InitializeTextures();
//
//            void SetSurfacePtr(VkSurfaceKHR surface);
//
//            /*
//            * Return 1 (true) if all layer names specified in check_names
//            * can be found in given layer properties.
//            */
//            static bool cvk_check_layers(uint32_t check_count, char **check_names, uint32_t layer_count, vk::LayerProperties *layers);
//
//        public:
//
//            vk::DispatchLoaderDynamic VulkanDispatcherExt;
//
//            SIrrlichtCreationParameters Params;
//            SExposedVideoData PrimaryContext;
//            ECOLOR_FORMAT ColorFormat;
//
//            CVulkanDriver* Driver;
//
//            vk::SurfaceKHR surface;
//            vk::Instance Instance;
//            vk::PhysicalDevice gpu;
//            vk::Device device;
//            vk::Queue graphics_queue;
//            vk::Queue present_queue;
//            uint32_t graphics_queue_family_index;
//            uint32_t present_queue_family_index;
//            vk::PhysicalDeviceProperties gpu_props;
//            std::unique_ptr<vk::QueueFamilyProperties[]> queue_props;
//            vk::PhysicalDeviceMemoryProperties memory_properties;
//
//            uint32_t enabled_extension_count;
//            uint32_t enabled_layer_count;
//            char const *extension_names[64];
//            char const *enabled_layers[64];
//
//            vk::Format format; // PixelFormat
//            vk::ColorSpaceKHR color_space;
//
//            uint32_t swapchainImageCount;
//            vk::SwapchainKHR swapchain;
//            std::unique_ptr<SwapchainImageResources[]> swapchain_image_resources;
//            vk::PresentModeKHR presentMode = vk::PresentModeKHR::eFifo;
//            uint32_t frame_index;
//
//            vk::CommandPool cmd_pool;
//            vk::CommandPool present_cmd_pool;
//
//            BackBuffer backBuffer[FRAME_LAG];
//
//            struct
//            {
//                vk::Format format;
//                vk::Image image;
//                vk::MemoryAllocateInfo mem_alloc;
//                vk::DeviceMemory mem;
//                vk::ImageView view;
//            } depth;
//
//            vk::DescriptorPool desc_pool;
//            vk::DescriptorSet desc_set;
//
//            std::unique_ptr<vk::Framebuffer[]> framebuffers;
//
//            bool initialized = false;
//            bool prepared = false;
//            bool use_staging_buffer = false;
//            bool separate_present_queue = false;
//            bool suppress_popups = false;
//
//            uint32_t current_buffer;
//            uint32_t queue_family_count;
//
//            vk::DebugUtilsMessengerCreateInfoEXT debug_info;
//        };
//    }
//}

#endif //!__C_VK_IMANAGER_H_INCLUDED__
