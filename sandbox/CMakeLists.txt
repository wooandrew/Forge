# The Forge : sandbox  :-> (c) Andrew Woo, 2020

cmake_minimum_required (VERSION 3.8)

add_executable(sandbox sandbox.cpp)
add_executable(sample sample.cpp)

# Vulkan Stuff
target_compile_definitions(sandbox PRIVATE VK_USE_PLATFORM_WIN32_KHR)					# REMEMBER TO CHANGE DEPENDING ON PLATFORM
target_include_directories(sandbox PRIVATE Vulkan::Vulkan)
target_link_libraries(sandbox PRIVATE Vulkan::Vulkan)

# Dependencies
target_link_libraries(sandbox PRIVATE glfw3.lib)
target_link_libraries(sandbox PRIVATE libforge)
target_link_libraries(sandbox PRIVATE "libaswl-shared.lib")

target_compile_definitions(sample PRIVATE VK_USE_PLATFORM_WIN32_KHR)					# REMEMBER TO CHANGE DEPENDING ON PLATFORM
target_include_directories(sample PRIVATE Vulkan::Vulkan)
target_link_libraries(sample PRIVATE Vulkan::Vulkan)

# GLFW
target_link_libraries(sample PRIVATE glfw3.lib)