# The Forge : root-src :-> (c) Andrew Woo, 2020

cmake_minimum_required (VERSION 3.8)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS YES CACHE BOOL "Export all symbols")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib")

# Engine
add_library(libforge SHARED 

    "include/forge.hpp"                 "src/forge.cpp"
    "include/forge_vars.hpp"            "src/forge_vars.cpp"
    "include/engine.hpp"                "src/engine.cpp"
    "include/core/core.hpp"             "src/core/core.cpp"
    "include/core/graphicscard.hpp"     "src/core/graphicscard.cpp"   
    "include/app/framework.hpp"         "src/app/framework.cpp"
    "include/app/renderer.hpp" 		    "src/app/renderer.cpp"
    "src/vk_mem_alloc.cpp"
    #"include/archive/physical_devices.hpp"      "src/archive/physical_devices.cpp"
    #"include/archive/logical_devices.hpp"       "src/archive/logical_devices.cpp"
    #"include/archive/swapchain.hpp"             "src/archive/swapchain.cpp"
    #"include/archive/pipeline.hpp"              "src/archive/pipeline.cpp"              
    #"include/archive/command_buffers.hpp"       "src/archive/command_buffers.cpp"
    #"include/archive/renderer.hpp"              "src/archive/renderer.cpp"
    #"include/archive/vertex.hpp"                "src/archive/vertex.cpp" 
    #"include/archive/vkcontainer.hpp"           "src/archive/vkcontainer.cpp"  
    )

include(GenerateExportHeader)
generate_export_header(libforge)

target_compile_definitions(libforge PRIVATE VK_USE_PLATFORM_WIN32_KHR)					# REMEMBER TO CHANGE DEPENDING ON PLATFORM
target_include_directories(libforge PRIVATE Vulkan::Vulkan)
target_link_libraries(libforge PRIVATE Vulkan::Vulkan)

target_link_libraries(libforge PRIVATE glfw3.lib)
target_link_libraries(libforge PRIVATE "libaswl-shared.lib")

#add_library(VMA SHARED
#    "${CMAKE_SOURCE_DIR}/includes/VMA/vk_mem_alloc.h"
#    "${CMAKE_SOURCE_DIR}/includes/VMA/vk_mem_alloc.cpp"
#)
#target_link_libraries(libforge PRIVATE "${CMAKE_SOURCE_DIR}/includes/VMA/vk_mem_alloc.h")
