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

        Swapchain();        // Default constructor
        ~Swapchain();       // Default destructor

        int init(VkPhysicalDevice& graphicscard, VkSurfaceKHR& surface, VkDevice& logicaldevice);       // Initialize swapchain
        void cleanup();

        VkExtent2D GetExtent();         // Returns swapchain extent
        VkFormat GetImageFormat();      // Returns swapchain image format
        const std::vector<VkImageView> GetImageViews();     // Returns swapchaim image views

    private:

        VkSwapchainKHR swapchain;                   // Handle to swapchain object
        std::vector<VkImage> swapchainImages;       // List of handles to image objects
        VkFormat swapchainImageFormat;              // Swapchain image format
        VkExtent2D swapchainExtent;                 // 2D array containing swapchain extent

        std::vector<VkImageView> swapchainImageViews;       // List of handles to image view object

        VkDevice logicaldevice;         // Handle to Logical device
    };
}

#endif // !THEFORGE_SWAPCHAIN
