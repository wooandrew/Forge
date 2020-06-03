// TheForge - src/shader (c) Andrew Woo, 2020

#ifndef THEFORGE_SHADER
#define THEFORGE_SHADER

// Standard Library
#include <string>
#include <vector>

// Dependencies
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Forge {

    enum class ShaderLanguage {
        THE_FORGE_VK_SHADER_LANGUAGE_SPV,
        THE_FORGE_VK_SHADER_LANGUAGE_GLSL
    };

    enum class ShaderType {
        THE_FORGE_VK_SHADER_TYPE_VERTEX,
        THE_FORGE_VK_SHADER_TYPE_FRAGMENT
    };

    class Shader {

    public:

        Shader();       // Default constructor
        ~Shader();      // Default destructor

        int init(VkDevice& device);     // Initialze shader

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

        VkDevice device;
    };
}

#endif // !THEFORGE_SHADER
