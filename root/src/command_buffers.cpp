// TheForge - src/command_buffers (c) Andrew Woo, 2020

#include "command_buffers.hpp"
#include "forge_vars.hpp"

// Standard Library
#include <string>

// Dependencies
#include <ASWL/utilities.hpp>

namespace Forge {

    // Default constructor
    CommandBuffers::CommandBuffers() {
        clearCanvasColor = { 1.f, 1.f, 1.f, 0.f };
        CommandPool = VK_NULL_HANDLE;
    }
    // Default destructor
    CommandBuffers::~CommandBuffers() {
    }

    // Initialize command buffers
    int CommandBuffers::init(VkPhysicalDevice& _graphicscard, VkDevice& _device, VkSurfaceKHR& _surface, Swapchain& _swapchain, Pipeline& _pipeline, VkBuffer& _buffer) {

        device = _device;

        QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(_graphicscard, _surface);

        VkCommandPoolCreateInfo commandPoolInfo = {};                                       // commandPoolInfo specifies the parameters of the command pool
        commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;                 // Identify commandPoolInfo as structure type COMMAND_POOL_CREATE_INFO
        commandPoolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();       // Set queue family index
        commandPoolInfo.flags = 0;                                                          // Set flags

        if (vkCreateCommandPool(device, &commandPoolInfo, nullptr, &CommandPool) != VK_SUCCESS) {       // If command pool creation fails
            ASWL::utilities::Logger("CB0P0", "Fatal Error: Failed to create command pool.");            // then log the error
            return 1;                                                                                   // and return the corresponding value
        }

        int status = CreateCommandBuffers(_surface, _swapchain, _pipeline, _buffer);

        return status;
    }

    // Create command buffers
    int CommandBuffers::CreateCommandBuffers(VkSurfaceKHR& _surface, Swapchain& _swapchain, Pipeline& _pipeline, VkBuffer& _buffer) {

        cmdBuffers.resize(_swapchain.GetFramebuffers().size());         // Resize command buffers list to match framebuffers size

        VkCommandBufferAllocateInfo allocInfo = {};                                     // allocInfo specifies the parameters of the command buffer allocation info
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;               // Identify allocInfo as structure type COMMAND_BUFFER_ALLOCATE_INFO
        allocInfo.commandPool = CommandPool;                                            // Set command pool
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;                              // Set command buffer level
        allocInfo.commandBufferCount = static_cast<uint32_t>(cmdBuffers.size());        // Number of command buffers

        if (vkAllocateCommandBuffers(device, &allocInfo, cmdBuffers.data()) != VK_SUCCESS) {            // If command buffer allocation fails
            ASWL::utilities::Logger("CB001", "Fatal Error: Failed to allocate command buffers.");       // then log the error
            return 2;                                                                                   // and return the corresponding value
        }

        // Iterate through every command buffer
        for (size_t i = 0; i < cmdBuffers.size(); i++) {

            VkCommandBufferBeginInfo beginInfo = {};                                // beginInfo specifies the parameters of the command buffer begin operation
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;          // Identify beginInfo as structure type COMMAND_BUFFER_BEGIN_INFO

            if (vkBeginCommandBuffer(cmdBuffers[i], &beginInfo) != VK_SUCCESS) {                                                // If beginning command buffer fails
                std::string msg = "Fatal Error: Failed to begin command buffer at index [" + std::to_string(i) + "].";          // 
                ASWL::utilities::Logger("CB002", msg);                                                                          // then log the error
                return 3;                                                                                                       // and return the corresponding value
            }

            VkRenderPassBeginInfo renderpassBeginInfo = {};                             // renderpassBeginInfo specifies the parameters of the render pass begin operation
            renderpassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;       // Identify renderpassBeginInfo as structure type RENDER_PASS_BEGIN_INFO
            renderpassBeginInfo.renderPass = _pipeline.GetRenderPass();                 // Set renderpass
            renderpassBeginInfo.framebuffer = _swapchain.GetFramebuffers()[i];          // Set framebuffer
            renderpassBeginInfo.renderArea.offset = { 0, 0 };                           // Set render canvas offset
            renderpassBeginInfo.renderArea.extent = _swapchain.GetExtent();             // Set render canvas extent
            renderpassBeginInfo.clearValueCount = 1;                                    // Number of elements in pClearValue
            renderpassBeginInfo.pClearValues = &clearCanvasColor;                       // List of canvas clear values

            VkBuffer buffers[] = { _buffer };       // Array of vertex buffers
            VkDeviceSize offsets[] = { 0 };         // Vulkan device memory size/offset

            vkCmdBeginRenderPass(cmdBuffers[i], &renderpassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);                      // Start a new render pass
            vkCmdBindPipeline(cmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline.GetGraphicsPipeline());         // Bind pipeline object to command buffer
            vkCmdBindVertexBuffers(cmdBuffers[i], 0, 1, buffers, offsets);                                              // Bind vertex buffer
            vkCmdDraw(cmdBuffers[i], static_cast<uint32_t>(vertices.size()), 1, 0, 0);                                  // Draw primitive
            vkCmdEndRenderPass(cmdBuffers[i]);                                                                          // End render pass

            if (vkEndCommandBuffer(cmdBuffers[i]) != VK_SUCCESS) {                                                          // If ending command buffer fails
                std::string msg = "Fatal Error: Failed to end command buffer at index [" + std::to_string(i) + "].";        // 
                ASWL::utilities::Logger("CB003", msg);                                                                      // then log the error
                return 4;                                                                                                   // and return the corresponding value
            }
        }

        return 0;
    }

    // Set canvas clear color
    void CommandBuffers::SetCanvasClearColor(VkClearValue clearcolor) {
        clearCanvasColor = clearcolor;
    }

    void CommandBuffers::cleanup() {
        vkDestroyCommandPool(device, CommandPool, nullptr);
    }
}
