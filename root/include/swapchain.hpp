// TheForge - src/swapchain (c) Andrew Woo, 2020

#pragma once
#pragma warning(disable : 26495)

#ifndef THEFORGE_SWAPCHAIN
#define THEFORGE_SWAPCHAIN

// Standard Library
#include <vector>

// Dependencies
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Forge {

    // Swapchain class
    class Swapchain {
        
    public:

        friend class VkContainer;
        friend class Renderer;

        Swapchain();        // Default constructor
        ~Swapchain();       // Default destructor

        int init(GLFWwindow* window, VkSurfaceKHR& surface, VkPhysicalDevice& graphicscard, VkDevice& device);      // Initialize swapchain
        int initFramebuffers(VkRenderPass& renderpass);
        void cleanup();

        VkExtent2D& GetExtent();                                // Returns swapchain extent
        VkFormat& GetImageFormat();                             // Returns swapchain image format
        const std::vector<VkImageView> GetImageViews();         // Returns swapchain image views
        std::vector<VkFramebuffer> GetFramebuffers();           // Returns swapchain framebuffers

    private:

        VkSwapchainKHR swapchain;       // Handle to swapchain object
        VkFormat ImageFormat;           // Swapchain image format
        VkExtent2D extent;              // 2D array containing swapchain extent

        std::vector<VkImage> images;                    // List of handles to image objects
        std::vector<VkImageView> ImageViews;            // List of handles to image view object
        std::vector<VkFramebuffer> framebuffers;        // List of handles to swapchain framebuffers

        VkDevice device;        // Handle to Logical device
    };
}

#endif // !THEFORGE_SWAPCHAIN
