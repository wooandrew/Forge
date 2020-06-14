// TheForge - src/pipeline (c) Andrew Woo, 2020

#pragma warning(disable : 26812)

#include "pipeline.hpp"

// Standard Library
#include <fstream>

// Dependencies
#include <ASWL/utilities.hpp>

namespace Forge {

    // Default constructor
    Pipeline::Pipeline() {
        device = VK_NULL_HANDLE;
        pipelineLayout = VK_NULL_HANDLE;
        renderPass = VK_NULL_HANDLE;
        graphicsPipeline = VK_NULL_HANDLE;
    }

    // Default destructor
    Pipeline::~Pipeline() {
    }

    int Pipeline::init(VkDevice& device, Swapchain& swapchain) {

        this->device = device;

        VkAttachmentDescription colorAttachment = {};                           // Structure specifies the attachment description
        colorAttachment.format = swapchain.GetImageFormat();                    // Set colorAttachment format
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;                        // Number of samples of the image
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;                   // Set image color load operation behavior
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;                 // Set image color store operation behavior
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;        // Set image stencil load operation behavior
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;      // Set image stencil store operation behavior
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;              // Specify image layout before render pass begins
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;          // Specify image layout to transition to after render pass ends

        VkAttachmentReference colorAttachmentRef = {};                              // Structure specifes the attachment reference
        colorAttachmentRef.attachment = 0;                                          // Set attachment reference index
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;       // Set attachment layout

        VkSubpassDescription subpass = {};                                  // Structure specifies subpass description
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;        // Set pipeline type supported by pipeline
        subpass.colorAttachmentCount = 1;                                   // Number of color attachments
        subpass.pColorAttachments = &colorAttachmentRef;                    // Pointer to VkAttachmentRef defining subpasss color attachments

        VkSubpassDependency subpassDependency = {};                                             // Structure specifies subpass dependency
        subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;                                     // Subpass index of first subpass in the dependency
        subpassDependency.dstSubpass = 0;                                                       // Subpass index of the destination subpass in the dependency
        subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;         // Bitmask of source stage mask
        subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;         // Bitmask of destination stage mask
        subpassDependency.srcAccessMask = 0;                                                    // Bitmask of source access mask
        subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;                 // Bitmask of destination access mask

        VkRenderPassCreateInfo renderpassInfo = {};                             // renderpassInfo specifies the parameters of the renderpass object
        renderpassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;       // Identify renderpassInfo as structure type RENDER_PASS_CREATE_INFO
        renderpassInfo.attachmentCount = 1;                                     // Number of attachments used by renderpass object
        renderpassInfo.pAttachments = &colorAttachment;                         // Pointer to structures describing renderpass attachments
        renderpassInfo.subpassCount = 1;                                        // Number of subpasses to create
        renderpassInfo.pSubpasses = &subpass;                                   // Pointer to array of structures describing each subpass
        renderpassInfo.dependencyCount = 1;                                     // Number of subpass dependencies
        renderpassInfo.pDependencies = &subpassDependency;                      // Pointer to subpass dependency structure

        if (vkCreateRenderPass(device, &renderpassInfo, nullptr, &renderPass) != VK_SUCCESS) {          // If renderpass creation fails
            ASWL::utilities::Logger("P00R0", "Fatal Error: Render Pass creation failed.");              // then log the error
            return 1;                                                                                   // and return the corresponding error value
        }

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
            ASWL::utilities::Logger("P01S0", "Fatal Error: Failed to create vertex shader module.");        // then log the error
            return 2;                                                                                       // and return the corresponding error value
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
        fragCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;         // Identify fragCreateInfo as structure type SHADER_MODULE_CREATE_INFO
        fragCreateInfo.codeSize = frag.size();                                      // Size of the code in bytes
        fragCreateInfo.pCode = reinterpret_cast<const uint32_t*>(frag.data());      // Pointer to shader code

        VkShaderModule fragShaderModule = VK_NULL_HANDLE;                                                   // Fragment Shader Module
        if (vkCreateShaderModule(device, &fragCreateInfo, nullptr, &fragShaderModule)) {                    // If fragment shader module creation fails
            ASWL::utilities::Logger("P02S1", "Fatal Error: Failed to create fragment shader module.");      // then log the error
            return 3;                                                                                       // and return the corresponding error value
        }

        // Create Fragment Shader Pipeline
        VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};                               // fragShaderStageInfo specifies the parameters of the fragment shader pipeline
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;        // Identify fragShaderStageInfo as structure type PIPELINE_SHADER_STAGE_CREATE_INFO
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;                               // Specify the pipeline stage
        fragShaderStageInfo.module = fragShaderModule;                                          // Handle to shader module object
        fragShaderStageInfo.pName = "main";                                                     // Specify entry point name

        VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };      // Pipeline shader stage

        VkPipelineVertexInputStateCreateInfo vertInputInfo = {};                                // vertInputInfo specifies the paramerters of the vertex pipeline input stage
        vertInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;        // Identify vertInputInfo as structure type PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO
        vertInputInfo.vertexBindingDescriptionCount = 0;                                        // Number of vertex binding descriptions
        vertInputInfo.pVertexBindingDescriptions = nullptr;                                     // Pointer to structure specifying vertex input binding description
        vertInputInfo.vertexAttributeDescriptionCount = 0;                                      // Number of vertex attribute descriptions
        vertInputInfo.pVertexAttributeDescriptions = nullptr;                                   // Pointer to structure specifying vertex attribute descriptions

        VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};                                  // inputAssembly specifies the parameters of the pipeline input assembly
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;          // Identify inputAssembly as structure type PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;                               // Define the type of primitive geometries that will be drawn from the vertices
        inputAssembly.primitiveRestartEnable = VK_FALSE;                                            // Determine whether a special vertex index value should restart the assembly of primitives.

        VkViewport viewport = {};                                                   // Structure containing viewport parameters
        viewport.x = 0.0f;                                                          // Set viewport upper left corner [LEFT]
        viewport.y = 0.0f;                                                          // Set viewport upper left corner [ UP ]
        viewport.width = static_cast<float>(swapchain.GetExtent().width);           // Set viewport width
        viewport.height = static_cast<float>(swapchain.GetExtent().height);         // Set viewport height
        viewport.minDepth = 0.0f;                                                   // Set viewport min depth
        viewport.maxDepth = 1.0f;                                                   // Set viewport max depth

        VkRect2D scissor = {};                      // Create scissor rect to store pixel data
        scissor.offset = { 0,0 };                   // Set top left corner of scissor rect
        scissor.extent = swapchain.GetExtent();     // Set size of scissor rect

        VkPipelineViewportStateCreateInfo viewportState = {};                               // viewportState specifies the parameters of the pipline viewport
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;        // Identify viewportState as structure type PIPELINE_VIEWPORT_STATE_CREATE_INFO
        viewportState.viewportCount = 1;                                                    // Number of viewports used by the pipeline
        viewportState.pViewports = &viewport;                                               // Pointer to viewport structure
        viewportState.scissorCount = 1;                                                     // Number of scissors used by the pipeline
        viewportState.pScissors = &scissor;                                                 // Pointer to scissor rect object

        // Rasterizer takes the geometry created by vertices and converts them into fragments for the shader
        VkPipelineRasterizationStateCreateInfo rasterizer = {};                                 // rasterizer specifies the parameters of the rasterizer object
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;          // Identify rasterizer as structure type PIPELINE_RASTERIZATION_STATE_CREATE_INFO
        rasterizer.depthClampEnable = VK_FALSE;                                                 // Set whether or not rasterizer should clamp the depth
        rasterizer.rasterizerDiscardEnable = VK_FALSE;                                          // Set whether or not rasterizer should discard primitives before rasterization
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;                                          // Set triangle render mode
        rasterizer.lineWidth = 1.0f;                                                            // Set width of rendered line segments
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;                                            // Set triangle culling mode
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;                                         // Set triangle orientation to be used for culling
        rasterizer.depthBiasEnable = VK_FALSE;                                                  // Set whether or not rasterizer should bias fragment depth values
        rasterizer.depthBiasConstantFactor = 0.0f;                                              // Set a constant depth value to be added to each fragment
        rasterizer.depthBiasClamp = 0.0f;                                                       // Maximum/minimum depth bias of a fragment
        rasterizer.depthBiasSlopeFactor = 0.0f;                                                 // Set scalar value to added to fragment slope depth bias calculations

        VkPipelineMultisampleStateCreateInfo multisampling = {};                                // multisampling specifies the parameters of pipeline multisampling object
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;         // Identify multisampling as structure type PIPELINE_MULTISAMPLE_STATE_CREATE_INFO
        multisampling.sampleShadingEnable = VK_FALSE;                                           // Set whether or not sample shading should be enabled
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;                             // Set number of samples used in rasterization
        multisampling.minSampleShading = 1.0f;                                                  // Specifies minimum fraction of sample shading
        multisampling.pSampleMask = nullptr;                                                    // Array of VkSampleMask values
        multisampling.alphaToCoverageEnable = VK_FALSE;                                         // Set whether or not a temporary coverage value should be generated based on alpha values
        multisampling.alphaToOneEnable = VK_FALSE;                                              // Set whether or not the alpha component of the fragment's first color output is replaced

        VkPipelineColorBlendAttachmentState colorBlendAttachment = {};                          // colorBlendAttachment specifies the pipeline color blend attachment state
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |                        // Set which color component is enabled for writing (R) RED
                                              VK_COLOR_COMPONENT_G_BIT |                        // Set which color component is enabled for writing (G) GREEN
                                              VK_COLOR_COMPONENT_B_BIT |                        // Set which color component is enabled for writing (B) BLUE
                                              VK_COLOR_COMPONENT_A_BIT;                         // Set which color component is enabled for writing (A) ALPHA
        colorBlendAttachment.blendEnable = VK_TRUE;                                             // Set whether or not color blending is enabled
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;                   // Set color blending source factor
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;         // Set color blending destination factor
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;                                    // Set color blending method
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;                         // Set alpha blending source factor
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;                        // Set alpha blending destination factor
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;                                    // Set alpha blending method

        VkPipelineColorBlendStateCreateInfo colorBlending = {};                                 // colorBlending specifies the pipeline color blend state
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;         // Identify colorBlending as structure type PIPELINE_COLOR_BLEND_STATE_CREATE_INFO
        colorBlending.logicOpEnable = VK_FALSE;                                                 // Set whether or not logical operation should be applied
        colorBlending.logicOp = VK_LOGIC_OP_COPY;                                               // Set which logical operation should be applied if enabled
        colorBlending.attachmentCount = 1;                                                      // Number of attachment states
        colorBlending.pAttachments = &colorBlendAttachment;                                     // Pointer to array of attachment states
        colorBlending.blendConstants[0] = 0.0f;                                                 // Optional
        colorBlending.blendConstants[1] = 0.0f;                                                 // Optional
        colorBlending.blendConstants[2] = 0.0f;                                                 // Optional
        colorBlending.blendConstants[3] = 0.0f;                                                 // Optional

        /*
        VkDynamicState dynamicStates[] = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_LINE_WIDTH
        };
        */

        VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};                             // pipelineLayoutInfo specifies the parameters of the pipeline layout object
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;       // Identify pipelineLayoutInfo as structure type PIPELINE_LAYOUT_CREATE_INFO

        if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {      // If pipeline layout creation fails
            ASWL::utilities::Logger("P0003", "Fatal Error: Failed to create pipeline layout.");                 // then log the error
            return 4;                                                                                           // and return the corresponding error value
        }

        VkGraphicsPipelineCreateInfo pipelineInfo = {};                             // pipelineInfo specifies the parameters of the pipeline object
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;       // Identify pipelineInfo as structure type GRAPHICS_PIPELINE_CREATE_INFO
        pipelineInfo.stageCount = 2;                                                // Set pipeline stage count
        pipelineInfo.pStages = shaderStages;                                        // Array of pipeline shader stages
        pipelineInfo.pVertexInputState = &vertInputInfo;                            // Pointer to pipeline vertex input info
        pipelineInfo.pInputAssemblyState = &inputAssembly;                          // Pointer to pipeline input assembly parameters
        pipelineInfo.pViewportState = &viewportState;                               // Pointer to pipeline viewport parameters
        pipelineInfo.pRasterizationState = &rasterizer;                             // Pointer to rasterizer
        pipelineInfo.pMultisampleState = &multisampling;                            // Pointer to pipeline multisampler
        pipelineInfo.pDepthStencilState = nullptr;                                  // Pointer to depth stencil state structure
        pipelineInfo.pColorBlendState = &colorBlending;                             // Pointer to pipeline color blend state
        pipelineInfo.pDynamicState = nullptr;                                       // Pointer to dynamic state structure parameters
        pipelineInfo.layout = pipelineLayout;                                       // Set pipeline layout
        pipelineInfo.renderPass = renderPass;                                       // Set pipeline render pass
        pipelineInfo.subpass = 0;                                                   // Render pass subpass index
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;                           // Optional
        pipelineInfo.basePipelineIndex = -1;                                        // Optional

        if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {        // If graphics pipeline creation fails
            ASWL::utilities::Logger("P0004", "Fatal Error: Failed to create graphics pipeline.");                                   // then log the error
            return 5;                                                                                                               // and return the corresponding error
        }

        vkDestroyShaderModule(device, vertShaderModule, nullptr);
        vkDestroyShaderModule(device, fragShaderModule, nullptr);

        return 0;
    }

    // Loads shader given shader path, type, and language (default language is SPIR-V)
    int Pipeline::LoadShader(const std::string& path, ShaderType type, ShaderLanguage language) {

        // If the shader type is SPIR-V (SPV)
        if (language == ShaderLanguage::THE_FORGE_VK_SHADER_LANGUAGE_SPV) {

            std::ifstream shader(path, std::ios::ate | std::ios::binary);       // Open shader file in binary mode, and start reading from EOF

            if (!shader.is_open()) {                                                                // If opening shader file fails
                std::string msg = "Fatal Error: Failed to load shader from [" + path + "].";        //
                ASWL::utilities::Logger("P05S2", msg);                                              // then log the error
                return 6;                                                                           // and return the corresponding error
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
            ASWL::utilities::Logger("XXP6S", "Fatal Error: Only SPIR-V is currently supported.");
            return -1;
        }
    }

    // Returns renderpass object on request
    VkRenderPass& Pipeline::GetRenderPass() {
        return renderPass;
    }
    VkPipeline& Pipeline::GetGraphicsPipeline() {
        return graphicsPipeline;
    }

    void Pipeline::cleanup() {
        
        vkDestroyPipeline(device, graphicsPipeline, nullptr);
        vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
        vkDestroyRenderPass(device, renderPass, nullptr);
    }
}
