#pragma once
#include <memory>
#include "shinobu/frontend/Renderer.hpp"
#include "shinobu/frontend/opengl/Renderer.hpp"
#include "shinobu/frontend/opengl/Texture.hpp"
#include "shinobu/frontend/opengl/Vertex.hpp"
#include "shinobu/frontend/opengl/Buffer.hpp"
#include "shinobu/frontend/opengl/Program.hpp"

namespace Core::Device::PictureProcessingUnit {
    class Processor;
};

namespace Shinobu {
    namespace Frontend {
        namespace Screen {
            class Renderer : public Shinobu::Frontend::Renderer {
                std::unique_ptr<Shinobu::Frontend::OpenGL::Renderer> renderer;
                std::unique_ptr<Shinobu::Frontend::OpenGL::Program> program;
                std::unique_ptr<Shinobu::Frontend::OpenGL::Texture> texture;
                std::unique_ptr<Shinobu::Frontend::OpenGL::Buffer<Shinobu::Frontend::OpenGL::Texel>> buffer;
            public:
                Renderer(std::unique_ptr<Shinobu::Frontend::SDL2::Window> &window, std::unique_ptr<Core::Device::PictureProcessingUnit::Processor> &PPU);
                ~Renderer();

                void update() override;
                void handleSDLEvent(SDL_Event event) override;
                Kind frontendKind() override { return Kind::LCD; }
            };
        };
    };
};
