// TheForge - src/engine (c) Andrew Woo, 2020

#pragma once

#ifndef THEFORGE_ENGINE
#define THEFORGE_ENGINE

// Standard Library
#include <string>
#include <vector>
#include <utility>

// Dependencies
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <ASWL/utilities.hpp>

// TheForge includes
#include "forge_vars.hpp"
#include "physical_devices.hpp"
#include "logical_devices.hpp"
#include "swapchain.hpp"
#include "pipeline.hpp"
#include "command_buffers.hpp"

namespace Forge {    // Wrapper namespace

    class Engine {

    public:

        Engine();               // Default constructor
        ~Engine();              // Default destructor

        int init();             // Initialize the engine. This function must be called before the engine is used.

        void update();          // Update the engine. This function should be called every iteration.
        void cleanup();         // Cleanup Engine, Vulkan, and GLFW

        struct MetaData {       // This struct contains the metadata for the engine/glfwWindow. The data inside should be set BEFORE the engine is initialized.

            // Vulkan initialization metadata
            const char* vkAppName{ "vkForgeDefault" };      // VkApp app name
            ASWL::utilities::Version version;               // Application version

            // GLFW window initialization metadata
            std::vector<std::pair<int, int>> windowHints{ { std::make_pair(GLFW_CLIENT_API, GLFW_NO_API),               // Window hints _ default (GLFW_CLIENT_API, GLFW_NO_API)
                                                            std::make_pair(GLFW_RESIZABLE, GLFW_FALSE) } };             // Window hints _ default (GLFW_RESIZABLE, GLFW_FALSE)

            const char* windowTitle{ "vkForgeDefault" };                                                                // Window title
            ASWL::utilities::Dimensions2D<int> windowDimensions{ ASWL::utilities::make_2d_dimension(1000, 600) };       // Window dimensions (x, y)

        }; MetaData metadata;

        GLFWwindow* GetWindow();                // Returns the window instance
        VkInstance& GetInstance();              // Returns the Vulkan instance
        VkSurfaceKHR& GetSurface();             // Returns the Vulkan surface

        bool WindowShouldClose() const;         // Return if window should close

    private:

        ASWL::utilities::Version version;       // Engine version

        GLFWwindow* window;         // GLFW Window ... it's just the window.
        VkInstance instance;        // Vulkan instance
        VkSurfaceKHR surface;       // Vulkan window surface

        std::vector<const char*> GetRequiredExtensions();       // Returns extensions required by application including GLFW

        GraphicsCard graphics_card;                         // Physical graphics card object
        LogicalGraphicsCard logical_graphics_card;          // Logical graphics card object
        Swapchain swapchain;                                // Swapchain object
        Pipeline pipeline;                                  // Pipeline object
        CommandBuffers command_buffers;                     // Command buffer objects


        // *** Start Validation Layer stuff ***************************************************************************************************************************************
        bool AllValidationLayersSupported();            // Check if all validation layers are supported
        VkDebugUtilsMessengerEXT dbgMessenger;          // Debug Callback handler

        // Setup Debug handler
        void SetupDebugMessenger();

        // Create Debug Message Handler
        VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,                                              // Vulkan Instance
                                              const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,            // Pointer to createDebugInfo struct
                                              const VkAllocationCallbacks* pAllocator,                          // Pointer to struct containing memory allocation data
                                              VkDebugUtilsMessengerEXT* pDebugMessenger);                       // Messenger object that passes debug message to callback

        static void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& debugCreateInfo);      // Populate Debug Create Info

        // Destroy Debug Message Handler
        void DestroyDebugUtilsMessengerEXT(VkInstance instance,                             // Vulkan Instance
                                           VkDebugUtilsMessengerEXT debugMessenger,         // Messenger object that passes debug message to callback
                                           const VkAllocationCallbacks* pAllocator);        // Pointer to struct containing memory allocation data

        // Validation Layer debug callback
        static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT msgSeverity,             // Report severity (diagnostic, info, warning, error)
                                                            VkDebugUtilsMessageTypeFlagsEXT msgType,                        // Type of report
                                                            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,      // Pointer to struct containing report details
                                                            void* pUsrData);                                                // Pointer to struct allowing user to pass data
        // *** End validation layer stuff *****************************************************************************************************************************************
    };
}

#endif // !THEFORGE_ENGINE
