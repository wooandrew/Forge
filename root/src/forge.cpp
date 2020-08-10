// TheForge - src/forge (c) Andrew Woo, 2020

#define VMA_IMPLEMENTATION
#include <VMA/vk_mem_alloc.h>

#include "forge.hpp"

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


    // Populates SwapChainSupportDetails
    SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice& device, VkSurfaceKHR& surface) {

        SwapChainSupportDetails details;        // Struct containing Swapchain support details

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);      // Fills VkSurfaceCapabilitiesKHR structure

        uint32_t formatCount = 0;                                                               // Number of formats the device surface supports
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);           // Get number of formats the device surface supports

        if (formatCount != 0) {                                                                                 // If the number of formats supported by surface is not zero
            details.formats.resize(formatCount);                                                                // Resizes list of VkSurfaceFormatKHRs
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());        // Then push supported formats into list
        }

        uint32_t presentModeCount = 0;                                                                  // Number of present modes supported by physical device
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);         // Get number of present modes supported by physical device

        if (presentModeCount != 0) {                                                                                            // If number of present modes supported by physical device is not zero
            details.presentModes.resize(presentModeCount);                                                                      // Resizes list of VkPresentModeKHRs
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());         // Then push supported presetn modes into list
        }

        return details;
    }

    // Choose surface format
    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {

        // Iterate through every available surface formats
        for (const auto& availableFormat : availableFormats) {

            // If the format is B8G8R8A8_SRGB and colorSpace is SRGB_NONLINEAR_KHR
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                return availableFormat;        // Return availableFormat
        }

        return availableFormats[0];        // Return the first available format
    }

    // Choose surface present mode
    VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {

        // Iterate through every available surface present modes
        for (const auto& availablePresentMode : availablePresentModes) {

            // If MAILBOX_KHR is an available present mode
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
                return availablePresentMode;    // Return availablePresentMode
        }

        return VK_PRESENT_MODE_FIFO_KHR;    // Return FIFO_KHR
    }

    VkExtent2D ChooseSwapExtent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR& capabilities) {

        if (capabilities.currentExtent.width != UINT32_MAX)         // If surface extent does not equal the max UINT32 size (4,294,967,294)
            return capabilities.currentExtent;                      // then return the current surface extent

        else {        // Otherwise set the surface extent

            int width = 0;          // Extent width
            int height = 0;         // Extent height

            // Get framebuffer extent from GLFW Window object
            glfwGetFramebufferSize(window, &width, &height);

            VkExtent2D actualExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);              // Clamp width
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);          // Clamp height

            return actualExtent;
        }
    }
}