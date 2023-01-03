// TheForge - src/app/renderer (c) Andrew Woo, 2020

// Disable C6387 ---
#pragma warning(disable : 6385)
#pragma warning(disable : 6387)

#include "renderer.hpp"

#include <chrono>

#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>

namespace Forge::App {

    // Default constructor
    Renderer::Renderer() {

        core = nullptr;
        framework = nullptr;

        clearCanvasColor = { 1.f, 1.f, 1.f, 0.f };

        VertexBuffer = VK_NULL_HANDLE;
        CommandPool = VK_NULL_HANDLE;
        TempCommandBuffer = VK_NULL_HANDLE;

        type = RendererType::Render_2D;
    }

    // Default destructor
    Renderer::~Renderer() {

    }

    // Initialize renderer
    int Renderer::init(std::shared_ptr<Logger> _logger, std::shared_ptr<Forge::Core::EngineCore> _core, std::shared_ptr<App::Framework> _framework) {

        logger = _logger;
        core = _core;
        framework = _framework;
        
        int acS = CreateAllocator();            // Create memory allocator
        int cpS = CreateCommandPool();          // Create command pool
        int vbS = CreateVertexBuffer();         // Create vertex buffer object
        int ibS = CreateIndexBuffer();          // Create index buffer object
        int ubS = CreateUniformBuffers();       // Create uniform buffers
        int cbS = CreateCommandBuffers();       // Create command buffers
        int spS = CreateSemaphores();           // Create rendering semaphores

        std::string msg = "Renderer initialization status is [" + std::to_string(acS) + "|" + std::to_string(cpS) + "|" + std::to_string(vbS) + "|" + 
                                                                  std::to_string(ibS) + "|" + std::to_string(cbS) + "|" + std::to_string(ubS) + "|" + std::to_string(spS) + "].";
        logger->log("R0000", msg);

        return acS+ cpS + vbS + ibS + ubS + cbS + spS;
    }

    int Renderer::reinitialize() {

        vkDeviceWaitIdle(core->GetLGPU());        // Wait for device to complete all operations

        // Free command buffers before recreation
        vkFreeCommandBuffers(core->GetLGPU(), CommandPool, static_cast<uint32_t>(cmdBuffers.size()), cmdBuffers.data());

        if (CreateCommandBuffers() != 0) {                                                      // If command buffer reinitialization fails
            logger->log("R1RI0", "Fatal Error: Failed to reinitialize command buffers.");       // then log the error
            return 2;                                                                           // and return the corresponding error value
        }

        return 0;
    }

    void Renderer::SetClearColor(VkClearValue _clearCanvasColor) {
        clearCanvasColor = _clearCanvasColor;
    }

    // Start temporary command buffer
    int Renderer::StartSingleTimeCommand() {

        VkCommandBufferBeginInfo cmdBufBeginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
        cmdBufBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        if (vkBeginCommandBuffer(TempCommandBuffer, &cmdBufBeginInfo) != VK_SUCCESS) {              // If temporary command buffer fails to start properly
            logger->log("R2TB0", "Fatal Error: Failed to start temporary command buffer.");         // then log the error
            return 3;                                                                               // and return the corresponding error value
        }

        return 0;
    }

    // Stop temporary command buffer
    int Renderer::EndSingleTimeCommand() {

        if (vkEndCommandBuffer(TempCommandBuffer) != VK_SUCCESS) {                                  // If temporary command buffer fails to end properly
            logger->log("R3TB1", "Fatal Error: Failed to stop temporary command buffer.");          // then log the error
            return 4;                                                                               // and return the corresponding error value
        }

        VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };        // submitInfo specifies queue submit operation properties
        submitInfo.commandBufferCount = 1;                                  // Specify amount of command buffers
        submitInfo.pCommandBuffers = &TempCommandBuffer;                    // Pass temp command buffer

        if (vkQueueSubmit(core->GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {                    // If submiting graphics queue fails
            logger->log("R4TB2", "Fatal Error: Failed to submit graphics queue to temporary command buffer.");          // then log the error
            return 5;                                                                                                   // and return the corresponding error value
        }
        if (vkQueueWaitIdle(core->GetGraphicsQueue()) != VK_SUCCESS) {                              // If vkQueueWaitIdle fails
            logger->log("R5TB3", "Fatal Error: Failed to wait for queue to become idle.");          // then log the error
            return 6;                                                                               // and return the corresponding error value
        }

        return 0;
    }

    // Create allocator on call
    int Renderer::CreateAllocator() {

        VmaAllocatorCreateInfo allocatorInfo = {};          // allocatorInfo specifies the parameters of the allocator object
        allocatorInfo.physicalDevice = core->GetPGPU();     // Pass physical graphics card
        allocatorInfo.device = core->GetLGPU();             // Pass logical graphics card
        allocatorInfo.instance = core->GetInstance();       // Pass Vulkan instance

        if (vmaCreateAllocator(&allocatorInfo, &allocator) != VK_SUCCESS) {             // If memory allocator creation fails
            logger->log("R06V0", "Fatal Error: Failed to create vertex buffer.");       // then log the error
            return 7;                                                                   // and return the corresponding error value
        }

        return 0;
    }

    // Create CommandPool on call
    int Renderer::CreateCommandPool() {            

        QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(core->GetPGPU(), core->GetSurface());

        VkCommandPoolCreateInfo commandPoolInfo = {};                                       // commandPoolInfo specifies the parameters of the command pool
        commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;                 // Identify commandPoolInfo as structure type COMMAND_POOL_CREATE_INFO
        commandPoolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();       // Set queue family index
        commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;            // Set flags

        if (vkCreateCommandPool(core->GetLGPU(), &commandPoolInfo, nullptr, &CommandPool) != VK_SUCCESS) {      // If command pool creation fails
            logger->log("R07C0", "Fatal Error: Failed to create command pool.");                                // then log the error
            return 8;                                                                                           // and return the corresponding value
        }

        VkCommandBufferAllocateInfo tCbAllocInfo = {};                              // tCbAllocInfo specifies the parameters of the temporary command buffer allocation info
        tCbAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;        // Identify tCbAllocInfo as structure type COMMAND_BUFFER_ALLOCATE_INFO
        tCbAllocInfo.commandPool = CommandPool;                                     // Set command pool
        tCbAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;                       // Set command buffer level
        tCbAllocInfo.commandBufferCount = 1;                                        // Number of command buffers

        if (vkAllocateCommandBuffers(core->GetLGPU(), &tCbAllocInfo, &TempCommandBuffer) != VK_SUCCESS) {       // If temporary command buffer creation fails
            logger->log("R08C1", "Fatal Error: Failed to create temporary command buffer.");                    // then log the error
            return 9;                                                                                           // and return the corresponding error value
        }

        return 0;
    }

    // Create VertexBuffer on call
    int Renderer::CreateVertexBuffer() {

        // Vertex buffer size
        VkDeviceSize vbSize = sizeof(vertices) * _countof(vertices);

        VkBufferCreateInfo vbCreateInfo = {};                               // vbCreateInfo specifies the parameters of the buffer object
        vbCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;          // Specify vbCreateInfo as structure type BUFFER_CREATE_INFO
        vbCreateInfo.size = vbSize;                                         // Size of buffer in bytes
        vbCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;              // Specify buffer usage
        vbCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;               // Specify if buffer can be shared or not between queue families

        VmaAllocationCreateInfo vbAllocInfo = {};                   // vbAllocInfo specifies the parameters of the memory allocation object
        vbAllocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;              // Specify memory usage
        vbAllocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;       // vbAllocInfo flags

        VkBuffer vbStagingBuffer = VK_NULL_HANDLE;      // Staging buffer object
        VmaAllocation sVbAlloc = VK_NULL_HANDLE;        // Staging buffer allocation object
        VmaAllocationInfo sVbAllocInfo = {};            // Staging buffer allocation info
        sVbAllocInfo.pMappedData = 0;

        if (vmaCreateBuffer(allocator, &vbCreateInfo, &vbAllocInfo, &vbStagingBuffer, &sVbAlloc, &sVbAllocInfo) != VK_SUCCESS) {        // If staging buffer creation fails
            logger->log("R09V0", "Fatal Error: Vertex Staging Buffer creation failed.");                                                // then log the error
            return 10;                                                                                                                  // and return the corresponding error value
        }

        std::memcpy(sVbAllocInfo.pMappedData, vertices, vbSize);        // Copy vertices to mapped data in staging allocation info

        vbCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;          // Specify buffer usage
        vbAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;                                                      // Specify memory usage
        vbAllocInfo.flags = 0;                                                                              // vbAllocInfo has no flags
        
        if (vmaCreateBuffer(allocator, &vbCreateInfo, &vbAllocInfo, &VertexBuffer, &vbAllocation, nullptr) != VK_SUCCESS) {     // If vertex buffer creation fails
            logger->log("R10V1", "Fatal Error: Vertex Buffer creation failed.");                                                // then log the error
            return 11;                                                                                                          // and return the corresponding error value
        }

        int rSSTC = StartSingleTimeCommand();                               // Start temporary command buffer
        if (rSSTC != 0) {                                                   // If temporary command buffer failed to start properly
            vmaDestroyBuffer(allocator, vbStagingBuffer, sVbAlloc);         // then destroy temporary staging buffer vbStagingBuffer
            return rSSTC;                                                   // and return failure
        }

        VkBufferCopy vbCopyRegion = {};             // vbCopyRegion specifies the parameters for copying staging buffer to vertex buffer
        vbCopyRegion.srcOffset = 0;                 // Set source offset
        vbCopyRegion.dstOffset = 0;                 // Set destination offset
        vbCopyRegion.size = vbCreateInfo.size;      // Specify number of bytes to copy

        // Copy vbStagingBuffer to VertexBuffer
        vkCmdCopyBuffer(TempCommandBuffer, vbStagingBuffer, VertexBuffer, 1, &vbCopyRegion);

        int rESTC = EndSingleTimeCommand();                                 // Stop temporary command buffer
        if (rESTC != 0) {                                                   // If temporary command buffer failed to end properly
            vmaDestroyBuffer(allocator, vbStagingBuffer, sVbAlloc);         // then destroy temporary staging buffer vbStagingBuffer
            return rESTC;                                                   // and return failure.
        }

        // Destroy temporary staging buffer vbStagingBuffer
        vmaDestroyBuffer(allocator, vbStagingBuffer, sVbAlloc);

        return 0;
    }

    // Create IndexBuffer on call
    int Renderer::CreateIndexBuffer() {


        // Index buffer size
        VkDeviceSize ibSize = sizeof(uint16_t) * _countof(indices);

        VkBufferCreateInfo ibCreateInfo = {};                           // ibCreateInfo specifies the parameters of the buffer object
        ibCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;      // Specify ibCreateInfo as structure type BUFFER_CREATE_INFO
        ibCreateInfo.size = ibSize;                                     // Size of buffer in bytes
        ibCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;          // Specify buffer usage
        ibCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;           // Specify if buffer can be shared or not between queue families

        VmaAllocationCreateInfo ibAllocInfo = {};                   // ibAllocInfo specifies the parameters of the memory allocation object
        ibAllocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;              // Specify memory usage
        ibAllocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;       // vbAllocInfo flags

        VkBuffer ibStagingBuffer = VK_NULL_HANDLE;          // Staging buffer object
        VmaAllocation sIbAlloc = VK_NULL_HANDLE;            // Staging buffer allocation object
        VmaAllocationInfo sIbAllocInfo = {};                // Staging buffer allocation info
        sIbAllocInfo.pMappedData = 0;
        
        if (vmaCreateBuffer(allocator, &ibCreateInfo, &ibAllocInfo, &ibStagingBuffer, &sIbAlloc, &sIbAllocInfo) != VK_SUCCESS) {        // If staging buffer creation fails
            logger->log("R11I0", "Fatal Error: Index Staging Buffer creation failed.");                                                 // then log the error
            return 12;                                                                                                                  // and return the corresponding error value
        }

        memcpy(sIbAllocInfo.pMappedData, indices, ibSize);      // Copy indices to mapped data in staging allocation info

        ibCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;       // Specify buffer usage
        ibAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;                                                  // Specify memory usage
        ibAllocInfo.flags = 0;                                                                          // ibAllocInfo has no flags

        if (vmaCreateBuffer(allocator, &ibCreateInfo, &ibAllocInfo, &IndexBuffer, &ibAllocation, nullptr) != VK_SUCCESS) {      // If staging buffer creation fails
            logger->log("R12I1", "Fatal Error: Index Buffer creation failed.");                                                 // then log the error
            return 13;                                                                                                          // and return the corresponding error value
        }

        int rSSTC = StartSingleTimeCommand();                               // Start temporary command buffer
        if (rSSTC != 0) {                                                   // If temporary command buffer failed to start properly
            vmaDestroyBuffer(allocator, ibStagingBuffer, sIbAlloc);         // then destroy temporary staging buffer ibStagingBuffer
            return rSSTC;                                                   // and return failure
        }

        VkBufferCopy ibCopyRegion = {};             // vbCopyRegion specifies the parameters for copying staging buffer to index buffer
        ibCopyRegion.srcOffset = 0;                 // Set source offset
        ibCopyRegion.dstOffset = 0;                 // Set destination offset
        ibCopyRegion.size = ibCreateInfo.size;      // Specify number of bytes to copy

        // Copy ibStagingBuffer to IndexBuffer
        vkCmdCopyBuffer(TempCommandBuffer, ibStagingBuffer, IndexBuffer, 1, &ibCopyRegion);

        int rESTC = EndSingleTimeCommand();                                 // Stop temporary command buffer
        if (rESTC != 0) {                                                   // If temporary command buffer failed to end properly
            vmaDestroyBuffer(allocator, ibStagingBuffer, sIbAlloc);         // then destroy temporary staging buffer ibStagingBuffer
            return rESTC;                                                   // and return failure.
        }

        // Destroy temporary staging buffer ibStagingBuffer
        vmaDestroyBuffer(allocator, ibStagingBuffer, sIbAlloc);

        return 0;
    }

    // Create uniform buffers on call
    int Renderer::CreateUniformBuffers() {

        // Size of individual uniform buffers
        VkDeviceSize uboSize = sizeof(UniformBuffer);

        VkBufferCreateInfo uboCreateInfo = {};                              // uboCreateInfo specifies the parameters of the uniform buffers
        uboCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;         // Specify uboCreateInfo as structure type BUFFER_CREATE_INFO
        uboCreateInfo.size = uboSize;                                       // Size of buffer in bytes
        uboCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;           // Specify buffer usage
        uboCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;              // Specify if buffer can be shared or not between queue families

        VmaAllocationCreateInfo uboAllocInfo = {};                  // uboAllocInfo specifies the parameters of the memory allocation object
        uboAllocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;             // Specify memory usage
        uboAllocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;      // uboAllocInfo flags
        
        UniformBuffers.resize(framework->GetImages().size());       // Resize UniformBuffer vector to number of images in the swapchain
        ubAllocations.resize(framework->GetImages().size());        // Resize ubAllocations vector to number of images in the swapchain

        for (int i = 0; i < UniformBuffers.size(); i++) {

            if (vmaCreateBuffer(allocator, &uboCreateInfo, &uboAllocInfo, &UniformBuffers[i], &ubAllocations[i], nullptr) != VK_SUCCESS) {          // If staging buffer creation fails
                std::string msg = "Fatal Error: Failed to create staging uniform buffer object at index [" + std::to_string(i) + "].";              //
                logger->log("R13U0", msg);                                                                                                          // then log the error
                return 14;                                                                                                                          // and return the corresponding error value
            }
        }

        return 0;
    }

    // Create command buffers on call
    int Renderer::CreateCommandBuffers() {

        cmdBuffers.resize(framework->GetFramebuffers().size());         // Resize command buffers list to match framebuffers size

        VkCommandBufferAllocateInfo allocInfo = {};                                     // allocInfo specifies the parameters of the command buffer allocation info
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;               // Identify allocInfo as structure type COMMAND_BUFFER_ALLOCATE_INFO
        allocInfo.commandPool = CommandPool;                                            // Set command pool
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;                              // Set command buffer level
        allocInfo.commandBufferCount = static_cast<uint32_t>(cmdBuffers.size());        // Number of command buffers

        if (vkAllocateCommandBuffers(core->GetLGPU(), &allocInfo, cmdBuffers.data()) != VK_SUCCESS) {       // If command buffer allocation fails
            logger->log("R13C2", "Fatal Error: Failed to allocate command buffers.");                       // then log the error
            return 14;                                                                                      // and return the corresponding value
        }

        // Iterate through every command buffer
        for (size_t i = 0; i < cmdBuffers.size(); i++) {

            VkCommandBufferBeginInfo beginInfo = {};                            // beginInfo specifies the parameters of the command buffer begin operation
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;      // Identify beginInfo as structure type COMMAND_BUFFER_BEGIN_INFO

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

            if (vkBeginCommandBuffer(cmdBuffers[i], &beginInfo) != VK_SUCCESS) {                                                // If beginning command buffer fails
                std::string msg = "Fatal Error: Failed to begin command buffer at index [" + std::to_string(i) + "].";          // 
                logger->log("R14C3", msg);                                                                                      // then log the error
                return 15;                                                                                                      // and return the corresponding value
            }

            vkCmdBeginRenderPass(cmdBuffers[i], &renderpassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);              // Start a new render pass
            vkCmdBindPipeline(cmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, framework->GetPipeline());        // Bind pipeline object to command buffer
            vkCmdBindVertexBuffers(cmdBuffers[i], 0, 1, buffers, offsets);                                      // Bind vertex buffer
            vkCmdBindIndexBuffer(cmdBuffers[i], IndexBuffer, 0, VK_INDEX_TYPE_UINT16);                          // Bind index buffer
            vkCmdDrawIndexed(cmdBuffers[i], static_cast<uint32_t>(_countof(indices)), 1, 0, 0, 0);              // Draw primitive
            vkCmdEndRenderPass(cmdBuffers[i]);                                                                  // End render pass

            if (vkEndCommandBuffer(cmdBuffers[i]) != VK_SUCCESS) {                                                          // If ending command buffer fails
                std::string msg = "Fatal Error: Failed to end command buffer at index [" + std::to_string(i) + "].";        // 
                logger->log("R15C4", msg);                                                                                  // then log the error
                return 16;                                                                                                  // and return the corresponding value
            }
        }

        return 0;
    }

    // Create rendering semaphores/fences on call
    int Renderer::CreateSemaphores() {

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
                    logger->log("R16S0", msg);                                                                                              // then log the error
                    return 17;                                                                                                              // and return the corresponding error value
                }
            }
        }
        else if (type == RendererType::Render_3D) {         // If the renderer is a 3D renderer
            logger->log("XXR3D", "3D Rendering is not yet supported.");
            return -1;
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

        if (acquireResult == VK_ERROR_OUT_OF_DATE_KHR) {                // If the swapchain goes out of date
            logger->log("R17D0", "Error: Swapchain out of date.");      // then log the error
            return 18;                                                  // and return the corresponding error value
        }
        else if (acquireResult == VK_SUBOPTIMAL_KHR)        // If the swapchain becomes suboptimal
            swapchainSubOptimal = true;                     // flag swapchain to be reinitialized

        else if (acquireResult != VK_SUCCESS) {                                                     // If acquiring an image to render from the swapchain fails
            logger->log("R18D1", "Error: Failed to acquire an image from the swapchain.");          // then log the error
            return 19;                                                                              // and return the corresponding error value
        }

        // Update Uniform Buffer
        UpdateUBO(imageIndex);

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

        if (vkQueueSubmit(core->GetGraphicsQueue(), 1, &submitInfo, InFlightFences[currentFrame]) != VK_SUCCESS) {      // If semaphore or command buffer sequence submission to queue fails
            logger->log("R19D2", "Error: Failed to submit semaphore/command buffer sequence to graphics queue.");       // then log the error
            return 20;                                                                                                  // and stop the rendering process
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

        if (queuePresentResult == VK_ERROR_OUT_OF_DATE_KHR) {       // If the swapchain goes out of date
            logger->log("R17D0", "Error: Swapchain out of date.");       // then log the error
            return 18;                                              // and return the corresponding error value
        }
        else if (queuePresentResult == VK_SUBOPTIMAL_KHR || swapchainSubOptimal) {                      // If the swapchain becomes suboptimal
            logger->log("R20D3", "Error: Swapcahin is suboptimal and must be reinitialized.");          // then log the error
            return 21;                                                                                  // and return the corresponding error value
        }
        else if (queuePresentResult != VK_SUCCESS) {                                                        // If acquiring an image to render from the swapchain fails
            logger->log("R21D4", "Error: Failed to queue an image from the swapchain to render.");          // then log the error
            return 22;                                                                                      // and return the corresponding error value
        }

        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;       // Track which frame is being rendered

        return 0;
    }

    void Renderer::UpdateUBO(uint32_t _imageIndex) {

        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        UniformBuffer ubo = {};
        ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.proj = glm::perspective(glm::radians(45.0f), framework->GetExtent().width / (float)framework->GetExtent().height, 0.1f, 10.0f);
        ubo.proj[1][1] *= -1;

        VmaAllocationInfo uboAllocInfo = {};

        memcpy(uboAllocInfo.pMappedData, &ubo, sizeof(UniformBuffer));

    }

    void Renderer::cleanup() {

        vkDeviceWaitIdle(core->GetLGPU());

        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {

            vkDestroySemaphore(core->GetLGPU(), ImageAvailableSemaphores[i], nullptr);
            vkDestroySemaphore(core->GetLGPU(), RenderFinishedSemaphores[i], nullptr);
            vkDestroyFence(core->GetLGPU(), InFlightFences[i], nullptr);
        }

        vkDestroyCommandPool(core->GetLGPU(), CommandPool, nullptr);

        for (int i = 0; i < ubAllocations.size(); i++)
            vmaDestroyBuffer(allocator, UniformBuffers[i], ubAllocations[i]);

        UniformBuffers.erase(UniformBuffers.begin(), UniformBuffers.end());
        ubAllocations.erase(ubAllocations.begin(), ubAllocations.end());
            

        vmaDestroyBuffer(allocator, IndexBuffer, ibAllocation);
        vmaDestroyBuffer(allocator, VertexBuffer, vbAllocation);

        vmaDestroyAllocator(allocator);
    }
}
