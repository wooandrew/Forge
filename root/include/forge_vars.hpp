// TheForge - src/forge_vars (c) Andrew Woo, 2020

#pragma once

#ifndef THEFORGE_VARS
#define THEFORGE_VARS

// Standard Library
#include <vector>
#include <utility>
#include <optional>

// Dependencies
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Forge {

    // Check if in debug mode
#ifdef NDEBUG
    constexpr bool DEBUG_MODE = false;          // If false, do not enable debug mode
#else
    constexpr bool DEBUG_MODE = true;           // If true, enable debug mode
#endif

    static const std::vector<const char*> ValidationLayers = { "VK_LAYER_KHRONOS_validation" };             // List of validation layers to enable
    static const std::vector<const char*> DeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };           // List of device extensions to enable

    constexpr int MAX_FRAMES_IN_FLIGHT = 2;

    struct QueueFamilyIndices {                         // Struct containing Queue Family indices

        std::optional<uint32_t> graphicsFamily;         // Index for graphics queue family
        std::optional<uint32_t> presentFamily;          // Index for presentation queue family

        bool hasValue();        // Function to determine if all queue families have indices
    };
    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice& device, VkSurfaceKHR& surface);     // Finds queue family supported by specified device
}

#endif // !THEFORGE_VARS
