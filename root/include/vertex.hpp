// TheForge - src/vertex (c) Andrew Woo, 2020

#pragma once

#ifndef THEFORGE_VERTEX
#define THEFORGE_VERTEX

// Standard Library
#include <vector>

// Dependencies
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Forge {

    class VertexBuffer {

    public:

        VertexBuffer();         // Default constructor
        ~VertexBuffer();        // Default destructor

        int init(VkPhysicalDevice& _graphicscard, VkDevice& _device);       // Initialize vertex buffer
        void cleanup();                                                     // Cleanup vertex buffer

        VkBuffer& GetVertexBuffer();        // Returns vertex buffer

    private:
        VkBuffer vertexbuffer;                      // Handle to buffer object
        VkDeviceMemory VertexBufferMemory;          // Handle to device memory object

        VkDevice device;                // Handle to logical device object
    };
}

#endif // !THEFORGE_VERTEX
