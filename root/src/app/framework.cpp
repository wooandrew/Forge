// TheForge - src/app/framework (c) Andrew Woo, 2020

#pragma warning(disable : 26812)


#include "app/framework.hpp"
#include "forge_vars.hpp"

namespace Forge::App {

    // Default constructor
    Framework::Framework() {

        swapchain = VK_NULL_HANDLE;
        RenderPass = VK_NULL_HANDLE;
        PipelineLayout = VK_NULL_HANDLE;
        pipeline = VK_NULL_HANDLE;

        ImageFormat = VK_FORMAT_UNDEFINED;
        extent = { 1000, 600 };
    };

    // Default destructor
    Framework::~Framework() {

    };

    // Initialize App Rendering framework
    int Framework::init(GLFWwindow* _window, std::shared_ptr<Core::EngineCore> _core) {

        core = _core;

        int resultSwapchain = 0;        // Swapchain initialization result
        int resultRenderPass = 0;       // RenderPass initialization result
        int resultFramebuffer = 0;      // Framebuffer initialization result
        int resultPipeline = 0;         // Pipeline initialization result

        resultSwapchain = initSwapchain(_window);       // Initialize the swapchain object
        resultRenderPass = initRenderPass();            // Initialize the RenderPass object
        resultFramebuffer = initFramebuffers();         // Initialize framebuffers

        //std::string msg = 
        //ASWL::utilities::Logger("F0000", msg);

        return resultSwapchain + resultRenderPass + resultFramebuffer + resultPipeline;     // Return the sum of results
    }

    int Framework::initSwapchain(GLFWwindow* _window) {

        SwapChainSupportDetails swapchainSupport = QuerySwapChainSupport(core->GetPGPU(), core->GetSurface());      // Get swapchain properties

        VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapchainSupport.formats);       // Get surface format
        VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapchainSupport.presentModes);        // Get surface present mode
        VkExtent2D _extent = ChooseSwapExtent(_window, swapchainSupport.capabilities);              // Get surface extent
        extent = _extent;

        // Request one more image than minimum to avoid bottleneck
        uint32_t imageCount = swapchainSupport.capabilities.minImageCount + 1;

        // If there is more than one image queued, or more images than max is queued
        if (swapchainSupport.capabilities.maxImageCount > 0 && imageCount > swapchainSupport.capabilities.maxImageCount)
            imageCount = swapchainSupport.capabilities.maxImageCount;       // Request maximum amount of images

        VkSwapchainCreateInfoKHR createInfo = {};                               // createInfo specifies the parameters of the swapchain object
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;         // Identify createInfo as structure type SWAPCHAIN_CREATE_INFO_KHR
        createInfo.surface = core->GetSurface();                                // Set swapchain surface
        createInfo.minImageCount = imageCount;                                  // Set the minimum image count
        createInfo.imageFormat = surfaceFormat.format;                          // Set image format to same format as surface
        createInfo.imageColorSpace = surfaceFormat.colorSpace;                  // Set image color space
        createInfo.imageExtent = _extent;                                       // Set swapchain extent
        createInfo.imageArrayLayers = 1;                                        // Number of views in a multiview surface
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;            // Swapchain intended use

        QueueFamilyIndices indices = FindQueueFamilies(core->GetPGPU(), core->GetSurface());                    // Get queue family indices
        uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };      // List of queue family indices

        if (indices.graphicsFamily != indices.presentFamily) {              // If queue family indices are not equal
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;       // Queue families can access image objects concurrently
            createInfo.queueFamilyIndexCount = 2;                           // Number of queue families that will access image objects
            createInfo.pQueueFamilyIndices = queueFamilyIndices;            // Pointer to queueFamilyIndices
        }
        else {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;        // Queue families can only access image objects exclusively
            createInfo.queueFamilyIndexCount = 0;                           // Number of queue families that will access image objects
            createInfo.pQueueFamilyIndices = nullptr;                       // Pointer to queueFamilyIndices
        }

        createInfo.preTransform = swapchainSupport.capabilities.currentTransform;       // Transform image to match presentation orientation
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;                  // Set alpha compositing mode
        createInfo.presentMode = presentMode;                                           // Set swapchain presentation mode
        createInfo.clipped = VK_TRUE;                                                   // Set whether or not to clip hidden pixels
        createInfo.oldSwapchain = VK_NULL_HANDLE;                                       // Invalidated/unoptimized swapchains

        if (vkCreateSwapchainKHR(core->GetLGPU(), &createInfo, nullptr, &swapchain) != VK_SUCCESS) {        // If vkSwapchain creation fails
            ASWL::utilities::Logger("F01S0", "Fatal Error: Failed to create swapchain.");                   // then log the error
            return 1;                                                                                       // and return the corresponding error value
        }

        vkGetSwapchainImagesKHR(core->GetLGPU(), swapchain, &imageCount, nullptr);              // Get number of images in swapchain
        images.resize(imageCount);                                                              // Resize swapchain images list to fit
        vkGetSwapchainImagesKHR(core->GetLGPU(), swapchain, &imageCount, images.data());        // Fill swapchain images list

        ImageFormat = surfaceFormat.format;         // Set image format to equal surface format
        extent = _extent;                           // Set swapchain extent

        ImageViews.resize(images.size());           // Set image views list size to equal the number of images in swapchain

        // Iterate through every image in swapchain images
        for (int i = 0; i < images.size(); i++) {

            // Create an image view object for each image
            VkImageViewCreateInfo createImageViewInfo = {};                                     // createImageViewInfo specifies the parameters of the image view object
            createImageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;               // Identify createImageViewInfo as structure type IMAGE_VIEW_CREATE_INFO
            createImageViewInfo.image = images[i];                                              // Set the VkImage on which the image will be created
            createImageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;                               // Set the image view type
            createImageViewInfo.format = ImageFormat;                                           // Set the image view format

            // Specify how to swizzle/map color
            createImageViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;                   // Specify how component r should be swizzled (red)
            createImageViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;                   // Specify how component g should be swizzled (green)
            createImageViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;                   // Specify how component b should be swizzled (blue)
            createImageViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;                   // Specify how component a should be swizzled (alpha)

            // Specify the images purpose
            createImageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;        // Determine which aspect(s) of the image are included in the view
            createImageViewInfo.subresourceRange.baseMipLevel = 0;                              // First mipmap level accessible to the view
            createImageViewInfo.subresourceRange.levelCount = 1;                                // Number of mipmap levels accessible to the view
            createImageViewInfo.subresourceRange.baseArrayLayer = 0;                            // First array level accessible to the view
            createImageViewInfo.subresourceRange.layerCount = 1;                                // Number of array levels accessible to the view

            if (vkCreateImageView(core->GetLGPU(), &createImageViewInfo, nullptr, &ImageViews[i]) != VK_SUCCESS) {          // If image view creation fails
                std::string msg = "Fatal Error: Failed to create image view at indice [" + std::to_string(i) + "]";         //
                ASWL::utilities::Logger("F02S1", msg);                                                                      // then log the error
                return 2;                                                                                                   // and return the corresponding error value
            }
        }

        return 0;
    }

    int Framework::initRenderPass() {

        VkAttachmentDescription colorAttachment = {};                           // Structure specifies the attachment description
        colorAttachment.format = ImageFormat;                                   // Set colorAttachment format
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

        VkRenderPassCreateInfo renderPassInfo = {};                             // renderPassInfo specifies the parameters of the RenderPass object
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;       // Identify renderPassInfo as structure type RENDER_PASS_CREATE_INFO
        renderPassInfo.attachmentCount = 1;                                     // Number of attachments used by RenderPass object
        renderPassInfo.pAttachments = &colorAttachment;                         // Pointer to structures describing RenderPass attachments
        renderPassInfo.subpassCount = 1;                                        // Number of subpasses to create
        renderPassInfo.pSubpasses = &subpass;                                   // Pointer to array of structures describing each subpass
        renderPassInfo.dependencyCount = 1;                                     // Number of subpass dependencies
        renderPassInfo.pDependencies = &subpassDependency;                      // Pointer to subpass dependency structure

        if (vkCreateRenderPass(core->GetLGPU(), &renderPassInfo, nullptr, &RenderPass) != VK_SUCCESS) {         // If RenderPass creation fails
            ASWL::utilities::Logger("F03R0", "Fatal Error: Render Pass creation failed.");                      // then log the error
            return 1;                                                                                           // and return the corresponding error value
        }

        return 0;
    }

    int Framework::initFramebuffers() {

        framebuffers.resize(ImageViews.size());                 // Resize framebuffers list
        for (size_t i = 0; i < ImageViews.size(); i++) {        // Iterate through every VkImageView objects

            VkImageView attachments[] = { ImageViews[i] };      // Create an array of VkImageView attachments

            VkFramebufferCreateInfo framebufferInfo = {};                               // framebufferInfo specifies the parameters of the framebuffer object
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;          // Identify framebufferInfo as structure type FRAMEBUFFER_CREATE_INFO
            framebufferInfo.renderPass = RenderPass;                                    // Set render pass
            framebufferInfo.attachmentCount = 1;                                        // Number of attachments per framebuffer
            framebufferInfo.pAttachments = attachments;                                 // Pointer to array attachments
            framebufferInfo.width = extent.width;                                       // Set framebuffer width
            framebufferInfo.height = extent.height;                                     // Set framebuffer height
            framebufferInfo.layers = 1;                                                 // Set framebuffer layer count

            if (vkCreateFramebuffer(core->GetLGPU(), &framebufferInfo, nullptr, &framebuffers[i]) != VK_SUCCESS) {          // If framebuffer creation fails at index
                std::string msg = "Fatal Error: Failed to create framebuffer at index [" + std::to_string(i) + "].";        //
                ASWL::utilities::Logger("F04FB", msg);                                                                      // then log the error
                return 1;                                                                                                   // and return the corresponding error
            }
        }

        return 0;
    }

    int Framework::initPipeline() {

        // Load Vertex Shader
        int vertShader = LoadShader(shaderMetadata.vertShaderPath, ShaderType::THE_FORGE_VK_SHADER_TYPE_VERTEX, ShaderLanguage::THE_FORGE_VK_SHADER_LANGUAGE_SPV);
        if (vertShader != 0)
            return vertShader;

        VkShaderModuleCreateInfo vertCreateInfo = {};                               // vertCreateInfo specifies the parameters of the vertex shader module
        vertCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;         // Identify vertCreateInfo as structure type SHADER_MODULE_CREATE_INFO
        vertCreateInfo.codeSize = vert.size();                                      // Size of the code in bytes
        vertCreateInfo.pCode = reinterpret_cast<const uint32_t*>(vert.data());      // Pointer to shader code

        VkShaderModule vertShaderModule = VK_NULL_HANDLE;                                                   // Vertex Shader Module
        if (vkCreateShaderModule(core->GetLGPU(), &vertCreateInfo, nullptr, &vertShaderModule)) {           // If vertex shader module creation fails
            ASWL::utilities::Logger("F04P0", "Fatal Error: Failed to create vertex shader module.");        // then log the error
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
        fragCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;         // Identify fragCreateInfo as structure type SHADER_MODULE_CREATE_INFO
        fragCreateInfo.codeSize = frag.size();                                      // Size of the code in bytes
        fragCreateInfo.pCode = reinterpret_cast<const uint32_t*>(frag.data());      // Pointer to shader code

        VkShaderModule fragShaderModule = VK_NULL_HANDLE;                                                   // Fragment Shader Module
        if (vkCreateShaderModule(core->GetLGPU(), &fragCreateInfo, nullptr, &fragShaderModule)) {           // If fragment shader module creation fails
            ASWL::utilities::Logger("F05P1", "Fatal Error: Failed to create fragment shader module.");      // then log the error
            return 2;                                                                                       // and return the corresponding error value
        }

        // Create Fragment Shader Pipeline
        VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};                               // fragShaderStageInfo specifies the parameters of the fragment shader pipeline
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;        // Identify fragShaderStageInfo as structure type PIPELINE_SHADER_STAGE_CREATE_INFO
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;                               // Specify the pipeline stage
        fragShaderStageInfo.module = fragShaderModule;                                          // Handle to shader module object
        fragShaderStageInfo.pName = "main";                                                     // Specify entry point name

        VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };      // Pipeline shader stage

        auto bindingDescription = Vertex::GetBindingDescription();
        auto attributeDescription = Vertex::GetAttributeDesciptions();

        VkPipelineVertexInputStateCreateInfo vertInputInfo = {};                                                    // vertInputInfo specifies the paramerters of the vertex pipeline input stage
        vertInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;                            // Identify vertInputInfo as structure type PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO
        vertInputInfo.vertexBindingDescriptionCount = 1;                                                            // Number of vertex binding descriptions
        vertInputInfo.pVertexBindingDescriptions = &bindingDescription;                                             // Pointer to structure specifying vertex input binding description
        vertInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescription.size());         // Number of vertex attribute descriptions
        vertInputInfo.pVertexAttributeDescriptions = attributeDescription.data();                                   // Pointer to structure specifying vertex attribute descriptions


        VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};                                  // inputAssembly specifies the parameters of the pipeline input assembly
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;          // Identify inputAssembly as structure type PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;                               // Define the type of primitive geometries that will be drawn from the vertices
        inputAssembly.primitiveRestartEnable = VK_FALSE;                                            // Determine whether a special vertex index value should restart the assembly of primitives.

        VkViewport viewport = {};                                   // Structure containing viewport parameters
        viewport.x = 0.0f;                                          // Set viewport upper left corner [LEFT]
        viewport.y = 0.0f;                                          // Set viewport upper left corner [ UP ]
        viewport.width = static_cast<float>(extent.width);          // Set viewport width
        viewport.height = static_cast<float>(extent.height);        // Set viewport height
        viewport.minDepth = 0.0f;                                   // Set viewport min depth
        viewport.maxDepth = 1.0f;                                   // Set viewport max depth

        VkRect2D scissor = {};          // Create scissor rect to store pixel data
        scissor.offset = { 0,0 };       // Set top left corner of scissor rect
        scissor.extent = extent;        // Set size of scissor rect

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

        if (vkCreatePipelineLayout(core->GetLGPU(), &pipelineLayoutInfo, nullptr, &PipelineLayout) != VK_SUCCESS) {         // If pipeline layout creation fails
            ASWL::utilities::Logger("F06P2", "Fatal Error: Failed to create pipeline layout.");                             // then log the error
            return 3;                                                                                                       // and return the corresponding error value
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
        pipelineInfo.layout = PipelineLayout;                                       // Set pipeline layout
        pipelineInfo.renderPass = RenderPass;                                       // Set pipeline render pass
        pipelineInfo.subpass = 0;                                                   // Render pass subpass index
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;                           // Optional
        pipelineInfo.basePipelineIndex = -1;                                        // Optional

        if (vkCreateGraphicsPipelines(core->GetLGPU(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS) {       // If graphics pipeline creation fails
            ASWL::utilities::Logger("F07P3", "Fatal Error: Failed to create graphics pipeline.");                                   // then log the error
            return 4;                                                                                                               // and return the corresponding error
        }

        vkDestroyShaderModule(core->GetLGPU(), vertShaderModule, nullptr);
        vkDestroyShaderModule(core->GetLGPU(), fragShaderModule, nullptr);

        return 0;
    }


    // Helper functions
    int Framework::LoadShader(const std::string& path, ShaderType type, ShaderLanguage language) {

        // If the shader type is SPIR-V (SPV)
        if (language == ShaderLanguage::THE_FORGE_VK_SHADER_LANGUAGE_SPV) {

            std::ifstream shader(path, std::ios::ate | std::ios::binary);       // Open shader file in binary mode, and start reading from EOF

            if (!shader.is_open()) {                                                                // If opening shader file fails
                std::string msg = "Fatal Error: Failed to load shader from [" + path + "].";        //
                ASWL::utilities::Logger("F08P4", msg);                                              // then log the error
                return 5;                                                                           // and return the corresponding error
            }

            size_t fileSize = (size_t)shader.tellg();       // Get input position associated with streambuf object
            std::vector<char> buffer(fileSize);             // Initialize buffer vector with the file size

            shader.seekg(0);                            // Set input position to start of file
            shader.read(buffer.data(), fileSize);       // Read the data and push into vector list

            shader.close();     // Close the file

            if (type == ShaderType::THE_FORGE_VK_SHADER_TYPE_VERTEX)                // If the shader is a vertex shader
                vert = buffer;                                                      // Set vert to buffer
            else if (type == ShaderType::THE_FORGE_VK_SHADER_TYPE_FRAGMENT)         // If the shader is a fragment shader
                frag = buffer;                                                      // Set frag to buffer

            return 0;
        }
        else {
            ASWL::utilities::Logger("SHDRT", "Fatal Error: Only SPIR-V is currently supported.");
            return -1;
        }
    }
}
