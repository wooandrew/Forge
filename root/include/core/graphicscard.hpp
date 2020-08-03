// TheForge - src/core/graphicscard (c) Andrew Woo, 2020

#pragma once

#ifndef THEFORGE_CORE_GRAPHICSCARD
#define THEFORGE_CORE_GRAPHICSCARD

// Dependencies
#include <vulkan/vulkan.hpp>
#include <ASWL/utilities.hpp>

namespace Forge::Core {

    namespace {

        // VkPhysicalDevice functions
        bool CheckDeviceSupport(VkPhysicalDevice& _pgpu, VkSurfaceKHR& _surface);       // Checks if physical device supports Vulkan operation
        bool CheckDeviceExtensionSupport(VkPhysicalDevice& _pgpu);                      // Checks if physical device supports Vulkan extensions
    }

    class GraphicsCard {

        /*
         * TheForge Engine Core - Graphics Card
         * Contains VkPhysicalDevice and VkDevice
        **/

        friend class EngineCore;

    public:

        GraphicsCard();         // Default constructor
        ~GraphicsCard();        // Default destructor

        // GraphicsCard object initializer
        int init(VkInstance& _instance, VkSurfaceKHR& _surface);

        int SelectGraphicsCard(VkInstance& _instance, VkSurfaceKHR& _surface);        // Selects the physical graphics card to use

        // Temp Getters .. to be deprecated
        VkQueue& GQueue() { return graphicsQueue; };     /// DEPRECATED 08/02/2020 - TO BE REMOVED
        VkQueue& PQueue() { return presentQueue; };      /// DEPRECATED 08/02/2020 - TO BE REMOVED

    private:

        void cleanup();

        VkPhysicalDevice PGPU;      // Handle to PhysicalDevice -> Physical Graphics card
        VkDevice LGPU;              // Handle to LogicalDevice  -> Logical Graphics Card

        VkQueue graphicsQueue;      // Handle to device queue -> graphics
        VkQueue presentQueue;       // Handle to device queue -> present
    };
}

#endif // !THEFORGE_CORE_GRAPHICSCARD
