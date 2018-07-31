#pragma once

#include "CVulkanUtility.h"
#include "CVulkanSharedDefines.h"
#include "CVulkanResources.h"

namespace irr
{
    namespace video
    {
        class VulkanImage;
	/** @addtogroup Vulkan
	 *  @{
	 */

	/** Represents a single attachment in a Vulkan frame-buffer. */
	struct VULKAN_ATTACHMENT_DESC
	{
		/** Image to attach or null if none. */
        VulkanImage* image = nullptr;

		/** Format of the attached image. */
		VkFormat format = VK_FORMAT_UNDEFINED;

		/** Initial layer of the surface as pointed to by the provided image view. */
		uint32_t baseLayer = 0;
	};

	/** Contains parameters used for initializing VulkanFrameBuffer object. */
	struct VULKAN_FRAMEBUFFER_DESC
	{
		/** Images describing the color attachments. */
		VULKAN_ATTACHMENT_DESC color[_MAX_MULTIPLE_RENDER_TARGETS];

		/** Image describing the depth attachment. */
		VULKAN_ATTACHMENT_DESC depth;

		/** Width of the images, in pixels. All images must be the same size. */
		uint32_t width = 0;

		/** Height of the images, in pixels. All images must be the same size. */
		uint32_t height = 0;

		/** Number of image layers to render to. This value is used for all provided surfaces. */
		uint32_t layers = 0;

		/** Number of samples in the attached images. All attachments must have the same number of samples. */
		uint32_t numSamples = 0;

		/** Set to true if framebuffer represents an offscreen surface that will not be presented. */
		bool offscreen = false;
	};

	/** Information about a single framebuffer attachment. */
	struct VulkanFramebufferAttachment
	{
        VulkanImage* image = nullptr;
		uint32_t baseLayer = 0;
		VkImageLayout finalLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		uint32_t index = 0;
	};

	/** Vulkan frame buffer containing one or multiple color surfaces, and an optional depth surface. */
	class VulkanFramebuffer : public CVulkanDeviceResource
	{
	public:
		/** Creates a new frame buffer with the specified image views attached. 
		 *
		 * @param[in]	owner	Resource manager that allocated this resource.
		 * @param[in]	desc	Description of the frame buffer.
		 */
		VulkanFramebuffer(CVulkanDriver* owner, const VULKAN_FRAMEBUFFER_DESC& desc);
		~VulkanFramebuffer();

		/** Returns a unique ID of this framebuffer. */
		uint32_t getId() const { return mId; }

		/** 
		 * Gets internal Vulkan render pass object. 
		 * 
		 * @param[in]	loadMask	Mask that control which render target surface contents should be preserved on load.
		 * @param[in]	readMask	Mask that controls which render targets can be read by shaders while they're bound.
		 * @param[in]	clearMask	Mask that controls which render targets should be cleared on render pass start. Target
		 *							cannot have both load and clear bits set. If load bit is set, clear will be ignored.
		 */
		VkRenderPass getRenderPass(RenderSurfaceMaskBits loadMask, RenderSurfaceMaskBits readMask, uint32_t clearMask) const;

		/**
		 * Gets internal Vulkan framebuffer object.
		 *
		 * @param[in]	loadMask	Mask that control which render target surface contents should be preserved on load.
		 * @param[in]	readMask	Mask that controls which render targets can be read by shaders while they're bound.
		 * @param[in]	clearMask	Mask that controls which render targets should be cleared on render pass start. Target
		 *							cannot have both load and clear bits set. If load bit is set, clear will be ignored.
		 */
		VkFramebuffer getFramebuffer(RenderSurfaceMaskBits loadMask, RenderSurfaceMaskBits readMask, uint32_t clearMask) const;

		/** 
		 * Gets the number of layers in each framebuffer surface. A layer is an element in a texture array, or a depth 
		 * slice in a 3D texture).
		 */
		uint32_t getNumLayers() const { return mNumLayers; }

		/** Returns information about a color attachment at the specified index. */
		const VulkanFramebufferAttachment& getColorAttachment(uint32_t colorIdx) const { return mColorAttachments[colorIdx]; }

		/** Returns information about a depth-stencil attachment. */
		const VulkanFramebufferAttachment& getDepthStencilAttachment() const { return mDepthStencilAttachment; }

		/** Gets the total number of frame-buffer attachments, including both color and depth. */
		uint32_t getNumAttachments() const { return mNumAttachments; }

		/** Gets the number of color frame-buffer attachments. */
		uint32_t getNumColorAttachments() const { return mNumColorAttachments; }

		/** Returns true if the framebuffer has a depth attachment. */
		bool hasDepthAttachment() const { return mHasDepth; }

		/** Returns sample flags that determine if the framebuffer supports multi-sampling, and for how many samples. */
		VkSampleCountFlagBits getSampleFlags() const { return mSampleFlags; }

		/** 
		 * Returns the maximum required number of clear entries to provide in a render pass start structure. This depends on
		 * the clear mask and the attachments on the framebuffer. 
		 */
		uint32_t getNumClearEntries(uint32_t clearMask) const;

        uint32_t Width() const { return mFramebufferCI.width; }
        uint32_t Height() const { return mFramebufferCI.height; }

    private:
		/** Information about a single frame-buffer variant. */
		struct Variant
		{
			VkRenderPass renderPass;
			VkFramebuffer framebuffer;
		};

		/** Key used for identifying different types of frame-buffer variants. */
		struct VariantKey
		{
			VariantKey(RenderSurfaceMaskBits loadMask, RenderSurfaceMaskBits readMask, uint32_t clearMask);

			class HashFunction
			{
			public:
				size_t operator()(const VariantKey& key) const;
			};

			class EqualFunction
			{
			public:
				bool operator()(const VariantKey& lhs, const VariantKey& rhs) const;
			};

            RenderSurfaceMaskBits loadMask;
            RenderSurfaceMaskBits readMask;
            uint32_t clearMask;
		};

		/** Creates a new variant of the framebuffer. */
		Variant createVariant(RenderSurfaceMaskBits loadMask, RenderSurfaceMaskBits readMask, u32 clearMask) const;

		uint32_t mId;

		Variant mDefault;
		//mutable std::unordered_map<VariantKey, Variant, VariantKey::HashFunction, VariantKey::EqualFunction> mVariants;

		uint32_t mNumAttachments;
		uint32_t mNumColorAttachments;
		uint32_t mNumLayers;
		VulkanFramebufferAttachment mColorAttachments[_MAX_MULTIPLE_RENDER_TARGETS];
		VulkanFramebufferAttachment mDepthStencilAttachment;
		bool mHasDepth;
		VkSampleCountFlagBits mSampleFlags;

		mutable VkAttachmentDescription mAttachments[_MAX_MULTIPLE_RENDER_TARGETS + 1];
		mutable VkImageView mAttachmentViews[_MAX_MULTIPLE_RENDER_TARGETS + 1];
		mutable VkAttachmentReference mColorReferences[_MAX_MULTIPLE_RENDER_TARGETS];
		mutable VkAttachmentReference mDepthReference;
		mutable VkSubpassDescription mSubpassDesc;
		mutable VkSubpassDependency mDependencies[2];
		mutable VkRenderPassCreateInfo mRenderPassCI;
		mutable VkFramebufferCreateInfo mFramebufferCI;

		static uint32_t sNextValidId;

        // Inherited via CVulkanDeviceResource
        virtual void OnDeviceLost(CVulkanDriver * device) override;
        virtual void OnDeviceRestored(CVulkanDriver * device) override;
};

	/** @} */
}}