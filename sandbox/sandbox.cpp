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
#include <pipeline.hpp>

#include <ASWL/utilities.hpp>

int main() {

    util::Logger("00000", "Hello this is a test");

    Forge::Engine e;
    Forge::GraphicsCard gc;
    Forge::LogicalGraphicsCard lgc;
    Forge::Swapchain spc;
    Forge::Pipeline pipeline;

    int ret = 0;

    auto CLOG = [=](std::string belt, int ret) {
        std::string m00001 = belt + " [" + std::to_string(ret) + "] returned by TheForge.";
        ASWL::utilities::Logger("00001", m00001);
    };

    CLOG("CLOG-TEST", 0);

    e.metadata.vkAppName = "TheForge Sandbox";
    e.metadata.windowTitle = "TheForge Sandbox";

    ret += e.init();
    if (ret != 0) {
        CLOG("engine", ret);
        return ret;
    }
    ret += gc.autochoose(e.GetInstance(), e.GetSurface());
    if (ret != 0) {
        CLOG("graphics card", ret);
        return ret;
    }
    ret += lgc.init(gc.GetGraphicsCard(), e.GetSurface());
    if (ret != 0) {
        CLOG("logical graphics card", ret);
        return ret;
    }
    ret += spc.init(gc.GetGraphicsCard(), e.GetSurface(), lgc.GetDevice());
    if (ret != 0) {
        CLOG("swapchain", ret);
        return ret;
    }
    ret += pipeline.init(lgc.GetDevice(), spc);
    if (ret != 0) {
        CLOG("pipeline", ret);
        return ret;
    }
    ret += spc.initFramebuffers(pipeline.GetRenderPass());
    if (ret != 0) {
        CLOG("swapchain framebuffers", ret);
        return ret;
    }

    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::string m00002 = "[" + std::to_string(extensionCount) + "] extensions supported.";
    ASWL::utilities::Logger("00002", m00002);


    while (!e.WindowShouldClose()) {
        e.update();
    }

    return 0;
}
