// TheForge - src/swapchain (c) Andrew Woo, 2020

#pragma warning(disable : 26812)

#include "swapchain.hpp"
#include "forge_vars.hpp"

// Standard Library
#include <cstdint>
#include <algorithm>

namespace Forge {

	// Populates SwapChainSupportDetails
	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice& device, VkSurfaceKHR& surface) {

		SwapChainSupportDetails details;		// Struct containing Swapchain support details

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);		// Fills VkSurfaceCapabilitiesKHR structure

		uint32_t formatCount = 0;															// Number of formats the device surface supports
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);		// Get number of formats the device surface supports

		if (formatCount != 0) {																					// If the number of formats supported by surface is not zero
			details.formats.resize(formatCount);																// Resizes list of VkSurfaceFormatKHRs
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());		// Then push supported formats into list
		}

		uint32_t presentModeCount = 0;																// Number of present modes supported by physical device
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);		// Get number of present modes supported by physical device

		if (presentModeCount != 0) {																						// If number of present modes supported by physical device is not zero
			details.presentModes.resize(presentModeCount);																	// Resizes list of VkPresentModeKHRs
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());		// Then push supported presetn modes into list
		}

		return details;
	}

	// Choose surface format
	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {

		// Iterate through every available surface formats
		for (const auto& availableFormat : availableFormats) {

			// If the format is B8G8R8A8_SRGB and colorSpace is SRGB_NONLINEAR_KHR
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				return availableFormat;		// Return availableFormat
		}

		return availableFormats[0];		// Return the first available format
	}

	// Choose surface present mode
	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {

		// Iterate through every available surface present modes
		for (const auto& availablePresentMode : availablePresentModes) {

			// If MAILBOX_KHR is an available present mode
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
				return availablePresentMode;	// Return availablePresentMode
		}

		return VK_PRESENT_MODE_FIFO_KHR;	// Return FIFO_KHR
	}

	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {

		if (capabilities.currentExtent.width != UINT32_MAX)		// If surface extent does not equal the max UINT32 size (4,294,967,294)
			return capabilities.currentExtent;					// then return the current surface extent

		else {		// Otherwise set the surface extent

			VkExtent2D actualExtent = { 1000, 600 };
			actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);			// Clamp width
			actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);		// Clamp height

			return actualExtent;
		}
	}

	// Default constructor
	Swapchain::Swapchain() {

		swapchain = VK_NULL_HANDLE;
		swapchainImageFormat = VK_FORMAT_UNDEFINED;
		swapchainExtent = { 1000, 600 };
	}

	// Default destructor
	Swapchain::~Swapchain() {

	}

	// Initialize swapchain
	int Swapchain::init(VkPhysicalDevice& device, VkSurfaceKHR& surface) {

		SwapChainSupportDetails swapchainSupport = QuerySwapChainSupport(device, surface);			// Get swapchain properties

		VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapchainSupport.formats);		// Get surface format
		VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapchainSupport.presentModes);		// Get surface present mode
		VkExtent2D extent = ChooseSwapExtent(swapchainSupport.capabilities);						// Get surface extent

		// Request one more image than minimum to avoid bottleneck
		uint32_t imageCount = swapchainSupport.capabilities.minImageCount + 1;

		// If there is more than one image queued, or more images than max is queued
		if (swapchainSupport.capabilities.maxImageCount > 0 && imageCount > swapchainSupport.capabilities.maxImageCount)
			imageCount = swapchainSupport.capabilities.maxImageCount;	// Request maximum amount of images

		VkSwapchainCreateInfoKHR createInfo = {};							// createInfo specifies the parameters of the swapchain object
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;		// Identify createInfo as structure type SWAPCHAIN_CREATE_INFO_KHR
		createInfo.surface = surface;										// Set swapchain surface
		createInfo.minImageCount = imageCount;								// Set the minimum image count
		createInfo.imageFormat = surfaceFormat.format;						// Set image format to same format as surface
		createInfo.imageColorSpace = surfaceFormat.colorSpace;				// Set image color space
		createInfo.imageExtent = extent;									// Set swapchain extent
		createInfo.imageArrayLayers = 1;									// Number of views in a multiview surface
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;		// Swapchain intended use

		QueueFamilyIndices indices = FindQueueFamilies(device);													// Get queue family indices
		uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };		// List of queue family indices

		if (indices.graphicsFamily != indices.presentFamily) {				// If queue family indices are not equal
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;		// Queue families can access image objects concurrently
			createInfo.queueFamilyIndexCount = 2;							// Number of queue families that will access image objects
			createInfo.pQueueFamilyIndices = queueFamilyIndices;			// Pointer to queueFamilyIndices
		}
		else {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;		// Queue families can only access image objects exclusively
			createInfo.queueFamilyIndexCount = 0;							// Number of queue families that will access image objects
			createInfo.pQueueFamilyIndices = nullptr;						// Pointer to queueFamilyIndices
		}

		return 0;
	}
}
