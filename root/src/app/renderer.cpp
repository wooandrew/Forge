// TheForge - src/app/renderer (c) Andrew Woo, 2020

#pragma warning(disable : 26812)

#include "app/renderer.hpp"
#include <ASWL/utilities.hpp>

namespace Forge::App {

    // Default constructor
    Renderer::Renderer() {

        core = std::make_shared<Core::EngineCore>();
        framework = std::make_shared<Framework>();

        clearCanvasColor = { 1.f, 1.f, 1.f, 0.f };

        VertexBuffer = VK_NULL_HANDLE;
        CommandPool = VK_NULL_HANDLE;

        type = RendererType::Render_2D;
    }

    // Default destructor
    Renderer::~Renderer() {

    }

    // Initialize renderer
    int Renderer::init(std::shared_ptr<Core::EngineCore> _core, std::shared_ptr<App::Framework> _framework) {

        core = _core;
        framework = _framework;

        QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(core->GetPGPU(), core->GetSurface());

        VkCommandPoolCreateInfo commandPoolInfo = {};                                       // commandPoolInfo specifies the parameters of the command pool
        commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;                 // Identify commandPoolInfo as structure type COMMAND_POOL_CREATE_INFO
        commandPoolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();       // Set queue family index
        commandPoolInfo.flags = 0;                                                          // Set flags

        if (vkCreateCommandPool(core->GetLGPU(), &commandPoolInfo, nullptr, &CommandPool) != VK_SUCCESS) {      // If command pool creation fails
            ASWL::utilities::Logger("R00C0", "Fatal Error: Failed to create command pool.");                    // then log the error
            return 1;                                                                                           // and return the corresponding value
        }

        VmaAllocatorCreateInfo allocatorInfo = {};
        allocatorInfo.physicalDevice = core->GetPGPU();
        allocatorInfo.device = core->GetLGPU();
        allocatorInfo.instance = core->GetInstance();
        
        if (vmaCreateAllocator(&allocatorInfo, &allocator) != VK_SUCCESS) {
            ASWL::utilities::Logger("R01V0", "Fatal Error: Failed to create vertex buffer.");
            return 2;
        }

        // Vertex buffer size
        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

        VkBufferCreateInfo bufferInfo = {};                             // bufferInfo specifies the parameters of the buffer object
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;        // Specify bufferInfo as structure type BUFFER_CREATE_INFO
        bufferInfo.size = bufferSize;                                         // Size of buffer in bytes
        bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;                                       // Specify allowed buffer usage
        //bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;             // Specify if buffer can be shared or not between queue families

        VmaAllocationCreateInfo allocInfo = {};             // allocInfo specifies the parameters of the memeory allocation object
        allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;      // Specify memory usage

        if (vmaCreateBuffer(allocator, &bufferInfo, &allocInfo, &VertexBuffer, &vbAllocation, nullptr) != VK_SUCCESS) {       // If buffer creation fails
            ASWL::utilities::Logger("B0000", "Fatal Error: Buffer creation failed.");                                   // then log the error
            return 1;                                                                                                   // and return the corresponding error value
        }

        //// Create a vertex buffer object
        //if (CreateBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VertexBuffer, allocator, vbAllocation) != VK_SUCCESS) {      // If vertex staging buffer creation fails
        //    ASWL::utilities::Logger("R01V0", "Fatal Error: Failed to create vertex buffer.");                                                                           // then log the error
        //    return 2;                                                                                                                                                   // and return the corresponding error value
        //}

        //void* data;
        //vkMapMemory(core->GetLGPU(), stagingBufferMemory, 0, bufferSize, 0, &data);     // Map memory to staging buffer memory
        //memcpy(data, vertices.data(), (size_t)bufferSize);                              // Copy data from vertices to data
        //vkUnmapMemory(core->GetLGPU(), stagingBufferMemory);                            // Unmap previously mapped data from VertexBufferMemory

        int status = CreateCommandBuffers();

        // If the renderer is a 2D renderer
        if (type == RendererType::Render_2D) {

            ImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);                      // Resize list of semaphore signal handles to match in-flight frame count
            RenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);                      // Resize list of semaphore signal handles to match in-flight frame count
            InFlightFences.resize(MAX_FRAMES_IN_FLIGHT);                                // Resize list of fences to match in-flight frame count
            InFlightImages.resize(framework->GetImages().size(), VK_NULL_HANDLE);       // Resize list of in-flight images to match number of images in swapchain

            VkSemaphoreCreateInfo semaphoreCreateInfo = {};                             // semaphoreCreateInfo specifies the parameters of the semaphore object
            semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;        // Specify semaphoreCreateInfo as structure type SEMAPHORE_CREATE_INFO

            VkFenceCreateInfo fenceCreateInfo = {};                             // fenceCreateInfo specifies the parameters of the fence object
            fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;        // Specify fenceCreateInfo as structure type FENCE_CREATE_INFO
            fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;               // Bitmask flags specifying initial state and behavior of fence

            for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {

                if (vkCreateSemaphore(core->GetLGPU(), &semaphoreCreateInfo, nullptr, &ImageAvailableSemaphores[i]) != VK_SUCCESS ||        // If ImageAvailable semaphore creation fails
                    vkCreateSemaphore(core->GetLGPU(), &semaphoreCreateInfo, nullptr, &RenderFinishedSemaphores[i]) != VK_SUCCESS ||        // or RenderFinished semaphore creation fails
                    vkCreateFence(core->GetLGPU(), &fenceCreateInfo, nullptr, &InFlightFences[i]) != VK_SUCCESS) {                          // or fence creation fails

                    std::string msg = "Fatal Error: Failed to create semaphore/fence object at index [" + std::to_string(i) + "].";         //
                    ASWL::utilities::Logger("R2SF0", msg);                                                                                  // then log the error
                    return 3;                                                                                                               // and return the corresponding error value
                }
            }
        }
        else if (type == RendererType::Render_3D) {         // If the renderer is a 3D renderer
            ASWL::utilities::Logger("XXR3D", "3D Rendering is not yet supported.");
            return -1;
        }

        return status;
    }

    int Renderer::CreateCommandBuffers() {

        cmdBuffers.resize(framework->GetFramebuffers().size());         // Resize command buffers list to match framebuffers size

        VkCommandBufferAllocateInfo allocInfo = {};                                     // allocInfo specifies the parameters of the command buffer allocation info
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;               // Identify allocInfo as structure type COMMAND_BUFFER_ALLOCATE_INFO
        allocInfo.commandPool = CommandPool;                                            // Set command pool
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;                              // Set command buffer level
        allocInfo.commandBufferCount = static_cast<uint32_t>(cmdBuffers.size());        // Number of command buffers

        if (vkAllocateCommandBuffers(core->GetLGPU(), &allocInfo, cmdBuffers.data()) != VK_SUCCESS) {       // If command buffer allocation fails
            ASWL::utilities::Logger("R03C1", "Fatal Error: Failed to allocate command buffers.");           // then log the error
            return 4;                                                                                       // and return the corresponding value
        }

        // Iterate through every command buffer
        for (size_t i = 0; i < cmdBuffers.size(); i++) {

            VkCommandBufferBeginInfo beginInfo = {};                                // beginInfo specifies the parameters of the command buffer begin operation
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;          // Identify beginInfo as structure type COMMAND_BUFFER_BEGIN_INFO

            if (vkBeginCommandBuffer(cmdBuffers[i], &beginInfo) != VK_SUCCESS) {                                                // If beginning command buffer fails
                std::string msg = "Fatal Error: Failed to begin command buffer at index [" + std::to_string(i) + "].";          // 
                ASWL::utilities::Logger("R04C2", msg);                                                                          // then log the error
                return 5;                                                                                                       // and return the corresponding value
            }

            VkRenderPassBeginInfo renderpassBeginInfo = {};                             // renderpassBeginInfo specifies the parameters of the render pass begin operation
            renderpassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;       // Identify renderpassBeginInfo as structure type RENDER_PASS_BEGIN_INFO
            renderpassBeginInfo.renderPass = framework->GetRenderPass();                // Set renderpass
            renderpassBeginInfo.framebuffer = framework->GetFramebuffers()[i];          // Set framebuffer
            renderpassBeginInfo.renderArea.offset = { 0, 0 };                           // Set render canvas offset
            renderpassBeginInfo.renderArea.extent = framework->GetExtent();             // Set render canvas extent
            renderpassBeginInfo.clearValueCount = 1;                                    // Number of elements in pClearValue
            renderpassBeginInfo.pClearValues = &clearCanvasColor;                       // List of canvas clear values

            VkBuffer buffers[] = { VertexBuffer };      // Array of vertex buffers
            VkDeviceSize offsets[] = { 0 };             // Vulkan device memory size/offset

            vkCmdBeginRenderPass(cmdBuffers[i], &renderpassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);              // Start a new render pass
            vkCmdBindPipeline(cmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, framework->GetPipeline());        // Bind pipeline object to command buffer
            vkCmdBindVertexBuffers(cmdBuffers[i], 0, 1, buffers, offsets);                                      // Bind vertex buffer
            vkCmdDraw(cmdBuffers[i], static_cast<uint32_t>(vertices.size()), 1, 0, 0);                          // Draw primitive
            vkCmdEndRenderPass(cmdBuffers[i]);                                                                  // End render pass

            if (vkEndCommandBuffer(cmdBuffers[i]) != VK_SUCCESS) {                                                          // If ending command buffer fails
                std::string msg = "Fatal Error: Failed to end command buffer at index [" + std::to_string(i) + "].";        // 
                ASWL::utilities::Logger("R05C3", msg);                                                                      // then log the error
                return 6;                                                                                                   // and return the corresponding value
            }
        }

        return 0;
    }

    int Renderer::draw() {

        uint32_t imageIndex;                // Image index
        static int currentFrame = 0;        // Current frame

        bool swapchainSubOptimal = false;

        vkWaitForFences(core->GetLGPU(), 1, &InFlightFences[currentFrame], VK_TRUE, UINT64_MAX);        // Wait for fence to finish

        // Acquire next image to render from swapchain at imageIndex
        VkResult acquireResult = vkAcquireNextImageKHR(core->GetLGPU(), framework->GetSwapchain(), UINT64_MAX, ImageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

        if (acquireResult == VK_ERROR_OUT_OF_DATE_KHR) {                            // If the swapchain goes out of date
            ASWL::utilities::Logger("R01S0", "Error: Swapchain out of date.");      // then log the error
            return 2;                                                               // and return the corresponding error value
        }
        else if (acquireResult == VK_SUBOPTIMAL_KHR)        // If the swapchain becomes suboptimal
            swapchainSubOptimal = true;                     // flag swapchain to be reinitialized

        else if (acquireResult != VK_SUCCESS) {                                                                 // If acquiring an image to render from the swapchain fails
            ASWL::utilities::Logger("R02S1", "Error: Failed to acquire an image from the swapchain.");          // then log the error
            return 3;                                                                                           // and return the corresponding error value
        }

        if (InFlightImages[imageIndex] != VK_NULL_HANDLE)                                                   // If previous frame is using the fence
            vkWaitForFences(core->GetLGPU(), 1, &InFlightFences[currentFrame], VK_TRUE, UINT64_MAX);        // then wait for the fence to finish

        // Set image fence at index to fence at current frame
        InFlightImages[imageIndex] = InFlightFences[currentFrame];

        VkSemaphore waitSemaphores[] = { ImageAvailableSemaphores[currentFrame] };                      // List of semaphores to wait for
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };          // Wait until images are available to write image to surface

        VkSemaphore signalSeamphores[] = { RenderFinishedSemaphores[currentFrame] };        // List of semaphores to signal once command buffers have finished execution

        VkSubmitInfo submitInfo = {};                               // submitInfo specifies the parameters of queue submit operation
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;           // Specify submitInfo as structure type SUBMIT_INFO
        submitInfo.waitSemaphoreCount = 1;                          // Number of semaphores to wait on before executing command buffers
        submitInfo.pWaitSemaphores = waitSemaphores;                // Pointer to array of semaphores to wait on
        submitInfo.pWaitDstStageMask = waitStages;                  // Pointer to array of pipeline stages at which each corresponding semaphore wait will occur
        submitInfo.commandBufferCount = 1;                          // Number of command buffers
        submitInfo.pCommandBuffers = &cmdBuffers[imageIndex];       // Pointer to array of command buffers
        submitInfo.signalSemaphoreCount = 1;                        // Number of semaphores to signal after command buffer execution finishes
        submitInfo.pSignalSemaphores = signalSeamphores;            // Pointer to array of signal semaphores

        vkResetFences(core->GetLGPU(), 1, &InFlightFences[currentFrame]);      // Reset fence

        if (vkQueueSubmit(core->GetGraphicsQueue(), 1, &submitInfo, InFlightFences[currentFrame]) != VK_SUCCESS) {                  // If semaphore or command buffer sequence submission to queue fails
            ASWL::utilities::Logger("R03D0", "Error: Failed to submit semaphore/command buffer sequence to graphics queue.");       // then log the error
            return 4;                                                                                                               // and stop the rendering process
        }

        VkSwapchainKHR swapchains[] = { framework->GetSwapchain() };        // List of swapchains

        VkPresentInfoKHR presentInfo = {};                              // presentInfo specifies the parameters of a present queue
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;         // Specify presentInfo as structure type PRESENT_INFO_KHR
        presentInfo.waitSemaphoreCount = 1;                             // Number of semaphores to wait on
        presentInfo.pWaitSemaphores = signalSeamphores;                 // Pointer to signal semaphores
        presentInfo.swapchainCount = 1;                                 // Number of swapchains
        presentInfo.pSwapchains = swapchains;                           // Pointer to array of swapchains
        presentInfo.pImageIndices = &imageIndex;                        // Pointer to image index
        presentInfo.pResults = nullptr;                                 // Optional

        // Queue an image to render
        VkResult queuePresentResult = vkQueuePresentKHR(core->GetPresentQueue(), &presentInfo);

        if (queuePresentResult == VK_ERROR_OUT_OF_DATE_KHR) {                       // If the swapchain goes out of date
            ASWL::utilities::Logger("R01S0", "Error: Swapchain out of date.");      // then log the error
            return 2;                                                               // and return the corresponding error value
        }
        else if (queuePresentResult == VK_SUBOPTIMAL_KHR || swapchainSubOptimal) {                              // If the swapchain becomes suboptimal
            ASWL::utilities::Logger("R04S2", "Error: Swapcahin is suboptimal and must be reinitialized.");      // then log the error
            return 5;                                                                                           // and return the corresponding error value
        }
        else if (queuePresentResult != VK_SUCCESS) {                                                                    // If acquiring an image to render from the swapchain fails
            ASWL::utilities::Logger("R05Q0", "Error: Failed to queue an image from the swapchain to render.");          // then log the error
            return 6;                                                                                                   // and return the corresponding error value
        }

        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;       // Track which frame is being rendered

        return 0;
    }

    void Renderer::cleanup() {

        vkDeviceWaitIdle(core->GetLGPU());

        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {

            vkDestroySemaphore(core->GetLGPU(), ImageAvailableSemaphores[i], nullptr);
            vkDestroySemaphore(core->GetLGPU(), RenderFinishedSemaphores[i], nullptr);
            vkDestroyFence(core->GetLGPU(), InFlightFences[i], nullptr);
        }

        vkDestroyCommandPool(core->GetLGPU(), CommandPool, nullptr);

        vmaDestroyBuffer(allocator, VertexBuffer, vbAllocation);
        vmaDestroyAllocator(allocator);
    }
}
