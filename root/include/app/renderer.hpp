// TheForge - src/app/renderer (c) Andrew Woo, 2020

#pragma once

#ifndef THEFORGE_APP_RENDERER
#define THEFORGE_APP_RENDERER

// Standard Library
#include <memory>

// Dependencies
#include <vulkan/vulkan.hpp>
#include <VMA/vk_mem_alloc.h>

// TheForge includes
#include "forge.hpp"
#include "core/core.hpp"
#include "framework.hpp"

namespace Forge::App {

    class Renderer {

        /**
         * TheForge App Renderer
         * Contains rendering logic
         *
         * The renderer contains higher-level
         * rendering logic for the end-user to
         * use more efficiently.
        **/

    public:

        Renderer();         // Default constructor
        ~Renderer();        // Default destructor

        int init(std::shared_ptr<Forge::Core::EngineCore> _core, std::shared_ptr<App::Framework> _framework);       // Initialize renderer
        int reinitialize();                                                                                         // Reinitialize renderer

        int draw();         // Draw frame

        void cleanup();     // Cleanup renderer

        RendererType type;      // Renderer type

    private:

        int StartSingleTimeCommand();       // Start temporary command buffer
        int EndSingleTimeCommand();         // Stop temporary command buffer

        int CreateAllocator();              // Create allocator
        int CreateCommandPool();            // Create CommandPool
        int CreateVertexBuffer();           // Create VertexBuffer
        int CreateCommandBuffers();         // Create command buffers
        int CreateSemaphores();             // Create rendering semaphores and fences

        std::shared_ptr<Forge::Core::EngineCore> core;
        std::shared_ptr<App::Framework> framework;

        VkClearValue clearCanvasColor;      // Canvas clearing color
        VmaAllocator allocator;             // VulkanMemeoryAllocator

        VkBuffer VertexBuffer;              // Handle to buffer object
        VmaAllocation vbAllocation;         // Vertex Buffer Memory allocation object

        VkCommandPool CommandPool;                      // Handle to VkCommandPool object
        std::vector<VkCommandBuffer> cmdBuffers;        // List of handles to command buffers
        VkCommandBuffer TempCommandBuffer;              // Command buffer for single time uses

        std::vector<VkSemaphore> ImageAvailableSemaphores;          // List of handles to semaphore signal -> image aquired
        std::vector<VkSemaphore> RenderFinishedSemaphores;          // List of handles to semaphore signal -> render finished
        std::vector<VkFence> InFlightFences;                        // List of handles to fence object
        std::vector<VkFence> InFlightImages;                        // List of fences for each image in swapchain
    };
}

#endif // !THEFORGE_APP_RENDERER