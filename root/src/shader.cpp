// TheForge - src/shader (c) Andrew Woo, 2020

#include "shader.hpp"

// Standard Library
#include <fstream>

// Dependencies
#include <ASWL/utilities.hpp>

namespace Forge {

    Shader::Shader() {
        device = VK_NULL_HANDLE;
    }
    Shader::~Shader() {

    }

    int Shader::init(VkDevice& device) {

        this->device = device;

        // Load Vertex Shader
        int vertShader = LoadShader(shaderMetadata.vertShaderPath, ShaderType::THE_FORGE_VK_SHADER_TYPE_VERTEX, ShaderLanguage::THE_FORGE_VK_SHADER_LANGUAGE_SPV);
        if (vertShader != 0)
            return vertShader;

        VkShaderModuleCreateInfo vertCreateInfo = {};                               // vertCreateInfo specifies the parameters of the vertex shader module
        vertCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;         // Identify vertCreateInfo as structure type SHADER_MODULE_CREATE_INFO
        vertCreateInfo.codeSize = vert.size();                                      // Size of the code in bytes
        vertCreateInfo.pCode = reinterpret_cast<const uint32_t*>(vert.data());      // Pointer to shader code

        VkShaderModule vertShaderModule = VK_NULL_HANDLE;                                                   // Vertex Shader Module
        if (vkCreateShaderModule(device, &vertCreateInfo, nullptr, &vertShaderModule)) {                    // If vertex shader module creation fails
            ASWL::utilities::Logger("S0000", "Fatal Error: Failed to create vertex shader module.");        // then log the error
            return 1;                                                                                       // and return the corresponding error value
        }

        // Create Vertex Shader Pipeline
        VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};                               // vertShaderStageInfo specifies the parameters of the vertex shader pipeline
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;        // Identify vertShaderStageInfo as structure type PIPELINE_SHADER_STAGE_CREATE_INFO
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;                                 // Specify the pipeline stage
        vertShaderStageInfo.module = vertShaderModule;                                          // Handle to shader module object
        vertShaderStageInfo.pName = "main";                                                     // Specify entry point name

        // Load Fragment Shader
        int fragShader = LoadShader(shaderMetadata.fragShaderPath, ShaderType::THE_FORGE_VK_SHADER_TYPE_FRAGMENT, ShaderLanguage::THE_FORGE_VK_SHADER_LANGUAGE_SPV);
        if (fragShader != 0)
            return fragShader;

        VkShaderModuleCreateInfo fragCreateInfo = {};                               // fragCreateInfo specifies the parameters of the vertex shader module
        vertCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;         // Identify fragCreateInfo as structure type SHADER_MODULE_CREATE_INFO
        vertCreateInfo.codeSize = frag.size();                                      // Size of the code in bytes
        vertCreateInfo.pCode = reinterpret_cast<const uint32_t*>(frag.data());      // Pointer to shader code

        VkShaderModule fragShaderModule = VK_NULL_HANDLE;                                                   // Fragment Shader Module
        if (vkCreateShaderModule(device, &fragCreateInfo, nullptr, &fragShaderModule)) {                    // If fragment shader module creation fails
            ASWL::utilities::Logger("S0001", "Fatal Error: Failed to create fragment shader module.");      // then log the error
            return 2;                                                                                       // and return the corresponding error value
        }

        // Create Fragment Shader Pipeline
        VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};                               // fragShaderStageInfo specifies the parameters of the fragment shader pipeline
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;        // Identify fragShaderStageInfo as structure type PIPELINE_SHADER_STAGE_CREATE_INFO
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;                               // Specify the pipeline stage
        fragShaderStageInfo.module = fragShaderModule;                                          // Handle to shader module object
        fragShaderStageInfo.pName = "main";                                                     // Specify entry point name

        VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

        vkDestroyShaderModule(device, vertShaderModule, nullptr);
        vkDestroyShaderModule(device, fragShaderModule, nullptr);

        return 0;
    }

    // Loads shader given shader path, type, and language (default language is SPIR-V)
    int Shader::LoadShader(const std::string& path, ShaderType type, ShaderLanguage language) {

        std::vector<char> null = {};

        // If the shader type is SPIR-V (SPV)
        if (language == ShaderLanguage::THE_FORGE_VK_SHADER_LANGUAGE_SPV) {

            std::ifstream shader(path, std::ios::ate | std::ios::binary);       // Open shader file in binary mode, and start reading from EOF

            if (!shader.is_open()) {                                                                    // If opening shader file fails
                std::string temp = "Fatal Error: Failed to load shader from [" + path + "].";           //
                ASWL::utilities::Logger("S0002", temp);                                                 // log the error
                return 3;                                                                               // and end loading process
            }

            size_t fileSize = (size_t)shader.tellg();       // Get input position associated with streambuf object
            std::vector<char> buffer(fileSize);             // Initialize buffer vector with the file size

            shader.seekg(0);                            // Set input position to start of file
            shader.read(buffer.data(), fileSize);       // Read the data and push into vector list

            shader.close();     // Close the file

            if (type == ShaderType::THE_FORGE_VK_SHADER_TYPE_VERTEX)                // If the shader is a vertex shader
                vert = buffer;                                                      // Set vert to buffer
            else if (type == ShaderType::THE_FORGE_VK_SHADER_TYPE_FRAGMENT)         // If the sahder is a fragment shader
                frag = buffer;                                                      // Set frag to buffer
            
            return 0;
        }
        else {
            ASWL::utilities::Logger("S0003", "Fatal Error: Only SPIR-V is currently supported.");       // log the error
            return 4;                                                                                   // and end loading process
        }
    }

    void Shader::cleanup() {
        
    }
}
