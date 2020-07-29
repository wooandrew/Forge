// TheForge - src/app/framework (c) Andrew Woo, 2020

#pragma once

#ifndef THEFORGE_APP_FRAMEWORK
#define THEFORGE_APP_FRAMEWORK

// Standard Library
#include <string>
#include <vector>
#include <memory>

// Dependencies
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <ASWL/utilities.hpp>

// TheForge includes
#include "core/core.hpp"
//#include "core/graphicscard.hpp"

namespace Forge::App {

    enum class ShaderLanguage {
        THE_FORGE_VK_SHADER_LANGUAGE_SPV,
        THE_FORGE_VK_SHADER_LANGUAGE_GLSL
    };

    enum class ShaderType {
        THE_FORGE_VK_SHADER_TYPE_VERTEX,
        THE_FORGE_VK_SHADER_TYPE_FRAGMENT
    };

    class Framework {

        /**
         * TheForge App Framework
         * Contains rendering framework
         *
         * The app framework initializes low-level
         * rendering components for higher-level app
         * use. The components in this object/class
         * is handled by Vulkan/Forge and not the
         * end-user.
        **/

    public:

        Framework();        // Default constructor
        ~Framework();       // Default destructor

        int init(GLFWwindow* _window, std::shared_ptr<Core::EngineCore> _core);         // Initialize the rendering framework
        int reinitialize(GLFWwindow* _window);                                          // Reinitialize rendering framework

        // Cleanup framework
        void cleanup();

    private:

        int initSwapchain(GLFWwindow* _window);         // Initialize swapchain object
        int initRenderPass();                           // Initialize renderpass object
        int initFramebuffers();                         // Initialize framebuffers
        int initPipeline();                             // Initialize pipeline object

        // Helper functions
        int LoadShader(const std::string& path, ShaderType type, ShaderLanguage language);      // Loads shaders

        // Metadata for Shaders. Should be set before shaders are initialized.
        struct ShaderMetadata {
            std::string vertShaderPath = "Assets/vert.spv";         // Path to vertex shader
            std::string fragShaderPath = "Assets/frag.spv";         // Path to fragment shader
        }; ShaderMetadata shaderMetadata;


        // Private Member Variables
        std::shared_ptr<Core::EngineCore> core;         // TheForge Engine core

        VkSwapchainKHR swapchain;       // Handle to swapchain object

        VkFormat ImageFormat;                       // Swapchain image format
        VkExtent2D extent;                          // 2D array containing swapchain extent
        std::vector<VkImage> images;                // List of handles to image objects
        std::vector<VkImageView> ImageViews;        // List of handles to image view object
        
        VkRenderPass RenderPass;        // Handle to renderpass object

        std::vector<VkFramebuffer> framebuffers;        // List of handles to swapchain framebuffers
        
        std::vector<char> vert;         // Vertex shader buffer
        std::vector<char> frag;         // Fragment shader buffer

        VkPipelineLayout PipelineLayout;        // Handle to pipeline layout object
        VkPipeline pipeline;                    // Handle to graphics pipeline object
    };
}

#endif // !THEFORGE_APP_FRAMEWORK
