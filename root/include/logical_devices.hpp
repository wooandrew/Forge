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

    class LogicalDevice {

    public:

        friend class Engine;

        LogicalDevice();        // Default constructor
        ~LogicalDevice();       // Default destructor

        int init(VkPhysicalDevice physicalDevice, VkSurfaceKHR& surface);       // Initalize LogicalDevice object
        void cleanup();                                                         // Cleanup LogicalDevice object

        VkDevice& GetDevice();      // Returns logical device object

        enum class DeviceType {     // Describes the type of logical device. Must be set before the object is initialized
            GraphicsCard
        }; DeviceType type;

    private:
        VkDevice device;            // Handle to logical device
        VkQueue graphicsQueue;      // Handle to graphics queue
        VkQueue presentQueue;       // Handle to present queue
    };
}
 
#endif // !THEFORGE_LOGICAL_DEVICES
