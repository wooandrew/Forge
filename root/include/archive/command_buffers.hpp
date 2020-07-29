// TheForge - src/command_buffers (c) Andrew Woo, 2020

#pragma once

#ifndef THEFORGE_COMMAND_BUFFERS
#define THEFORGE_COMMAND_BUFFERS

// Standard Library
#include <vector>

// Dependencies
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// TheForge includes
#include "swapchain.hpp"
#include "pipeline.hpp"

namespace Forge {

    class CommandBuffers {      // Command buffers class

    public:

        friend class VkContainer;

        CommandBuffers();       // Default constructor
        ~CommandBuffers();      // Default destructor

        int init(VkPhysicalDevice& _graphicscard, VkDevice& _device, VkSurfaceKHR& _surface, Swapchain& _swapchain, Pipeline& _pipeline, VkBuffer& _buffer);        // CommandBuffers initializer
        int CreateCommandBuffers(VkSurfaceKHR& _surface, Swapchain& _swapchain, Pipeline& _pipeline, VkBuffer& _buffer);                                            // Create command buffers

        void cleanup();

        void SetCanvasClearColor(VkClearValue clearcolor);         // Set canvas clear color

    private:

        VkClearValue clearCanvasColor;                  // Canvas clearing color
        std::vector<VkCommandBuffer> cmdBuffers;        // List of handles to command buffers

        VkCommandPool CommandPool;      // Handle to VkCommandPool object
        VkDevice device;                // Handle to logical graphics card
    };
}

#endif // !THEFORGE_COMMAND_BUFFERS