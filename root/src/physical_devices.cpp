// TheForge - src/physical_devices (c) Andrew Woo, 2020

#include "physical_devices.hpp"
#include "swapchain.hpp"

// Standard Library
#include <set>
#include <string>

// Dependencies
#include <ASWL/utilities.hpp>

namespace Forge {

    // Default constructor
    GraphicsCard::GraphicsCard() {
        device = VK_NULL_HANDLE;
    }
    // Default destructor
    GraphicsCard::~GraphicsCard() {
    }

    // Automatically chooses an appropriate graphics card
    int GraphicsCard::autochoose(VkInstance& instance, VkSurfaceKHR& surface) {
        
        int graphicsCardFound = SelectGraphicsCard(instance, surface);
        bool graphicsCardSupported = CheckDeviceSupport(device, surface);

        if (graphicsCardFound == 0 || !graphicsCardSupported)           // If a graphics card is not found or the graphics card is not supported
            return graphicsCardFound & (int)graphicsCardSupported;      // Return the error as a combination of both errors

        return 0;
    }

    // Selects the graphics card to use
    int GraphicsCard::SelectGraphicsCard(VkInstance& instance, VkSurfaceKHR& surface) {

        uint32_t deviceCount = 0;                                           // Number of devices that support Vulkan
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);        // Get number of devices that support Vulkan

        if (deviceCount == 0) {                                                                             // If there are no devices that support Vulkan
            ASWL::utilities::Logger("P00G0", "Fatal Error: Failed to find GPU that supports Vulkan.");      // then log the error
            return 1;                                                                                       // and return the error
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);                         // Initialize list of supported devices
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());         // Then push supported devices into list

        // Iterate through all supported devices
        for (VkPhysicalDevice& _device : devices) {

            if (CheckDeviceSupport(_device, surface)) {     // If the device supports Vulkan operations
                device = _device;                           // Set the selected graphics card to that device
                break;                                      // then break out of the loop
            }
        }

        if (device == VK_NULL_HANDLE) {                                                                                         // If the GPU is null
            ASWL::utilities::Logger("P01G1", "Fatal Error: Failed to find GPU that supports required Vulkan operation.");       // then log the error
            return 2;                                                                                                           // and return the error
        }

        return 0;
    }

    // Checks if the physical device supports necessary Vulkan operations
    bool GraphicsCard::CheckDeviceSupport(VkPhysicalDevice& device, VkSurfaceKHR& surface) {

        //VkPhysicalDeviceProperties deviceProperties;
        //vkGetPhysicalDeviceProperties(device, &deviceProperties);

        //VkPhysicalDeviceFeatures deviceFeatures;
        //vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        QueueFamilyIndices indices = FindQueueFamilies(device, surface);
        bool extensionsSupported = CheckDeviceExtensionSupport(device);

        bool swapChainAdequacy = false;                                                                             // Is swap chain adequate
        if (extensionsSupported) {                                                                                  // If the device supports extensions
            SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device, surface);                      // Populates SwapChainSupportDetails
            swapChainAdequacy = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();        // The swap chain is adequate if the list of formats and present modes is not empty
        }

        return indices.hasValue() && extensionsSupported && swapChainAdequacy;
    }

    // Checks if physical device supports Vulkan extensions
    bool GraphicsCard::CheckDeviceExtensionSupport(VkPhysicalDevice& device) {

        uint32_t extensionCount;                                                                // Number of extensions supported by the device
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);        // Get number of extensions the device supports

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);                                     // List of available extensions
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());         // Then push extensions into list

        std::set<std::string> requiredExtensions(DeviceExtensions.begin(), DeviceExtensions.end());         // Sorted list of unique extensions

        for (const auto& extension : availableExtensions)               // Iterate through all available extensions
            requiredExtensions.erase(extension.extensionName);          // Remove available extension from required extensions list

        return requiredExtensions.empty();
    }

    // Returns the handle to the graphics card on request
    VkPhysicalDevice& GraphicsCard::GetGraphicsCard() {
        return device;
    }
}
