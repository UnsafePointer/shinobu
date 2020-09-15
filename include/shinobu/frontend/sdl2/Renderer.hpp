#pragma once
#include <memory>
#include <deque>
#include <imgui/imgui.h>
#include "shinobu/frontend/Renderer.hpp"
#include "shinobu/frontend/sdl2/Window.hpp"
#include "shinobu/frontend/opengl/TextureRenderer.hpp"

namespace Core::Device::PictureProcessingUnit {
    class Processor;
};

namespace Shinobu {
    namespace Frontend {
        namespace SDL2 {
            class Renderer : public Shinobu::Frontend::Renderer {
                std::unique_ptr<Shinobu::Frontend::OpenGL::TextureRenderer> renderer;

                ImGuiIO *io;
                std::deque<Common::Performance::Frame> frames;
                float maxValue;
                float minValue;
                unsigned int overlayScale;
            public:
                Renderer(std::unique_ptr<Shinobu::Frontend::SDL2::Window> &window, std::unique_ptr<Core::Device::PictureProcessingUnit::Processor> &PPU);
                ~Renderer();

                void update() override;
                void handleSDLEvent(SDL_Event event) override;
                Kind frontendKind() override { return Kind::SDL; }
                void setLastPerformanceFrame(Common::Performance::Frame frame);
            };
        };
    };
};
