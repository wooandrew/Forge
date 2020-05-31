// TheForge - src/physical_devices (c) Andrew Woo, 2020

#pragma once

#ifndef THEFORGE_PHYSICAL_DEVICES
#define THEFORGE_PHYSICAL_DEVICES

// Dependencies
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// TheForge includes
#include "forge_vars.hpp"

namespace Forge {

	// Physical Device -> Graphics Card
	class GraphicsCard {

	public:

		GraphicsCard();			// Default Constructor
		~GraphicsCard();		// Default Destructor

		int autochoose();		// Automatically chooses an appropriate graphics card

		int SelectGraphicsCard();												// Selects the physical graphics card to use
		static bool CheckDeviceSupport(VkPhysicalDevice device);				// Checks if physical device supports Vulkan operation
		static bool CheckDeviceExtensionSupport(VkPhysicalDevice device);		// Checks if physical device supports Vulkan extensions

		VkPhysicalDevice& GetGraphicsCard();		// Returns the handle to the physical graphics card

	private:
		VkPhysicalDevice PhysicalDevice;		// Handle to physical device object
	};
}

#endif // !THEFORGE_PHYSICAL_DEVICES
