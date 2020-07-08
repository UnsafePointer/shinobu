#pragma once
#include <cstdint>
#include <memory>
#include "shinobu/frontend/Renderer.hpp"
#include "shinobu/frontend/sdl2/Window.hpp"
#include "shinobu/frontend/opengl/Renderer.hpp"
#include <imgui/imgui.h>
#include <deque>

namespace Core::Device::PictureProcessingUnit {
    class Processor;
};

namespace Shinobu {
    namespace Frontend {
        namespace Performance {
            class Renderer : public Shinobu::Frontend::Renderer {
                std::unique_ptr<Shinobu::Frontend::OpenGL::Renderer> renderer;
                ImGuiIO *io;
                std::deque<Common::Performance::Frame> frames;
                bool shouldDisplayPerformanceOverlay;
                int overlayScale;
                uint32_t maxValue;
                uint32_t minValue;
            public:
                Renderer(std::unique_ptr<Shinobu::Frontend::SDL2::Window> &window, std::unique_ptr<Core::Device::PictureProcessingUnit::Processor> &PPU);
                ~Renderer();

                void update() override;
                void handleSDLEvent(SDL_Event event) override;
                void setLastPerformanceFrame(Common::Performance::Frame frame) override;
            };
        };
    };
};
