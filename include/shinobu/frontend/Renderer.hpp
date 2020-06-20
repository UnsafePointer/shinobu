#pragma once
#include <memory>
#include "shinobu/frontend/sdl2/Window.hpp"

namespace Core::Device::PictureProcessingUnit {
    class Processor;
};

namespace Shinobu {
    namespace Frontend {
        class Renderer {
        protected:
            std::unique_ptr<Shinobu::Frontend::SDL2::Window> &window;
            std::unique_ptr<Core::Device::PictureProcessingUnit::Processor> &PPU;
        public:
            Renderer(std::unique_ptr<Shinobu::Frontend::SDL2::Window> &window, std::unique_ptr<Core::Device::PictureProcessingUnit::Processor> &PPU);
            ~Renderer();
            virtual void update() = 0;
            virtual void handleSDLEvent(SDL_Event event) = 0;
        };
    };
};
