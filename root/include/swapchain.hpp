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

    struct SwapChainSupportDetails {                    // Struct containing Swap Chain support details

        VkSurfaceCapabilitiesKHR capabilities;          // Struct containing details of the capabilities of the swap chains
        std::vector<VkSurfaceFormatKHR> formats;        // List of structures describing a supported swapchain format-color space pair
        std::vector<VkPresentModeKHR> presentModes;     // List of presentation modes supported by the device
    };
    SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice& device, VkSurfaceKHR& surface);             // Populates SwapChainSupportDetails
    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);        // Choose surface format
    VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);         // Choose surface present mode
    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);                                  // Choose swap extent -> surface resolution

    // Swapchain class
    class Swapchain {
        
    public:

        friend class Engine;
        friend class Renderer;

        Swapchain();        // Default constructor
        ~Swapchain();       // Default destructor

        int init(VkPhysicalDevice& graphicscard, VkSurfaceKHR& surface, VkDevice& device);          // Initialize swapchain
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
