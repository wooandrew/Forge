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
        CommandPool = VK_NULL_HANDLE;
    }
    // Default destructor
    CommandBuffers::~CommandBuffers() {

    }

    // Initialize command buffers
    int CommandBuffers::init(VkPhysicalDevice& graphicscard, VkDevice& device, VkSurfaceKHR& surface, Swapchain& swapchain, Pipeline& pipeline) {

        this->device = device;

        QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(graphicscard, surface);

        VkCommandPoolCreateInfo commandPoolInfo = {};                                       // commandPoolInfo specifies the parameters of the command pool
        commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;                 // Identify commandPoolInfo as structure type COMMAND_POOL_CREATE_INFO
        commandPoolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();       // Set queue family index
        commandPoolInfo.flags = 0;                                                          // Set flags

        if (vkCreateCommandPool(device, &commandPoolInfo, nullptr, &CommandPool) != VK_SUCCESS) {       // If command pool creation fails
            ASWL::utilities::Logger("CB0P0", "Fatal Error: Failed to create command pool.");            // then log the error
            return 1;                                                                                   // and return the corresponding value
        }

        command_buffers.resize(swapchain.GetFramebuffers().size());         // Resize command buffers list to match framebuffers size

        VkCommandBufferAllocateInfo allocInfo = {};                                         // allocInfo specifies the parameters of the command buffer allocation info
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;                   // Identify allocInfo as structure type COMMAND_BUFFER_ALLOCATE_INFO
        allocInfo.commandPool = CommandPool;                                                // Set command pool
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;                                  // Set command buffer level
        allocInfo.commandBufferCount = static_cast<uint32_t>(command_buffers.size());       // Number of command buffers

        if (vkAllocateCommandBuffers(device, &allocInfo, command_buffers.data()) != VK_SUCCESS) {       // If command buffer allocation fails
            ASWL::utilities::Logger("CB001", "Fatal Error: Failed to allocate command buffers.");       // then log the error
            return 2;                                                                                   // and return the corresponding value
        }

        for (size_t i = 0; i < command_buffers.size(); i++) {

            VkCommandBufferBeginInfo beginInfo = {};                                // beginInfo specifies the parameters of the command buffer begin operation
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;          // Identify beginInfo as structure type COMMAND_BUFFER_BEGIN_INFO
            
            if (vkBeginCommandBuffer(command_buffers[i], &beginInfo) != VK_SUCCESS) {                                           // If beginning command buffer fails
                std::string msg = "Fatal Error: Failed to begin command buffer at index [" + std::to_string(i) + "].";          // 
                ASWL::utilities::Logger("CB002", msg);                                                                          // then log the error
                return 3;                                                                                                       // and return the corresponding value
            }

            VkRenderPassBeginInfo renderpassBeginInfo = {};                             // renderpassBeginInfo specifies the parameters of the renderpass begin operation
            renderpassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;       // Identify renderpassBeginInfo as structure type RENDER_PASS_BEGIN_INFO
            renderpassBeginInfo.renderPass = pipeline.GetRenderPass();                  // Set renderpass
            renderpassBeginInfo.framebuffer = swapchain.GetFramebuffers()[i];           // Set framebuffer
            renderpassBeginInfo.renderArea.offset = { 0, 0 };                           // Set render canvas offset
            renderpassBeginInfo.renderArea.extent = swapchain.GetExtent();              // Set render canvas extent

            VkClearValue clearCanvasColor = { 0.f, 0.f, 0.f, 0.f };         // Set render canvas clearing color
            renderpassBeginInfo.clearValueCount = 1;                        // Number of elements in pClearValue
            renderpassBeginInfo.pClearValues = &clearCanvasColor;           // List of canvas clear values
        }



        return 0;
    }

    void CommandBuffers::cleanup() {
        vkDestroyCommandPool(device, CommandPool, nullptr);
    }
}
