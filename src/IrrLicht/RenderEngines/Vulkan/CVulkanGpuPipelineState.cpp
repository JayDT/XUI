#include "CVulkanGpuPipelineState.h"
#include "CVulkanDevice.h"
#include "CVulkanShader.h"
#include "CVulkanFramebuffer.h"
#include "CVulkanUtility.h"
#include "CVulkanVertexDeclaration.h"
#include "CVulkanCommandBuffer.h"
#include "CVulkanDriver.h"
#include "CVulkanDescriptorLayout.h"

using namespace irr;
using namespace irr::video;

VulkanPipeline::VulkanPipeline(CVulkanDriver* owner, VkPipeline pipeline, const std::array<bool, _MAX_MULTIPLE_RENDER_TARGETS>& colorReadOnly, bool depthStencilReadOnly)
    : CVulkanDeviceResource(owner), mPipeline(pipeline), mReadOnlyColor(colorReadOnly)
    , mReadOnlyDepth(depthStencilReadOnly)
{ }

VulkanPipeline::VulkanPipeline(CVulkanDriver* owner, VkPipeline pipeline)
    : CVulkanDeviceResource(owner), mPipeline(pipeline), mReadOnlyColor(), mReadOnlyDepth(false)
{ }

VulkanPipeline::~VulkanPipeline()
{
    vkDestroyPipeline(Driver->_getPrimaryDevice()->getLogical(), mPipeline, VulkanDevice::gVulkanAllocator);
}

void irr::video::VulkanPipeline::OnDeviceLost(CVulkanDriver * device)
{
}

void irr::video::VulkanPipeline::OnDeviceRestored(CVulkanDriver * device)
{
}

VulkanGraphicsPipelineState::VulkanGraphicsPipelineState(const irr::video::SMaterial& desc, CVulkanGLSLProgram* shader, CVulkanVertexDeclaration* inputLayout, GpuDeviceFlags deviceMask)
    : mScissorEnabled(false)
    , mVertexDecl(inputLayout)
    , mShader(shader)
    , mMaterial(desc)
    , mDeviceMask(deviceMask)
{
    for (uint32_t i = 0; i < _MAX_DEVICES; i++)
    {
        mPerDeviceData[i].device = nullptr;
        mPerDeviceData[i].pipelineLayout = VK_NULL_HANDLE;
    }
}

VulkanGraphicsPipelineState::~VulkanGraphicsPipelineState()
{
    for (uint32_t i = 0; i < _MAX_DEVICES; i++)
    {
        if (mPerDeviceData[i].device == nullptr)
            continue;

        delete mPerDeviceData[i].pipeline;
        mPerDeviceData[i].pipeline = nullptr;

        for (auto& entry : mPerDeviceData[i].pipelines)
            entry.second->drop();
        mPerDeviceData[i].pipelines.clear();
    }
}

void VulkanGraphicsPipelineState::initialize()
{
    //std::unique_lock<std::mutex> lock(mMutex);
    mIsDirty = true;

    uint32_t stageOutputIdx = 0;
    bool tesselationEnabled = mShader->HasShaderModule(E_ShaderTypes::EST_HULL_SHADER) && mShader->HasShaderModule(E_ShaderTypes::EST_DOMAIN_SHADER);

    for (uint32_t i = 0; i < E_ShaderTypes::EST_HIGH_LEVEL_SHADER; i++)
    {
        if (!mShader->HasShaderModule(E_ShaderTypes(i)))
            continue;

        VkPipelineShaderStageCreateInfo& stageCI = mShaderStageInfos[stageOutputIdx];
        stageCI.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stageCI.pNext = nullptr;
        stageCI.flags = 0;
        stageCI.stage = mShader->GetShaderStageBit(E_ShaderTypes(i));
        stageCI.module = *mShader->GetShaderModule(E_ShaderTypes(i));
        stageCI.pName = mShader->GetShaderEntryPoint(E_ShaderTypes(i));
        stageCI.pSpecializationInfo = nullptr;

        stageOutputIdx++;
    }

    assert(stageOutputIdx <= 5);
    uint32_t numUsedStages = stageOutputIdx;

    mInputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    mInputAssemblyInfo.pNext = nullptr;
    mInputAssemblyInfo.flags = 0;
    mInputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; // Assigned at runtime
    mInputAssemblyInfo.primitiveRestartEnable = false;

    mTesselationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
    mTesselationInfo.pNext = nullptr;
    mTesselationInfo.flags = 0;
    mTesselationInfo.patchControlPoints = 3; // Assigned at runtime

    mViewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    mViewportInfo.pNext = nullptr;
    mViewportInfo.flags = 0;
    mViewportInfo.viewportCount = 1; // Spec says this need to be at least 1...
    mViewportInfo.scissorCount = 1;
    mViewportInfo.pViewports = nullptr; // Dynamic
    mViewportInfo.pScissors = nullptr; // Dynamic

    mRasterizationInfo.sType                    = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    mRasterizationInfo.pNext                    = nullptr;
    mRasterizationInfo.flags                    = 0;
    mRasterizationInfo.depthClampEnable         = !mMaterial.DepthClipEnable;
    mRasterizationInfo.rasterizerDiscardEnable  = VK_FALSE;
    mRasterizationInfo.polygonMode              = VulkanUtility::getPolygonMode(!mMaterial.Wireframe);
    mRasterizationInfo.cullMode                 = VulkanUtility::getCullMode(mMaterial);
    mRasterizationInfo.frontFace                = VkFrontFace::VK_FRONT_FACE_CLOCKWISE;
    mRasterizationInfo.depthBiasEnable          = mMaterial.PolygonOffsetFactor != 0;
    mRasterizationInfo.depthBiasConstantFactor  = (float)mMaterial.PolygonOffsetFactor;
    mRasterizationInfo.depthBiasSlopeFactor     = mMaterial.PolygonOffsetDirection == EPO_BACK ? 1.f : 0.f;
    mRasterizationInfo.depthBiasClamp           = mRasterizationInfo.depthClampEnable ? 1.0f : 0.0f;
    mRasterizationInfo.lineWidth                = 1.0f;

    mMultiSampleInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    mMultiSampleInfo.pNext                  = nullptr;
    mMultiSampleInfo.flags                  = 0;
    mMultiSampleInfo.rasterizationSamples   = VK_SAMPLE_COUNT_1_BIT; // Assigned at runtime
    mMultiSampleInfo.sampleShadingEnable    = VK_FALSE; // When enabled, perform shading per sample instead of per pixel (more expensive, essentially FSAA)
    mMultiSampleInfo.minSampleShading       = 0.0f; // Minimum percent of samples to run full shading for when sampleShadingEnable is enabled (1.0f to run for all)
    mMultiSampleInfo.pSampleMask            = nullptr; // Normally one bit for each sample: e.g. 0x0000000F to enable all samples in a 4-sample setup
    mMultiSampleInfo.alphaToCoverageEnable  = mMaterial.AntiAliasing & EAAM_ALPHA_TO_COVERAGE;
    mMultiSampleInfo.alphaToOneEnable       = VK_FALSE;

    VkStencilOpState stencilFrontInfo;
    stencilFrontInfo.compareOp      = VulkanUtility::getCompareOp(E_COMPARISON_FUNC::ECFN_ALWAYS);
    stencilFrontInfo.depthFailOp    = VulkanUtility::getStencilOp(E_STENCIL_OPERATION::ESO_KEEP);
    stencilFrontInfo.passOp         = VulkanUtility::getStencilOp(E_STENCIL_OPERATION::ESO_KEEP);
    stencilFrontInfo.failOp         = VulkanUtility::getStencilOp(E_STENCIL_OPERATION::ESO_KEEP);
    stencilFrontInfo.reference      = 0; // Dynamic
    if (mMaterial.ZWriteEnable && !mMaterial.isTransparent())
    {
        stencilFrontInfo.compareMask = (uint32_t)0xff;
        stencilFrontInfo.writeMask = (uint32_t)0xff;
    }
    else
    {
        stencilFrontInfo.compareMask = (uint32_t)0;
        stencilFrontInfo.writeMask = (uint32_t)0;
    }

    VkStencilOpState stencilBackInfo;
    stencilBackInfo.compareOp   = VulkanUtility::getCompareOp(E_COMPARISON_FUNC::ECFN_ALWAYS);
    stencilBackInfo.depthFailOp = VulkanUtility::getStencilOp(E_STENCIL_OPERATION::ESO_KEEP);
    stencilBackInfo.passOp      = VulkanUtility::getStencilOp(E_STENCIL_OPERATION::ESO_KEEP);
    stencilBackInfo.failOp      = VulkanUtility::getStencilOp(E_STENCIL_OPERATION::ESO_KEEP);
    stencilBackInfo.reference   = 0; // Dynamic
    if (mMaterial.ZWriteEnable && !mMaterial.isTransparent())
    {
        stencilBackInfo.compareMask = (uint32_t)0xff;
        stencilBackInfo.writeMask = (uint32_t)0xff;
    }
    else
    {
        stencilBackInfo.compareMask = (uint32_t)0;
        stencilBackInfo.writeMask = (uint32_t)0;
    }

    mDepthStencilInfo.sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    mDepthStencilInfo.pNext                 = nullptr;
    mDepthStencilInfo.flags                 = 0;
    mDepthStencilInfo.depthBoundsTestEnable = VkBool32(false);
    mDepthStencilInfo.minDepthBounds        = 0.0f;
    mDepthStencilInfo.maxDepthBounds        = 1.0f;
    mDepthStencilInfo.depthTestEnable       = VkBool32((/*true ||*/ mMaterial.ZBuffer == ECFN_NEVER) ? false : true);
    mDepthStencilInfo.depthWriteEnable      = VkBool32((mMaterial.ZWriteEnable && !mMaterial.isTransparent()) ? true : false);
    mDepthStencilInfo.depthCompareOp        = VulkanUtility::getCompareOp((E_COMPARISON_FUNC)mMaterial.ZBuffer);
    mDepthStencilInfo.front                 = stencilFrontInfo;
    mDepthStencilInfo.back                  = stencilBackInfo;
    mDepthStencilInfo.stencilTestEnable     = VkBool32(false);

    VkBool32 blendEnabled = false;
    E_BLEND_FACTOR srcFact = E_BLEND_FACTOR::EBF_ONE, dstFact = E_BLEND_FACTOR::EBF_ZERO, srcFactAlpha = E_BLEND_FACTOR::EBF_ONE, dstFactAlpha = E_BLEND_FACTOR::EBF_ZERO;
    E_BLEND_OPERATION blendOp = E_BLEND_OPERATION::EBO_ADD, blendOpAlpha = E_BLEND_OPERATION::EBO_ADD;
    E_MODULATE_FUNC modulate = E_MODULATE_FUNC::EMFN_MODULATE_1X;
    u32 alphaSource = 1.f;

    switch (mMaterial.MaterialType)
    {
        case E_MATERIAL_TYPE::EMT_SOLID:
        case E_MATERIAL_TYPE::EMT_TRANSPARENT_ALPHA_CHANNEL_REF:
        {
            blendEnabled = false;
            break;
        }
        case E_MATERIAL_TYPE::EMT_TRANSPARENT_ALPHA_CHANNEL:
        {
            blendEnabled = true;
            blendOp = E_BLEND_OPERATION::EBO_ADD;
            srcFact = E_BLEND_FACTOR::EBF_SRC_ALPHA;
            dstFact = E_BLEND_FACTOR::EBF_ONE_MINUS_SRC_ALPHA;
            break;
        }
        default:
        {
            unpack_textureBlendFunc(srcFact, dstFact, modulate, alphaSource, mMaterial.uMaterialTypeParam, &srcFactAlpha, &dstFactAlpha, &blendOp, &blendOpAlpha);
            break;
        }
    }

    const UINT8 colorWriteMask =
        ((mMaterial.ColorMask & ECP_RED)    ? VkColorComponentFlagBits::VK_COLOR_COMPONENT_R_BIT : 0) |
        ((mMaterial.ColorMask & ECP_GREEN)  ? VkColorComponentFlagBits::VK_COLOR_COMPONENT_G_BIT : 0) |
        ((mMaterial.ColorMask & ECP_BLUE)   ? VkColorComponentFlagBits::VK_COLOR_COMPONENT_B_BIT : 0) |
        ((mMaterial.ColorMask & ECP_ALPHA)  ? VkColorComponentFlagBits::VK_COLOR_COMPONENT_A_BIT : 0);

    for (uint32_t i = 0; i < _MAX_MULTIPLE_RENDER_TARGETS; i++)
    {
        uint32_t rtIdx = 0;

        VkPipelineColorBlendAttachmentState& blendState = mAttachmentBlendStates[i];
        blendState.blendEnable          = blendEnabled;
        blendState.colorBlendOp         = VulkanUtility::getBlendOp(blendOp);
        blendState.srcColorBlendFactor  = VulkanUtility::getBlendFactor(srcFact);
        blendState.dstColorBlendFactor  = VulkanUtility::getBlendFactor(dstFact);
        blendState.alphaBlendOp         = VulkanUtility::getBlendOp(blendOpAlpha);
        blendState.srcAlphaBlendFactor  = VulkanUtility::getBlendFactor(srcFactAlpha);
        blendState.dstAlphaBlendFactor  = VulkanUtility::getBlendFactor(dstFactAlpha);
        blendState.colorWriteMask       = VkColorComponentFlagBits(colorWriteMask);
    }

    mColorBlendStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    mColorBlendStateInfo.pNext = nullptr;
    mColorBlendStateInfo.flags = 0;
    mColorBlendStateInfo.logicOpEnable = VK_FALSE;
    mColorBlendStateInfo.logicOp = VK_LOGIC_OP_NO_OP;
    mColorBlendStateInfo.attachmentCount = 0; // Assigned at runtime
    mColorBlendStateInfo.pAttachments = mAttachmentBlendStates;
    mColorBlendStateInfo.blendConstants[0] = 0.0f;
    mColorBlendStateInfo.blendConstants[1] = 0.0f;
    mColorBlendStateInfo.blendConstants[2] = 0.0f;
    mColorBlendStateInfo.blendConstants[3] = 0.0f;

    mDynamicStates[0] = VK_DYNAMIC_STATE_VIEWPORT;
    mDynamicStates[1] = VK_DYNAMIC_STATE_SCISSOR;
    mDynamicStates[2] = VK_DYNAMIC_STATE_STENCIL_REFERENCE;

    uint32_t numDynamicStates = sizeof(mDynamicStates) / sizeof(mDynamicStates[0]);
    assert(numDynamicStates == 3);

    mDynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    mDynamicStateInfo.pNext = nullptr;
    mDynamicStateInfo.flags = 0;
    mDynamicStateInfo.dynamicStateCount = numDynamicStates;
    mDynamicStateInfo.pDynamicStates = mDynamicStates;

    mPipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    mPipelineInfo.pNext = nullptr;
    mPipelineInfo.flags = 0;
    mPipelineInfo.stageCount = numUsedStages;
    mPipelineInfo.pStages = mShaderStageInfos;
    mPipelineInfo.pVertexInputState = nullptr; // Assigned at runtime
    mPipelineInfo.pInputAssemblyState = &mInputAssemblyInfo;
    mPipelineInfo.pTessellationState = tesselationEnabled ? &mTesselationInfo : nullptr;
    mPipelineInfo.pViewportState = &mViewportInfo;
    mPipelineInfo.pRasterizationState = &mRasterizationInfo;
    mPipelineInfo.pMultisampleState = &mMultiSampleInfo;
    mPipelineInfo.pDepthStencilState = nullptr; // Assigned at runtime
    mPipelineInfo.pColorBlendState = nullptr; // Assigned at runtime
    mPipelineInfo.pDynamicState = &mDynamicStateInfo;
    mPipelineInfo.renderPass = VK_NULL_HANDLE; // Assigned at runtime
    mPipelineInfo.layout = VK_NULL_HANDLE; // Assigned at runtime
    mPipelineInfo.subpass = 0;
    mPipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    mPipelineInfo.basePipelineIndex = -1;

    for (uint32_t st = 0; st < MATERIAL_MAX_TEXTURES; ++st)
    {
        SMaterialLayer& layer = mMaterial.TextureLayer[st];
        const float tmp = layer.LODBias * 0.125f;
        VkSamplerCreateInfo& samplerCI = mSamplerInfo[st];
        samplerCI.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerCI.flags = 0;
        samplerCI.pNext = nullptr;
        samplerCI.magFilter = VulkanUtility::getFilter(layer.AnisotropicFilter || layer.TrilinearFilter || layer.BilinearFilter);
        samplerCI.minFilter = VulkanUtility::getFilter(layer.AnisotropicFilter || layer.TrilinearFilter);
        samplerCI.mipmapMode = VulkanUtility::getMipFilter(mMaterial.UseMipMaps && (layer.AnisotropicFilter || layer.TrilinearFilter));
        samplerCI.addressModeU = VulkanUtility::getAddressingMode(E_TEXTURE_CLAMP(layer.TextureWrapU));
        samplerCI.addressModeV = VulkanUtility::getAddressingMode(E_TEXTURE_CLAMP(layer.TextureWrapV));
        samplerCI.addressModeW = VulkanUtility::getAddressingMode(E_TEXTURE_CLAMP(layer.TextureWrapU));
        samplerCI.mipLodBias = tmp;
        samplerCI.anisotropyEnable = (VkBool32)layer.AnisotropicFilter;
        samplerCI.maxAnisotropy = (float)layer.AnisotropicFilter;
        samplerCI.compareEnable = false;
        samplerCI.compareOp = VulkanUtility::getCompareOp(E_COMPARISON_FUNC::ECFN_NEVER);
        samplerCI.minLod = 0;
        samplerCI.maxLod = std::numeric_limits<float>::max();
        samplerCI.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
        samplerCI.unnormalizedCoordinates = false;
    }

    mScissorEnabled = false;

    VulkanDevice* devices[_MAX_DEVICES];
    VulkanUtility::getDevices(mShader->Driver, mDeviceMask, devices);

    for (uint32_t i = 0; i < _MAX_DEVICES; i++)
    {
        if (devices[i] == nullptr)
            continue;

        mPerDeviceData[i].device = devices[i];

        VkPipelineLayoutCreateInfo layoutCI;
        layoutCI.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        layoutCI.pNext = nullptr;
        layoutCI.flags = 0;
        layoutCI.pushConstantRangeCount = 0;
        layoutCI.pPushConstantRanges = nullptr;
        layoutCI.setLayoutCount = 1;
        layoutCI.pSetLayouts = mShader->getLayout()->getHandle();

        VkResult result = vkCreatePipelineLayout(devices[i]->getLogical(), &layoutCI, VulkanDevice::gVulkanAllocator, &mPerDeviceData[i].pipelineLayout);

        for (uint32_t st = 0; st < MATERIAL_MAX_TEXTURES; ++st)
        {
            VkSamplerCreateInfo& samplerCI = mSamplerInfo[st];
            VkResult result = vkCreateSampler(devices[i]->getLogical(), &samplerCI, VulkanDevice::gVulkanAllocator, &mPerDeviceData[i].mSampler[st]);
            assert(result == VK_SUCCESS);
        }
    }

    update(0, mMaterial, scene::E_PRIMITIVE_TYPE::EPT_TRIANGLES);
    PerDeviceData& perDeviceData = mPerDeviceData[0];
    perDeviceData.pipelineGuid = mRequestPipelineGuid;
}

void irr::video::VulkanGraphicsPipelineState::update(u8 deviceIdx, const irr::video::SMaterial & material, scene::E_PRIMITIVE_TYPE drawOp)
{
    PerDeviceData& perDeviceData = mPerDeviceData[deviceIdx];
    mRequestPipelineGuid = 0;
    if (mDrawOp != drawOp)
    {
        if (!mIsDirty)
            mIsDirty = true;

        mDrawOp = drawOp;
    }
    mRequestPipelineGuid |= u64(mDrawOp & 0xF) << 60;

    // fillmode
    if (mMaterial.Wireframe != material.Wireframe || mMaterial.PointCloud != material.PointCloud)
    {
        if (!mIsDirty)
            mIsDirty = true;

        mRasterizationInfo.polygonMode = VulkanUtility::getPolygonMode(!material.Wireframe);
    }
    mRequestPipelineGuid |= u64(mRasterizationInfo.polygonMode & 0x3) << 58;

    // back face culling
    if ((mMaterial.FrontfaceCulling != material.FrontfaceCulling) || (mMaterial.BackfaceCulling != material.BackfaceCulling))
    {
        if (!mIsDirty)
            mIsDirty = true;

        mRasterizationInfo.cullMode = VulkanUtility::getCullMode(material);
    }
    mRequestPipelineGuid |= u64(mRasterizationInfo.cullMode & 0x7) << 55;

    // Polygon offset
    if (mMaterial.PolygonOffsetDirection != material.PolygonOffsetDirection ||
        mMaterial.PolygonOffsetFactor != material.PolygonOffsetFactor)
    {
        if (!mIsDirty)
            mIsDirty = true;

        mRasterizationInfo.depthBiasEnable = material.PolygonOffsetFactor != 0;
        mRasterizationInfo.depthBiasConstantFactor = (float)material.PolygonOffsetFactor;
        mRasterizationInfo.depthBiasSlopeFactor = material.PolygonOffsetDirection == EPO_BACK ? 1.f : -1.f;
        mRasterizationInfo.depthBiasClamp = (FLOAT)material.PolygonOffsetFactor;
    }

    mRequestPipelineGuid |= u64(mRasterizationInfo.depthBiasEnable & 0x1) << 54;
    mRequestPipelineGuid |= u64(u8(mRasterizationInfo.depthBiasConstantFactor * 15) & 0xF) << 50;
    mRequestPipelineGuid |= u64(u8(mRasterizationInfo.depthBiasSlopeFactor * 15) & 0xF) << 46;
    mRequestPipelineGuid |= u64(u8(mRasterizationInfo.depthBiasClamp * 15) & 0xF) << 42;


    //mRasterizationInfo.depthClampEnable = !material.DepthClipEnable;
    //mRasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
    //mRasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
    //mRasterizationInfo.lineWidth = 1.0f;

    // zbuffer
    if (mMaterial.ZBuffer != material.ZBuffer)
    {
        if (!mIsDirty)
            mIsDirty = true;

        mDepthStencilInfo.depthTestEnable = VkBool32((material.ZBuffer == ECFN_NEVER) ? false : true);
        mDepthStencilInfo.depthCompareOp = VulkanUtility::getCompareOp((E_COMPARISON_FUNC)material.ZBuffer);
    }

    mRequestPipelineGuid |= u64(mDepthStencilInfo.depthTestEnable & 0x1) << 41;
    mRequestPipelineGuid |= u64(mDepthStencilInfo.depthCompareOp & 0x7) << 38;


    // zwrite
    if (mMaterial.ZWriteEnable != material.ZWriteEnable)
    {
        if (!mIsDirty)
            mIsDirty = true;

        mDepthStencilInfo.depthWriteEnable = VkBool32((material.ZWriteEnable && !material.isTransparent()) ? true : false);
    }

    mRequestPipelineGuid |= u64(mDepthStencilInfo.depthWriteEnable & 0x1) << 37;

    // Anti Aliasing
    if (mMaterial.AntiAliasing != material.AntiAliasing)
    {
        if (!mIsDirty)
            mIsDirty = true;

        mMultiSampleInfo.alphaToCoverageEnable = material.AntiAliasing & EAAM_ALPHA_TO_COVERAGE;
        mMultiSampleInfo.alphaToOneEnable = VK_FALSE;
        mMultiSampleInfo.sampleShadingEnable = material.AntiAliasing & (EAAM_SIMPLE | EAAM_QUALITY | EAAM_LINE_SMOOTH) ? VK_TRUE : VK_FALSE;
    }

    mRequestPipelineGuid |= u64(mMultiSampleInfo.alphaToCoverageEnable & 0x1) << 36;
    mRequestPipelineGuid |= u64(mMultiSampleInfo.alphaToOneEnable & 0x1) << 35;
    mRequestPipelineGuid |= u64(mMultiSampleInfo.sampleShadingEnable & 0x1) << 34;

    if (mMaterial.ColorMask != material.ColorMask || mMaterial.BlendOperation != material.BlendOperation || mMaterial.uMaterialTypeParam != material.uMaterialTypeParam)
    {
        if (!mIsDirty)
            mIsDirty = true;

        VkBool32 blendEnabled = material.BlendOperation != E_BLEND_OPERATION::EBO_NONE;
        E_BLEND_FACTOR srcFact = E_BLEND_FACTOR::EBF_ONE, dstFact = E_BLEND_FACTOR::EBF_ZERO, srcFactAlpha = E_BLEND_FACTOR::EBF_ONE, dstFactAlpha = E_BLEND_FACTOR::EBF_ZERO;
        E_BLEND_OPERATION blendOp = E_BLEND_OPERATION::EBO_ADD, blendOpAlpha = E_BLEND_OPERATION::EBO_ADD;
        E_MODULATE_FUNC modulate = E_MODULATE_FUNC::EMFN_MODULATE_1X;
        u32 alphaSource = 1.f;

        switch (material.MaterialType)
        {
            case E_MATERIAL_TYPE::EMT_TRANSPARENT_ALPHA_CHANNEL_REF:
                pack_textureBlendFunc(srcFact, dstFact, modulate, alphaSource, srcFactAlpha, dstFactAlpha, blendOp, blendOpAlpha);
                break;
            case E_MATERIAL_TYPE::EMT_TRANSPARENT_ALPHA_CHANNEL:
            {
                blendEnabled = true;
                blendOp = E_BLEND_OPERATION::EBO_ADD;
                srcFact = E_BLEND_FACTOR::EBF_SRC_ALPHA;
                dstFact = E_BLEND_FACTOR::EBF_ONE_MINUS_SRC_ALPHA;
                pack_textureBlendFunc(srcFact, dstFact, modulate, alphaSource, srcFactAlpha, dstFactAlpha, blendOp, blendOpAlpha);
                break;
            }
            default:
            {
                unpack_textureBlendFunc(srcFact, dstFact, modulate, alphaSource, material.uMaterialTypeParam, &srcFactAlpha, &dstFactAlpha, &blendOp, &blendOpAlpha);
                break;
            }
        }

        const UINT8 colorWriteMask =
            ((material.ColorMask & ECP_RED) ? VkColorComponentFlagBits::VK_COLOR_COMPONENT_R_BIT : 0) |
            ((material.ColorMask & ECP_GREEN) ? VkColorComponentFlagBits::VK_COLOR_COMPONENT_G_BIT : 0) |
            ((material.ColorMask & ECP_BLUE) ? VkColorComponentFlagBits::VK_COLOR_COMPONENT_B_BIT : 0) |
            ((material.ColorMask & ECP_ALPHA) ? VkColorComponentFlagBits::VK_COLOR_COMPONENT_A_BIT : 0);

        for (uint32_t i = 0; i < _MAX_MULTIPLE_RENDER_TARGETS; i++)
        {
            VkPipelineColorBlendAttachmentState& blendState = mAttachmentBlendStates[i];
            blendState.blendEnable = blendEnabled;
            blendState.colorBlendOp = VulkanUtility::getBlendOp(blendOp);
            blendState.srcColorBlendFactor = VulkanUtility::getBlendFactor(srcFact);
            blendState.dstColorBlendFactor = VulkanUtility::getBlendFactor(dstFact);
            blendState.alphaBlendOp = VulkanUtility::getBlendOp(blendOpAlpha);
            blendState.srcAlphaBlendFactor = VulkanUtility::getBlendFactor(srcFactAlpha);
            blendState.dstAlphaBlendFactor = VulkanUtility::getBlendFactor(dstFactAlpha);
            blendState.colorWriteMask = VkColorComponentFlagBits(colorWriteMask);
        }
    }

    mRequestPipelineGuid |= u64(mAttachmentBlendStates[0].blendEnable & 0x1) << 33;
    mRequestPipelineGuid |= (material.uMaterialTypeParam & 0xFFFFFFFFFF);
    mRequestPipelineGuid |= (material.ColorMask & 0xFFFFFFFFFF) << 8;

    for (uint32_t st = 0; st < MATERIAL_MAX_TEXTURES; ++st)
    {
        bool isDirty = false;

        const SMaterialLayer& layer = material.TextureLayer[st];
        const float tmp = layer.LODBias * 0.125f;
        VkSamplerCreateInfo& samplerCI = mSamplerInfo[st];

        if (mMaterial.TextureLayer[st].BilinearFilter != material.TextureLayer[st].BilinearFilter ||
            mMaterial.TextureLayer[st].TrilinearFilter != material.TextureLayer[st].TrilinearFilter ||
            mMaterial.TextureLayer[st].AnisotropicFilter != material.TextureLayer[st].AnisotropicFilter ||
            mMaterial.UseMipMaps != material.UseMipMaps)
        {
            isDirty = true;
            samplerCI.magFilter = VulkanUtility::getFilter(layer.AnisotropicFilter || layer.TrilinearFilter || layer.BilinearFilter);
            samplerCI.minFilter = VulkanUtility::getFilter(layer.AnisotropicFilter || layer.TrilinearFilter);
            samplerCI.mipmapMode = VulkanUtility::getMipFilter(mMaterial.UseMipMaps && (layer.AnisotropicFilter || layer.TrilinearFilter));
        }

        if (mMaterial.TextureLayer[st].TextureWrapU != material.TextureLayer[st].TextureWrapU)
        {
            if (!isDirty) isDirty = true;
            samplerCI.addressModeU = VulkanUtility::getAddressingMode(E_TEXTURE_CLAMP(layer.TextureWrapU));
        }

        if (mMaterial.TextureLayer[st].TextureWrapV != material.TextureLayer[st].TextureWrapV)
        {
            if (!isDirty) isDirty = true;
            samplerCI.addressModeV = VulkanUtility::getAddressingMode(E_TEXTURE_CLAMP(layer.TextureWrapV));
        }

        if (mMaterial.TextureLayer[st].TextureWrapU != material.TextureLayer[st].TextureWrapU)
        {
            if (!isDirty) isDirty = true;
            samplerCI.addressModeW = VulkanUtility::getAddressingMode(E_TEXTURE_CLAMP(layer.TextureWrapU));
        }

        if (samplerCI.mipLodBias != tmp)
        {
            if (!isDirty) isDirty = true;
            samplerCI.mipLodBias = tmp;
        }

        if (samplerCI.maxAnisotropy != layer.AnisotropicFilter)
        {
            if (!isDirty) isDirty = true;
            samplerCI.anisotropyEnable = (VkBool32)layer.AnisotropicFilter;
            samplerCI.maxAnisotropy = (float)layer.AnisotropicFilter;
        }

        //samplerCI.compareEnable = false;
        //samplerCI.compareOp = VulkanUtility::getCompareOp(E_COMPARISON_FUNC::ECFN_NEVER);
        //samplerCI.minLod = 0;
        //samplerCI.maxLod = std::numeric_limits<float>::max();
        //samplerCI.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
        //samplerCI.unnormalizedCoordinates = false;

        if (!isDirty)
            continue;

        for (uint32_t i = 0; i < _MAX_DEVICES; i++)
        {
            if (mPerDeviceData[i].device == nullptr)
                continue;

            VkSamplerCreateInfo& samplerCI = mSamplerInfo[st];
            VkResult result = vkCreateSampler(mPerDeviceData[i].device->getLogical(), &samplerCI, VulkanDevice::gVulkanAllocator, &mPerDeviceData[i].mSampler[st]);
            assert(result == VK_SUCCESS);
        }
    }

    mMaterial = material;
}

VulkanPipeline* VulkanGraphicsPipelineState::getPipeline(uint32_t deviceIdx, VulkanFramebuffer* framebuffer, uint32_t readOnlyFlags, scene::E_PRIMITIVE_TYPE drawOp, CVulkanVertexDeclaration* vertexInput)
{
    //Lock lock(mMutex);

    if (mPerDeviceData[deviceIdx].device == nullptr)
        return nullptr;

    assert(mDrawOp == drawOp);

    readOnlyFlags &= ~FBT_COLOR; // Ignore the color
    PerDeviceData& perDeviceData = mPerDeviceData[deviceIdx];
    if (!perDeviceData.pipeline || mIsDirty)
    {
        VulkanPipeline*& _pipeline = perDeviceData.pipelines[mRequestPipelineGuid];
        if (!_pipeline)
        {
            _pipeline = createPipeline(deviceIdx, framebuffer, readOnlyFlags, drawOp);
        }

        perDeviceData.pipelineGuid = mRequestPipelineGuid;
        perDeviceData.pipeline = _pipeline;
        mIsDirty = false;
    }

    return perDeviceData.pipeline;
}

VkPipelineLayout VulkanGraphicsPipelineState::getPipelineLayout(uint32_t deviceIdx) const
{
    return mPerDeviceData[deviceIdx].pipelineLayout;
}

void VulkanGraphicsPipelineState::registerPipelineResources(VulkanCmdBuffer* cmdBuffer)
{
    uint32_t deviceIdx = cmdBuffer->getDeviceIdx();

    cmdBuffer->registerResource(mShader, VulkanUseFlag::Read);
}

VulkanPipeline* VulkanGraphicsPipelineState::createPipeline(uint32_t deviceIdx, VulkanFramebuffer* framebuffer, uint32_t readOnlyFlags, scene::E_PRIMITIVE_TYPE drawOp)
{
    mInputAssemblyInfo.topology = VulkanUtility::getDrawOp(drawOp);
    mTesselationInfo.patchControlPoints = 3; // Not provided by our shaders for now
    mMultiSampleInfo.rasterizationSamples = framebuffer->getSampleFlags();
    mColorBlendStateInfo.attachmentCount = framebuffer->getNumColorAttachments();
    
    bool enableDepthWrites = mMaterial.ZWriteEnable && !mMaterial.isTransparent() && (readOnlyFlags & FBT_DEPTH) == 0;
    
    mDepthStencilInfo.depthWriteEnable = enableDepthWrites; // If depth stencil attachment is read only, depthWriteEnable must be VK_FALSE
    
    // Save stencil ops as we might need to change them if depth/stencil is read-only
    VkStencilOp oldFrontPassOp = mDepthStencilInfo.front.passOp;
    VkStencilOp oldFrontFailOp = mDepthStencilInfo.front.failOp;
    VkStencilOp oldFrontZFailOp = mDepthStencilInfo.front.depthFailOp;
    
    VkStencilOp oldBackPassOp = mDepthStencilInfo.back.passOp;
    VkStencilOp oldBackFailOp = mDepthStencilInfo.back.failOp;
    VkStencilOp oldBackZFailOp = mDepthStencilInfo.back.depthFailOp;
    
    if ((readOnlyFlags & FBT_STENCIL) != 0)
    {
        // Disable any stencil writes
        mDepthStencilInfo.front.passOp = VK_STENCIL_OP_KEEP;
        mDepthStencilInfo.front.failOp = VK_STENCIL_OP_KEEP;
        mDepthStencilInfo.front.depthFailOp = VK_STENCIL_OP_KEEP;
    
        mDepthStencilInfo.back.passOp = VK_STENCIL_OP_KEEP;
        mDepthStencilInfo.back.failOp = VK_STENCIL_OP_KEEP;
        mDepthStencilInfo.back.depthFailOp = VK_STENCIL_OP_KEEP;
    }
    
    // Note: We can use the default render pass here (default clear/load/read flags), even though that might not be the
    // exact one currently bound. This is because load/store operations and layout transitions are allowed to differ
    // (as per spec 7.2., such render passes are considered compatible).
    mPipelineInfo.renderPass = framebuffer->getRenderPass(RT_NONE, RT_NONE, 0);
    mPipelineInfo.layout = mPerDeviceData[deviceIdx].pipelineLayout;
    mPipelineInfo.pVertexInputState = &mVertexDecl->getVertexDeclaration();
    
    bool depthReadOnly;
    if (framebuffer->hasDepthAttachment())
    {
        mPipelineInfo.pDepthStencilState = &mDepthStencilInfo;
        depthReadOnly = (readOnlyFlags & FBT_DEPTH) != 0;
    }
    else
    {
        mPipelineInfo.pDepthStencilState = nullptr;
        depthReadOnly = true;
    }
    
    std::array<bool, _MAX_MULTIPLE_RENDER_TARGETS> colorReadOnly;
    if (framebuffer->getNumColorAttachments() > 0)
    {
        mPipelineInfo.pColorBlendState = &mColorBlendStateInfo;
    
        for (uint32_t i = 0; i < _MAX_MULTIPLE_RENDER_TARGETS; i++)
        {
            VkPipelineColorBlendAttachmentState& blendState = mAttachmentBlendStates[i];
            colorReadOnly[i] = blendState.colorWriteMask == 0;
        }
    }
    else
    {
        mPipelineInfo.pColorBlendState = nullptr;
    
        for (uint32_t i = 0; i < _MAX_MULTIPLE_RENDER_TARGETS; i++)
            colorReadOnly[i] = true;
    }
    
    uint32_t stageOutputIdx = 0;

    for (uint32_t i = 0; i < E_ShaderTypes::EST_HIGH_LEVEL_SHADER; i++)
    {
        if (!mShader->HasShaderModule(E_ShaderTypes(i)))
            continue;

        //VkPipelineShaderStageCreateInfo& stageCI = mShaderStageInfos[stageOutputIdx];
        //stageCI.module = *mShader->GetShaderModule(E_ShaderTypes(i));

        stageOutputIdx++;
    }
    
    VulkanDevice* device = mPerDeviceData[deviceIdx].device;
    VkDevice vkDevice = mPerDeviceData[deviceIdx].device->getLogical();
    
    VkPipeline pipeline;
    VkResult result = vkCreateGraphicsPipelines(vkDevice, VK_NULL_HANDLE, 1, &mPipelineInfo, VulkanDevice::gVulkanAllocator, &pipeline);
    assert(result == VK_SUCCESS);
    
    // Restore previous stencil op states
    mDepthStencilInfo.front.passOp = oldFrontPassOp;
    mDepthStencilInfo.front.failOp = oldFrontFailOp;
    mDepthStencilInfo.front.depthFailOp = oldFrontZFailOp;
    
    mDepthStencilInfo.back.passOp = oldBackPassOp;
    mDepthStencilInfo.back.failOp = oldBackFailOp;
    mDepthStencilInfo.back.depthFailOp = oldBackZFailOp;
    
    return new VulkanPipeline(mShader->GetDriver(), pipeline, colorReadOnly, depthReadOnly);
}

//VulkanComputePipelineState::VulkanComputePipelineState(const SPtr<GpuProgram>& program,
//    GpuDeviceFlags deviceMask)
//    :ComputePipelineState(program, deviceMask), mDeviceMask(deviceMask)
//{
//    bs_zero_out(mPerDeviceData);
//}
//
//VulkanComputePipelineState::~VulkanComputePipelineState()
//{
//    for (uint32_t i = 0; i < BS_MAX_DEVICES; i++)
//    {
//        if (mPerDeviceData[i].device == nullptr)
//            continue;
//
//        mPerDeviceData[i].pipeline->destroy();
//    }
//}
//
//void VulkanComputePipelineState::initialize()
//{
//    //ComputePipelineState::initialize();
//    //
//    //// This might happen fairly often if shaders with unsupported languages are loaded, in which case the pipeline
//    //// will never get used, and its fine not to initialize it.
//    //if (!mProgram->isCompiled())
//    //    return;
//    //
//    //VulkanGpuProgram* vkProgram = static_cast<VulkanGpuProgram*>(mProgram.get());
//    //
//    //VkPipelineShaderStageCreateInfo stageCI;
//    //stageCI.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
//    //stageCI.pNext = nullptr;
//    //stageCI.flags = 0;
//    //stageCI.stage = VK_SHADER_STAGE_COMPUTE_BIT;
//    //stageCI.module = VK_NULL_HANDLE;
//    //stageCI.pName = vkProgram->getEntryPoint().c_str();
//    //stageCI.pSpecializationInfo = nullptr;
//    //
//    //VkComputePipelineCreateInfo pipelineCI;
//    //pipelineCI.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
//    //pipelineCI.pNext = nullptr;
//    //pipelineCI.flags = 0;
//    //pipelineCI.stage = stageCI;
//    //pipelineCI.basePipelineHandle = VK_NULL_HANDLE;
//    //pipelineCI.basePipelineIndex = -1;
//    //
//    //VulkanRenderAPI& rapi = static_cast<VulkanRenderAPI&>(RenderAPI::instance());
//    //
//    //VulkanDevice* devices[BS_MAX_DEVICES];
//    //VulkanUtility::getDevices(rapi, mDeviceMask, devices);
//    //
//    //for (uint32_t i = 0; i < BS_MAX_DEVICES; i++)
//    //{
//    //    if (devices[i] == nullptr)
//    //        continue;
//    //
//    //    mPerDeviceData[i].device = devices[i];
//    //
//    //    VulkanDescriptorManager& descManager = devices[i]->getDescriptorManager();
//    //    VulkanResourceManager& rescManager = devices[i]->getResourceManager();
//    //    VulkanGpuPipelineParamInfo& vkParamInfo = static_cast<VulkanGpuPipelineParamInfo&>(*mParamInfo);
//    //
//    //    uint32_t numLayouts = vkParamInfo.getNumSets();
//    //    VulkanDescriptorLayout** layouts = (VulkanDescriptorLayout**)bs_stack_alloc(sizeof(VulkanDescriptorLayout*) * numLayouts);
//    //
//    //    for (uint32_t j = 0; j < numLayouts; j++)
//    //        layouts[j] = vkParamInfo.getLayout(i, j);
//    //
//    //    VulkanShaderModule* module = vkProgram->getShaderModule(i);
//    //
//    //    if (module != nullptr)
//    //        pipelineCI.stage.module = module->getHandle();
//    //    else
//    //        pipelineCI.stage.module = VK_NULL_HANDLE;
//    //
//    //    pipelineCI.layout = descManager.getPipelineLayout(layouts, numLayouts);
//    //
//    //    VkPipeline pipeline;
//    //    VkResult result = vkCreateComputePipelines(devices[i]->getLogical(), VK_NULL_HANDLE, 1, &pipelineCI,
//    //        gVulkanAllocator, &pipeline);
//    //    assert(result == VK_SUCCESS);
//    //
//    //
//    //    mPerDeviceData[i].pipeline = rescManager.create<VulkanPipeline>(pipeline);
//    //    mPerDeviceData[i].pipelineLayout = pipelineCI.layout;
//    //    bs_stack_free(layouts);
//    //}
//    //
//    //BS_INC_RENDER_STAT_CAT(ResCreated, RenderStatObject_PipelineState);
//}
//
//VulkanPipeline* VulkanComputePipelineState::getPipeline(uint32_t deviceIdx) const
//{
//    return mPerDeviceData[deviceIdx].pipeline;
//}
//
//VkPipelineLayout VulkanComputePipelineState::getPipelineLayout(uint32_t deviceIdx) const
//{
//    return mPerDeviceData[deviceIdx].pipelineLayout;
//}
//
//void VulkanComputePipelineState::registerPipelineResources(VulkanCmdBuffer* cmdBuffer)
//{
//    //uint32_t deviceIdx = cmdBuffer->getDeviceIdx();
//    //
//    //VulkanGpuProgram* program = static_cast<VulkanGpuProgram*>(mProgram.get());
//    //if (program != nullptr)
//    //{
//    //    VulkanShaderModule* module = program->getShaderModule(deviceIdx);
//    //
//    //    if (module != nullptr)
//    //        cmdBuffer->registerResource(module, VulkanUseFlag::Read);
//    //}
//}
