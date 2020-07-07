#pragma once
#include <memory>
#include <imgui/imgui.h>
#include "shinobu/frontend/Renderer.hpp"
#include "shinobu/frontend/sdl2/Window.hpp"
#include "shinobu/frontend/opengl/Renderer.hpp"

namespace Core::Device::PictureProcessingUnit {
    class Processor;
};

namespace Shinobu {
    namespace Frontend {
        namespace Imgui {
            class Renderer : public Shinobu::Frontend::Renderer {
                std::unique_ptr<Shinobu::Frontend::OpenGL::Renderer> tileDataRenderer;
                std::unique_ptr<Shinobu::Frontend::OpenGL::Renderer> backgroundMapRenderer;
                std::unique_ptr<Shinobu::Frontend::OpenGL::Renderer> LCDOutputRenderer;
                std::unique_ptr<Shinobu::Frontend::OpenGL::Renderer> spriteRenderer;
                ImGuiIO *io;
                ImVec4 backgroundColor;
            public:
                Renderer(std::unique_ptr<Shinobu::Frontend::SDL2::Window> &window, std::unique_ptr<Core::Device::PictureProcessingUnit::Processor> &PPU);
                ~Renderer();

                void update() override;
                void handleSDLEvent(SDL_Event event) override;
                void setLastPerformanceFrame(Shinobu::Frontend::Performance::Frame frame) override;
            };
        };
    };
};
