// TheForge - src/logical_devices (c) Andrew Woo, 2020

#include "logical_devices.hpp"
#include "forge_vars.hpp"

// Standard Library
#include <set>

// Dependencies
#include <ASWL/utilities.hpp>

namespace Forge {

    // Default constructor
    LogicalGraphicsCard::LogicalGraphicsCard() {

        device = VK_NULL_HANDLE;
        graphicsQueue = VK_NULL_HANDLE;
        presentQueue = VK_NULL_HANDLE;
    }

    // Default destructor
    LogicalGraphicsCard::~LogicalGraphicsCard() {
        cleanup();
    }


    // Initialize 
    int LogicalGraphicsCard::init(VkPhysicalDevice physicalDevice, VkSurfaceKHR& surface) {

        // Get the queue family index of the graphics card
        QueueFamilyIndices indices = FindQueueFamilies(physicalDevice, surface);

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

        if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {                              // If device creation fails
            ASWL::utilities::Logger("L00G0", "Fatal Error: Failed to create a logical device -> graphics card.");       // then log the error
            return 1;                                                                                                   // and return corresponding error value
        }

        // Retrieves queue handles for queue family
        vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
        vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);

        return 0;
    }

    // Cleanup Logical Device -> graphics card
    void LogicalGraphicsCard::cleanup() {
        vkDestroyDevice(device, nullptr);       // Destroy logical device
    }
}
 