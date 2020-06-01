// Vulkan Engine.cpp : Defines the entry point for the application.

// #define GLM_FORCE_RADIANS
// #define GLM_FORCE_DEPTH_ZERO_TO_ONE
// #include <GLM/vec4.hpp>
// #include <GLM/mat4x4.hpp>

#include <iostream>

#include <engine.hpp>
#include <physical_devices.hpp>
#include <logical_devices.hpp>

#include <ASWL/utilities.hpp>

int main() {

    util::Logger("00000", "Hello this is a test");

    Forge::Engine e;
    Forge::GraphicsCard gc;
    Forge::LogicalGraphicsCard lgc;

    int ret = 0;
    ret += e.init();
    gc.autochoose(e.GetInstance(), e.GetSurface());
    ret += lgc.init(gc.GetGraphicsCard(), e.GetSurface());

    if (ret != 0) {
        std::cout << ret << " returned by TheForge\n";
        return ret;
    }

    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::cout << ret << " returned by TheForge\n";
    std::cout << extensionCount << " extensions supported\n";


    while (!e.WindowShouldClose()) {
        e.update();
        
    }

    return 0;
}
