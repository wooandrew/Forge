# The Forge _ root :-> (c) Andrew Woo, 2020

cmake_minimum_required (VERSION 3.8)

add_executable(main "main.cpp")

# Engine
add_library(VKEngine STATIC

	engine.cpp


)



# Vulkan Stuff
target_compile_definitions(main PRIVATE VK_USE_PLATFORM_WIN32_KHR)					# REMEMBER TO CHANGE DEPENDING ON PLATFORM
target_include_directories(main PRIVATE Vulkan::Vulkan)
target_link_libraries(main PRIVATE Vulkan::Vulkan)

target_link_libraries(main PRIVATE glfw3.lib)
