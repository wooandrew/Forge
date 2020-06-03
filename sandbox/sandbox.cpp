// Vulkan Engine.cpp : Defines the entry point for the application.

// #define GLM_FORCE_RADIANS
// #define GLM_FORCE_DEPTH_ZERO_TO_ONE
// #include <GLM/vec4.hpp>
// #include <GLM/mat4x4.hpp>

#include <iostream>
#include <string>

#include <engine.hpp>
#include <physical_devices.hpp>
#include <logical_devices.hpp>
#include <swapchain.hpp>

#include <ASWL/utilities.hpp>

int main() {

    util::Logger("00000", "Hello this is a test");

    Forge::Engine e;
    Forge::GraphicsCard gc;
    Forge::LogicalGraphicsCard lgc;
    Forge::Swapchain spc;

    int ret = 0;
    ret += e.init();
    ret += gc.autochoose(e.GetInstance(), e.GetSurface());
    ret += lgc.init(gc.GetGraphicsCard(), e.GetSurface());
    ret += spc.init(gc.GetGraphicsCard(), e.GetSurface(), lgc.GetDevice());

    std::string m00001 = "[" + std::to_string(ret) + "] returned by TheForge.";
    ASWL::utilities::Logger("00001", m00001);

    if (ret != 0)
        return ret;

    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::string m00002 = "[" + std::to_string(extensionCount) + "] extensions supported.";
    ASWL::utilities::Logger("00002", m00002);


    while (!e.WindowShouldClose()) {
        e.update();
        
    }

    return 0;
}
