// TheForge - src/vertex (c) Andrew Woo, 2020

#include "vertex.hpp"
#include "forge_vars.hpp"

// Dependencies
#include <ASWL/utilities.hpp>

namespace Forge {

    // Default constructor
    VertexBuffer::VertexBuffer() {

        device = VK_NULL_HANDLE;
        vertexbuffer = VK_NULL_HANDLE;
        VertexBufferMemory = VK_NULL_HANDLE;
    }

    // Default destructor
    VertexBuffer::~VertexBuffer() {

    }

    // Initialize vertex buffer
    int VertexBuffer::init(VkPhysicalDevice& _graphicscard, VkDevice& _device) {

        device = _device;

        // Vertex buffer size
        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

        VkBuffer stagingBuffer;                     // Temporary vertex buffer object
        VkDeviceMemory stagingBufferMemory;         // Temporary vertex buffer memory

        // Create a staging buffer object
        if (CreateBuffer(_graphicscard, device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |         // If vertex staging buffer creation fails
                         VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory) != VK_SUCCESS) {                         //
            ASWL::utilities::Logger("V00B0", "Fatal Error: Failed to create the staging buffer.");                                          // then log the error
            return 1;                                                                                                                       // and return the corresponding error value
        }

        void* data;
        vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);      // Map memory to staging buffer memory
        memcpy(data, vertices.data(), (size_t)bufferSize);                      // Copy data from vertices to data
        vkUnmapMemory(device, stagingBufferMemory);                             // Unmap previously mapped data from VertexBufferMemory

        if(CreateBuffer(_graphicscard, device, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,        // If vertex buffer creation fails
                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexbuffer, VertexBufferMemory) != VK_SUCCESS) {                         //
            ASWL::utilities::Logger("V01B1", "Fatal Error: Failed to create the vertex buffer.");                                       // then log the error
            return 2;                                                                                                                   // and return the corresponding error value
        }

        return 0;
    }

    // Returns vertex buffer on request
    VkBuffer& VertexBuffer::GetVertexBuffer() {
        return vertexbuffer;
    }

    // Cleanup vertex buffer
    void VertexBuffer::cleanup() {
        vkDestroyBuffer(device, vertexbuffer, nullptr);
        vkFreeMemory(device, VertexBufferMemory, nullptr);
    }
}
