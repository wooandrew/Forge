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
#include "core.hpp"
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

        int init(std::shared_ptr<Logger> _logger, std::shared_ptr<Forge::Core::EngineCore> _core, std::shared_ptr<App::Framework> _framework);      // Initialize renderer
        int reinitialize();                                                                                                                         // Reinitialize renderer
        
        // Set the color that the canvas will clear to
        void SetClearColor(VkClearValue _clearCanvasColor);

        int draw();         // Draw frame

        void cleanup();     // Cleanup renderer

        RendererType type;      // Renderer type

    private:

        int StartSingleTimeCommand();       // Start temporary command buffer
        int EndSingleTimeCommand();         // Stop temporary command buffer

        int CreateAllocator();              // Create allocator
        int CreateCommandPool();            // Create CommandPool
        int CreateVertexBuffer();           // Create VertexBuffer
        int CreateIndexBuffer();            // Create IndexBuffer
        int CreateUniformBuffers();         // Create uniform buffers
        int CreateCommandBuffers();         // Create command buffers
        int CreateSemaphores();             // Create rendering semaphores and fences

        void UpdateUBO(uint32_t _imageIndex);       // Update uniform buffer

        std::shared_ptr<Logger> logger;                         // Instanced logger
        std::shared_ptr<Forge::Core::EngineCore> core;          // Engine core
        std::shared_ptr<App::Framework> framework;              // Rendering framework

        VkClearValue clearCanvasColor;      // Canvas clearing color
        VmaAllocator allocator;             // VulkanMemeoryAllocator

        VkBuffer VertexBuffer;              // Handle to vertex buffer object
        VmaAllocation vbAllocation;         // Vertex Buffer Memory allocation object

        VkBuffer IndexBuffer;               // Handle to index buffer object
        VmaAllocation ibAllocation;         // Index Buffer Memory allocation object

        std::vector<VkBuffer> UniformBuffers;           // Vector contains uniform buffers contiguously
        std::vector<VmaAllocation> ubAllocations;       // Vector of uniform buffer memory allocations

        VkCommandPool CommandPool;                      // Handle to VkCommandPool object
        std::vector<VkCommandBuffer> cmdBuffers;        // List of handles to command buffers
        VkCommandBuffer TempCommandBuffer;              // Command buffer for single time uses

        std::vector<VkSemaphore> ImageAvailableSemaphores;          // Vector of handles to semaphore signal -> image acquired
        std::vector<VkSemaphore> RenderFinishedSemaphores;          // Vector of handles to semaphore signal -> render finished
        std::vector<VkFence> InFlightFences;                        // Vector of handles to fence object
        std::vector<VkFence> InFlightImages;                        // Vector of fences for each image in swapchain
    };
}

#endif // !THEFORGE_APP_RENDERER