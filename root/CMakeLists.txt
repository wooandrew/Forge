# The Forge : root-src :-> (c) Andrew Woo, 2020

cmake_minimum_required (VERSION 3.8)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS YES CACHE BOOL "Export all symbols")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib")
file(GLOB SOURCES CONFIGURE_DEPENDS "src/*.cpp")

# Engine
add_library(libforge SHARED ${SOURCES})

include(GenerateExportHeader)
generate_export_header(libforge)

target_compile_definitions(libforge PRIVATE VK_USE_PLATFORM_WIN32_KHR)					# REMEMBER TO CHANGE DEPENDING ON PLATFORM
target_include_directories(libforge PRIVATE Vulkan::Vulkan)
target_link_libraries(libforge PRIVATE Vulkan::Vulkan)

target_link_libraries(libforge PRIVATE glfw3.lib)
target_link_libraries(libforge PRIVATE "libaswl-shared.lib")
