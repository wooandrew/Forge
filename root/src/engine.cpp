// TheForge - src/engine (c) Andrew Woo, 2020


#include "engine.hpp"
#include "forge.hpp"

#include <cstring>

namespace Forge {

    // Engine constructor
    Engine::Engine() : version({ 0, 1, 0 }) {

        window = nullptr;
        core = std::make_shared<Core::EngineCore>();
        framework = std::make_shared<App::Framework>();
    }

    // Engine destructor
    Engine::~Engine() {
        cleanup();
    };

    // Initalize engine
    int Engine::init() {

        // Initialize GLFW
        if (!glfwInit()) {
            Logger("E00G0", "Fatal Error: Failed to initialize GLFW -> !glfwInit().");      // Log the error if GLFW fails to initialize
            return 1;                                                                       // then return the corresponding error value
        }

        // Set the window hint using engine metadata. Defaulted to (GLFW_CLIENT_API, GLFW_NO_API) and (GLFW_RESIZABLE, GLFW_FALSE).                    
        for (const auto hint : metadata.windowHints)
            glfwWindowHint(hint.first, hint.second);

        // Create window using engine metadata
        window = glfwCreateWindow(metadata.windowDimensions.width, metadata.windowDimensions.height, metadata.windowTitle, nullptr, nullptr);

        if (!window) {                                                                              // Check window creation status
            Logger("E01G1", "Fatal Error: Failed to create window -> !glfwCreateWindow().");        // Log the error if window creation fails
            glfwTerminate();                                                                        // terminate GLFW

            return 2;                                                                               // then return the corresponding error value
        }

        // Create window context
        glfwMakeContextCurrent(window);

        // If Engine components are set to initialize automatically
        if (metadata.autoinit) {

            core->coredata.ForgeVersion = version;
            core->coredata.vkAppName = metadata.vkAppName;
            core->coredata.AppVersion = metadata.version;

            // Initialize the Engine's core
            int ret = core->init(window);
            if (ret != FORGE_SUCCESS) {                                                                                             // If engine core initialization fails
                std::string msg = "Fatal Error: Failed to initialize engine core with error [" + std::to_string(ret) + "].";        //
                Logger("E02C0", msg);                                                                                               // then log the error
                return 3;                                                                                                           // and return the corresponding error value
            }

            // Initialize rendering framework
            ret = framework->init(window, core);
            if (ret != FORGE_SUCCESS) {                                                                                                             // If rendering framework initialization fails
                std::string msg = "Fatal Error: Failed to initialize engine rendering framework with error [" + std::to_string(ret) + "].";         // 
                Logger("E03F0", msg);                                                                                                               // then log the error
                return 4;                                                                                                                           // and return the corresponding error value
            }

            // Initialize renderer
            ret = renderer.init(core, framework);
            if (ret != FORGE_SUCCESS) {                                                                                         // If renderer initialization fails
                std::string msg = "Fatal Error: Failed to initialize renderer with error [" + std::to_string(ret) + "].";       //
                Logger("E04R0", msg);                                                                                           // then log the error
                return 5;                                                                                                       // and return the corresponding error value
            }
        }

        return 0;
    }

    // Update engine
    void Engine::update() {
        glfwPollEvents();       // Poll events
    }

    // Draw frames
    int Engine::draw() {

        int ret = renderer.draw();

        if (ret == 16 || ret == 19) {

            int reinitFramework = framework->reinitialize(window);
            if (reinitFramework != FORGE_SUCCESS)
                return 5;

            int reinitRenderer = renderer.reinitialize();
            if (reinitRenderer != FORGE_SUCCESS)
                return 6;

            Logger("E05D0", "Renderer reinitialization succeeded.");
        }

        return 0;
    }

    // Set render surface clear color
    void Engine::SetClearColor() {
        renderer.SetClearColor(metadata.clearcolor);
        renderer.reinitialize();
    }

 
    // Return window instance on request
    GLFWwindow* Engine::GetWindow() {
        return window;
    }
    // Return the Vulkan instance on request /// TO DEPRECATE
    VkInstance& Engine::GetInstance() {
        return core->GetInstance();
    }
    // Returns the Vulkan surface on request /// TO DEPRECATE
    VkSurfaceKHR& Engine::GetSurface() {
        return core->GetSurface();
    }

    // Return whether or not window should close based on polled events
    bool Engine::WindowShouldClose() const {
        return glfwWindowShouldClose(window);
    }

    // Cleanup Engine -> Vulkan/GLFW
    void Engine::cleanup() {

        //container.cleanup();
        renderer.cleanup();
        framework->cleanup();
        core->cleanup();

        glfwDestroyWindow(window);      // Destroy window
        glfwTerminate();                // then terminate GLFW
    }
}
