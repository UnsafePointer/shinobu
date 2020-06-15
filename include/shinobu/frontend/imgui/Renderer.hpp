#pragma once
#include <memory>
#include "shinobu/frontend/sdl2/Window.hpp"
#include <imgui/imgui.h>
#include <SDL2/SDL.h>
#include <vector>
#include <string>
#include "shinobu/frontend/opengl/Renderer.hpp"

namespace Core::Device::PictureProcessingUnit {
    class Processor;
};

namespace Shinobu {
    namespace Frontend {
        namespace Imgui {
            class Renderer {
                std::unique_ptr<Shinobu::Frontend::SDL2::Window> &window;
                std::unique_ptr<Core::Device::PictureProcessingUnit::Processor> &PPU;
                std::unique_ptr<Shinobu::Frontend::OpenGL::Renderer> renderer;
                ImGuiIO *io;
                ImVec4 backgroundColor;
            public:
                Renderer(std::unique_ptr<Shinobu::Frontend::SDL2::Window> &window, std::unique_ptr<Core::Device::PictureProcessingUnit::Processor> &PPU);
                ~Renderer();

                void update();
                void handleSDLEvent(SDL_Event event);
            };
        };
    };
};
