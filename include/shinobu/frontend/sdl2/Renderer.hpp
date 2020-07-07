#pragma once
#include <memory>
#include "shinobu/frontend/Renderer.hpp"
#include "shinobu/frontend/sdl2/Window.hpp"
#include "shinobu/frontend/opengl/Renderer.hpp"
#include <imgui/imgui.h>
#include "shinobu/frontend/Performance.hpp"

namespace Core::Device::PictureProcessingUnit {
    class Processor;
};

namespace Shinobu {
    namespace Frontend {
        namespace SDL2 {
            class Renderer : public Shinobu::Frontend::Renderer {
                std::unique_ptr<Shinobu::Frontend::OpenGL::Renderer> renderer;
                ImGuiIO *io;
                Shinobu::Frontend::Performance::Frame lastFrame;
                bool shouldDisplayPerformanceOverlay;
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
