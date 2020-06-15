// Vulkan Engine.cpp : Defines the entry point for the application.

// #define GLM_FORCE_RADIANS
// #define GLM_FORCE_DEPTH_ZERO_TO_ONE
// #include <GLM/vec4.hpp>
// #include <GLM/mat4x4.hpp>

#include <iostream>
#include <string>

#include <engine.hpp>

#include <ASWL/utilities.hpp>

int main() {

    util::Logger("00000", "Hello, this is a test.");

    Forge::Engine e;

    auto CLOG = [=](std::string belt, int ret) {
        std::string m00001 = belt + " [" + std::to_string(ret) + "] returned by TheForge.";
        ASWL::utilities::Logger("00001", m00001);
    };

    CLOG("CLOG-TEST", 0);

    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::string m00002 = "[" + std::to_string(extensionCount) + "] extensions supported.";
    ASWL::utilities::Logger("00002", m00002);

    e.metadata.vkAppName = "TheForge Sandbox";
    e.metadata.windowTitle = "TheForge Sandbox";

    int ret = e.init();
    if (ret != 0) {
        CLOG("engine", ret);
        return ret;
    }

    float num = 0;
    int renderResult = 0;

    while (!e.WindowShouldClose()) {

        e.update();
        renderResult = e.render2D.draw();

        if (renderResult == 2 || renderResult == 5) {
            int reinitCode = e.reinitialize();
            if (reinitCode != 0)
                return reinitCode;
        }

        //num++;
        //e.metadata.clearcolor = { std::fmod(num, 255.f) / 255.f, std::fmod(num, 255.f) / 255.f, std::fmod(num, 255.f) / 255.f, 0.f };
        //e.SetClearColor();
    }

    return 0;
}
