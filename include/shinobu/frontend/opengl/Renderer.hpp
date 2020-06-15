#pragma once
#include <SDL2/SDL.h>
#include <memory>
#include <vector>
#include "shinobu/frontend/opengl/Program.hpp"
#include "shinobu/frontend/opengl/Buffer.hpp"
#include "shinobu/frontend/opengl/Vertex.hpp"
#include "shinobu/frontend/opengl/Texture.hpp"

namespace Shinobu {
    namespace Frontend {
        namespace OpenGL {
            class Renderer {
                SDL_GLContext glContext;
                SDL_Window *window;

                uint32_t width;
                uint32_t height;
                uint32_t scale;

                std::unique_ptr<Program> program;
                std::unique_ptr<Buffer<Vertex>> buffer;

                std::unique_ptr<Texture> framebufferTexture;
                std::vector<Vertex> verticesForPixel(Vertex pixel) const;

                void checkForceDraw(uint32_t verticesToRender);
            public:
                Renderer(uint32_t width, uint32_t height, uint32_t scale);
                ~Renderer();

                void render();
                void addPixels(std::vector<Vertex> pixels);
                GLuint framebufferTextureObject() const;
            };
        };
    };
};
