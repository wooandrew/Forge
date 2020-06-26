// TheForge - src/core/core (c) Andrew Woo, 2020

#pragma warning(disable : 26812)

#include "core/core.hpp"
#include "forge_vars.hpp"

namespace Forge::Core {

    // Default constructor
    EngineCore::EngineCore() {

        instance = VK_NULL_HANDLE;          // Prepare Vulkan instance for initialization
        surface = VK_NULL_HANDLE;           // Prepare Vulkan surface for initialization

        dbgMessenger = VK_NULL_HANDLE;      // Prepare debug messenger for initialization

        gpu = std::make_shared<Core::GraphicsCard>();
    }

    // Default destructor
    EngineCore::~EngineCore() {

    }

    void EngineCore::cleanup() {

        gpu->cleanup();         // Cleanup graphics card object

        vkDestroySurfaceKHR(instance, surface, nullptr);        // Destroy Vulkan surface

        if (DEBUG_MODE)                                                             // If DEBUG_MODE is enabled
            DestroyDebugUtilsMessengerEXT(instance, dbgMessenger, nullptr);         // Destroy Debug Message handler

        vkDestroyInstance(instance, nullptr);       // Destroy Vulkan instance
    }

    // Initialize engine core
    int EngineCore::init(GLFWwindow* window) {

        if (DEBUG_MODE && !AllValidationLayersSupported()) {                                                    // If DEBUG_MODE is enabled and the required validation layers are not found
            ASWL::utilities::Logger("EC0V0", "Fatal Error: Requested validation layers were not found.");       // then log the error
            return 1;                                                                                           // then return the corresponding error value
        }

        // TheForge version
        int tfMajor = coredata.ForgeVersion.MAJOR;
        int tfMinor = coredata.ForgeVersion.MINOR;
        int tfPatch = coredata.ForgeVersion.PATCH;

        // Application version
        int apMajor = coredata.AppVersion.MAJOR;
        int apMinor = coredata.AppVersion.MINOR;
        int apPatch = coredata.AppVersion.PATCH;

        // Initalize Vulkan Instance
        VkApplicationInfo appInfo = {};                                                 // appInfo is filled with information about the application
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;                             // Indentify appInfo as structure type APPLICATION_INFO
        appInfo.pNext = nullptr;                                                        // Pointer to list of structures
        appInfo.pApplicationName = coredata.vkAppName;                                  // Set the vk application name
        appInfo.applicationVersion = VK_MAKE_VERSION(apMajor, apMinor, apPatch);        // Set the vk application version
        appInfo.pEngineName = "TheForge";                                               // Set engine to TheForge
        appInfo.engineVersion = VK_MAKE_VERSION(tfMajor, tfMinor, tfPatch);             // Set engine version
        appInfo.apiVersion = VK_API_VERSION_1_0;                                        // Set minimum Vulkan API version required to run application

        VkInstanceCreateInfo createInfo = {};                           // createInfo passes which extensions GLFW requires from Vulkan 
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;      // Identify createInfo as structure type INSTANCE_CREATE_INFO
        //createInfo.flags = 0;                                         // Reserved for future use
        createInfo.pApplicationInfo = &appInfo;                         // Pass appInfo as reference

        auto extensions = GetRequiredExtensions();                                          // Get required extensions
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());        // Number of enabled extensions
        createInfo.ppEnabledExtensionNames = extensions.data();                             // Set extensions

        if (DEBUG_MODE) {   // If DEBUG_MODE is enabled

            VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;         // debugCreateInfo contains parameters for the debug messenger

            createInfo.enabledLayerCount = static_cast<uint32_t>(ValidationLayers.size());      // Set number of global validation layers to enable
            createInfo.ppEnabledLayerNames = ValidationLayers.data();                           // Pass names of global validation layers to enable

            PopulateDebugMessengerCreateInfo(debugCreateInfo);                                  // Populates debug create info
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;          // Pointer to extension-function structure
        }
        else {
            createInfo.enabledLayerCount = 0;       // Set number of global validation layers to enable
            createInfo.pNext = nullptr;             // Set pointer to debug create info to null
        }

        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {                                      // If vkInstance creation fails
            ASWL::utilities::Logger("EC1V1", "Fatal Error: Failed to create instance -> vkCreateInstance().");      // then log the error
            return 2;                                                                                               // and return corresponding error value
        }

        SetupDebugMessenger();

        if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {           // If vkSurface creation fails
            ASWL::utilities::Logger("EC2S0", "Fatal Error: Window surface creation failed.");       // then log the error
            return 3;                                                                               // and return corresponding error value
        }

        // Initialize GPU object
        gpu->init(instance, surface);

        return 0;
    }


    VkPhysicalDevice& EngineCore::GetPGPU() {
        return gpu->PGPU;
    }
    VkDevice& EngineCore::GetLGPU() {
        return gpu->LGPU;
    }

    // Return the extensions required by application
    std::vector<const char*> EngineCore::GetRequiredExtensions() {

        // Get extensions GLFW requires from Vulkan
        uint32_t glfwExtensionCount = 0;                                                // Number of required extensions
        const char** glfwExtensions;                                                    // Initialize list of required extensions

        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);        // Get instance extensions required by GLFW

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);    // Push every GLFW extension into the required extensions list

        // If in debug mode, push validation layer extensions as well
        if (DEBUG_MODE)
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        // Return the required extensions
        return extensions;
    }

    // Check if validation layers are supported
    bool EngineCore::AllValidationLayersSupported() {

        uint32_t layerCount;                                            // Initalize layerCount
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);       // populate layerCount

        std::vector<VkLayerProperties> availableLayers(layerCount);                     // Intialize layersAvailable with size layerCount
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());        // then populate layersAvailable

        // Iterate through all existing validation layers
        for (const char* layerName : ValidationLayers) {

            bool layerFound = false;

            // Iterate through available layers
            for (const auto& layerProperties : availableLayers) {

                if (strcmp(layerName, layerProperties.layerName) == 0) {        // If existing validation layers is found in available validation layers
                    layerFound = true;                                          // Set layers found to true, and break the nested for loop, continue
                    break;
                }
            }

            if (!layerFound)        // If a validation layer is not found
                return false;       // End the check and return false
        }

        return true;        // If all validation layers are found, return true
    }

    // Debug handler setup
    void EngineCore::SetupDebugMessenger() {

        if (!DEBUG_MODE)        // If DEBUG_MODE is disabled
            return;             // then do not setup debug messenger

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;     // Struct containing data for debug messenger object
        PopulateDebugMessengerCreateInfo(debugCreateInfo);      // Populate the struct

        if (CreateDebugUtilsMessengerEXT(instance, &debugCreateInfo, nullptr, &dbgMessenger) != VK_SUCCESS)     // If DebuMessenger creation fails
            ASWL::utilities::Logger("EC3V2", "Error: Failed to setup debug messenger.");                        // Log the message
    }

    VkResult EngineCore::CreateDebugUtilsMessengerEXT(VkInstance instance,                                  // Vulkan Instance
                                                const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,      // Pointer to debugCreateInfo struct
                                                const VkAllocationCallbacks* pAllocator,                    // Pointer to struct containing memory allocation data
                                                VkDebugUtilsMessengerEXT* pDebugMessenger)                  // Messenger object that passes debug message to callback
    {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");          // Get extension function address to load
        if (func != nullptr)                                                                                                        // If extension function address is not null
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);                                                        // Return the extension function
        else
            return VK_ERROR_EXTENSION_NOT_PRESENT;      // Otherwise, return an error
    }
    
    void EngineCore::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {

        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");       // Get extension function address
        if (func != nullptr)                                                                                                        // If extension function exists
            func(instance, debugMessenger, pAllocator);                                                                             // Run extension function
    }

    // Debug callback
    VKAPI_ATTR VkBool32 VKAPI_CALL EngineCore::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT msgSeverity,        // Report severity (diagnostic, info, warning, error)
                                                       VkDebugUtilsMessageTypeFlagsEXT msgType,                         // Type of report
                                                       const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,       // Pointer to struct containing report details
                                                       void* pUsrData)                                                  // Pointer to struct allowing user to pass data
    {
        std::string msg = "Error: Validation Layer -> " + std::string(pCallbackData->pMessage);
        ASWL::utilities::Logger("VALID", msg);      // Log the error
        return VK_FALSE;
    }

    // Populates debugCreateInfo
    void EngineCore::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& debugCreateInfo) {

        debugCreateInfo = {};                                                                   // debugCreateInfo specifies the parameters of the debug messenger
        debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;        // Identify debugCreateInfo as structure type DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT
        debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT       // Set callback filter -> VERBOSE
                                        | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT       // Set callback filter -> WARNING
                                        | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;        // Set callback filter -> ERROR
        debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT               // Set message filter -> GENERAL
                                    | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT            // Set message filter -> VALIDATAION
                                    | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;          // Set message filter -> PERFORAMANCE
        debugCreateInfo.pfnUserCallback = DebugCallback;                                        // Pointer to Debug Callback function
        debugCreateInfo.pUserData = nullptr;                                                    // Optional pointer to user inputted data
    }
}
