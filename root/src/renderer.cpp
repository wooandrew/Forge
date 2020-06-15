// TheForge - src/2D/renderer (c) Andrew Woo, 2020

#pragma warning(disable : 26812)

#include "renderer.hpp"

// Standard Library
#include <string>

// Dependencies
#include <ASWL/utilities.hpp>

namespace Forge {

    // Default constructor
    Renderer::Renderer() {

    }
    // Default destructor
    Renderer::~Renderer() {

    }

    // Initialize renderer object
    int Renderer::init(LogicalDevice& _lgc, Swapchain& _swapchain, Pipeline& _pipeline, std::vector<VkCommandBuffer>& _cmdBuffers) {

        logical_graphics_card = _lgc;
        SetComponents(_swapchain, _pipeline, _cmdBuffers);

        if (type == RendererType::Render_2D) {          // If the renderer is a 2D renderer

            ImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);                  // Resize list of semaphore signal handles to match in-flight frame count
            RenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);                  // Resize list of semaphore signal handles to match in-flight frame count
            InFlightFences.resize(MAX_FRAMES_IN_FLIGHT);                            // Resize list of fences to match in-flight frame count
            InFlightImages.resize(swapchain.images.size(), VK_NULL_HANDLE);         // Resize list of in-flight images to match number of images in swapchain

            VkSemaphoreCreateInfo semaphoreCreateInfo = {};                             // semaphoreCreateInfo specifies the parameters of the semaphore object
            semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;        // Specify semaphoreCreateInfo as structure type SEMAPHORE_CREATE_INFO

            VkFenceCreateInfo fenceCreateInfo = {};                             // fenceCreateInfo specifies the parameters of the fence object
            fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;        // Specify fenceCreateInfo as structure type FENCE_CREATE_INFO
            fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;               // Bitmask flags specifying initial state and behavior of fence
            
            for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {

                if (vkCreateSemaphore(_lgc.device, &semaphoreCreateInfo, nullptr, &ImageAvailableSemaphores[i]) != VK_SUCCESS ||        // If ImageAvailable semaphore creation fails
                    vkCreateSemaphore(_lgc.device, &semaphoreCreateInfo, nullptr, &RenderFinishedSemaphores[i]) != VK_SUCCESS ||        // or RenderFinished semaphore creation fails
                    vkCreateFence(_lgc.device, &fenceCreateInfo, nullptr, &InFlightFences[i]) != VK_SUCCESS) {                          // or fence creation fails

                    std::string msg = "Fatal Error: Failed to create semaphore/fence object at index [" + std::to_string(i) + "].";         //
                    ASWL::utilities::Logger("2D0S0", msg);                                                                                  // then log the error
                    return 1;                                                                                                               // and return the corresponding error value
                }
            }
        }
        else if (type == RendererType::Render_3D) {         // If the renderer is a 3D renderer
            ASWL::utilities::Logger("XXR3D", "3D Rendering is not yet supported.");
            return -1;
        }

        return 0;
    }

    // Draw frame
    int Renderer::draw() {

        uint32_t imageIndex;                // Image index
        static int currentFrame = 0;        // Current frame

        bool swapchainSubOptimal = false;

        vkWaitForFences(logical_graphics_card.device, 1, &InFlightFences[currentFrame], VK_TRUE, UINT64_MAX);       // Wait for fence to finish

        // Acquire next image to render from swapchain at imageIndex
        VkResult acquireResult = vkAcquireNextImageKHR(logical_graphics_card.device, swapchain.swapchain, UINT64_MAX, ImageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

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

        if (InFlightImages[imageIndex] != VK_NULL_HANDLE)                                                               // If previous frame is using the fence
            vkWaitForFences(logical_graphics_card.device, 1, &InFlightFences[currentFrame], VK_TRUE, UINT64_MAX);       // then wait for the fence to finish

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

        vkResetFences(logical_graphics_card.device, 1, &InFlightFences[currentFrame]);      // Reset fence
        
        if (vkQueueSubmit(logical_graphics_card.graphicsQueue, 1, &submitInfo, InFlightFences[currentFrame]) != VK_SUCCESS) {       // If semaphore or command buffer sequence submission to queue fails
            ASWL::utilities::Logger("R03D0", "Error: Failed to submit semaphore/command buffer sequence to graphics queue.");       // then log the error
            return 4;                                                                                                               // and stop the rendering process
        }

        VkSwapchainKHR swapchains[] = { swapchain.swapchain };          // List of swapchains

        VkPresentInfoKHR presentInfo = {};                              // presentInfo specifies the parameters of a present queue
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;         // Specify presentInfo as structure type PRESENT_INFO_KHR
        presentInfo.waitSemaphoreCount = 1;                             // Number of semaphores to wait on
        presentInfo.pWaitSemaphores = signalSeamphores;                 // Pointer to signal semaphores
        presentInfo.swapchainCount = 1;                                 // Number of swapchains
        presentInfo.pSwapchains = swapchains;                           // Pointer to array of swapchains
        presentInfo.pImageIndices = &imageIndex;                        // Pointer to image index
        presentInfo.pResults = nullptr;                                 // Optional

        // Queue an image to render
        VkResult queuePresentResult = vkQueuePresentKHR(logical_graphics_card.presentQueue, &presentInfo);

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
    }

    // Set Vulkan components
    void Renderer::SetComponents(Swapchain& _swapchain, Pipeline& _pipeline, std::vector<VkCommandBuffer>& _cmdBuffers) {
        swapchain = _swapchain;
        pipeline = _pipeline;
        cmdBuffers = _cmdBuffers;
    }

    // Cleanup renderer
    void Renderer::cleanup() {

        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            vkDestroySemaphore(logical_graphics_card.device, ImageAvailableSemaphores[i], nullptr);
            vkDestroySemaphore(logical_graphics_card.device, RenderFinishedSemaphores[i], nullptr);
            vkDestroyFence(logical_graphics_card.device, InFlightFences[i], nullptr);
        }
    }
}
