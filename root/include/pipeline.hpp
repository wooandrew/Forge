// TheForge - src/pipeline (c) Andrew Woo, 2020

#ifndef THEFORGE_PIPELINE
#define THEFORGE_PIPELINE

// Standard Library
#include <string>
#include <vector>

// Dependencies
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// TheForge includes
#include "swapchain.hpp"

namespace Forge {

    enum class ShaderLanguage {
        THE_FORGE_VK_SHADER_LANGUAGE_SPV,
        THE_FORGE_VK_SHADER_LANGUAGE_GLSL
    };

    enum class ShaderType {
        THE_FORGE_VK_SHADER_TYPE_VERTEX,
        THE_FORGE_VK_SHADER_TYPE_FRAGMENT
    };

    class Pipeline {

    public:

        Pipeline();       // Default constructor
        ~Pipeline();      // Default destructor

        int init(VkDevice& device, Swapchain& swapchain);       // Initialze shader

        // Loads shader given shader path, type, and language
        int LoadShader(const std::string& path, ShaderType type, ShaderLanguage language = ShaderLanguage::THE_FORGE_VK_SHADER_LANGUAGE_SPV);
        void cleanup();

        // Metadata for Shaders. Should be set before shaders are initialized.
        struct ShaderMetadata {
            std::string vertShaderPath = "Assets/vert.spv";         // Path to vertex shader
            std::string fragShaderPath = "Assets/frag.spv";         // Path to fragment shader
        }; ShaderMetadata shaderMetadata;

    private:

        std::vector<char> vert;         // Vertex shader buffer
        std::vector<char> frag;         // Fragment shader buffer

        VkDevice device;        // Handle to virtual device

        std::vector<VkFramebuffer> swapchainFramebuffers;       // List of handles to swapchain framebuffers

        VkPipelineLayout pipelineLayout;        // Handle to pipeline layout object
        VkRenderPass renderPass;                // Handle to renderpass object
        VkPipeline graphicsPipeline;            // Handle to graphics pipeline object
    };
}

#endif // !THEFORGE_PIPELINE
