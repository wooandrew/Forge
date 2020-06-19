// TheForge - src/forge_vars (c) Andrew Woo, 2020

#include "forge_vars.hpp"

// Standard Library
#include <vector>

// Dependencies
#include <ASWL/utilities.hpp>

namespace Forge {

    // Create a vertex binding description
    VkVertexInputBindingDescription Vertex::GetBindingDescription() {

        VkVertexInputBindingDescription bindingDescription = {};            // Structure specifies input vertex binding parameters
        bindingDescription.binding = 0;                                     // Binding index in vertex binding array
        bindingDescription.stride = sizeof(Vertex);                         // Distance between elements within buffer
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;         // Specify whether vertex attribute is a function of vertex index or instance index

        return bindingDescription;
    }

    // Array of vertex attribute descriptions
    std::array<VkVertexInputAttributeDescription, 2> Vertex::GetAttributeDesciptions() {

        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions = {};        // Array of vertex attribute descriptions
        attributeDescriptions[0].binding = 0;                                               // Binding number which attribute recieves data from
        attributeDescriptions[0].location = 0;                                              // Shader binding location
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;                          // Size/type of vertex attribute data
        attributeDescriptions[0].offset = offsetof(Vertex, position);                       // Offset of attribute relative to start of vertex input binding

        attributeDescriptions[1].binding = 0;                                               // Binding number which attribute recieves data from
        attributeDescriptions[1].location = 1;                                              // Shader binding location
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;                       // Size/type of vertex attribute data
        attributeDescriptions[1].offset = offsetof(Vertex, color);                          // Offset of attribute relative to start of vertex input binding

        return attributeDescriptions;
    }

    // Create a buffer object
    int CreateBuffer(VkPhysicalDevice& _graphicscard, VkDevice& device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {

        VkBufferCreateInfo bufferInfo = {};                             // bufferInfo specifies the parameters of the buffer object
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;        // Specify bufferInfo as structure type BUFFER_CREATE_INFO
        bufferInfo.size = size;                                         // Size of buffer in bytes
        bufferInfo.usage = usage;                                       // Specify allowed buffer usage
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;             // Specify if buffer can be shared or not between queue families

        if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {          // If buffer creation fails
            ASWL::utilities::Logger("B0000", "Fatal Error: Buffer creation failed.");       // then log the error
            return 1;                                                                       // and return the corresponding error value
        }

        VkMemoryRequirements memoryRequirements;                                    // Structure specifies memory requirements of the buffer
        vkGetBufferMemoryRequirements(device, buffer, &memoryRequirements);         // Get the buffer's memory requirements and fill structure

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
            ASWL::utilities::Logger("B0001", "Fatal Error: Failed to find a suitable memory type.");        // then log the error
            return 2;                                                                                       // and return the corresponding error value
        }

        VkMemoryAllocateInfo allocInfo = {};                                // allocInfo specifies the parameters of the buffer memory allocation
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;           // Specify allocInfo as structure type MEMORY_ALLOCATE_INFO
        allocInfo.allocationSize = memoryRequirements.size;                 // Set memory allocation size
        allocInfo.memoryTypeIndex = static_cast<uint32_t>(memory);          // Set memory type index

        if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {                       // If allocation of buffer memory fails
            ASWL::utilities::Logger("B0002", "Fatal Error: Failed to allocate memory for the buffer.");         // then log the error
            return 3;                                                                                           // and return the corresponding error value
        }

        vkBindBufferMemory(device, buffer, bufferMemory, 0);        // Bind the allocated memory to the buffer

        return 0;
    }

    // Copy buffer from source to destination
    void CopyBuffer(VkDevice& device, VkCommandPool& cmdPool, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {

        VkCommandBufferAllocateInfo allocInfo = {};                             // allocInfo specifies the parameters of the command buffer memory allocation
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;       // Specify allocInfo as structure type COMMAND_BUFFER_ALLOCATE_INFO
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;                      // Specify command buffer level
        allocInfo.commandPool = cmdPool;                                        // Pointer to command pool
        allocInfo.commandBufferCount = 1;                                       // Number of command buffers

        VkCommandBuffer cmdBuffer;                                      // Command buffer object
        vkAllocateCommandBuffers(device, &allocInfo, &cmdBuffer);       // 

        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(cmdBuffer, &beginInfo);
    }

    // Function determines if all optional values in QueueFamilyIndices has a value
    bool QueueFamilyIndices::hasValue() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }

    // Finds and returns queue family supported by specified device
    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice& device, VkSurfaceKHR& surface) {

        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;                                                      // Number of queue families supported by the device
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);       // Get number of queue families supported by the device

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);                           // List of structs containing queue family properties
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());      // Then push queue families into list

        for (unsigned int i = 0; i < queueFamilies.size(); i++) {           // Iterate through all queue family properties
        
            if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)        // If queue family supports VK_QUEUE_GRAPHICS_BIT
                indices.graphicsFamily = i;                                 // Set graphics queue index
        
            VkBool32 presentSupport = false;                                                // Initialize present support
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);      // Check if physical device supports presenting to surface
        
            if (presentSupport)                 // If device supports presenting to surface
                indices.presentFamily = i;      // Set present queue index
        
            if (indices.hasValue())         // If the graphics family queue index has a value
                break;                      // break out of the loop
        }

        return indices;
    }
}
