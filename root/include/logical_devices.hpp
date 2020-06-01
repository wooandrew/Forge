// TheForge - src/logical_devices (c) Andrew Woo, 2020

#pragma once

#ifndef THEFORGE_LOGICAL_DEVICES
#define THEFORGE_LOGICAL_DEVICES

// Standard Library
#include <vector>

// Dependencies
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Forge {

    class LogicalGraphicsCard {

    public:

        LogicalGraphicsCard();          // Default constructor
        ~LogicalGraphicsCard();         // Default destructor

        int init(VkPhysicalDevice physicalDevice, VkSurfaceKHR& surface);       // Initalize logical device
        void cleanup();                                                         // Cleanup logical device

    private:
        VkDevice device;            // Handle to logical device
        VkQueue graphicsQueue;      // Handle to graphics queue
        VkQueue presentQueue;       // Handle to present queue
    };
}
 
#endif // !THEFORGE_LOGICAL_DEVICES
