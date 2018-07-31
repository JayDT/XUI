#pragma once

#include "CVulkanUtility.h"
#include "CVulkanFramebuffer.h"

namespace irr
{
    namespace video
    {
        class VulkanImage;
        class VulkanDevice;
        class VulkanSemaphore;

        /** @addtogroup Vulkan
         *  @{
         */

         /** Description of a single swap chain surface. */
        struct SwapChainSurface
        {
            VulkanImage* image;
            VulkanSemaphore* sync;
            bool acquired;
            bool needsWait;

            VulkanFramebuffer* framebuffer;
            VULKAN_FRAMEBUFFER_DESC framebufferDesc;
        };

        /** Vulkan swap chain containing two or more buffers for rendering and presenting onto the screen. */
        class VulkanSwapChain
        {
        public:
            ~VulkanSwapChain();

            /**
             * Rebuilds the swap chain with the provided properties. Destroys any previously existing swap chain. Caller must
             * ensure the swap chain is not used at the device when this is called.
             */
            void rebuild(VulkanDevice* device, VkSurfaceKHR surface, uint32_t width, uint32_t height, bool vsync,
                VkFormat colorFormat, VkColorSpaceKHR colorSpace, bool createDepth, VkFormat depthFormat);

            /**
             * Returns the actual width of the swap chain, in pixels. This might differ from the requested size in case it
             * wasn't supported.
             */
            uint32_t getWidth() const { return mWidth; }

            /**
             * Returns the actual height of the swap chain, in pixels. This might differ from the requested size in case it
             * wasn't supported.
             */
            uint32_t getHeight() const { return mHeight; }

            /**
             * Acquires a new back buffer image. Caller can retrieve the surface by calling getBackBuffer(). Caller must wait
             * on the semaphore provided by the surface before rendering to it.
             *
             * @note Must only be called once in-between present() calls, or before the first present() call.
             */
            void acquireBackBuffer();

            /**
             * Prepares the swap chain for the present operation.
             *
             * @param[out] backBufferIdx	Index of the image representing the current back buffer.
             * @return						True if there is anything to present, false otherwise.
             */
            bool prepareForPresent(uint32_t& backBufferIdx);

            /** Notifies the chain that the semaphore waiting for the back buffer to become available is being waited on. */
            void notifyBackBufferWaitIssued();

            /** Returns information describing the current back buffer. */
            const SwapChainSurface& getBackBuffer() { return mSurfaces[mCurrentBackBufferIdx]; }

            /** Returns the number of available color surfaces. */
            uint32_t getNumColorSurfaces() const { return (uint32_t)mSurfaces.size(); }

            /** Returns the internal swap chain handle. */
            VkSwapchainKHR getHandle() const { return mSwapChain; }
        private:
            /** Destroys current swap chain and depth stencil image (if any). */
            void clear(VkSwapchainKHR swapChain);

            VulkanDevice* mDevice = nullptr;
            VkSwapchainKHR mSwapChain = VK_NULL_HANDLE;

            uint32_t mWidth = 0;
            uint32_t mHeight = 0;
            std::vector<SwapChainSurface> mSurfaces;

            VulkanImage* mDepthStencilImage = nullptr;

            uint32_t mCurrentSemaphoreIdx = 0;
            uint32_t mCurrentBackBufferIdx = 0;
        };

        /** @} */
    }
}