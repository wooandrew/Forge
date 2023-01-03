// TheForge - src/forge (c) Andrew Woo, 2020

#pragma once

/// FILE GUIDELINES
/*  This file must be self-contained.
 *  It should not include any headers
 *  from TheForge's internal library.
 *  Dependencies are allowed.
**/

#ifndef THEFORGE_FORGE
#define THEFORGE_FORGE

// Standard Library
#include <array>
#include <vector>
#include <utility>
#include <optional>

// Dependencies
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include <GLM/glm.hpp>

#include <ASWL/utilities.hpp>
#include <ASWL/logger.hpp>

namespace Forge {

#ifdef NDEBUG
    constexpr bool DEBUG_MODE = false;          // If false, do not enable debug mode
#else
    constexpr bool DEBUG_MODE = true;           // If true, enable debug mode
#endif

    static const std::vector<const char*> ValidationLayers = { "VK_LAYER_KHRONOS_validation" };             // List of validation layers to enable
    static const std::vector<const char*> DeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };           // List of device extensions to enable

    constexpr int MAX_FRAMES_IN_FLIGHT = 2;

    constexpr int FORGE_SUCCESS = 0;


    struct Vertex {

        glm::vec2 position;         // Vertex position in 2D space
        glm::vec3 color;            // Vertex color

        // Create a vertex binding description
        static VkVertexInputBindingDescription GetBindingDescription();

        // Array of vertex attribute descriptions
        static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDesciptions();
    };

    const Vertex vertices[] = {
        {{ -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{ 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{ 0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f,  0.5f}, {0.0f, 0.0f, 0.0f}}
    };

    // Index buffer indice texcoords
    static uint16_t indices[] = {
         0, 1, 2, 2, 3, 0
    };

    struct QueueFamilyIndices {                         // Struct containing Queue Family indices

        std::optional<uint32_t> graphicsFamily;         // Index for graphics queue family
        std::optional<uint32_t> presentFamily;          // Index for presentation queue family

        bool hasValue();        // Function to determine if all queue families have indices
    };
    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice& device, VkSurfaceKHR& surface);     // Finds queue family supported by specified device

    struct SwapChainSupportDetails {                    // Struct containing Swap Chain support details

        VkSurfaceCapabilitiesKHR capabilities;          // Struct containing details of the capabilities of the swap chains
        std::vector<VkSurfaceFormatKHR> formats;        // List of structures describing a supported swapchain format-color space pair
        std::vector<VkPresentModeKHR> presentModes;     // List of presentation modes supported by the device
    };
    SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice& device, VkSurfaceKHR& surface);             // Populates SwapChainSupportDetails
    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);        // Choose surface format
    VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);         // Choose surface present mode
    VkExtent2D ChooseSwapExtent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR& capabilities);              // Choose swap extent -> surface resolution

    struct UniformBuffer {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
    };

    // TEMP
    enum class RendererType {       // Renderer type
        Render_2D,
        Render_3D
    };

    enum class ShaderLanguage {
        THE_FORGE_VK_SHADER_LANGUAGE_SPV,
        THE_FORGE_VK_SHADER_LANGUAGE_GLSL
    };

    enum class ShaderType {
        THE_FORGE_VK_SHADER_TYPE_VERTEX,
        THE_FORGE_VK_SHADER_TYPE_FRAGMENT
    };

    // Forge-ASWL wrappers. The end user can redefine these wrappers if they do not wish to use ASWL.
    struct Version : public ASWL::utilities::Version {};
    template<typename T> struct Dimensions2D : public ASWL::utilities::Dimensions2D<T> {};
    template<typename T> Dimensions2D<T> make_2D_dimensions(T x, T y) {
        return Dimensions2D<T>({ x, y });
    }
    template <typename ERRCODE, typename ERRMSG> void _uLogger(ERRCODE errcode, ERRMSG errmsg) {
        ASWL::utilities::_uLogger(errcode, errmsg);
    }
    class Logger : public ASWL::utilities::Logger {};

    /*
    template<typename FUNCRET, typename ERRCODE, typename ERRMSG, typename RETTYPE> 
    struct ERR_GUARD {

        ERR_GUARD(FUNCRET _retOpt, ERRCODE _errCode, ERRMSG _errMsg, RETTYPE _retOnOpt, RETTYPE _retOnErr, bool _logErr) {

            retOpt = _retOpt;
            errCode = _errCode;
            errMsg = _errMsg;
            retOnOpt = _retOnOpt;
            retOnErr = _retOnErr;
            logErr = _logErr;
        }

        FUNCRET retOpt;         // The error value the function should return on success
        ERRCODE errCode;        // The error code FORGE_ERROR_GUARD will spit
        ERRMSG errMsg;          // The error message FORGE_ERROR_GUARD will spit
        RETTYPE retOnOpt;       // The value FORGE_ERROR_GUARD should return if the function returns success
        RETTYPE retOnErr;       // The value FORGE_ERROR_GUARD should return if the function returns failure
        bool logErr;            // Flag handles whether or not FORGE_ERROR_GUARD should log the error
    };

    template <typename FUNCRET, typename ERRCODE, typename ERRMSG, typename RETTYPE> 
    RETTYPE FORGE_ERROR_GUARD(FUNCRET RET, ERR_GUARD<FUNCRET, ERRCODE, ERRMSG, RETTYPE> errGuard) {

        if (RET != errGuard.retOpt) {                               // If function does not return success
            if (errGuard.logErr)                                    // and error logging is enabled;
                Logger(errGuard.errCode, errGuard.errMsg);          // log the error
            return errGuard.retOnErr;                               // and return the corresponding error value
        }

        // Return success code
        return errGuard.retOnOpt;
    } */
}

#endif // !THEFORGE_FORGE
