# The Forge : root-src :-> (c) Andrew Woo, 2020

cmake_minimum_required (VERSION 3.8)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS YES CACHE BOOL "Export all symbols")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib")

# Engine
add_library(libforge SHARED 

    "include/forge.hpp"                 "src/forge.cpp"
    "include/forge_vars.hpp"            "src/forge_vars.cpp"
    "include/engine.hpp"                "src/engine.cpp"
    #"include/archive/physical_devices.hpp"      "src/archive/physical_devices.cpp"
    #"include/archive/logical_devices.hpp"       "src/archive/logical_devices.cpp"
    "include/swapchain.hpp"             "src/swapchain.cpp"
    "include/pipeline.hpp"              "src/pipeline.cpp"              
    "include/command_buffers.hpp"       "src/command_buffers.cpp"
    "include/renderer.hpp"              "src/renderer.cpp"
    "include/vertex.hpp"                "src/vertex.cpp" 
    "include/vkcontainer.hpp"           "src/vkcontainer.cpp"  
    "include/core/core.hpp"             "src/core/core.cpp"
    "include/core/graphicscard.hpp"     "src/core/graphicscard.cpp" )

include(GenerateExportHeader)
generate_export_header(libforge)

target_compile_definitions(libforge PRIVATE VK_USE_PLATFORM_WIN32_KHR)					# REMEMBER TO CHANGE DEPENDING ON PLATFORM
target_include_directories(libforge PRIVATE Vulkan::Vulkan)
target_link_libraries(libforge PRIVATE Vulkan::Vulkan)

target_link_libraries(libforge PRIVATE glfw3.lib)
target_link_libraries(libforge PRIVATE "libaswl-shared.lib")
