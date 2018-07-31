#include "CVulkanUtility.h"
#include "CVulkanDriver.h"
#include "CVulkanDevice.h"

#include "os.h"

using namespace irr;
using namespace irr::video;

static inline const char* string_VkObjectType(VkObjectType input_value)
{
    switch ((VkObjectType)input_value)
    {
        case VK_OBJECT_TYPE_QUERY_POOL:
            return "VK_OBJECT_TYPE_QUERY_POOL";
        case VK_OBJECT_TYPE_OBJECT_TABLE_NVX:
            return "VK_OBJECT_TYPE_OBJECT_TABLE_NVX";
        case VK_OBJECT_TYPE_SAMPLER_YCBCR_CONVERSION:
            return "VK_OBJECT_TYPE_SAMPLER_YCBCR_CONVERSION";
        case VK_OBJECT_TYPE_SEMAPHORE:
            return "VK_OBJECT_TYPE_SEMAPHORE";
        case VK_OBJECT_TYPE_SHADER_MODULE:
            return "VK_OBJECT_TYPE_SHADER_MODULE";
        case VK_OBJECT_TYPE_SWAPCHAIN_KHR:
            return "VK_OBJECT_TYPE_SWAPCHAIN_KHR";
        case VK_OBJECT_TYPE_SAMPLER:
            return "VK_OBJECT_TYPE_SAMPLER";
        case VK_OBJECT_TYPE_INDIRECT_COMMANDS_LAYOUT_NVX:
            return "VK_OBJECT_TYPE_INDIRECT_COMMANDS_LAYOUT_NVX";
        case VK_OBJECT_TYPE_DEBUG_REPORT_CALLBACK_EXT:
            return "VK_OBJECT_TYPE_DEBUG_REPORT_CALLBACK_EXT";
        case VK_OBJECT_TYPE_IMAGE:
            return "VK_OBJECT_TYPE_IMAGE";
        case VK_OBJECT_TYPE_UNKNOWN:
            return "VK_OBJECT_TYPE_UNKNOWN";
        case VK_OBJECT_TYPE_DESCRIPTOR_POOL:
            return "VK_OBJECT_TYPE_DESCRIPTOR_POOL";
        case VK_OBJECT_TYPE_COMMAND_BUFFER:
            return "VK_OBJECT_TYPE_COMMAND_BUFFER";
        case VK_OBJECT_TYPE_BUFFER:
            return "VK_OBJECT_TYPE_BUFFER";
        case VK_OBJECT_TYPE_SURFACE_KHR:
            return "VK_OBJECT_TYPE_SURFACE_KHR";
        case VK_OBJECT_TYPE_INSTANCE:
            return "VK_OBJECT_TYPE_INSTANCE";
        case VK_OBJECT_TYPE_VALIDATION_CACHE_EXT:
            return "VK_OBJECT_TYPE_VALIDATION_CACHE_EXT";
        case VK_OBJECT_TYPE_IMAGE_VIEW:
            return "VK_OBJECT_TYPE_IMAGE_VIEW";
        case VK_OBJECT_TYPE_DESCRIPTOR_SET:
            return "VK_OBJECT_TYPE_DESCRIPTOR_SET";
        case VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT:
            return "VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT";
        case VK_OBJECT_TYPE_COMMAND_POOL:
            return "VK_OBJECT_TYPE_COMMAND_POOL";
        case VK_OBJECT_TYPE_PHYSICAL_DEVICE:
            return "VK_OBJECT_TYPE_PHYSICAL_DEVICE";
        case VK_OBJECT_TYPE_DISPLAY_KHR:
            return "VK_OBJECT_TYPE_DISPLAY_KHR";
        case VK_OBJECT_TYPE_BUFFER_VIEW:
            return "VK_OBJECT_TYPE_BUFFER_VIEW";
        case VK_OBJECT_TYPE_DEBUG_UTILS_MESSENGER_EXT:
            return "VK_OBJECT_TYPE_DEBUG_UTILS_MESSENGER_EXT";
        case VK_OBJECT_TYPE_FRAMEBUFFER:
            return "VK_OBJECT_TYPE_FRAMEBUFFER";
        case VK_OBJECT_TYPE_DESCRIPTOR_UPDATE_TEMPLATE:
            return "VK_OBJECT_TYPE_DESCRIPTOR_UPDATE_TEMPLATE";
        case VK_OBJECT_TYPE_PIPELINE_CACHE:
            return "VK_OBJECT_TYPE_PIPELINE_CACHE";
        case VK_OBJECT_TYPE_PIPELINE_LAYOUT:
            return "VK_OBJECT_TYPE_PIPELINE_LAYOUT";
        case VK_OBJECT_TYPE_DEVICE_MEMORY:
            return "VK_OBJECT_TYPE_DEVICE_MEMORY";
        case VK_OBJECT_TYPE_FENCE:
            return "VK_OBJECT_TYPE_FENCE";
        case VK_OBJECT_TYPE_QUEUE:
            return "VK_OBJECT_TYPE_QUEUE";
        case VK_OBJECT_TYPE_DEVICE:
            return "VK_OBJECT_TYPE_DEVICE";
        case VK_OBJECT_TYPE_RENDER_PASS:
            return "VK_OBJECT_TYPE_RENDER_PASS";
        case VK_OBJECT_TYPE_DISPLAY_MODE_KHR:
            return "VK_OBJECT_TYPE_DISPLAY_MODE_KHR";
        case VK_OBJECT_TYPE_EVENT:
            return "VK_OBJECT_TYPE_EVENT";
        case VK_OBJECT_TYPE_PIPELINE:
            return "VK_OBJECT_TYPE_PIPELINE";
        default:
            return "Unhandled VkObjectType";
    }
}

static bool memory_type_from_properties(vk::PhysicalDeviceMemoryProperties const& memory_properties, uint32_t typeBits, vk::MemoryPropertyFlags requirements_mask, uint32_t *typeIndex)
{
    // Search memtypes to find first index with those properties
    for (uint32_t i = 0; i < VK_MAX_MEMORY_TYPES; i++)
    {
        if ((typeBits & 1) == 1)
        {
            // Type is available, does it match user properties?
            if ((memory_properties.memoryTypes[i].propertyFlags & requirements_mask) == requirements_mask)
            {
                *typeIndex = i;
                return true;
            }
        }
        typeBits >>= 1;
    }
    // No memory types matched, return failure
    return false;
}

VKAPI_ATTR VkBool32 debugMsgCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t srcObject,
    size_t location, int32_t msgCode, const char* pLayerPrefix, const char* pMsg, void* pUserData)
{
    std::stringstream message;

    // Determine prefix
    if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
        message << "ERROR";

    if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
        message << "WARNING";

    if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
        message << "PERFORMANCE";

    if (flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT)
        message << "INFO";

    if (flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT)
        message << "DEBUG";

    message << ": [" << pLayerPrefix << "] Code " << msgCode << ": " << pMsg << std::endl;

    if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
    {
        os::Printer::log(message.str().c_str());
        //EXCEPT(false)
    }
    else if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT || flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
        os::Printer::log(message.str().c_str());
    else
        os::Printer::log(message.str().c_str());

    // Don't abort calls that caused a validation message
    return VK_FALSE;
}


ECOLOR_FORMAT VulkanUtility::getClosestSupportedPixelFormat(VulkanDevice& device, ECOLOR_FORMAT format, TextureType texType, int usage, bool optimalTiling, bool hwGamma)
{
    // Check for any obvious issues first
    ITexture::checkFormat(format, texType, usage);

    // Check actual device for format support
    VkFormatFeatureFlags wantedFeatureFlags = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;
    if ((usage & ETCF_USAGE_RENDERTARGET) != 0)
        wantedFeatureFlags |= VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;

    if ((usage & ETCF_USAGE_DEPTHSTENCIL) != 0)
        wantedFeatureFlags |= VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;

    if ((usage & ETCF_USAGE_LOADSTORE) != 0)
        wantedFeatureFlags |= VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;

    VkFormatProperties props;
    auto isSupported = [&](VkFormat vkFmt)
    {
        vkGetPhysicalDeviceFormatProperties(device.getPhysical(), vkFmt, &props);
        VkFormatFeatureFlags featureFlags = optimalTiling ? props.optimalTilingFeatures : props.linearTilingFeatures;

        return (featureFlags & wantedFeatureFlags) != 0;
    };

    VkFormat vkFormat = getPixelFormat(format, hwGamma);
    if (!isSupported(vkFormat))
    {
        format = ECOLOR_FORMAT::ECF_RGBA8;

        //if ((usage & ETCF_USAGE_DEPTHSTENCIL) != 0)
        //{
        //    bool hasStencil = format == PF_D24S8 || format == PF_D32_S8X24;
        //
        //    // Spec guarantees at least one depth-only, and one depth-stencil format to be supported
        //    if (hasStencil)
        //    {
        //        if (isSupported(VK_FORMAT_D32_SFLOAT_S8_UINT))
        //            format = PF_D32_S8X24;
        //        else
        //            format = PF_D24S8;
        //
        //        // We ignore 8-bit stencil-only, and 16/8 depth/stencil combo buffers as engine doesn't expose them,
        //        // and spec guarantees one of the above must be implemented.
        //    }
        //    else
        //    {
        //        // The only format that could have failed is 32-bit depth, so we must use the alternative 16-bit.
        //        // Spec guarantees it is always supported.
        //        format = PF_D16;
        //    }
        //}
        //else
        //{
        //    int bitDepths[4];
        //    PixelUtil::getBitDepths(format, bitDepths);
        //
        //    if (bitDepths[0] == 16) // 16-bit format, fall back to 4-channel 16-bit, guaranteed to be supported
        //        format = PF_RGBA16F;
        //    else if (format == PF_BC6H) // Fall back to uncompressed alternative
        //        format = PF_RGBA16F;
        //    else // Must be 8-bit per channel format, compressed format or some uneven format
        //        format = PF_RGBA8;
        //}
    }

    return format;
}

VkFormat VulkanUtility::getPixelFormat(ECOLOR_FORMAT format, bool sRGB)
{
    switch (format)
    {
        case ECF_DXT1:
            if (sRGB)
                return VK_FORMAT_BC1_RGB_SRGB_BLOCK;

            return VK_FORMAT_BC1_RGB_UNORM_BLOCK;

        case ECF_DXT3:
            if (sRGB)
                return VK_FORMAT_BC2_SRGB_BLOCK;

            return VK_FORMAT_BC2_UNORM_BLOCK;

        case ECF_DXT5:
            if (sRGB)
                return VK_FORMAT_BC3_SRGB_BLOCK;

            return VK_FORMAT_BC3_UNORM_BLOCK;

        case ECF_A1R5G5B5:
            return VK_FORMAT_A1R5G5B5_UNORM_PACK16;

        case ECF_R5G6B5:
            return VK_FORMAT_R5G6B5_UNORM_PACK16;

        case ECF_R8G8B8:
            return VK_FORMAT_R8G8B8_UNORM;

        case ECF_RGBA8:
        case ECF_A8R8G8B8:
            return VK_FORMAT_A8B8G8R8_UNORM_PACK32;

        case ECF_R16F:
            return VK_FORMAT_R16_UNORM;

        case ECF_G16R16F:
            return VK_FORMAT_R16_SFLOAT;

        case ECF_A16B16G16R16F:
            return VK_FORMAT_R16G16B16A16_SFLOAT;

        case ECF_R32F:
            return VK_FORMAT_R32_SFLOAT;

        case ECF_G32R32F:
            return VK_FORMAT_R32G32_SFLOAT;

        case ECF_A32B32G32R32F:
            return VK_FORMAT_R32G32B32A32_SFLOAT;
    }

    return VK_FORMAT_UNDEFINED;
}

ECOLOR_FORMAT irr::video::VulkanUtility::getPixelFormat(VkFormat format)
{
    switch (format)
    {
        case VK_FORMAT_BC1_RGB_SRGB_BLOCK:
        case VK_FORMAT_BC1_RGB_UNORM_BLOCK:
            return ECF_DXT1;

        case VK_FORMAT_BC2_SRGB_BLOCK:
        case VK_FORMAT_BC2_UNORM_BLOCK:
            return ECF_DXT3;

        case VK_FORMAT_BC3_SRGB_BLOCK:
        case VK_FORMAT_BC3_UNORM_BLOCK:
            return ECF_DXT5;

        case VK_FORMAT_A1R5G5B5_UNORM_PACK16:
            return ECF_A1R5G5B5;

        case VK_FORMAT_R5G6B5_UNORM_PACK16:
            return ECF_R5G6B5;

        case VK_FORMAT_R8G8B8_UNORM:
            return ECF_R8G8B8;

        case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
            return ECF_A8R8G8B8;

        case VK_FORMAT_R16_UNORM:
            return ECF_R16F;

        case VK_FORMAT_R16_SFLOAT:
            return ECF_G16R16F;

        case VK_FORMAT_R16G16B16A16_SFLOAT:
            return ECF_A16B16G16R16F;

        case VK_FORMAT_R32_SFLOAT:
            return ECF_R32F;

        case VK_FORMAT_R32G32_SFLOAT:
            return ECF_G32R32F;

        case ECF_A32B32G32R32F:
            return ECF_A32B32G32R32F;
    }

    return ECOLOR_FORMAT();
}

u32 irr::video::VulkanUtility::getIndexAmount(scene::E_PRIMITIVE_TYPE primType, u32 primitiveCount)
{
    switch (primType)
    {
        case scene::EPT_LINE_STRIP:
            return primitiveCount * 2;

        case scene::EPT_LINE_LOOP:
        case scene::EPT_LINES:
            return primitiveCount * 2;

        case scene::EPT_TRIANGLE_STRIP:
            return primitiveCount + 2;

        case scene::EPT_TRIANGLES:
            return primitiveCount * 3;

        default:
            return 0;
    }
}

VkSampleCountFlagBits VulkanUtility::getSampleFlags(uint32_t numSamples)
{
    switch (numSamples)
    {
        case 0:
        case 1:
            return VK_SAMPLE_COUNT_1_BIT;
        case 2:
            return VK_SAMPLE_COUNT_2_BIT;
        case 4:
            return VK_SAMPLE_COUNT_4_BIT;
        case 8:
            return VK_SAMPLE_COUNT_8_BIT;
        case 16:
            return VK_SAMPLE_COUNT_16_BIT;
        case 32:
            return VK_SAMPLE_COUNT_32_BIT;
        case 64:
            return VK_SAMPLE_COUNT_64_BIT;
    }

    return VK_SAMPLE_COUNT_1_BIT;
}

VkSamplerAddressMode VulkanUtility::getAddressingMode(E_TEXTURE_CLAMP mode)
{
    switch (mode)
    {
        case ETC_REPEAT:
            return VK_SAMPLER_ADDRESS_MODE_REPEAT;
        case ETC_MIRROR:
            return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
        case ETC_CLAMP:
        case ETC_CLAMP_TO_EDGE:
            return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        case ETC_CLAMP_TO_BORDER:
            return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        case ETC_MIRROR_CLAMP:
        case ETC_MIRROR_CLAMP_TO_EDGE:
        case ETC_MIRROR_CLAMP_TO_BORDER:
            return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
    }

    return VK_SAMPLER_ADDRESS_MODE_REPEAT;
}

VkBlendFactor VulkanUtility::getBlendFactor(E_BLEND_FACTOR factor)
{
    switch (factor)
    {
        case EBF_ONE:
            return VK_BLEND_FACTOR_ONE;
        case EBF_ZERO:
            return VK_BLEND_FACTOR_ZERO;
        case EBF_DST_COLOR:
            return VK_BLEND_FACTOR_DST_COLOR;
        case EBF_SRC_COLOR:
            return VK_BLEND_FACTOR_SRC_COLOR;
        case EBF_ONE_MINUS_DST_COLOR:
            return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
        case EBF_ONE_MINUS_SRC_COLOR:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
        case EBF_DST_ALPHA:
            return VK_BLEND_FACTOR_DST_ALPHA;
        case EBF_SRC_ALPHA:
            return VK_BLEND_FACTOR_SRC_ALPHA;
        case EBF_ONE_MINUS_DST_ALPHA:
            return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
        case EBF_ONE_MINUS_SRC_ALPHA:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        case EBF_SRC_ALPHA_SATURATE:
            return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
    }

    // Unsupported type
    return VK_BLEND_FACTOR_ZERO;
}

VkBlendOp VulkanUtility::getBlendOp(E_BLEND_OPERATION op)
{
    switch (op)
    {
        case EBO_ADD:
            return VK_BLEND_OP_ADD;
        case EBO_SUBTRACT:
            return VK_BLEND_OP_SUBTRACT;
        case EBO_REVSUBTRACT:
            return VK_BLEND_OP_REVERSE_SUBTRACT;
        case EBO_MIN:
            return VK_BLEND_OP_MIN;
        case EBO_MAX:
            return VK_BLEND_OP_MAX;
    }

    // Unsupported type
    return VK_BLEND_OP_ADD;
}

VkCompareOp VulkanUtility::getCompareOp(E_COMPARISON_FUNC op)
{
    switch (op)
    {
        case ECFN_NEVER:
            return VK_COMPARE_OP_NEVER;
        case ECFN_ALWAYS:
            return VK_COMPARE_OP_ALWAYS;
        case ECFN_LESS:
            return VK_COMPARE_OP_LESS;
        case ECFN_LESSEQUAL:
            return VK_COMPARE_OP_LESS_OR_EQUAL;
        case ECFN_EQUAL:
            return VK_COMPARE_OP_EQUAL;
        case ECFN_NOTEQUAL:
            return VK_COMPARE_OP_NOT_EQUAL;
        case ECFN_GREATEREQUAL:
            return VK_COMPARE_OP_GREATER_OR_EQUAL;
        case ECFN_GREATER:
            return VK_COMPARE_OP_GREATER;
    };

    // Unsupported type
    return VK_COMPARE_OP_ALWAYS;
}

VkCullModeFlagBits VulkanUtility::getCullMode(const irr::video::SMaterial& material)
{
    if (material.FrontfaceCulling && !material.Wireframe)
        return VK_CULL_MODE_FRONT_BIT;
    else if (material.BackfaceCulling && !material.Wireframe)
        return VK_CULL_MODE_BACK_BIT;

    return VK_CULL_MODE_NONE;
}

VkPolygonMode VulkanUtility::getPolygonMode(bool fill)
{
    return fill ? VK_POLYGON_MODE_FILL : VK_POLYGON_MODE_LINE;
}

VkStencilOp VulkanUtility::getStencilOp(E_STENCIL_OPERATION op)
{
    switch (op)
    {
        case ESO_KEEP:
            return VK_STENCIL_OP_KEEP;
        case ESO_ZERO:
            return VK_STENCIL_OP_ZERO;
        case ESO_REPLACE:
            return VK_STENCIL_OP_REPLACE;
        case ESO_INCREMENT:
            return VK_STENCIL_OP_INCREMENT_AND_CLAMP;
        case ESO_DECREMENT:
            return VK_STENCIL_OP_DECREMENT_AND_CLAMP;
        case ESO_INCREMENT_WRAP:
            return VK_STENCIL_OP_INCREMENT_AND_WRAP;
        case ESO_DECREMENT_WRAP:
            return VK_STENCIL_OP_DECREMENT_AND_WRAP;
        case ESO_INVERT:
            return VK_STENCIL_OP_INVERT;
    }

    // Unsupported type
    return VK_STENCIL_OP_KEEP;
}

VkIndexType VulkanUtility::getIndexType(E_INDEX_TYPE op)
{
    switch (op)
    {
        case E_INDEX_TYPE::EIT_16BIT:
            return VK_INDEX_TYPE_UINT16;
        case E_INDEX_TYPE::EIT_32BIT:
            return VK_INDEX_TYPE_UINT32;
    }

    // Unsupported type
    return VK_INDEX_TYPE_UINT32;
}

VkPrimitiveTopology VulkanUtility::getDrawOp(scene::E_PRIMITIVE_TYPE op)
{
    switch (op)
    {
        case scene::E_PRIMITIVE_TYPE::EPT_POINTS:
            return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
        case scene::E_PRIMITIVE_TYPE::EPT_LINES:
            return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
        case scene::E_PRIMITIVE_TYPE::EPT_LINE_STRIP:
            return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
        case scene::E_PRIMITIVE_TYPE::EPT_TRIANGLES:
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        case scene::E_PRIMITIVE_TYPE::EPT_TRIANGLE_STRIP:
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
        case scene::E_PRIMITIVE_TYPE::EPT_TRIANGLE_FAN:
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
    }

    // Unsupported type
    return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
}

VkFilter VulkanUtility::getFilter(bool linear)
{
    return linear ? VK_FILTER_LINEAR : VK_FILTER_NEAREST;
}

VkSamplerMipmapMode VulkanUtility::getMipFilter(bool linear)
{
    return linear ? VK_SAMPLER_MIPMAP_MODE_LINEAR : VK_SAMPLER_MIPMAP_MODE_NEAREST;
}

void VulkanUtility::getDevices(CVulkanDriver* driver, GpuDeviceFlags flags, VulkanDevice*(&devices)[_MAX_DEVICES])
{
    uint32_t numDevices = driver->_getNumDevices();

    for (uint32_t i = 0; i < _MAX_DEVICES; i++)
    {
        if (i >= numDevices)
        {
            devices[i] = nullptr;
            continue;
        }

        VulkanDevice* device = driver->_getDevice(i);

        if (isDeviceIdxSet(driver, i, flags))
            devices[i] = device;
        else
            devices[i] = nullptr;
    }
}

bool VulkanUtility::isDeviceIdxSet(CVulkanDriver* driver, uint32_t idx, GpuDeviceFlags flags)
{
    VulkanDevice* device = driver->_getDevice(idx);

    return ((flags & (1 << idx)) != 0 || (flags == GDF_DEFAULT && device->isPrimary()));
}

void cutHorizontal(const VkImageSubresourceRange& toCut, const VkImageSubresourceRange& cutWith,
    VkImageSubresourceRange* output, uint32_t& numAreas)
{
    numAreas = 0;

    INT32 leftCut = cutWith.baseArrayLayer - toCut.baseArrayLayer;
    INT32 rightCut = (cutWith.baseArrayLayer + cutWith.layerCount) - toCut.baseArrayLayer;

    if (leftCut > 0 && leftCut < (INT32)(toCut.baseArrayLayer + toCut.layerCount))
    {
        output[numAreas] = toCut;
        VkImageSubresourceRange& range = output[numAreas];

        range.baseArrayLayer = toCut.baseArrayLayer;
        range.layerCount = leftCut;

        numAreas++;
    }

    if (rightCut > 0 && rightCut < (INT32)toCut.layerCount)
    {
        output[numAreas] = toCut;
        VkImageSubresourceRange& range = output[numAreas];

        range.baseArrayLayer = toCut.baseArrayLayer + rightCut;
        range.layerCount = toCut.layerCount - rightCut;

        numAreas++;
    }

    // If we made both left and right cuts, this means we need a middle one as well
    if (numAreas == 2)
    {
        output[numAreas] = toCut;
        VkImageSubresourceRange& range = output[numAreas];

        range.baseArrayLayer = toCut.baseArrayLayer + leftCut;
        range.layerCount = toCut.layerCount - (toCut.layerCount - rightCut) - leftCut;

        numAreas++;
    }

    // Nothing to cut
    if (numAreas == 0)
    {
        output[numAreas] = toCut;
        numAreas++;
    }
}

void cutVertical(const VkImageSubresourceRange& toCut, const VkImageSubresourceRange& cutWith,
    VkImageSubresourceRange* output, uint32_t& numAreas)
{
    numAreas = 0;

    INT32 topCut = cutWith.baseMipLevel - toCut.baseMipLevel;
    INT32 bottomCut = (cutWith.baseMipLevel + cutWith.levelCount) - toCut.baseMipLevel;

    if (topCut > 0 && topCut < (INT32)(toCut.baseMipLevel + toCut.levelCount))
    {
        output[numAreas] = toCut;
        VkImageSubresourceRange& range = output[numAreas];

        range.baseMipLevel = toCut.baseMipLevel;
        range.levelCount = topCut;

        numAreas++;
    }

    if (bottomCut > 0 && bottomCut < (INT32)toCut.levelCount)
    {
        output[numAreas] = toCut;
        VkImageSubresourceRange& range = output[numAreas];

        range.baseMipLevel = toCut.baseMipLevel + bottomCut;
        range.levelCount = toCut.levelCount - bottomCut;

        numAreas++;
    }

    // If we made both top and bottom cuts, this means we need a middle one as well
    if (numAreas == 2)
    {
        output[numAreas] = toCut;
        VkImageSubresourceRange& range = output[numAreas];

        range.baseMipLevel = toCut.baseMipLevel + topCut;
        range.levelCount = toCut.levelCount - (toCut.levelCount - bottomCut) - topCut;

        numAreas++;
    }

    // Nothing to cut
    if (numAreas == 0)
    {
        output[numAreas] = toCut;
        numAreas++;
    }
}

void VulkanUtility::cutRange(const VkImageSubresourceRange& toCut, const VkImageSubresourceRange& cutWith, std::array<VkImageSubresourceRange, 5>& output, uint32_t& numAreas)
{
    numAreas = 0;

    // Cut horizontally
    uint32_t numHorzCuts = 0;
    std::array<VkImageSubresourceRange, 3> horzCuts;
    cutHorizontal(toCut, cutWith, horzCuts.data(), numHorzCuts);

    // Cut vertically
    for (uint32_t i = 0; i < numHorzCuts; i++)
    {
        VkImageSubresourceRange& range = horzCuts[i];

        if (range.baseArrayLayer >= cutWith.baseArrayLayer &&
            (range.baseArrayLayer + range.layerCount) <= (cutWith.baseArrayLayer + cutWith.layerCount))
        {
            uint32_t numVertCuts = 0;
            cutVertical(range, cutWith, output.data() + numAreas, numVertCuts);

            numAreas += numVertCuts;
        }
        else
        {
            output[numAreas] = range;
            numAreas++;
        }
    }

    assert(numAreas <= 5);
}

bool VulkanUtility::rangeOverlaps(const VkImageSubresourceRange& a, const VkImageSubresourceRange& b)
{
    INT32 aRight = a.baseArrayLayer + (INT32)a.layerCount;
    INT32 bRight = b.baseArrayLayer + (INT32)b.layerCount;

    INT32 aBottom = a.baseMipLevel + (INT32)a.levelCount;
    INT32 bBottom = b.baseMipLevel + (INT32)b.levelCount;

    if ((INT32)a.baseArrayLayer < bRight && aRight >(INT32)b.baseArrayLayer &&
        (INT32)a.baseMipLevel < bBottom && aBottom >(INT32)b.baseMipLevel)
        return true;

    return false;
}
