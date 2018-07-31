#include "CVulkanDriver.h"
#if defined(_IRR_COMPILE_WITH_WINDOWS_DEVICE_)
#include "ContextManager/CWinPlatform.h"
#endif
#include "CVulkanVertexDeclaration.h"
#include "CVulkanHardwareBuffer.h"
#include "CVulkanTexture.h"
#include "CVulkanShader.h"
#include "CVulkanDevice.h"
#include "CVulkanQueue.h"
#include "CVulkanGpuPipelineState.h"
#include "CVulkanGpuParams.h"
#include "CVulkanSwapChain.h"

#include "System/Uri.h"
#include "System/Resource/ResourceManager.h"
#include "System/Resource/ResourceObject.h"
#include "standard/client/DataSource_Standard.h"

#include "os.h"
#include "glslang/Public/ShaderLang.h"

#define GLM_FORCE_LEFT_HANDED
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
//#include <gli/gli.hpp>

extern bool InitializeModulResourceCVulkan();

using namespace irr;
using namespace irr::video;

namespace glslang
{
    bool InitProcess();
    bool DetachProcess();
}

VKAPI_ATTR VkBool32 debugMsgCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t srcObject,
    size_t location, int32_t msgCode, const char* pLayerPrefix, const char* pMsg, void* pUserData);

irr::video::CVulkanDriver::CVulkanDriver(const SIrrlichtCreationParameters & params, io::IFileSystem * io)
    : CNullDriver(io, params.WindowSize)
#if defined(_IRR_COMPILE_WITH_WINDOWS_DEVICE_)
    , DeviceType(EIDT_WIN32)
#elif defined(_IRR_COMPILE_WITH_X11_DEVICE_)
    , DeviceType(EIDT_X11)
#elif defined(_IRR_COMPILE_WITH_OSX_DEVICE_)
    , DeviceType(EIDT_OSX)
#endif
    , mCreateParams(params)
{
    InitializeModulResourceCVulkan();
}

irr::video::CVulkanDriver::~CVulkanDriver()
{
    if (mInstance)
        vkDestroyInstance(mInstance, NULL);
}

void irr::video::CVulkanDriver::ReleaseDriver()
{
}

void irr::video::CVulkanDriver::getSyncSemaphores(u32 deviceIdx, u32 syncMask, VulkanSemaphore ** semaphores, u32 & count)
{
    bool semaphoreRequestFailed = false;
    VulkanDevice* device = _getDevice(deviceIdx);

    u32 semaphoreIdx = 0;
    for (u32 i = 0; i < GQT_COUNT; i++)
    {
        GpuQueueType queueType = (GpuQueueType)i;

        u32 numQueues = device->getNumQueues(queueType);
        for (u32 j = 0; j < numQueues; j++)
        {
            VulkanQueue* queue = device->getQueue(queueType, j);
            VulkanCmdBuffer* lastCB = queue->getLastCommandBuffer();

            // Check if a buffer is currently executing on the queue
            if (lastCB == nullptr || !lastCB->isSubmitted())
                continue;

            // Check if we care about this specific queue
            u32 queueMask = device->getQueueMask(queueType, j);
            if ((syncMask & queueMask) == 0)
                continue;

            VulkanSemaphore* semaphore = lastCB->requestInterQueueSemaphore();
            if (semaphore == nullptr)
            {
                semaphoreRequestFailed = true;
                continue;
            }

            semaphores[semaphoreIdx++] = semaphore;
        }
    }

    count = semaphoreIdx;

    if (semaphoreRequestFailed)
    {
        os::Printer::log(("Failed to allocate semaphores for a command buffer sync. This means some of the dependency requests "
            "will not be fulfilled. This happened because a command buffer has too many dependant command "
            "buffers. The maximum allowed number is " + std::to_string(_MAX_VULKAN_CB_DEPENDENCIES) + " but can be "
            "increased by incrementing the value of _MAX_VULKAN_CB_DEPENDENCIES.").c_str());
    }
}

VulkanSwapChain * irr::video::CVulkanDriver::_getSwapChain()
{
    return mPlatform->GetSwapChain();
}

void irr::video::CVulkanDriver::initialize()
{
    // Create instance
    VkApplicationInfo appInfo;
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = nullptr;
    appInfo.pApplicationName = "Irrlicht App";
    appInfo.applicationVersion = 1;
    appInfo.pEngineName = "IrrVulkan";
    appInfo.engineVersion = (0 << 24) | (4 << 16) | 0;
    appInfo.apiVersion = VK_API_VERSION_1_0;

#if VULKAN_DEBUG_MODE
    const char* layers[] =
    {
        "VK_LAYER_LUNARG_standard_validation"
    };

    const char* extensions[] =
    {
        nullptr, /** Surface extension */
        nullptr, /** OS specific surface extension */
        VK_EXT_DEBUG_REPORT_EXTENSION_NAME
    };

    uint32_t numLayers = sizeof(layers) / sizeof(layers[0]);
#else
    const char** layers = nullptr;
    const char* extensions[] =
    {
        nullptr, /** Surface extension */
        nullptr, /** OS specific surface extension */
    };

    uint32_t numLayers = 0;
#endif

    extensions[0] = VK_KHR_SURFACE_EXTENSION_NAME;

#if defined(_IRR_COMPILE_WITH_WINDOWS_DEVICE_)
    extensions[1] = VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
#elif defined(_IRR_COMPILE_WITH_ANDROID_DEVICE_)
    extensions[1] = VK_KHR_ANDROID_SURFACE_EXTENSION_NAME;
#elif defined(_IRR_COMPILE_WITH_OSX_DEVICE_)
    extensions[1] = VK_MVK_MACOS_SURFACE_EXTENSION_NAME;
#else
    extensions[1] = VK_KHR_XLIB_SURFACE_EXTENSION_NAME;
#endif

    uint32_t numExtensions = sizeof(extensions) / sizeof(extensions[0]);

    VkInstanceCreateInfo instanceInfo;
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pNext = nullptr;
    instanceInfo.flags = 0;
    instanceInfo.pApplicationInfo = &appInfo;
    instanceInfo.enabledLayerCount = numLayers;
    instanceInfo.ppEnabledLayerNames = layers;
    instanceInfo.enabledExtensionCount = numExtensions;
    instanceInfo.ppEnabledExtensionNames = extensions;

    VkResult result = vkCreateInstance(&instanceInfo, VulkanDevice::gVulkanAllocator, &mInstance);
    assert(result == VK_SUCCESS);

    VulkanDispatcherExt.init(mInstance);

    // Set up debugging
#if VULKAN_DEBUG_MODE
    VkDebugReportFlagsEXT debugFlags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT /*| VK_DEBUG_REPORT_INFORMATION_BIT_EXT | VK_DEBUG_REPORT_DEBUG_BIT_EXT*/;

    VkDebugReportCallbackCreateInfoEXT debugInfo;
    debugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
    debugInfo.pNext = nullptr;
    debugInfo.pfnCallback = (PFN_vkDebugReportCallbackEXT)debugMsgCallback;
    debugInfo.flags = debugFlags;
    debugInfo.pUserData = this;

    if (VulkanDispatcherExt.vkCreateDebugReportCallbackEXT)
        result = VulkanDispatcherExt.vkCreateDebugReportCallbackEXT(mInstance, &debugInfo, VulkanDevice::gVulkanAllocator, &mDebugCallback);
    assert(result == VK_SUCCESS);
#endif

    // Enumerate all devices
    result = vkEnumeratePhysicalDevices(mInstance, &mNumDevices, nullptr);
    assert(result == VK_SUCCESS);

    std::vector<VkPhysicalDevice> physicalDevices(mNumDevices);
    result = vkEnumeratePhysicalDevices(mInstance, &mNumDevices, physicalDevices.data());
    assert(result == VK_SUCCESS);

    mDevices.resize(mNumDevices);
    for (uint32_t i = 0; i < mNumDevices; i++)
        mDevices[i] = new VulkanDevice(this, physicalDevices[i], i);

    // Find primary device
    // Note: MULTIGPU - Detect multiple similar devices here if supporting multi-GPU
    for (uint32_t i = 0; i < mNumDevices; i++)
    {
        bool isPrimary = mDevices[i]->getDeviceProperties().deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;

        if (isPrimary)
        {
            mDevices[i]->setIsPrimary();
            mPrimaryDevices.push_back(mDevices[i]);
            break;
        }
    }

    if (mPrimaryDevices.size() == 0)
        mPrimaryDevices.push_back(mDevices[0]);

    VulkanDispatcherExt.init(mInstance, _getPrimaryDevice()->getLogical());

    // Create main command buffer
    mMainCommandBuffer = new VulkanCommandBuffer(this, GQT_GRAPHICS, 0, 0, false);

#if defined(_IRR_COMPILE_WITH_WINDOWS_DEVICE_)
    mPlatform = new CWinVulkanPlatform(this);
#elif defined(_IRR_COMPILE_WITH_ANDROID_DEVICE_)
    mPlatform = new CAndroidVulkanPlatform(this);
#elif defined(_IRR_COMPILE_WITH_OSX_DEVICE_)
    mPlatform = new CMacVulkanPlatform(this);
#else
    mPlatform = new CLinuxVulkanPlatform(this);
#endif

    mPlatform->initialize();
}

bool irr::video::CVulkanDriver::initDriver(HWND hwnd, bool pureSoftware)
{
    CNullDriver::initDriver();

    initialize();
    glslang::InitProcess();
    ShInitialize();
    ShInitialize();  // also test reference counting of users
    ShFinalize();    // also test reference counting of users

    OnDebugMode = true;

    //ExposedData = ContextManager->getContext();

    // Only enable multisampling if needed
    disableFeature(EVDF_TEXTURE_MULTISAMPLING, true);

    // Set render targets
    setRenderTarget(0, true, true);

    // set fog mode
    //setFog(FogColor, FogType, FogStart, FogEnd, FogDensity, PixelFog, RangeFog);
    setFog(FogColor, (E_FOG_TYPE)0, FogStart, FogEnd, FogDensity, PixelFog, RangeFog);

    ResetRenderStates = true;

    for (auto const& vertDecl : VertexDeclarations)
        static_cast<irr::video::CVulkanVertexDeclaration*>(vertDecl.second)->getVertexDeclaration();

    // create materials
    createMaterialRenderers();

    // clear textures
    for (u16 i = 0; i < MATERIAL_MAX_TEXTURES; i++)
        setActiveTexture(i, 0);

    u8 dummydata[16];

    // Note: When multi-GPU is properly tested, make sure to create these textures on all GPUs
    mDummyStorageBuffer = static_cast<CVulkanHardwareBuffer*>(createHardwareBuffer(E_HARDWARE_BUFFER_TYPE::EHBT_CONSTANTS, E_HARDWARE_BUFFER_ACCESS::EHBA_IMMUTABLE, 16));
    mDummyStorageBuffer->UpdateBuffer(E_HARDWARE_BUFFER_TYPE::EHBT_CONSTANTS, E_HARDWARE_BUFFER_ACCESS::EHBA_IMMUTABLE, dummydata, 16);
    mDummyStorageBuffer->UpdateBuffer(E_HARDWARE_BUFFER_TYPE::EHBT_SHADER_RESOURCE, E_HARDWARE_BUFFER_ACCESS::EHBA_IMMUTABLE, dummydata, 16);
    //mDummyUniformBuffer = static_cast<CVulkanHardwareBuffer*>(createHardwareBuffer(E_HARDWARE_BUFFER_TYPE::EHBT_SHADER_RESOURCE, E_HARDWARE_BUFFER_ACCESS::EHBA_IMMUTABLE, 16));

    blankImage = createImage(ECF_A1R5G5B5, getCurrentRenderTargetSize());
    // Size.Height * Pitch
    memset(blankImage->lock(), 0, (blankImage->getPitch() * blankImage->getDimension().Height));
    blankImage->unlock();

    // ToDo: per device
    VkSamplerCreateInfo samplerCI = {};
    samplerCI.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    VkResult result = vkCreateSampler(_getPrimaryDevice()->getLogical(), &samplerCI, VulkanDevice::gVulkanAllocator, &mDummySampler);
    assert(result == VK_SUCCESS);

    blankTexture = static_cast<CVulkanTexture*>(createDeviceDependentTexture(blankImage, "internal_null_texture"));

    auto vertShader = System::Resource::ResourceManager::Instance()->FindResource(System::URI("pack://application:,,,/CVulkan;/GLSL/VkDefault.vert", true));
    auto fragShader = System::Resource::ResourceManager::Instance()->FindResource(System::URI("pack://application:,,,/CVulkan;/GLSL/VkDefault.frag", true));
    
    m_defaultShader = static_cast<CVulkanGLSLProgram*>(createShader(vertShader->ToMemoryStreamReader(), fragShader->ToMemoryStreamReader(), nullptr, nullptr));
    m_defaultShader->addDataBuffer(new irr::video::VulkanShaderGenericValuesBuffer(video::IShaderDataBuffer::EUT_PER_FRAME_PER_MESH), nullptr);
    
    activeGPUProgram(m_defaultShader);

    return true;
}

bool irr::video::CVulkanDriver::_getErrorsFromQueue() const
{
    return false;
}

bool irr::video::CVulkanDriver::beginScene(bool backBuffer, bool zBuffer, SColor color, const SExposedVideoData & videoData, core::rect<s32>* sourceRect)
{
    CNullDriver::beginScene(backBuffer, zBuffer, color, videoData, sourceRect);

    mPlatform->acquireBackBuffer();

    mMainCommandBuffer->getInternal()->setRenderTarget(nullptr, 0, RenderSurfaceMaskBits::RT_ALL, true);
    mMainCommandBuffer->getInternal()->clearViewport(FrameBufferType::FBT_COLOR | FrameBufferType::FBT_DEPTH | FrameBufferType::FBT_STENCIL, irr::video::SColor(255, 0, 0, 0), 1.0f, 1, 0xFF);

    mMainCommandBuffer->getInternal()->beginRenderPass();

    return true;
}

bool irr::video::CVulkanDriver::endScene()
{
    CNullDriver::endScene();

    mMainCommandBuffer->getInternal()->endRenderPass();

    _getPrimaryDevice()->flushTransferBuffers();
    mMainCommandBuffer->submit(0xFFFFFFFF);
    mPlatform->swapBuffers();

    return true;
}

bool irr::video::CVulkanDriver::queryFeature(E_VIDEO_DRIVER_FEATURE feature) const
{
    return true;
}

void convertProjectionMatrix(const core::matrix4& matrix, core::matrix4& dest)
{
    dest = matrix;

    // Flip Y axis
    dest(1,1) = -dest(1, 1);

    // Convert depth range from [-1,1] to [0,1]
    dest(2, 2) = dest(2, 2) * 0.5f;
    dest(3, 2) = dest(3, 2) * 0.5f;


    //dest(2, 0) =  (dest(2, 0) + dest(3, 0)) / 2;
    //dest(2, 1) =  (dest(2, 1) + dest(3, 1)) / 2;
    //dest(2, 2) =  (dest(2, 2) + dest(3, 2)) / 2;
    //dest(2, 3) =  (dest(2, 3) + dest(3, 3)) / 2;
}

void irr::video::CVulkanDriver::setTransform(E_TRANSFORMATION_STATE state, const core::matrix4 & mat)
{
    Transformation3DChanged = true;

    if (state == ETS_PROJECTION)
        convertProjectionMatrix(mat, Matrices[state]);
    else
        Matrices[state] = mat;

    if (state == ETS_PROJECTION || state == ETS_VIEW)
        Matrices[ETS_PROJECTION_VIEW] = Matrices[ETS_PROJECTION] * Matrices[ETS_VIEW];
}

void irr::video::CVulkanDriver::setMaterial(const SMaterial & material)
{
    Material = material;
    OverrideMaterial.apply(Material);

    // set textures
    for (u16 i = 0; i < MATERIAL_MAX_TEXTURES; i++)
    {
        setActiveTexture(i, Material.getTexture(i));
        setTransform((E_TRANSFORMATION_STATE)(ETS_TEXTURE_0 + i), material.TextureLayer[i].getTextureMatrixConst());
    }
}

bool irr::video::CVulkanDriver::setRenderTarget(video::ITexture * texture, bool clearBackBuffer, bool clearZBuffer, SColor color)
{
    //video::IRenderTarget
    //mMainCommandBuffer->getInternal()->setRenderTarget();
    //setViewPort(core::rect<s32>(0, 0, getCurrentRenderTargetSize().Width, getCurrentRenderTargetSize().Height));
    return true;
}

bool irr::video::CVulkanDriver::setRenderTarget(const core::array<video::IRenderTarget>& texture, bool clearBackBuffer, bool clearZBuffer, SColor color)
{
    return false;
}

void irr::video::CVulkanDriver::setViewPort(const core::rect<s32>& area)
{
    //core::rect<s32> vp = area;
    //core::rect<s32> rendert(0, 0, getCurrentRenderTargetSize().Width, getCurrentRenderTargetSize().Height);
    //vp.clipAgainst(rendert);
    //
    //VkViewport viewport;
    //memset(&viewport, 0, sizeof(viewport));
    //
    //viewport.x          = (float)vp.UpperLeftCorner.X;
    //viewport.y          = (float)vp.UpperLeftCorner.Y;
    //viewport.width      = (float)vp.getWidth();
    //viewport.height     = (float)vp.getHeight();
    //viewport.minDepth   = (float)0.0f;
    //viewport.maxDepth   = (float)1.0f;

    //mMainCommandBuffer->getInternal()->setViewport(area);

    //ContextManager->GetActiveContext()->cmd.setViewport(0, 1, &viewport, *ContextManager->GetVulkanDispatcher());
}

const core::rect<s32>& irr::video::CVulkanDriver::getViewPort() const
{
    return ViewPort;
}

bool irr::video::CVulkanDriver::updateVertexHardwareBuffer(CVulkanHardwareBuffer * HWBuffer, E_HARDWARE_BUFFER_TYPE Type)
{
    if (!HWBuffer)
        return false;

    E_HARDWARE_BUFFER_ACCESS MemoryAccess = E_HARDWARE_BUFFER_ACCESS::EHBA_DEFAULT;

    const scene::IMeshBuffer* mb = HWBuffer->GetBuffer();
    const E_VERTEX_TYPE vType = mb->getVertexType();

    if (Type == E_HARDWARE_BUFFER_TYPE::EHBT_VERTEX)
    {
        if (HWBuffer->GetBuffer()->getHardwareMappingHint_Vertex() == scene::EHM_DYNAMIC)
            MemoryAccess = E_HARDWARE_BUFFER_ACCESS::EHBA_DYNAMIC;

        const void* vertices = mb->getVertices();
        const u32 vertexCount = mb->getVertexCount();
        const u32 vertexSize = HWBuffer->GetBuffer()->GetVertexStride() ? HWBuffer->GetBuffer()->GetVertexStride() : getVertexPitchFromType(vType);
        const u32 bufSize = vertexSize * vertexCount;

        HWBuffer->UpdateBuffer(Type, MemoryAccess, vertices, bufSize);
    }
    else if (Type == E_HARDWARE_BUFFER_TYPE::EHBT_VERTEX_INSTANCE_STREAM)
    {
        IShaderDataBufferElement* variable = HWBuffer->GetInstanceBuffer()->m_bufferDataArray[0];

        if (HWBuffer->GetBuffer()->getHardwareMappingHint_Instance() == scene::EHM_DYNAMIC)
            MemoryAccess = E_HARDWARE_BUFFER_ACCESS::EHBA_DYNAMIC;

        HWBuffer->UpdateBuffer(Type, MemoryAccess, variable->getShaderValues(), variable->getValueSizeOf() * variable->getShaderValueCount());
    }
    return true;
}

bool irr::video::CVulkanDriver::updateIndexHardwareBuffer(CVulkanHardwareBuffer * HWBuffer)
{
    if (!HWBuffer)
        return false;

    const scene::IMeshBuffer* mb = HWBuffer->GetBuffer();
    const u16* indices = mb->getIndices();
    const u32 indexCount = mb->getIndexCount();
    const u32 indexStride = video::getIndexSize(mb->getIndexType());
    const u32 bufSize = indexStride * indexCount;

    E_HARDWARE_BUFFER_ACCESS MemoryAccess = E_HARDWARE_BUFFER_ACCESS::EHBA_DEFAULT;
    if (HWBuffer->GetBuffer()->getHardwareMappingHint_Index() == scene::EHM_DYNAMIC)
        MemoryAccess = E_HARDWARE_BUFFER_ACCESS::EHBA_DYNAMIC;

    HWBuffer->UpdateBuffer(E_HARDWARE_BUFFER_TYPE::EHBT_INDEX, MemoryAccess, indices, bufSize);
    return true;
}

bool irr::video::CVulkanDriver::updateHardwareBuffer(IHardwareBuffer * HWBuffer)
{
    if (!HWBuffer)
        return false;

    if (HWBuffer->GetBuffer()->getHardwareMappingHint_Vertex() == scene::EHM_NEVER || HWBuffer->GetBuffer()->getHardwareMappingHint_Index() == scene::EHM_NEVER)
        HWBuffer->GetBuffer()->setHardwareMappingHint(scene::EHM_STATIC);

    {
        if (HWBuffer->getChangeID(E_HARDWARE_BUFFER_TYPE::EHBT_VERTEX) != HWBuffer->GetBuffer()->getChangedID_Vertex())
        {
            if (!updateVertexHardwareBuffer(static_cast<CVulkanHardwareBuffer*>(HWBuffer), E_HARDWARE_BUFFER_TYPE::EHBT_VERTEX))
                return false;

            HWBuffer->setChangeID(E_HARDWARE_BUFFER_TYPE::EHBT_VERTEX, HWBuffer->GetBuffer()->getChangedID_Vertex());
        }

        if (HWBuffer->GetInstanceBuffer() && HWBuffer->getChangeID(E_HARDWARE_BUFFER_TYPE::EHBT_VERTEX_INSTANCE_STREAM) != HWBuffer->GetInstanceBuffer()->getChangedID())
        {
            if (!updateVertexHardwareBuffer(static_cast<CVulkanHardwareBuffer*>(HWBuffer), E_HARDWARE_BUFFER_TYPE::EHBT_VERTEX_INSTANCE_STREAM))
                return false;

            HWBuffer->setChangeID(E_HARDWARE_BUFFER_TYPE::EHBT_VERTEX_INSTANCE_STREAM, HWBuffer->GetInstanceBuffer()->getChangedID());
        }
    }

    {
        if (HWBuffer->getChangeID(E_HARDWARE_BUFFER_TYPE::EHBT_INDEX) != HWBuffer->GetBuffer()->getChangedID_Index())
        {
            if (!updateIndexHardwareBuffer(static_cast<CVulkanHardwareBuffer*>(HWBuffer)))
                return false;

            HWBuffer->setChangeID(E_HARDWARE_BUFFER_TYPE::EHBT_INDEX, HWBuffer->GetBuffer()->getChangedID_Vertex());
        }
    }

    return true;
}

IHardwareBuffer * irr::video::CVulkanDriver::createHardwareBuffer(const scene::IMeshBuffer * mb)
{
    if (!mb)
        return 0;

    CVulkanHardwareBuffer *hwBuffer = new CVulkanHardwareBuffer(this, const_cast<scene::IMeshBuffer*>(mb), const_cast<scene::IMeshBuffer*>(mb)->GetHWInstanceBuffer(),
        mb->getIndexType() == video::EIT_16BIT ? (u32)E_HARDWARE_BUFFER_FLAGS::EHBF_INDEX_16_BITS : (u32)E_HARDWARE_BUFFER_FLAGS::EHBF_INDEX_32_BITS, mb->getVertexType());

    CVulkanGLSLProgram* vkShader = mb->GetGPUProgram() ? static_cast<CVulkanGLSLProgram*>(mb->GetGPUProgram()) : m_defaultShader;

    hwBuffer->SetCommandBuffer(mMainCommandBuffer);

    hwBuffer->SetPipeline(0, new VulkanGraphicsPipelineState(GetMaterial(), vkShader, static_cast<CVulkanVertexDeclaration*>(GetVertexDeclaration(mb->getVertexType())), GpuDeviceFlags::GDF_PRIMARY));
    hwBuffer->GetPipeline(0)->initialize();
    
    hwBuffer->SetGpuParams(new VulkanGpuParams(this, vkShader, GpuDeviceFlags::GDF_PRIMARY));
    hwBuffer->GetGpuParams()->initialize();
    hwBuffer->GetGpuParams()->drop();

    if (!updateHardwareBuffer(hwBuffer))
    {
        deleteHardwareBuffer(hwBuffer);
        return 0;
    }
    
    hwBuffer->Initialize();
    return hwBuffer;
}

void irr::video::CVulkanDriver::deleteHardwareBuffer(IHardwareBuffer * HWBuffer)
{
    if (!HWBuffer)
        return;

    CNullDriver::deleteHardwareBuffer(HWBuffer);
}

void irr::video::CVulkanDriver::InitDrawStates(CVulkanHardwareBuffer* mb, scene::E_PRIMITIVE_TYPE pType)
{
    irr::video::CVulkanGLSLProgram* _vkShader = static_cast<irr::video::CVulkanGLSLProgram*>(getActiveGPUProgram());

    VulkanGraphicsPipelineState* _bindGraphicsPipeline = nullptr;
    if (mb)
    {
        _bindGraphicsPipeline = mb->GetPipeline(0);
        mMainCommandBuffer->getInternal()->setPipelineState(mb->GetPipeline(0));
        mMainCommandBuffer->getInternal()->setGpuParams(mb->GetGpuParams());
        mMainCommandBuffer->getInternal()->setDrawOp(pType);
    }

    for (int i = 0; i < MATERIAL_MAX_TEXTURES; ++i)
    {
        if (CurrentTexture[i])
        {
            mMainCommandBuffer->getInternal()->getGpuParams()->setTexture(0, i, CurrentTexture[i], static_cast<const CVulkanTexture*>(CurrentTexture[i])->GetSurface());
            mMainCommandBuffer->getInternal()->getGpuParams()->setSamplerState(0, i, _bindGraphicsPipeline->getSampler(0, i));
        }
        else
        {
            static TextureSurface emptySurface;
            mMainCommandBuffer->getInternal()->getGpuParams()->setTexture(0, i, nullptr, emptySurface);
        }
    }
}

void irr::video::CVulkanDriver::drawMeshBuffer(const scene::IMeshBuffer * mb, scene::IMesh * mesh, scene::ISceneNode * node)
{
    if (!mb || !mb->getIndexCount())
        return;

    scene::E_PRIMITIVE_TYPE pType = mb->getRenderPrimitive();
    CVulkanHardwareBuffer* HWBuffer = static_cast<CVulkanHardwareBuffer*>(mb->GetHWBuffer());

    //if (mb->Is3DMode())
    //{
    //    auto& m = (core::matrix4&)glm::perspective(core::PI / 2.5f, (float)getCurrentRenderTargetSize().Width / (float)getCurrentRenderTargetSize().Height, 0.1f, 3000.0f);
    //    m.setScale(irr::core::vector3df(1.f, -1.f, 1.f));
    //    setTransform(ETS_PROJECTION, m);
    //}


    if (!mb->GetGPUProgram())
    {
        activeGPUProgram(m_defaultShader);
    }
    else
    {
        activeGPUProgram(mb->GetGPUProgram());
    }

    _IRR_DEBUG_BREAK_IF(!getActiveGPUProgram());

    if (mb->Is3DMode())
    {
        if (!setRenderStates3DMode())
            return;
    }
    else
    {
        if (Material.MaterialType == EMT_ONETEXTURE_BLEND)
        {
            E_BLEND_FACTOR srcFact;
            E_BLEND_FACTOR dstFact;
            E_MODULATE_FUNC modulo;
            u32 alphaSource;
            unpack_textureBlendFunc(srcFact, dstFact, modulo, alphaSource, Material.uMaterialTypeParam);
            setRenderStates2DMode(alphaSource & video::EAS_VERTEX_COLOR, (Material.getTexture(0) != 0), (alphaSource&video::EAS_TEXTURE) != 0);
        }
        else
            setRenderStates2DMode(Material.MaterialType == EMT_TRANSPARENT_VERTEX_ALPHA, (Material.getTexture(0) != 0), Material.MaterialType == EMT_TRANSPARENT_ALPHA_CHANNEL);
    }

    if (getActiveGPUProgram())
        getActiveGPUProgram()->UpdateValues(video::IShaderDataBuffer::EUT_PER_FRAME_PER_MATERIAL, (scene::IMeshBuffer*)mb, mesh, node, video::IShaderDataBuffer::EUF_COMMIT_VALUES);

    if (HWBuffer)
    {
        if (HWBuffer->GetInstanceBuffer())
            HWBuffer->GetInstanceBuffer()->UpdateBuffer(getActiveGPUProgram(), HWBuffer->GetBuffer(), mesh, node, 0);
        updateHardwareBuffer(HWBuffer); //check if update is needed

        if (!HWBuffer->IsBinded() || !HWBuffer->IsManualBind())
        {
            if (getActiveGPUProgram())
                getActiveGPUProgram()->UpdateValues(video::IShaderDataBuffer::EUT_PER_FRAME_PER_MESH, (scene::IMeshBuffer*)mb, mesh, node, video::IShaderDataBuffer::EUF_COMMIT_VALUES);
            HWBuffer->Bind();
        }
}
    else
    {
        // copy vertices to dynamic buffers, if needed
        uploadVertexData(mb->getVertices(), mb->getVertexCount(), mb->getIndices(), mb->getIndexCount(), mb->getVertexType(), mb->getIndexType());
    }

#ifdef _DEBUG
    if (!checkPrimitiveCount(mb->getVertexCount()))
        return;
#endif

    HWBuffer->GetPipeline(0)->update(0, GetMaterial(), pType);

    InitDrawStates(HWBuffer, pType);
    SyncShaderConstant(HWBuffer->GetCommandBuffer());

    u32 instanceCount = (!mesh || mesh->IsInstanceModeAvailable()) && HWBuffer && HWBuffer->GetInstanceBuffer() ? HWBuffer->GetInstanceBuffer()->getInstanceCount() : 1;

    HWBuffer->GetCommandBuffer()->getInternal()->drawIndexed(mb->GetIndexRangeStart(), mb->GetIndexRangeCount(), 0, instanceCount);

    if (HWBuffer && !HWBuffer->IsManualBind())
        HWBuffer->Unbind();
}

void irr::video::CVulkanDriver::drawHardwareBuffer(IHardwareBuffer * HWBuffer, scene::IMesh * mesh, scene::ISceneNode * node)
{
}

void irr::video::CVulkanDriver::drawHardwareBuffer(IHardwareBuffer * vertices, IHardwareBuffer * indices, E_VERTEX_TYPE vType, scene::E_PRIMITIVE_TYPE pType, E_INDEX_TYPE iType, u32 numInstances)
{
}

void irr::video::CVulkanDriver::drawAuto(IHardwareBuffer * vertices, E_VERTEX_TYPE vType, scene::E_PRIMITIVE_TYPE pType)
{
}

void irr::video::CVulkanDriver::drawVertexPrimitiveList(const void * vertices, u32 vertexCount, const void * indexList, u32 primitiveCount, E_VERTEX_TYPE vType, scene::E_PRIMITIVE_TYPE pType, E_INDEX_TYPE iType)
{
}

void irr::video::CVulkanDriver::draw2DVertexPrimitiveList(const void * vertices, u32 vertexCount, const void * indexList, u32 primitiveCount, E_VERTEX_TYPE vType, scene::E_PRIMITIVE_TYPE pType, E_INDEX_TYPE iType)
{
    if (!getActiveGPUProgram())
        activeGPUProgram(m_defaultShader);

    _IRR_DEBUG_BREAK_IF(!getActiveGPUProgram());

    size_t indexCount = VulkanUtility::getIndexAmount(pType, primitiveCount);

    if (!indexCount)
        return;

    uploadVertexData(vertices, vertexCount, indexList, indexCount, vType, iType);

    if (getActiveGPUProgram())
        getActiveGPUProgram()->UpdateValues(video::IShaderDataBuffer::EUT_PER_FRAME_PER_MATERIAL, nullptr, nullptr, nullptr, video::IShaderDataBuffer::EUF_COMMIT_VALUES);
    if (getActiveGPUProgram())
        getActiveGPUProgram()->UpdateValues(video::IShaderDataBuffer::EUT_PER_FRAME_PER_MESH, nullptr, nullptr, nullptr, video::IShaderDataBuffer::EUF_COMMIT_VALUES);

#ifdef _DEBUG
    if (!checkPrimitiveCount(vertexCount))
        return;
#endif

    DynamicHardwareBuffer->GetPipeline(0)->update(0, GetMaterial(), pType);

    DynamicHardwareBuffer->Bind();

    InitDrawStates(DynamicHardwareBuffer, pType);
    SyncShaderConstant(DynamicHardwareBuffer->GetCommandBuffer());

    DynamicHardwareBuffer->GetCommandBuffer()->getInternal()->drawIndexed(0, indexCount, 0, 1);

    DynamicHardwareBuffer->Unbind();
}

bool irr::video::CVulkanDriver::initOutput(HWND hwnd, bool pureSoftware)
{
    return false;
}

const wchar_t * irr::video::CVulkanDriver::getName() const
{
    return L"Vulkan 1.0";
}

void irr::video::CVulkanDriver::deleteAllDynamicLights()
{
}

s32 irr::video::CVulkanDriver::addDynamicLight(const SLight & light)
{
    return s32();
}

void irr::video::CVulkanDriver::turnLightOn(s32 lightIndex, bool turnOn)
{
}

u32 irr::video::CVulkanDriver::getMaximalDynamicLightAmount() const
{
    return u32();
}

void irr::video::CVulkanDriver::setAmbientLight(const SColorf & color)
{
}

void irr::video::CVulkanDriver::drawStencilShadowVolume(const core::array<core::vector3df>& triangles, bool zfail, u32 debugDataVisible)
{
}

void irr::video::CVulkanDriver::drawStencilShadow(bool clearStencilBuffer, video::SColor leftUpEdge, video::SColor rightUpEdge, video::SColor leftDownEdge, video::SColor rightDownEdge)
{
}

u32 irr::video::CVulkanDriver::getMaximalPrimitiveCount() const
{
    return 0x7fffffff;
}

void irr::video::CVulkanDriver::setTextureCreationFlag(E_TEXTURE_CREATION_FLAG flag, bool enabled)
{
}

void irr::video::CVulkanDriver::setFog(SColor color, E_FOG_TYPE fogType, f32 start, f32 end, f32 density, bool pixelFog, bool rangeFog)
{
}

void irr::video::CVulkanDriver::OnResize(const core::dimension2d<u32>& size)
{
    CNullDriver::OnResize(size);

    mPlatform->resizeSwapBuffers();
}

void irr::video::CVulkanDriver::setBasicRenderStates(const SMaterial & material, const SMaterial & lastMaterial, bool resetAllRenderstates)
{
}

E_DRIVER_TYPE irr::video::CVulkanDriver::getDriverType() const
{
    return E_DRIVER_TYPE::EDT_VULKAN;
}

const core::matrix4 & irr::video::CVulkanDriver::getTransform(E_TRANSFORMATION_STATE state) const
{
    return Matrices[state];
}

void irr::video::CVulkanDriver::setVertexShaderConstant(const f32 * data, s32 startRegister, s32 constantAmount)
{
}

void irr::video::CVulkanDriver::setPixelShaderConstant(const f32 * data, s32 startRegister, s32 constantAmount)
{
}

bool irr::video::CVulkanDriver::setVertexShaderConstant(const c8 * name, const f32 * floats, int count)
{
    return false;
}

bool irr::video::CVulkanDriver::setPixelShaderConstant(const c8 * name, const f32 * floats, int count)
{
    return false;
}

bool irr::video::CVulkanDriver::setStreamOutputBuffer(IHardwareBuffer * buffer)
{
    return false;
}

IVideoDriver * irr::video::CVulkanDriver::getVideoDriver()
{
    return this;
}

ITexture * irr::video::CVulkanDriver::addRenderTargetTexture(const core::dimension2d<u32>& size, const io::path & name, const ECOLOR_FORMAT format)
{
    return nullptr;
}

void irr::video::CVulkanDriver::clearZBuffer()
{
    mMainCommandBuffer->getInternal()->clearViewport(FrameBufferType::FBT_DEPTH | FrameBufferType::FBT_STENCIL, irr::video::SColor(255, 0, 0, 0), 1.0f, 1, 0xFF);
}

IImage * irr::video::CVulkanDriver::createScreenShot(video::ECOLOR_FORMAT format, video::E_RENDER_TARGET target)
{
    return nullptr;
}

bool irr::video::CVulkanDriver::setClipPlane(u32 index, const core::plane3df & plane, bool enable)
{
    if (index > 3)
        return false;

    ClipPlanes[index] = plane;

    enableClipPlane(index, enable);

    return true;
}

void irr::video::CVulkanDriver::enableClipPlane(u32 index, bool enable)
{
    ClipPlaneEnabled[index] = enable;
}

void irr::video::CVulkanDriver::getClipPlane(u32 index, core::plane3df & plane, bool & enable)
{
    plane = ClipPlanes[index];
    enable = ClipPlaneEnabled[index];
}

void irr::video::CVulkanDriver::enableMaterial2D(bool enable)
{
    CNullDriver::enableMaterial2D(enable);
}

ECOLOR_FORMAT irr::video::CVulkanDriver::getColorFormat() const
{
    return ColorFormat;
}

core::dimension2du irr::video::CVulkanDriver::getMaxTextureSize() const
{
    return core::dimension2du(16384, 16384);
}

IHardwareBuffer * irr::video::CVulkanDriver::createHardwareBuffer(E_HARDWARE_BUFFER_TYPE type, E_HARDWARE_BUFFER_ACCESS accessType, u32 size, u32 flags, const void * initialData)
{
    return new CVulkanHardwareBuffer(this, type, accessType, size, flags, initialData);
}

video::VertexDeclaration * irr::video::CVulkanDriver::createVertexDeclaration()
{
    return new CVulkanVertexDeclaration(this);
}

E_VERTEX_TYPE irr::video::CVulkanDriver::registerVertexType(core::array<SVertexElement>& elements)
{
    return E_VERTEX_TYPE();
}

u32 irr::video::CVulkanDriver::queryMultisampleLevels(ECOLOR_FORMAT format, u32 numSamples) const
{
    return u32();
}

bool irr::video::CVulkanDriver::setShaderConstant(ShaderVariableDescriptor const * desc, const void * values, int count, IHardwareBuffer * buffer)
{
    irr::video::CVulkanGLSLProgram* hlsl = static_cast<irr::video::CVulkanGLSLProgram*>(getActiveGPUProgram());

    if (desc->m_type == ESVT_INPUT_STREAM)
        return false;

    irr::video::CVulkanGLSLProgram::CbufferDesc& cbuffer = *hlsl->GetBlockBufferInfos()[desc->m_shaderIndex];
    irr::video::CVulkanGLSLProgram::ShaderConstantDesc const& constantDecl = cbuffer.Variables[desc->m_location & 0xFF];

    UINT elementSize = constantDecl.elementSize * count;

    _IRR_DEBUG_BREAK_IF(elementSize > constantDecl.BufferDesc->DataBuffer.size() || (cbuffer.DataBuffer.size() < (constantDecl.offset + elementSize)));

    if (memcmp(&cbuffer.DataBuffer[constantDecl.offset], values, elementSize))
    {
        if (cbuffer.ChangeStartOffset > constantDecl.offset)
            cbuffer.ChangeStartOffset = constantDecl.offset;

        if (cbuffer.ChangeEndOffset < (constantDecl.offset + elementSize))
            cbuffer.ChangeEndOffset = (constantDecl.offset + elementSize);

        memcpy(&cbuffer.DataBuffer[constantDecl.offset], values, elementSize);
        ++cbuffer.ChangeId;
    }
    return true;
}

core::array<u8> irr::video::CVulkanDriver::GetShaderVariableMemoryBlock(ShaderVariableDescriptor const * desc, video::IShader * shader)
{
    return core::array<u8>();
}

bool irr::video::CVulkanDriver::setShaderMapping(ShaderVariableDescriptor const * desc, IShader * shader, scene::E_HARDWARE_MAPPING constantMapping)
{
    irr::video::CVulkanGLSLProgram* _vkShader = static_cast<irr::video::CVulkanGLSLProgram*>(shader);

    if (!desc || desc->m_type == ESVT_INPUT_STREAM)
        return false;

    const auto& cbuffer = _vkShader->ShaderBuffers[desc->m_shaderIndex];
    cbuffer->setHardwareMappingHint(constantMapping);

    return true;
}

bool irr::video::CVulkanDriver::SyncShaderConstant(VulkanCommandBuffer* cb)
{
    irr::video::CVulkanGLSLProgram* _vkShader = static_cast<irr::video::CVulkanGLSLProgram*>(getActiveGPUProgram());
    _IRR_DEBUG_BREAK_IF(!_vkShader);

    VulkanGpuParams* gpuParams = cb->getInternal()->getGpuParams();

    for (u32 ib = 0; ib != _vkShader->ShaderBuffers.size(); ++ib)
    {
        irr::video::CVulkanGLSLProgram::CbufferDesc& cbuffer = *_vkShader->GetBlockBufferInfos()[ib];

        if (cbuffer.getHardwareMappingHint() == scene::EHM_NEVER)
            cbuffer.setHardwareMappingHint(scene::EHM_STATIC);

        {
            if (cbuffer.getHardwareBuffer()->getChangeID(E_HARDWARE_BUFFER_TYPE::EHBT_CONSTANTS) != cbuffer.getChangeId())
            {
                E_HARDWARE_BUFFER_ACCESS MemoryAccess = E_HARDWARE_BUFFER_ACCESS::EHBA_DEFAULT;
                if (cbuffer.getHardwareMappingHint() == scene::E_HARDWARE_MAPPING::EHM_DYNAMIC)
                    MemoryAccess = E_HARDWARE_BUFFER_ACCESS::EHBA_DYNAMIC;
                else if (cbuffer.getHardwareMappingHint() == scene::E_HARDWARE_MAPPING::EHM_STATIC)
                    MemoryAccess = E_HARDWARE_BUFFER_ACCESS::EHBA_IMMUTABLE;
                else if (cbuffer.getHardwareMappingHint() == scene::E_HARDWARE_MAPPING::EHM_STREAM)
                    MemoryAccess = E_HARDWARE_BUFFER_ACCESS::EHBA_SYSTEM_MEMORY;

                if (cbuffer.getHardwareBuffer()->UpdateBuffer(E_HARDWARE_BUFFER_TYPE::EHBT_CONSTANTS, MemoryAccess, cbuffer.DataBuffer.pointer(),
                    cbuffer.DataBuffer.size(), cbuffer.ChangeStartOffset, cbuffer.ChangeEndOffset))
                {
                    cbuffer.ChangeStartOffset = cbuffer.DataBuffer.size();
                    cbuffer.ChangeEndOffset = 0;
                    cbuffer.getHardwareBuffer()->setChangeID(E_HARDWARE_BUFFER_TYPE::EHBT_CONSTANTS, cbuffer.ChangeId);
                }
            }
        }

        gpuParams->setBuffer(0, cbuffer.varDesc.m_shaderIndex, cbuffer.getHardwareBuffer());
        cb->getInternal()->registerResource(cbuffer.getHardwareBuffer()->GetBufferDesc(E_HARDWARE_BUFFER_TYPE::EHBT_CONSTANTS)->Buffer, VulkanUseFlag::Read);
    }

    return true;
}

IShader * irr::video::CVulkanDriver::createShader(System::IO::IFileReader * vertexShader, System::IO::IFileReader * fragmentShader, System::IO::IFileReader * geometryShader, System::IO::IFileReader * tesselationShader)
{
    irr::video::CVulkanGLSLProgram* gpuProgram = new irr::video::CVulkanGLSLProgram(this, EST_HIGH_LEVEL_SHADER);
    if (vertexShader)
    {
        const char* ShaderEntryPoint = "main";
        gpuProgram->CreateShaderModul(E_ShaderTypes::EST_VERTEX_SHADER, this, vertexShader, ShaderEntryPoint);
    }

    if (geometryShader)
    {
        const char* ShaderEntryPoint = "main";
        gpuProgram->CreateShaderModul(E_ShaderTypes::EST_GEOMETRY_SHADER, this, geometryShader, ShaderEntryPoint);
    }

    if (fragmentShader)
    {
        const char* ShaderEntryPoint = "main";
        gpuProgram->CreateShaderModul(E_ShaderTypes::EST_FRAGMENT_SHADER, this, fragmentShader, ShaderEntryPoint);
    }

    gpuProgram->Init();
    return gpuProgram;
}

void irr::video::CVulkanDriver::useShader(IShader * gpuProgram)
{
}

void irr::video::CVulkanDriver::HandleDeviceLost()
{
}

void irr::video::CVulkanDriver::createMaterialRenderers()
{
}

void irr::video::CVulkanDriver::draw2D3DVertexPrimitiveList(const void * vertices, u32 vertexCount, const void * indexList, u32 primitiveCount, E_VERTEX_TYPE vType, scene::E_PRIMITIVE_TYPE pType, E_INDEX_TYPE iType, bool is3D)
{
}

bool irr::video::CVulkanDriver::setRenderStates3DMode()
{
    LastMaterial = Material;

    ResetRenderStates = false;

    CurrentRenderMode = ERM_3D;

    return true;
}

void irr::video::CVulkanDriver::setRenderStates2DMode(bool alpha, bool texture, bool alphaChannel)
{
    if (CurrentRenderMode != ERM_2D || Transformation3DChanged)
    {
        // unset last 3d material
        if (CurrentRenderMode == ERM_3D)
        {
            if (static_cast<u32>(LastMaterial.MaterialType) < MaterialRenderers.size())
                MaterialRenderers[LastMaterial.MaterialType].Renderer->OnUnsetMaterial();
        }

        if (!OverrideMaterial2DEnabled)
        {
            //if (!DepthStencilStateChanged)
            //    DepthStencilStateChanged = true;

            setBasicRenderStates(InitMaterial2D, LastMaterial, true);
            LastMaterial = InitMaterial2D;

            //DepthStencilDesc.StencilEnable = FALSE;
        }

        // Set world to identity
        core::matrix4 m;
        setTransform(ETS_WORLD, m);

        // Set view to translate a little forward
        //m.setTranslation(core::vector3df(-0.5f, -0.5f, 0)ü);
        setTransform(ETS_VIEW, getTransform(ETS_VIEW_2D));

        // Adjust projection
        const core::dimension2d<u32>& renderTargetSize = getCurrentRenderTargetSize();

        m = (core::matrix4&)glm::ortho(0.0f, (f32)renderTargetSize.Width, 0.0f, (f32)renderTargetSize.Height, 0.0f, 1.0f);
        Matrices[ETS_PROJECTION] = m;

        Transformation3DChanged = false;
    }

    // no alphaChannel without texture
    alphaChannel &= texture;

    if (texture)
    {
        setTransform(ETS_TEXTURE_0, core::IdentityMatrix);
        // Due to the transformation change, the previous line would call a reset each frame
        // but we can safely reset the variable as it was false before
        Transformation3DChanged = false;
    }

    if (OverrideMaterial2DEnabled)
    {
        OverrideMaterial2D.Lighting = false;
        OverrideMaterial2D.ZBuffer = ECFN_NEVER;
        OverrideMaterial2D.ZWriteEnable = false;
        setBasicRenderStates(OverrideMaterial2D, LastMaterial, false);
        LastMaterial = OverrideMaterial2D;
    }

    // handle alpha
    if (alpha || alphaChannel)
    {
        Material.BlendOperation = E_BLEND_OPERATION::EBO_ADD;
        Material.uMaterialTypeParam = pack_textureBlendFunc(video::EBF_SRC_ALPHA, video::EBF_ONE_MINUS_SRC_ALPHA, video::EMFN_MODULATE_1X, video::EAS_TEXTURE | video::EAS_VERTEX_COLOR, video::EBF_SRC_ALPHA, video::EBF_ONE_MINUS_SRC_ALPHA, E_BLEND_OPERATION::EBO_ADD, E_BLEND_OPERATION::EBO_ADD);
    }
    else
        Material.BlendOperation = E_BLEND_OPERATION::EBO_NONE;

    CurrentRenderMode = ERM_2D;
}

bool irr::video::CVulkanDriver::setActiveTexture(u32 stage, const video::ITexture * texture)
{
    if (texture && texture->getDriverType() != EDT_VULKAN)
    {
        os::Printer::log("Fatal Error: Tried to set a texture not owned by this driver.", ELL_ERROR);
        return false;
    }

    // For first stage, set NullTexture if texture is NULL
    CurrentTexture[stage] = (stage == 0 && texture == 0) ? nullptr : texture;

    return true;

}

video::ITexture * irr::video::CVulkanDriver::createDeviceDependentTexture(IImage * surface, const io::path & name, void * mipmapData)
{
    return new CVulkanTexture(surface, this, TextureCreationFlags, name, 1, mipmapData, 1, 0);
}

void irr::video::CVulkanDriver::checkDepthBuffer(ITexture * tex)
{
}

const core::dimension2d<u32>& irr::video::CVulkanDriver::getCurrentRenderTargetSize() const
{
    if (CurrentRendertargetSize.Width == 0)
        return ScreenSize;
    else
        return CurrentRendertargetSize;
}

bool irr::video::CVulkanDriver::reallocateDynamicBuffers(u32 vertexBufferSize, u32 indexBufferSize)
{
    return false;
}

bool irr::video::CVulkanDriver::uploadVertexData(const void * vertices, u32 vertexCount, const void * indexList, u32 indexCount, E_VERTEX_TYPE vType, E_INDEX_TYPE iType)
{
    if (!DynamicHardwareBuffer)
    {
        irr::video::CVulkanGLSLProgram* _vkShader = static_cast<irr::video::CVulkanGLSLProgram*>(getActiveGPUProgram());

        DynamicHardwareBuffer = new CVulkanHardwareBuffer(this, E_HARDWARE_BUFFER_TYPE::EHBT_VERTEX, E_HARDWARE_BUFFER_ACCESS::EHBA_DYNAMIC, 0);

        DynamicHardwareBuffer->UpdateBuffer(E_HARDWARE_BUFFER_TYPE::EHBT_VERTEX, E_HARDWARE_BUFFER_ACCESS::EHBA_DYNAMIC, vertices, getVertexPitchFromType(vType) * vertexCount);
        DynamicHardwareBuffer->UpdateBuffer(E_HARDWARE_BUFFER_TYPE::EHBT_INDEX, E_HARDWARE_BUFFER_ACCESS::EHBA_DYNAMIC, indexList, indexCount * (iType == E_INDEX_TYPE::EIT_32BIT ? sizeof(u32) : sizeof(u16)));

        DynamicHardwareBuffer->SetCommandBuffer(mMainCommandBuffer);
        DynamicHardwareBuffer->SetPipeline(0, new VulkanGraphicsPipelineState(GetMaterial(), static_cast<CVulkanGLSLProgram*>(getActiveGPUProgram()), static_cast<CVulkanVertexDeclaration*>(GetVertexDeclaration(vType)), GpuDeviceFlags::GDF_PRIMARY));
        DynamicHardwareBuffer->GetPipeline(0)->initialize();

        DynamicHardwareBuffer->SetGpuParams(_vkShader->GetDefaultGpuParams());
    }
    else
    {
        if (vertices)
        {
            DynamicHardwareBuffer->UpdateBuffer(E_HARDWARE_BUFFER_TYPE::EHBT_VERTEX, E_HARDWARE_BUFFER_ACCESS::EHBA_DYNAMIC, vertices, getVertexPitchFromType(vType) * vertexCount);
        }

        if (indexList)
        {
            DynamicHardwareBuffer->UpdateBuffer(E_HARDWARE_BUFFER_TYPE::EHBT_INDEX, E_HARDWARE_BUFFER_ACCESS::EHBA_DYNAMIC, indexList, indexCount * (iType == E_INDEX_TYPE::EIT_32BIT ? sizeof(u32) : sizeof(u16)));
        }
    }

    return true;
}

void irr::video::CVulkanDriver::reset()
{
}

namespace irr
{
    namespace video
    {
        //! creates a video driver
        IVideoDriver* createVulkanDriver(const SIrrlichtCreationParameters& params,
            io::IFileSystem* io, HWND hwnd)
        {
            bool pureSoftware = false;
            CVulkanDriver* driver = new CVulkanDriver(params, io);
            if (!driver->initDriver(hwnd, pureSoftware))
            {
                driver->drop();
                driver = 0;
            }

            return driver;
        }
    }
}