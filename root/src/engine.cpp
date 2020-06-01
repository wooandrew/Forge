// TheForge - src/engine (c) Andrew Woo, 2020

#pragma warning(disable : 26812)


#include "engine.hpp"

#include <cstring>


namespace Forge {

    // Engine constructor
    Engine::Engine() {

        version.MAJOR = 0;
        version.MINOR = 1;
        version.PATCH = 0;

        window = nullptr;
        instance = VK_NULL_HANDLE;
        surface = VK_NULL_HANDLE;

        dbgMessenger = VK_NULL_HANDLE;
    }

    // Engine destructor
    Engine::~Engine() {
        cleanup();
    };

    // Initalize engine
    int Engine::init() {

        // Initialize GLFW
        if (!glfwInit()) {
            ASWL::utilities::Logger("E00G0", "Fatal Error: Failed to initialize GLFW -> !glfwInit().");     // Log the error if GLFW fails to initialize
            return 1;                                                                                       // then return the corresponding error value
        }

        // Set the window hint using engine metadata. Defaulted to (GLFW_CLIENT_API, GLFW_NO_API) and (GLFW_RESIZABLE, GLFW_FALSE).                    
        for (const auto hint : metadata.windowHints)
            glfwWindowHint(hint.first, hint.second);

        // Create window using engine metadata
        window = glfwCreateWindow(metadata.windowDimensions.width, metadata.windowDimensions.height, metadata.windowTitle, nullptr, nullptr);

        if (!window) {                                                                                              // Check window creation status
            ASWL::utilities::Logger("E01G1", "Fatal Error: Failed to create window -> !glfwCreateWindow().");       // Log the error if window creation fails
            glfwTerminate();                                                                                        // terminate GLFW

            return 2;                                                                                               // then return the corresponding error value
        }

        // Create window context
        glfwMakeContextCurrent(window);

        if (DEBUG_MODE && !AllValidationLayersSupported()) {                                                                                        // If DEBUG_MODE is enabled and the required validation layers are not found
            ASWL::utilities::Logger("E02V0", "Fatal Error: Requested validation layers were not found -> !AllValidationLayersSupported().");        // then log the error
            return 3;                                                                                                                               // then return the corresponding error value
        }

        // Initalize Vulkan Instance
        VkApplicationInfo appInfo = {};                                                                                             // appInfo is filled with information about the application
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;                                                                         // Indentify appInfo as structure type APPLICATION_INFO
        appInfo.pNext = nullptr;                                                                                                    // Pointer to list of structures
        appInfo.pApplicationName = metadata.vkAppName;                                                                              // Set the vk application name
        appInfo.applicationVersion = VK_MAKE_VERSION(metadata.version.MAJOR, metadata.version.MINOR, metadata.version.PATCH);       // Set the vk application version
        appInfo.pEngineName = "TheForge";                                                                                           // Set engine to TheForge
        appInfo.engineVersion = VK_MAKE_VERSION(version.MAJOR, version.MINOR, version.PATCH);                                       // Set engine version
        appInfo.apiVersion = VK_API_VERSION_1_0;                                                                                    // Set minimum Vulkan API version required to run application

        VkInstanceCreateInfo createInfo = {};                           // createInfo passes which extensions GLFW requires from Vulkan 
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;      // Identify createInfo as structure type INSTANCE_CREATE_INFO
        //createInfo.flags = 0;                                         // Reserved for future use
        createInfo.pApplicationInfo = &appInfo;                         // Pass appInfo as reference

        auto extensions = GetRequiredExtensions();                                          // Get required extensions
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());        // Set extensions count
        createInfo.ppEnabledExtensionNames = extensions.data();                             // Set extensions

        if (DEBUG_MODE) {   // If DEBUG_MODE is enabled

            VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;         // debugCreateInfo contains parameters for the debug messenger

            createInfo.enabledLayerCount = static_cast<uint32_t>(ValidationLayers.size());      // Set number of global validation layers to enable
            createInfo.ppEnabledLayerNames = ValidationLayers.data();                           // Pass names of global validation layers to enable

            PopulateDebugMessengerCreateInfo(debugCreateInfo);                                  // Populates debug create info
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;          // Pointer to extension-function structure
        }
        else {
            createInfo.enabledLayerCount = 0;       // Set number of global validation layers to enable
            createInfo.pNext = nullptr;             // Set pointer to debug create info to null
        }

        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {                                      // If vkInstance creation fails
            ASWL::utilities::Logger("E03V1", "Fatal Error: Failed to create instance -> vkCreateInstance().");      // then log the error
            return 4;                                                                                               // and return corresponding error value
        }

        SetupDebugMessenger();

        if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {           // If vkSurface creation fails
            ASWL::utilities::Logger("E04S0", "Fatal Error: Window surface creation failed.");       // then log the error
            return 5;                                                                               // and return corresponding error value
        }

        return 0;
    }

    // Update engine
    void Engine::update() {
        glfwPollEvents();       // Poll events
    }

    // Cleanup Engine -> Vulkan/GLFW
    void Engine::cleanup() {

        vkDestroySurfaceKHR(instance, surface, nullptr);        // Destroy Vulkan surface

        if (DEBUG_MODE)                                                             // If DEBUG_MODE is enabled
            DestroyDebugUtilsMessengerEXT(instance, dbgMessenger, nullptr);         // Destroy Debug Message handler

        vkDestroyInstance(instance, nullptr);       // Destroy Vulkan instance

        glfwDestroyWindow(window);      // destroy window
        glfwTerminate();                // then terminate GLFW
    }

    // Check if validation layers are supported
    bool Engine::AllValidationLayersSupported() {

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

    // Return the extensions required by application
    std::vector<const char*> Engine::GetRequiredExtensions() {

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

    // Debug handler setup
    void Engine::SetupDebugMessenger() {

        if (!DEBUG_MODE)        // If DEBUG_MODE is disabled
            return;             // then do not setup

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;     // Struct containing data for debug messenger object
        PopulateDebugMessengerCreateInfo(debugCreateInfo);      // Populate the struct

        if (CreateDebugUtilsMessengerEXT(instance, &debugCreateInfo, nullptr, &dbgMessenger) != VK_SUCCESS)     // If DebuMessenger creation fails
            ASWL::utilities::Logger("E05V3", "Error: Failed to setup debug messenger.");                        // Log the message
    }

    VkResult Engine::CreateDebugUtilsMessengerEXT(VkInstance instance,                                          // Vulkan Instance
                                                  const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,        // Pointer to debugCreateInfo struct
                                                  const VkAllocationCallbacks* pAllocator,                      // Pointer to struct containing memory allocation data
                                                  VkDebugUtilsMessengerEXT* pDebugMessenger)                    // Messenger object that passes debug message to callback
    {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");         // Get extension function address to load
        if (func != nullptr)                                                                                                        // If extension function address is not null
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);                                                        // Return the extension function
        else
            return VK_ERROR_EXTENSION_NOT_PRESENT;      // Otherwise, return an error
    }

    void Engine::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {

        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");       // Get extension function address
        if (func != nullptr)                                                                                                        // If extension function exists
            func(instance, debugMessenger, pAllocator);                                                                             // Run extension function
    }

    // Debug callback
    VKAPI_ATTR VkBool32 VKAPI_CALL Engine::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT msgSeverity,                // Report severity (diagnostic, info, warning, error)
                                                         VkDebugUtilsMessageTypeFlagsEXT msgType,                           // Type of report
                                                         const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,         // Pointer to struct containing report details
                                                         void* pUsrData)                                                    // Pointer to struct allowing user to pass data
    {
        ASWL::utilities::Logger(std::string("E06V4"), std::string("Error: Validation Layer ->"), std::string(pCallbackData->pMessage));         // Log the error
        return VK_FALSE;
    }

    // Populates debugCreateInfo
    void Engine::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& debugCreateInfo) {
        
        debugCreateInfo = {};                                                                   // Empty debugCreateInfo
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

    // Return window instance on request
    GLFWwindow* Engine::GetWindow() {
        return window;
    }

    // Return the Vulkan instance on request
    VkInstance& Engine::GetInstance() {
        return instance;
    }
    // Returns the Vulkan surface on request
    VkSurfaceKHR& Engine::GetSurface() {
        return surface;
    }

    // Return whether or not window should close based on polled events
    bool Engine::WindowShouldClose() const {
        return glfwWindowShouldClose(window);
    }
}
