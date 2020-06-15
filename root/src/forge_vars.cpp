// TheForge - src/forge_vars (c) Andrew Woo, 2020

#include "forge_vars.hpp"

// Standard Library
#include <vector>

namespace Forge {

    // Create a vertex binding description
    VkVertexInputBindingDescription Vertex::GetBindingDescription() {

        VkVertexInputBindingDescription bindingDescription = {};            // Structure specifies input vertex binding parameters
        bindingDescription.binding = 0;                                     // Binding index in vertex binding array
        bindingDescription.stride = sizeof(Vertex);                         // Distance between elements within buffer
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;         // Specify whether vertex attribute is a function of vertex index or instance index

        return bindingDescription;
    }

    // Array of vertex attribute descriptions
    std::array<VkVertexInputAttributeDescription, 2> Vertex::GetAttributeDesciptions() {

        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions = {};        // Array of vertex attribute descriptions
        attributeDescriptions[0].binding = 0;                                               // Binding number which attribute recieves data from
        attributeDescriptions[0].location = 0;                                              // Shader binding location
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;                          // Size/type of vertex attribute data
        attributeDescriptions[0].offset = offsetof(Vertex, position);                       // Offset of attribute relative to start of vertex input binding

        attributeDescriptions[1].binding = 0;                                               // Binding number which attribute recieves data from
        attributeDescriptions[1].location = 1;                                              // Shader binding location
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;                       // Size/type of vertex attribute data
        attributeDescriptions[1].offset = offsetof(Vertex, color);                          // Offset of attribute relative to start of vertex input binding

        return attributeDescriptions;
    }

    // Function determines if all optional values in QueueFamilyIndices has a value
    bool QueueFamilyIndices::hasValue() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }

    // Finds and returns queue family supported by specified device
    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice& device, VkSurfaceKHR& surface) {

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
