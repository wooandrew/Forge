// TheForge - src/core/graphicscard (c) Andrew Woo, 2020

#include "core/graphicscard.hpp"
#include "forge_vars.hpp"

#include <set>

namespace Forge::Core {

    namespace {

        // Checks if the physical device supports necessary Vulkan operations
        bool CheckDeviceSupport(VkPhysicalDevice& _pgpu, VkSurfaceKHR& _surface) {

            //VkPhysicalDeviceProperties deviceProperties;
            //vkGetPhysicalDeviceProperties(device, &deviceProperties);

            //VkPhysicalDeviceFeatures deviceFeatures;
            //vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

            QueueFamilyIndices indices = FindQueueFamilies(_pgpu, _surface);
            bool extensionsSupported = CheckDeviceExtensionSupport(_pgpu);

            bool swapChainAdequacy = false;                                                                             // Is swap chain adequate
            if (extensionsSupported) {                                                                                  // If the device supports extensions
                SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(_pgpu, _surface);                      // Populates SwapChainSupportDetails
                swapChainAdequacy = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();        // The swap chain is adequate if the list of formats and present modes is not empty
            }

            return indices.hasValue() && extensionsSupported && swapChainAdequacy;
        }

        // Checks if physical device supports Vulkan extensions
        bool CheckDeviceExtensionSupport(VkPhysicalDevice& _pgpu) {

            uint32_t extensionCount;                                                                // Number of extensions supported by the device
            vkEnumerateDeviceExtensionProperties(_pgpu, nullptr, &extensionCount, nullptr);         // Get number of extensions the device supports

            std::vector<VkExtensionProperties> availableExtensions(extensionCount);                                     // List of available extensions
            vkEnumerateDeviceExtensionProperties(_pgpu, nullptr, &extensionCount, availableExtensions.data());          // Then push extensions into list

            std::set<std::string> requiredExtensions(DeviceExtensions.begin(), DeviceExtensions.end());         // Sorted list of unique extensions

            for (const auto& extension : availableExtensions)               // Iterate through all available extensions
                requiredExtensions.erase(extension.extensionName);          // Remove available extension from required extensions list

            return requiredExtensions.empty();
        }
    }

    // Default constructor
    GraphicsCard::GraphicsCard() {

        PGPU = VK_NULL_HANDLE;
        LGPU = VK_NULL_HANDLE;
    }

    // Default destructor
    GraphicsCard::~GraphicsCard() {

    }

    void GraphicsCard::cleanup() {
        vkDestroyDevice(LGPU, nullptr);       // Destroy logical device
    }

    // Initializes graphics card object
    int GraphicsCard::init(VkInstance& _instance, VkSurfaceKHR& _surface) {

        // Select a physical device to use
        int graphicsCardFound = SelectGraphicsCard(_instance, _surface);        // Find a graphics card candidate
        bool graphicsCardSupported = CheckDeviceSupport(PGPU, _surface);        // and check for Vulkan extensions support

        if (graphicsCardFound != 0 || !graphicsCardSupported)           // If a graphics card is not found or the graphics card is not supported
            return graphicsCardFound & (int)graphicsCardSupported;      // Return the error as a combination of both errors

        // Set up logical device
        QueueFamilyIndices indices = FindQueueFamilies(PGPU, _surface);         // Get the queue family index of the graphics card

        float queuePriority = 1.0f;     // Set the queue priority of queue index. From 0.0f to 1.0f, higher values garner higher priorities.

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;                                                              // List of queueCreateInfos
        std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };         // Initialize set

        for (auto& queueFamily : uniqueQueueFamilies) {     // Iterate through every value in uniqueQueueFamilies

            VkDeviceQueueCreateInfo queueCreateInfo = {};                               // queueCreateInfo specifies the parameters of a device work queue
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;         // Identify queueCreateInfo as structure type DEVICE_QUEUE_CREATE_INFO
            queueCreateInfo.queueFamilyIndex = queueFamily;                             // Index to the queue family which the queue belongs to
            queueCreateInfo.queueCount = 1;                                             // Number of queues to create for the queue family
            queueCreateInfo.pQueuePriorities = &queuePriority;                          // Pointer to the queue priority
            queueCreateInfos.push_back(queueCreateInfo);                                // Push back queueCreateInfo to list
        }

        VkPhysicalDeviceFeatures deviceFeatures = {};       // List of device features

        VkDeviceCreateInfo createInfo = {};                                                         // createInfo specifies the parameters of the created logical device
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;                                    // Identify createInfo as structure type DEVICE_CREATE_INFO
        createInfo.pQueueCreateInfos = queueCreateInfos.data();                                     // Pointer to structure containing device queue parameters
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());           // Size of pQueueCreateInfos
        createInfo.pEnabledFeatures = &deviceFeatures;                                              // Set the enabled features of the graphics card
        createInfo.enabledExtensionCount = static_cast<uint32_t>(DeviceExtensions.size());          // Set the number of extensions to enable
        createInfo.ppEnabledExtensionNames = DeviceExtensions.data();                               // Pointer to list of enabled device extensions

        if (DEBUG_MODE) {                                                                           // If DEBUG_MODE is enabled
            createInfo.enabledLayerCount = static_cast<uint32_t>(ValidationLayers.size());          // Set number of global validation layers to enable
            createInfo.ppEnabledLayerNames = ValidationLayers.data();                               // Pass names of global validation layers to enable
        }
        else {                                              // If DEBUG_MODE is disabled
            createInfo.enabledLayerCount = 0;               // Enable zero validation layers
            createInfo.ppEnabledLayerNames = nullptr;       // and do not pass any validation layers to enable
        }

        if (vkCreateDevice(PGPU, &createInfo, nullptr, &LGPU) != VK_SUCCESS) {                                          // If device creation fails
            ASWL::utilities::Logger("GC000", "Fatal Error: Failed to create a logical device -> graphics card.");       // then log the error
            return 1;                                                                                                   // and return corresponding error value
        }

        // Retrieves queue handles for queue family
        vkGetDeviceQueue(LGPU, indices.graphicsFamily.value(), 0, &graphicsQueue);
        vkGetDeviceQueue(LGPU, indices.presentFamily.value(), 0, &presentQueue);

        return 0;
    }

    // Selects the graphics card to use
    int GraphicsCard::SelectGraphicsCard(VkInstance& _instance, VkSurfaceKHR& _surface) {

        uint32_t deviceCount = 0;                                           // Number of devices that support Vulkan
        vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr);       // Get number of devices that support Vulkan

        if (deviceCount == 0) {                                                                             // If there are no devices that support Vulkan
            ASWL::utilities::Logger("GC001", "Fatal Error: Failed to find GPU that supports Vulkan.");      // then log the error
            return 2;                                                                                       // and return the error
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);                         // Initialize list of supported devices
        vkEnumeratePhysicalDevices(_instance, &deviceCount, devices.data());        // Then push supported devices into list

        // Iterate through all supported devices
        for (VkPhysicalDevice& _pgpu : devices) {

            if (CheckDeviceSupport(_pgpu, _surface)) {      // If the device supports Vulkan operations
                PGPU = _pgpu;                               // Set the selected graphics card to that device
                break;                                      // then break out of the loop
            }
        }

        if (PGPU == VK_NULL_HANDLE) {                                                                                           // If the GPU is null
            ASWL::utilities::Logger("GC002", "Fatal Error: Failed to find GPU that supports required Vulkan operation.");       // then log the error
            return 3;                                                                                                           // and return the error
        }

        return 0;
    }
}