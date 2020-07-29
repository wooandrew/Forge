// TheForge - src/engine (c) Andrew Woo, 2020


#include "engine.hpp"

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
            ASWL::utilities::Logger("E00G0", "Fatal Error: Failed to initialize GLFW -> !glfwInit().");     // Log the error if GLFW fails to initialize
            return 1;                                                                                       // then return the corresponding error value
        }

        // Set the window hint using engine metadata. Defaulted to (GLFW_CLIENT_API, GLFW_NO_API) and (GLFW_RESIZABLE, GLFW_FALSE).                    
        for (const auto hint : metadata.windowHints)
            glfwWindowHint(hint.first, hint.second);

        // Create window using engine metadata
        window = glfwCreateWindow(metadata.windowDimensions.width, metadata.windowDimensions.height, metadata.windowTitle, nullptr, nullptr);

        if (!window) {                                                                                              // Check window creation status
            ASWL::utilities::Logger("E01G1", "Fatal Error: Failed to create window -> !glfwCreateWindow().");       // Log the error if window creation fails
            glfwTerminate();                                                                                        // terminate GLFW

            return 2;                                                                                               // then return the corresponding error value
        }

        // Create window context
        glfwMakeContextCurrent(window);

        // If Engine components are set to initialize automatically
        if (metadata.autoinit) {

            core->coredata.ForgeVersion = version;
            core->coredata.vkAppName = metadata.vkAppName;
            core->coredata.AppVersion = metadata.version;

            int ret = core->init(window);        // Initialize the Engine's core
            if (ret != 0) {
                std::string msg = "Fatal Error: Failed to initialize engine core with error [" + std::to_string(ret) + "].";
                ASWL::utilities::Logger("E02C0", msg);
                return 3;
            }

            // Initialize rendering framework
            ret = framework->init(window, core);
            if (ret != 0) {                                                                                                                         // If rendering framework initialization fails
                std::string msg = "Fatal Error: Failed to initialize engine rendering framework with error [" + std::to_string(ret) + "].";         // 
                ASWL::utilities::Logger("E03F0", msg);                                                                                              // then log the error
                return 4;                                                                                                                           // and return the corresponding error value
            }

            //int ret = container.autoinit(window, core);        // Automatically initialize Vulkan components
            //if (ret != 0) {                                                                                                                 // If Vulkan component initialization fails
            //    std::string msg = "Fatal Error: Failed to initialize Vulkan components with error [" + std::to_string(ret) + "].";          //
            //    ASWL::utilities::Logger("E05V2", msg);                                                                                      // then log the error
            //    return 6;                                                                                                                   // and return the corresponding error value
            //}
            
            // Initialize renderer
            //ret = container.initRenderer();
            //if (ret != 0) {                                                                                                         // If renderer initialization fails
            //    std::string msg = "Fatal Error: Failed to initialize renderer with error [" + std::to_string(ret) + "].";           //
            //    ASWL::utilities::Logger("E04R0", msg);                                                                              // then log the error
            //    return 5;                                                                                                           // and return the corresponding error value
            //}
        }

        return 0;
    }

    // Update engine
    void Engine::update() {
        glfwPollEvents();       // Poll events
    }

    // Draw frames
    void Engine::draw() {

        int renderResult = 0;

        //if (metadata.rendermode == RendererType::Render_2D)
        //    renderResult = container.render2D.draw(core->gpu->GQueue(), core->gpu->PQueue());
        //else if (metadata.rendermode == RendererType::Render_3D)
        //    ASWL::utilities::Logger("XXR3D", "3D Rendering is not yet supported.");
        //
        //if (renderResult == 2 || renderResult == 5)
        //    int reinitCode = container.reinitialize(window, core->surface);
    }

    // Set render surface clear color
    void Engine::SetClearColor() {
        //container.cb.SetCanvasClearColor(metadata.clearcolor);
        //container.reinitialize(window, core->surface);

        framework->reinitialize(window);
    }

 
    // Return window instance on request
    GLFWwindow* Engine::GetWindow() {
        return window;
    }

    // Return the Vulkan instance on request
    VkInstance& Engine::GetInstance() {
        return core->instance;
    }
    // Returns the Vulkan surface on request
    VkSurfaceKHR& Engine::GetSurface() {
        return core->surface;
    }

    // Return whether or not window should close based on polled events
    bool Engine::WindowShouldClose() const {
        return glfwWindowShouldClose(window);
    }

    // Cleanup Engine -> Vulkan/GLFW
    void Engine::cleanup() {

        //container.cleanup();
        framework->cleanup();
        core->cleanup();

        glfwDestroyWindow(window);      // Destroy window
        glfwTerminate();                // then terminate GLFW
    }
}
