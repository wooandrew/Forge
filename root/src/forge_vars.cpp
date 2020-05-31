// TheForge - src/forge_vars (c) Andrew Woo, 2020

#include "forge_vars.hpp"

// Standard Library
#include <vector>

namespace Forge {

    // Function determines if all optional values in QueueFamilyIndices has a value
    bool QueueFamilyIndices::hasValue() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }

    // Finds and returns queue family supported by specified device
    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice& device) {

        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;                                                      // Number of queue families supported by the device
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);       // Get number of queue families supported by the device

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);                           // List of structs containing queue family properties
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());      // Then push queue families into list

        for (unsigned int i = 0; i < queueFamilies.size(); i++) {           // Iterate through all queue family properties

            if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)        // If queue family supports VK_QUEUE_GRAPHICS_BIT
                indices.graphicsFamily = i;                                 // Set graphics queue index

            VkBool32 presentSupport = false;                                                // Initialize present support
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);      // Check if physical device supports presenting to surface

            if (presentSupport)                 // If device supports presenting to surface
                indices.presentFamily = i;      // Set present queue index

            if (indices.hasValue())         // If the graphics family queue index has a value
                break;                      // break out of the loop
        }

        return indices;
    }
}
