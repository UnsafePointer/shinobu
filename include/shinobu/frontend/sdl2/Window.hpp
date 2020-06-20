#pragma once
#include <SDL2/SDL.h>
#include <string>
#include <cstdint>
#include <tuple>
#include "common/Logger.hpp"

namespace Shinobu {
    namespace Frontend {
        namespace SDL2 {
            class Window {
                Common::Logs::Logger logger;
                std::string title;
                uint32_t width;
                uint32_t height;
                SDL_GLContext SDLGLContext;
                SDL_Window *window;
                uint32_t windowID;
            public:
                Window(std::string title, uint32_t width, uint32_t height);
                ~Window();

                std::pair<uint32_t,uint32_t> dimensions() const;
                SDL_Window* windowRef() const;
                SDL_GLContext GLContext() const;
                void handleSDLEvent(SDL_Event event) const;
                void updateWindowTitleWithSuffix(std::string suffix) const;
            };
        };
    };
};
