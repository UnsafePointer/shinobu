#pragma once
#include <memory>
#include <imgui/imgui.h>
#include "shinobu/frontend/Renderer.hpp"
#include "shinobu/frontend/sdl2/Window.hpp"
#include "shinobu/frontend/opengl/VertexRenderer.hpp"
#include "shinobu/frontend/opengl/Texture.hpp"
#include "shinobu/frontend/opengl/TextureArray.hpp"

namespace Core::Device::PictureProcessingUnit {
    class Processor;
};

namespace Shinobu {
    namespace Frontend {
        namespace Imgui {
            class Renderer : public Shinobu::Frontend::Renderer {
                ImGuiIO *io;
                ImVec4 backgroundColor;

                Shinobu::Frontend::OpenGL::VertexRenderer backgroundViewportRenderer;
                Shinobu::Frontend::OpenGL::Texture backgroundMapTexture;
                Shinobu::Frontend::OpenGL::Texture tileDataTexture;
                Shinobu::Frontend::OpenGL::Texture LCDOutputTexture;
                Shinobu::Frontend::OpenGL::TextureArray spriteTextures;
            public:
                Renderer(std::unique_ptr<Shinobu::Frontend::SDL2::Window> &window, std::unique_ptr<Core::Device::PictureProcessingUnit::Processor> &PPU);
                ~Renderer();

                void update() override;
                void handleSDLEvent(SDL_Event event) override;
                Kind frontendKind() override { return Kind::PPU; }
            };
        };
    };
};
