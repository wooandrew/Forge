# The Forge : Proper name TBD -> (c) Andrew Woo, 2020

cmake_minimum_required (VERSION 3.8)
project ("The Forge")

set(CMAKE_CXX_STANDARD 17)

# Find Package
find_package(Vulkan REQUIRED FATAL_ERROR)

# Link
include_directories(${CMAKE_SOURCE_DIR}/includes)
link_directories(${CMAKE_SOURCE_DIR}/lib)

# Directories
add_subdirectory(root)
add_subdirectory(sandbox)
