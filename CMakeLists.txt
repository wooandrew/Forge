# The Forge :-> (c) Andrew Woo, 2020

cmake_minimum_required (VERSION 3.8)
set(PROJECT_VERSION "0.1.0")

project (TheForge VERSION ${PROJECT_VERSION})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Find Package
find_package(Vulkan REQUIRED FATAL_ERROR)

# Link
include_directories("${CMAKE_SOURCE_DIR}/includes")
include_directories("${CMAKE_SOURCE_DIR}/root/include")

link_directories("${CMAKE_SOURCE_DIR}/lib")

# Directories
add_subdirectory(root)
add_subdirectory(sandbox)

# Libraries
#add_library(libaswl "${CMAKE_SOURCE_DIR}/lib/dlls/libaswl.dll")
#set_target_properties(libaswl PROPERTIES LINKER_LANGUAGE CXX)
#add_library(libaswl STATIC "${CMAKE_SOURCE_DIR}/includes/ASWL/utilities.cpp")

if(MSVC)
    set(CMAKE_SHARED_LINKER_FLAGS ${CMAKE_SHARED_LINKER_FLAGS} "/NODEFAULTLIBS:LIBCMT")
    set(CMAKE_SHARED_LINKER_FLAGS ${CMAKE_SHARED_LINKER_FLAGS} "/NODEFAULTLIBS:MSVCRT")
    set(CMAKE_SHARED_LINKER_FLAGS ${CMAKE_SHARED_LINKER_FLAGS} "/NODEFAULTLIBS:MSVCPRTD")
endif()