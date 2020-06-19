// TheForge - src/vkcontainer (c) Andrew Woo, 2020

#pragma once

#ifndef THEFORGE_VK_CONTAINER
#define THEFORGE_VK_CONTAINER

// Dependencies
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// TheForge includes
#include "forge.hpp"
#include "forge_vars.hpp"
#include "physical_devices.hpp"
#include "logical_devices.hpp"
#include "swapchain.hpp"
#include "pipeline.hpp"
#include "vertex.hpp"
#include "command_buffers.hpp"
#include "renderer.hpp"

namespace Forge {

    class VkContainer {

    public:

        friend class Engine;

        VkContainer();          // Default constructor
        ~VkContainer();         // Default destructor

        // Initialize Vulkan components of the engine. If engine.metadata.autoinit is disabled, Vulkan components must be initialized manually.
        int autoinit(GLFWwindow* window, VkInstance& instance, VkSurfaceKHR& surface);
        int initRenderer();                                                                 // Initialize graphics renderer
        int reinitialize(GLFWwindow* window, VkSurfaceKHR& surface);                        // Reinitialize swapchain

        void cleanup();
                                                                                            
    private:

        GraphicsCard   gc;          // Physical graphics card object
        LogicalDevice  ld;          // Logical graphics card object
        Swapchain      sc;          // Swapchain object
        Pipeline       pl;          // Pipeline object
        VertexBuffer   vb;          // Vertex buffer object
        CommandBuffers cb;          // Command buffers object

        Renderer render2D;          // 2D Renderer
    };

}

#endif // !THEFORGE_VK_CONTAINER
