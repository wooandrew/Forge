// TheForge - src/app/renderer (c) Andrew Woo, 2020

#pragma once

#ifndef THEFORGE_APP_RENDERER
#define THEFORGE_APP_RENDERER

// Standard Library
#include <memory>

// TheForge includes
#include "core/core.hpp"
#include "framework.hpp"

namespace Forge::App {

    class Renderer {

        /**
         * TheForge App Renderer
         * Contains rendering logic
         *
         * The renderer contains higher-level
         * rendering logic for the end-user to
         * use more efficiently.
        **/

    public:

        Renderer();         // Default constructor
        ~Renderer();        // Default destructor

        int init(std::shared_ptr<Core::EngineCore> _core, std::shared_ptr<App::Framework> _framework);         // Initialize renderer

    private:

        std::shared_ptr<Core::EngineCore> core;
        std::shared_ptr<App::Framework> framework;

    };
}

#endif // !THEFORGE_APP_RENDERER