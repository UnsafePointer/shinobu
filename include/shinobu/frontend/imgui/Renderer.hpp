#pragma once
#include <memory>
#include "shinobu/frontend/sdl2/Window.hpp"
#include <imgui/imgui.h>
#include <SDL2/SDL.h>
#include <vector>
#include <string>

namespace Shinobu {
    namespace Frontend {
        namespace Imgui {
            class Renderer {
                std::unique_ptr<Shinobu::Frontend::SDL2::Window> &window;
                ImGuiIO *io;
                ImVec4 backgroundColor;
            public:
                Renderer(std::unique_ptr<Shinobu::Frontend::SDL2::Window> &window);
                ~Renderer();

                void update();
                void handleSDLEvent(SDL_Event event);
            };
        };
    };
};
