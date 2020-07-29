// TheForge - src/app/renderer (c) Andrew Woo, 2020

#include "app/renderer.hpp"

namespace Forge::App {

    // Default constructor
    Renderer::Renderer() {

        core = std::make_shared<Core::EngineCore>();
        framework = std::make_shared<Framework>();
    }

    // Default destructor
    Renderer::~Renderer() {

    }
}
