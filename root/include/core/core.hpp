// TheForge - src/core/core (c) Andrew Woo, 2020

#pragma once

#ifndef THEFORGE_CORE
#define THEFORGE_CORE

// Standard Library
#include <memory>

// Dependencies
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <ASWL/utilities.hpp>

// TheForge includes
#include "core/graphicscard.hpp"

namespace Forge {

    class Engine;               // To be deprecated
    class VkContainer;          // To be deprecated

    namespace Core {

        namespace {

            struct CoreData { // Struct containing engine core initialization data

                ASWL::utilities::Version ForgeVersion;                  // Engine version

                // Vulkan initialization metadata
                const char* vkAppName = "vkForgeDefault";       // Application name
                ASWL::utilities::Version AppVersion;            // Application version

            };
        }

        class EngineCore {

            /**
             * TheForge Engine Core
             * Contains required Vulkan components
             *
             * The engine core cannot be directly accessed by the end user;
             * it must be initialized via the Engine, and its parameters
             * must be set using the engine metadata.
            **/

            friend class Forge::Engine;             // To be deprecated
            friend class Forge::VkContainer;        // To be deprecated

        public:

            EngineCore();         // Default constructor
            ~EngineCore();        // Default destructor

            int init(GLFWwindow* window);       // Initialize Forge::Core

            // Major Getters
            VkInstance& GetInstance();          // Returns Vulkan instance
            VkSurfaceKHR& GetSurface();         // Reutnrs Vulkan surface
            VkPhysicalDevice& GetPGPU();        // Returns Physical Graphics Card
            VkDevice& GetLGPU();                // Returns Logical Graphics Card

            // Minor Getters
            VkQueue& GetGraphicsQueue();        // Returns Graphics queue
            VkQueue& GetPresentQueue();         // Returns Present queue

        private:

            void cleanup();

            CoreData coredata;          // Engine core initialization data

            VkInstance instance;        // Handle to Vulkan instance
            VkSurfaceKHR surface;       // Handle to Vulkan surface

            std::shared_ptr<Core::GraphicsCard> gpu;      // Pointer to GraphicsCard object

            std::vector<const char*> GetRequiredExtensions();       // Returns extensions required by application including GLFW


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
}

#endif // !THEFORGE_CORE
