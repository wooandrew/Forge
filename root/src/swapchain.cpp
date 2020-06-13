// TheForge - src/swapchain (c) Andrew Woo, 2020

#pragma warning(disable : 26812)

#include "swapchain.hpp"
#include "forge_vars.hpp"

// Standard Library
#include <cstdint>
#include <algorithm>

// Dependencies
#include <ASWL/utilities.hpp>

namespace Forge {

    // Populates SwapChainSupportDetails
    SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice& device, VkSurfaceKHR& surface) {

        SwapChainSupportDetails details;        // Struct containing Swapchain support details

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);      // Fills VkSurfaceCapabilitiesKHR structure

        uint32_t formatCount = 0;                                                               // Number of formats the device surface supports
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);           // Get number of formats the device surface supports

        if (formatCount != 0) {                                                                                 // If the number of formats supported by surface is not zero
            details.formats.resize(formatCount);                                                                // Resizes list of VkSurfaceFormatKHRs
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());        // Then push supported formats into list
        }

        uint32_t presentModeCount = 0;                                                                  // Number of present modes supported by physical device
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);         // Get number of present modes supported by physical device

        if (presentModeCount != 0) {                                                                                            // If number of present modes supported by physical device is not zero
            details.presentModes.resize(presentModeCount);                                                                      // Resizes list of VkPresentModeKHRs
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());         // Then push supported presetn modes into list
        }

        return details;
    }

    // Choose surface format
    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {

        // Iterate through every available surface formats
        for (const auto& availableFormat : availableFormats) {

            // If the format is B8G8R8A8_SRGB and colorSpace is SRGB_NONLINEAR_KHR
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                return availableFormat;        // Return availableFormat
        }

        return availableFormats[0];        // Return the first available format
    }

    // Choose surface present mode
    VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {

        // Iterate through every available surface present modes
        for (const auto& availablePresentMode : availablePresentModes) {

            // If MAILBOX_KHR is an available present mode
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
                return availablePresentMode;    // Return availablePresentMode
        }

        return VK_PRESENT_MODE_FIFO_KHR;    // Return FIFO_KHR
    }

    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {

        if (capabilities.currentExtent.width != UINT32_MAX)         // If surface extent does not equal the max UINT32 size (4,294,967,294)
            return capabilities.currentExtent;                      // then return the current surface extent

        else {        // Otherwise set the surface extent

            VkExtent2D actualExtent = { 1000, 600 };
            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);              // Clamp width
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);          // Clamp height

            return actualExtent;
        }
    }

    // Default constructor
    Swapchain::Swapchain() {

        swapchain = VK_NULL_HANDLE;
        ImageFormat = VK_FORMAT_UNDEFINED;
        extent = { 1000, 600 };
    }

    // Default destructor
    Swapchain::~Swapchain() {
    }

    // Initialize swapchain
    int Swapchain::init(VkPhysicalDevice& graphicscard, VkSurfaceKHR& surface, VkDevice& _device) {

        this->device = _device;         // Logical device

        SwapChainSupportDetails swapchainSupport = QuerySwapChainSupport(graphicscard, surface);          // Get swapchain properties

        VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapchainSupport.formats);       // Get surface format
        VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapchainSupport.presentModes);        // Get surface present mode
        VkExtent2D _extent = ChooseSwapExtent(swapchainSupport.capabilities);                       // Get surface extent

        // Request one more image than minimum to avoid bottleneck
        uint32_t imageCount = swapchainSupport.capabilities.minImageCount + 1;

        // If there is more than one image queued, or more images than max is queued
        if (swapchainSupport.capabilities.maxImageCount > 0 && imageCount > swapchainSupport.capabilities.maxImageCount)
            imageCount = swapchainSupport.capabilities.maxImageCount;       // Request maximum amount of images

        VkSwapchainCreateInfoKHR createInfo = {};                               // createInfo specifies the parameters of the swapchain object
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;         // Identify createInfo as structure type SWAPCHAIN_CREATE_INFO_KHR
        createInfo.surface = surface;                                           // Set swapchain surface
        createInfo.minImageCount = imageCount;                                  // Set the minimum image count
        createInfo.imageFormat = surfaceFormat.format;                          // Set image format to same format as surface
        createInfo.imageColorSpace = surfaceFormat.colorSpace;                  // Set image color space
        createInfo.imageExtent = _extent;                                       // Set swapchain extent
        createInfo.imageArrayLayers = 1;                                        // Number of views in a multiview surface
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;            // Swapchain intended use

        QueueFamilyIndices indices = FindQueueFamilies(graphicscard, surface);                                  // Get queue family indices
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

        if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapchain) != VK_SUCCESS) {         // If vkSwapchain creation fails
            ASWL::utilities::Logger("S0000", "Fatal Error: Failed to create swapchain.");           // then log the error
            return 1;                                                                               // and return the corresponding error value
        }

        vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr);               // Get number of images in swapchain
        images.resize(imageCount);                                                      // Resize swapchain images list to fit
        vkGetSwapchainImagesKHR(device, swapchain, &imageCount, images.data());         // Fill swapchain images list

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

            if (vkCreateImageView(device, &createImageViewInfo, nullptr, &ImageViews[i]) != VK_SUCCESS) {       // If image view creation fails
                ASWL::utilities::Logger("S0001", "Fatal Error: Failed to create Image View.");                  // then log the error
                return 2;                                                                                       // and return the corresponding error value
            }
        }

        return 0;
    }

    int Swapchain::initFramebuffers(VkRenderPass& renderpass) {

        framebuffers.resize(ImageViews.size());                 // Resize framebuffers list
        for (size_t i = 0; i < ImageViews.size(); i++) {        // Iterate through every VkImageView objects

            VkImageView attachments[] = { ImageViews[i] };      // Create an array of VkImageView attachments

            VkFramebufferCreateInfo framebufferInfo = {};                               // framebufferInfo specifies the parameters of the framebuffer object
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;          // Identify framebufferInfo as structure type FRAMEBUFFER_CREATE_INFO
            framebufferInfo.renderPass = renderpass;                                    // Set render pass
            framebufferInfo.attachmentCount = 1;                                        // Number of attachments per framebuffer
            framebufferInfo.pAttachments = attachments;                                 // Pointer to array attachments
            framebufferInfo.width = extent.width;                                       // Set framebuffer width
            framebufferInfo.height = extent.height;                                     // Set framebuffer height
            framebufferInfo.layers = 1;                                                 // Set framebuffer layer count

            if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &framebuffers[i]) != VK_SUCCESS) {                               // If framebuffer creation fails at index
                std::string msg = "Fatal Error: Failed to create swapchain framebuffer at index [" + std::to_string(i) + "].";          //
                ASWL::utilities::Logger("S02F0", msg);                                                                                  // then log the error
                return 3;                                                                                                               // and return the corresponding error
            }
        }

        return 0;
    }

    void Swapchain::cleanup() {

        vkDestroySwapchainKHR(device, swapchain, nullptr);

        for (auto imageview : ImageViews)
            vkDestroyImageView(device, imageview, nullptr);

        for (auto framebuffer : framebuffers)
            vkDestroyFramebuffer(device, framebuffer, nullptr);
    }

    // Returns swapchain extent on request
    VkExtent2D& Swapchain::GetExtent() {
        return extent;
    }
    // Returns swapchain image format on request
    VkFormat& Swapchain::GetImageFormat() {
        return ImageFormat;
    }
    // Returns swapchaim image views on request
    const std::vector<VkImageView> Swapchain::GetImageViews() {
        return ImageViews;
    }
    // Returns framebuffers on request
    std::vector<VkFramebuffer> Swapchain::GetFramebuffers() {
        return framebuffers;
    }
}
