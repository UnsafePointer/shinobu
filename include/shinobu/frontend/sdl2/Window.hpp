#pragma once
#include <SDL2/SDL.h>
#include <string>
#include <cstdint>
#include <tuple>
#include <optional>
#include "common/Logger.hpp"
#include "common/Performance.hpp"

namespace Shinobu {
    namespace Frontend {
        namespace Performance {
            struct Frame;
        };

        namespace SDL2 {
            class Window {
                Common::Logs::Logger logger;
                std::string title;
                std::string ROMfilename;
                uint32_t width;
                uint32_t height;
                SDL_GLContext SDLGLContext;
                SDL_Window *window;
                uint32_t windowID;
                std::optional<std::tuple<uint32_t,uint32_t,float,float>> lastFullscreenViewport;

                void toggleFullscreen() const;
                void handleWindowResize(uint32_t width, uint32_t height);
            public:
                Window(std::string title, uint32_t width, uint32_t height, bool fullscreen);
                ~Window();

                std::pair<uint32_t,uint32_t> dimensions() const;
                std::optional<std::tuple<uint32_t,uint32_t,float,float>> fullscreenViewport() const;
                SDL_Window* windowRef() const;
                SDL_GLContext GLContext() const;
                void handleSDLEvent(SDL_Event event);
                void updateWindowTitleWithFramePerformance(Common::Performance::Frame frame) const;
                void setROMFilename(std::string filename);
            };
        };
    };
};
