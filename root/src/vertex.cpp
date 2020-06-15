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

        VkBufferCreateInfo bufferInfo = {};                             // bufferInfo specifies the parameters of the VertexBuffer object
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;        // Specify bufferInfo as structure type BUFFER_CREATE_INFO
        bufferInfo.size = sizeof(vertices[0]) * vertices.size();        // Size of buffer in bytes
        bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;           // Specify allowed buffer usage
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;             // Specify if buffer can be shared or not between queue families
        
        if (vkCreateBuffer(device, &bufferInfo, nullptr, &vertexbuffer) != VK_SUCCESS) {            // If Vertex Buffer creation fails
            ASWL::utilities::Logger("V0000", "Fatal Error: Vertex buffer creation failed.");        // then log the error
            return 1;                                                                               // and return the corresponding error value
        }

        VkMemoryRequirements memoryRequirements;                                        // Structure specifies memory requirements of vertex buffer
        vkGetBufferMemoryRequirements(device, vertexbuffer, &memoryRequirements);       // Get vertex buffer memory requirements and fill structure

        // Lambda function to find a suitable memory type to assign to the vertex buffer
        auto FindMemoryType = [](VkPhysicalDevice& device, uint32_t typeFilter, VkMemoryPropertyFlags properties) {

            VkPhysicalDeviceMemoryProperties memProperties;                     // Structure specifies the memory properties of the graphics card
            vkGetPhysicalDeviceMemoryProperties(device, &memProperties);        // Get graphics card memory properties and fill structure

            // Iterate through every type of memory supported by the device
            for (int i = 0; i < memProperties.memoryTypeCount; i++) {
                if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)     // If a suitable memory type is found
                    return i;                                                                                               // return the type index
            }

            return -1;      // Return an error if a suitable memory type is not found.
        };

        // Find a suitable memory type to assign to the vertex buffer
        int memory = FindMemoryType(_graphicscard, memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        if (memory == -1) {                                                                                 // If a suitable memory type is not found
            ASWL::utilities::Logger("V01M0", "Fatal Error: Failed to find a suitable memory type.");        // then log the error
            return 2;                                                                                       // and return the corresponding error value
        }

        VkMemoryAllocateInfo allocInfo = {};                                // allocInfo specifies the parameters of vertex buffer memory allocation
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;           // Specify allocInfo as structure type MEMORY_ALLOCATE_INFO
        allocInfo.allocationSize = memoryRequirements.size;                 // Set memory allocation size
        allocInfo.memoryTypeIndex = static_cast<uint32_t>(memory);          // Set memory type index

        if (vkAllocateMemory(device, &allocInfo, nullptr, &VertexBufferMemory) != VK_SUCCESS) {                     // If allocation of vertex buffer memory fails
            ASWL::utilities::Logger("V02M1", "Fatal Error: Failed to allocate memory for the vertex buffer.");      // then log the error
            return 3;                                                                                               // and return the corresponding error value
        }

        vkBindBufferMemory(device, vertexbuffer, VertexBufferMemory, 0);        // Bind the allocated memory to the vertex buffer

        void* data;
        vkMapMemory(device, VertexBufferMemory, 0, bufferInfo.size, 0, &data);      // Map memory to VertexBufferMemory
        memcpy(data, vertices.data(), (size_t)bufferInfo.size);                     // Copy data from vertices to data
        vkUnmapMemory(device, VertexBufferMemory);                                  // Unmap previously mapped data from VertexBufferMemory

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
