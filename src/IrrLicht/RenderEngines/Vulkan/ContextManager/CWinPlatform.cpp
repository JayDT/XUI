#include "CWinPlatform.h"
#include "CVulkanDriver.h"
#include "CVulkanDevice.h"
#include "CVulkanSwapChain.h"

//#include "ContextManager/CWvkManager.h"
//
//#ifdef _IRR_COMPILE_WITH_WINDOWS_DEVICE_
//
//#include "os.h"
//
//#include "CVulkanDriver.h"
//
//using namespace irr;
//using namespace irr::video;
//
//constexpr const char * const instance_validation_layers_alt1[] = { "VK_LAYER_LUNARG_standard_validation" };
//
//constexpr const char * const instance_validation_layers_alt2[] = { "VK_LAYER_GOOGLE_threading", "VK_LAYER_LUNARG_parameter_validation",
//                                                                   "VK_LAYER_LUNARG_object_tracker", "VK_LAYER_LUNARG_core_validation",
//                                                                   "VK_LAYER_GOOGLE_unique_objects" };
//
//irr::video::CWvkManager::CWvkManager()
//{
//}
//
//irr::video::CWvkManager::~CWvkManager()
//{
//}
//
//bool irr::video::CWvkManager::initialize(const SIrrlichtCreationParameters & params, const SExposedVideoData & data)
//{
//    if (!GenericVulkanContext::initialize(params, data))
//        return false;
//    return true;
//}
//
//void irr::video::CWvkManager::terminate()
//{
//}
//
//bool irr::video::CWvkManager::generateSurface()
//{
//    // get handle to exe file
//    HINSTANCE hInstance = GetModuleHandle(0);
//
//    auto const createInfo = vk::Win32SurfaceCreateInfoKHR().setHinstance(hInstance).setHwnd((HWND)Params.WindowId);
//
//    auto result = Instance.createWin32SurfaceKHR(&createInfo, nullptr, &surface);
//    EXCEPT(result == vk::Result::eSuccess);
//    return true;
//}
//
//void irr::video::CWvkManager::destroySurface()
//{
//}
//
//bool irr::video::CWvkManager::generateContext()
//{
//    if (!GenericVulkanContext::generateContext())
//        return false;
//
//    PrimaryContext.Vulkan.SwapChain = &swapchain;
//    PrimaryContext.Vulkan.Device = &device;
//    return true;
//}
//
//void irr::video::CWvkManager::destroyContext()
//{
//}
//
//const SExposedVideoData & irr::video::CWvkManager::getContext() const
//{
//    return PrimaryContext;
//}
//
//char ** irr::video::CWvkManager::InitEnvironment()
//{
//    uint32_t instance_extension_count = 0;
//    uint32_t instance_layer_count = 0;
//    uint32_t validation_layer_count = 0;
//    char **instance_validation_layers = NULL;
//    enabled_extension_count = 0;
//    enabled_layer_count = 0;
//
//    /* Look for validation layers */
//    vk::Bool32 validation_found = VK_FALSE;
//    if (Driver->IsDebugMode())
//    {
//        auto result = vk::enumerateInstanceLayerProperties(&instance_layer_count, nullptr);
//        EXCEPT(result == vk::Result::eSuccess);
//
//        instance_validation_layers = (char**)instance_validation_layers_alt1;
//        if (instance_layer_count > 0)
//        {
//            std::unique_ptr<vk::LayerProperties[]> instance_layers(new vk::LayerProperties[instance_layer_count]);
//            result = vk::enumerateInstanceLayerProperties(&instance_layer_count, instance_layers.get());
//            EXCEPT(result == vk::Result::eSuccess);
//
//            validation_found = cvk_check_layers(ARRAY_SIZE(instance_validation_layers_alt1), instance_validation_layers, instance_layer_count, instance_layers.get());
//            if (validation_found)
//            {
//                enabled_layer_count = ARRAY_SIZE(instance_validation_layers_alt1);
//                enabled_layers[0] = "VK_LAYER_LUNARG_standard_validation";
//                validation_layer_count = 1;
//            }
//            else
//            {
//                // use alternative set of validation layers
//                instance_validation_layers  = (char**)instance_validation_layers_alt2;
//                enabled_layer_count         = ARRAY_SIZE(instance_validation_layers_alt2);
//                validation_found            = cvk_check_layers(ARRAY_SIZE(instance_validation_layers_alt2), instance_validation_layers, instance_layer_count, instance_layers.get());
//                validation_layer_count      = ARRAY_SIZE(instance_validation_layers_alt2);
//
//                for (uint32_t i = 0; i < validation_layer_count; i++)
//                    enabled_layers[i] = instance_validation_layers[i];
//            }
//        }
//
//        if (!validation_found) {
//            os::Printer::log(
//                "vkEnumerateInstanceLayerProperties failed to find required validation layer.\n\n"
//                "Please look at the Getting Started guide for additional information.\n",
//                "vkCreateInstance Failure");
//        }
//    }
//    /* Look for instance extensions */
//    vk::Bool32 surfaceExtFound = VK_FALSE;
//    vk::Bool32 platformSurfaceExtFound = VK_FALSE;
//    memset(extension_names, 0, sizeof(extension_names));
//
//    auto result = vk::enumerateInstanceExtensionProperties(nullptr, &instance_extension_count, nullptr);
//    EXCEPT(result == vk::Result::eSuccess);
//
//    if (instance_extension_count > 0)
//    {
//        std::unique_ptr<vk::ExtensionProperties[]> instance_extensions(new vk::ExtensionProperties[instance_extension_count]);
//        result = vk::enumerateInstanceExtensionProperties(nullptr, &instance_extension_count, instance_extensions.get());
//        EXCEPT(result == vk::Result::eSuccess);
//
//        for (uint32_t i = 0; i < instance_extension_count; i++)
//        {
//            if (!strcmp(VK_KHR_SURFACE_EXTENSION_NAME, instance_extensions[i].extensionName))
//            {
//                surfaceExtFound = 1;
//                extension_names[enabled_extension_count++] = VK_KHR_SURFACE_EXTENSION_NAME;
//            }
//
//            if (!strcmp(VK_KHR_WIN32_SURFACE_EXTENSION_NAME, instance_extensions[i].extensionName)) {
//                platformSurfaceExtFound = 1;
//                extension_names[enabled_extension_count++] = VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
//            }
//
//            assert(enabled_extension_count < 64);
//        }
//    }
//
//    if (!surfaceExtFound)
//    {
//        os::Printer::log("vkEnumerateInstanceExtensionProperties failed to find the " VK_KHR_SURFACE_EXTENSION_NAME
//            " extension.\n\n"
//            "Do you have a compatible Vulkan installable client driver (ICD) installed?\n"
//            "Please look at the Getting Started guide for additional information.\n",
//            "vkCreateInstance Failure");
//        return nullptr;
//    }
//
//    if (!platformSurfaceExtFound)
//    {
//        os::Printer::log("vkEnumerateInstanceExtensionProperties failed to find the " VK_KHR_WIN32_SURFACE_EXTENSION_NAME
//            " extension.\n\n"
//            "Do you have a compatible Vulkan installable client driver (ICD) installed?\n"
//            "Please look at the Getting Started guide for additional information.\n",
//            "vkCreateInstance Failure");
//        return nullptr;
//    }
//
//    return instance_validation_layers;
//}
//
//#endif


//! Destructor

irr::video::CWinVulkanPlatform::~CWinVulkanPlatform()
{
}

void irr::video::CWinVulkanPlatform::initialize()
{
    // get handle to exe file
    HINSTANCE hInstance = GetModuleHandle(0);

    // Create Vulkan surface
    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo;
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.pNext = nullptr;
    surfaceCreateInfo.flags = 0;
    surfaceCreateInfo.hwnd = (HWND)mDriver->mCreateParams.WindowId;
    surfaceCreateInfo.hinstance = hInstance;

    VkInstance instance = mDriver->_getInstance();
    VkResult result = vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, VulkanDevice::gVulkanAllocator, &mSurface);
    assert(result == VK_SUCCESS);

    VulkanDevice* presentDevice = mDriver->_getPrimaryDevice();
    VkPhysicalDevice physicalDevice = presentDevice->getPhysical();

    mPresentQueueFamily = presentDevice->getQueueFamily(GQT_GRAPHICS);

    VkBool32 supportsPresent;
    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, mPresentQueueFamily, mSurface, &supportsPresent);

    if (!supportsPresent)
    {
        // Note: Not supporting present only queues at the moment
        // Note: Also present device can only return one family of graphics queue, while there could be more (some of
        // which support present)
        throw std::runtime_error("Cannot find a graphics queue that also supports present operations.");
    }

    SurfaceFormat format = presentDevice->getSurfaceFormat(mSurface, false);
    mColorFormat = format.colorFormat;
    mColorSpace = format.colorSpace;
    mDepthFormat = format.depthFormat;

    // Create swap chain
    mSwapChain = new VulkanSwapChain();
    mSwapChain->rebuild(presentDevice, mSurface, mDriver->getScreenSize().Width, mDriver->getScreenSize().Height, mDriver->mCreateParams.Vsync, mColorFormat, mColorSpace, mDriver->mCreateParams.Stencilbuffer, mDepthFormat);

    // Make the window full screen if required
    if (mDriver->mCreateParams.Fullscreen)
    {
        if (!mIsFullScreen)
        {
            DEVMODE displayDeviceMode;
    
            memset(&displayDeviceMode, 0, sizeof(displayDeviceMode));
            displayDeviceMode.dmSize = sizeof(DEVMODE);
            displayDeviceMode.dmBitsPerPel = 32;
            displayDeviceMode.dmPelsWidth = mDriver->getScreenSize().Width;
            displayDeviceMode.dmPelsHeight = mDriver->getScreenSize().Height;
            displayDeviceMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
    
            //if (mDisplayFrequency)
            //{
            //    displayDeviceMode.dmDisplayFrequency = mDisplayFrequency;
            //    displayDeviceMode.dmFields |= DM_DISPLAYFREQUENCY;
            //
            //    if (ChangeDisplaySettingsEx(NULL, &displayDeviceMode, NULL, CDS_FULLSCREEN | CDS_TEST, NULL) != DISP_CHANGE_SUCCESSFUL)
            //    {
            //        assert("ChangeDisplaySettings with user display frequency failed.");
            //    }
            //}
    
            if (ChangeDisplaySettingsEx(NULL, &displayDeviceMode, NULL, CDS_FULLSCREEN, NULL) != DISP_CHANGE_SUCCESSFUL)
            {
                assert("ChangeDisplaySettings failed.");
            }
        }
    }
}

void irr::video::CWinVulkanPlatform::resizeSwapBuffers()
{
    // Resize swap chain

    //// Need to make sure nothing is using the swap buffer before we re-create it
    // Note: Optionally I can detect exactly on which queues (if any) are the swap chain images used on, and only wait
    // on those

    mDriver->_getPrimaryDevice()->waitIdle();
    mSwapChain->rebuild(mDriver->_getPrimaryDevice(), mSurface, mDriver->getScreenSize().Width, mDriver->getScreenSize().Height, mDriver->mCreateParams.Vsync, mColorFormat, mColorSpace, mDriver->mCreateParams.Stencilbuffer, mDepthFormat);
}
