// TheForge - src/2D/renderer (c) Andrew Woo, 2020

#pragma once

#ifndef THEFORGE_2D_RENDERER
#define THEFORGE_2D_RENDERER

// Standard Library
#include <vector>

// Dependencies
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// TheForge includes
#include "forge_vars.hpp"
#include "logical_devices.hpp"
#include "swapchain.hpp"
#include "pipeline.hpp"

namespace Forge {

    class Renderer {

    public:

        Renderer();         // Default constructor
        ~Renderer();        // Default destructor

        enum class RendererType {       // Renderer type
            Render_2D,
            Render_3D
        }; RendererType type;

        int init(LogicalDevice& _lgc, Swapchain& _swapchain, Pipeline& _pipeline, std::vector<VkCommandBuffer>& _cmdBuffers);       // Initialize renderer
        void cleanup();     // Cleanup renderer

        int draw();         // Draw frame
        void SetComponents(Swapchain& _swapchain, Pipeline& _pipeline, std::vector<VkCommandBuffer>& _cmdBuffers);      // Set Vulkan components

    private:

        std::vector<VkSemaphore> ImageAvailableSemaphores;          // List of handles to semaphore signal -> image aquired
        std::vector<VkSemaphore> RenderFinishedSemaphores;          // List of handles to semaphore signal -> render finished
        std::vector<VkFence> InFlightFences;                        // List of handles to fence object
        std::vector<VkFence> InFlightImages;                        // List of fences for each image in swapchain

        LogicalDevice logical_graphics_card;        // Logical graphics card object
        Swapchain swapchain;                        // Swapchain object instance
        Pipeline pipeline;                          // Pipeline object instance

        std::vector<VkCommandBuffer> cmdBuffers;        // Command buffers
    };
}

#endif // !THEFORGE_2D_RENDERER
