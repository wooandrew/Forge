// TheForge - src/vkcontainer (c) Andrew Woo, 2020

#include "vkcontainer.hpp"

// Dependencies
#include "ASWL/utilities.hpp"

namespace Forge {

    VkContainer::VkContainer() {
        render2D.type = RendererType::Render_2D;
    }

    VkContainer::~VkContainer() {

    }

    int VkContainer::autoinit(GLFWwindow* window, std::shared_ptr<Core::EngineCore> _EngineCore) {

        int status = 0;         // Vulkan initialization status

        // Automatically choose a suitable graphics card to use
        //status = gc.autochoose(instance, surface);
        //if (status != 0) return 1;
        //
        //// Initialize logical graphics card object
        //status = ld.init(gc.device, surface);
        //if (status != 0) return 2;

        EngineCore = _EngineCore;

        // Initialize swapchain object
        status = sc.init(window, EngineCore->surface, EngineCore->GetPGPU(), EngineCore->GetLGPU());
        if (status != 0) return 3;

        // Initialize pipeline object
        status = pl.init(EngineCore->GetLGPU(), sc);
        if (status != 0) return 4;

        // Initialize vertex buffer
        status = vb.init(EngineCore->GetPGPU(), EngineCore->GetLGPU());
        if (status != 0) return 5;

        // Initialize swapchain framebuffers
        status = sc.initFramebuffers(pl.renderPass);
        if (status != 0) return 6;

        // Initialize command buffers
        status = cb.init(EngineCore->GetPGPU(), EngineCore->GetLGPU(), EngineCore->surface, sc, pl, vb.GetVertexBuffer());
        if (status != 0) return 7;

        return 0;
    }

    // Initialize graphics renderer
    int VkContainer::initRenderer() {

        int status = render2D.init(EngineCore->GetLGPU(), sc, pl, cb.cmdBuffers);
        if (status != 0) return status;

        return 0;
    }

    // Reinitialize swapchain
    int VkContainer::reinitialize(GLFWwindow* window, VkSurfaceKHR& surface) {

        int width = 0;
        int height = 0;

        do {

            glfwGetFramebufferSize(window, &width, &height);    // Get framebuffer extent
            if (width == 0 || height == 0)      // If framebuffer extent is minimized in any way
                glfwWaitEvents();               // wait for an event and reevaluate framebuffer size

        } while (width == 0 || height == 0);

        vkDeviceWaitIdle(EngineCore->GetLGPU());        // Wait for device to complete all operations

        sc.cleanup();       // Cleanup swapchain
        pl.cleanup();       // Cleanup pipeline

        // Free command buffers before recreation
        vkFreeCommandBuffers(EngineCore->GetLGPU(), cb.CommandPool, static_cast<uint32_t>(cb.cmdBuffers.size()), cb.cmdBuffers.data());

        auto reinitializeSwapchain = [&]() {

            int status = 0;

            // Reinitialize swapchain object
            status = sc.init(window, surface, EngineCore->GetPGPU(), EngineCore->GetLGPU());
            if (status != 0) return 1;

            // Reinitialize pipeline object
            status = pl.init(EngineCore->GetLGPU(), sc);
            if (status != 0) return 2;

            // Reinitialize swapchain framebuffers
            status = sc.initFramebuffers(pl.renderPass);
            if (status != 0) return 3;

            // Reinitialize command buffers
            status = cb.CreateCommandBuffers(surface, sc, pl, vb.GetVertexBuffer());
            if (status != 0) return 4;

            render2D.SetComponents(sc, pl, cb.cmdBuffers);

            return 0;
        };

        int ret = reinitializeSwapchain();
        if (ret != 0) {                                                                                                         // If swapchain reinitialization fails
            std::string msg = "Fatal Error: Failed to reinitialize swapchain with error [" + std::to_string(ret) + "].";        //
            ASWL::utilities::Logger("VKC00", msg);                                                                              // then log the error
            return 1;                                                                                                           // and return the corresponding error value
        }

        return 0;
    }

    void VkContainer::cleanup() {

        vkDeviceWaitIdle(EngineCore->GetLGPU());

        render2D.cleanup();

        vb.cleanup();
        pl.cleanup();
        sc.cleanup();
        cb.cleanup();
        //ld.cleanup();
    }
}
