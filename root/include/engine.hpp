// TheForge - src/engine (c) Andrew Woo, 2020

#pragma once

#ifndef THEFORGE_ENGINE
#define THEFORGE_ENGINE

// Standard Library
#include <string>
#include <vector>
#include <memory>
#include <utility>

// Dependencies
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <ASWL/utilities.hpp>

// TheForge includes
#include "forge_vars.hpp"
#include "vkcontainer.hpp"
#include "core/core.hpp"
#include "app/framework.hpp"

namespace Forge {    // Wrapper namespace

    class Engine {

    public:

        Engine();               // Default constructor
        ~Engine();              // Default destructor

        struct Metadata {       // This struct contains the metadata for the engine/glfwWindow. The data inside should be set BEFORE the engine is initialized.

            // Engine metadata
            bool autoinit = true;       // Automatically initialize engine components

            VkClearValue clearcolor = { 1.f, 1.f, 1.f, 0.f };       // Render surface clear color
            RendererType rendermode = RendererType::Render_2D;      // Engine render mode

            // Vulkan initialization metadata
            const char* vkAppName = "vkForgeDefault";       // VkApp app name
            ASWL::utilities::Version version;               // Application version

            // GLFW window initialization metadata
            std::vector<std::pair<int, int>> windowHints{ { std::make_pair(GLFW_CLIENT_API, GLFW_NO_API),               // Window hints _ default (GLFW_CLIENT_API, GLFW_NO_API)
                                                            std::make_pair(GLFW_RESIZABLE, GLFW_TRUE) } };              // Window hints _ default (GLFW_RESIZABLE, GLFW_FALSE)

            const char* windowTitle = "vkForgeDefault";                                                                 // Window title
            ASWL::utilities::Dimensions2D<int> windowDimensions{ ASWL::utilities::make_2d_dimension(1000, 600) };       // Window dimensions (x, y)

        }; Metadata metadata;

        int init();             // Initialize the engine. This function must be called before the engine is used.
        void cleanup();         // Cleanup Engine, Vulkan, and GLFW

        void update();          // Reinitialize swapchain Update the engine. This function should be called every iteration.
        void draw();            // Draw frames

        void SetClearColor();       // Set canvas clear color

        GLFWwindow* GetWindow();                // Returns the window instance
        VkInstance& GetInstance();              // Returns the Vulkan instance
        VkSurfaceKHR& GetSurface();             // Returns the Vulkan surface

        bool WindowShouldClose() const;         // Return if window should close

        const ASWL::utilities::Version version;     // Engine version

    private:

        GLFWwindow* window;                             // GLFW Window ... it's just the window.
        std::shared_ptr<Core::EngineCore> core;         // TheForge Engine core
        std::shared_ptr<App::Framework> framework;      // Rendering framework

        VkContainer container;      // Container for Engine's Vulkan components .. To be deprecated
    };
}

#endif // !THEFORGE_ENGINE
