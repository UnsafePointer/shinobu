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
            class VertexRenderer {
                uint32_t width;
                uint32_t height;
                GLenum currentDrawMode;

                std::unique_ptr<Program> program;
                std::unique_ptr<Buffer<Vertex>> buffer;

                void checkForceDraw(uint32_t verticesToRender, GLenum drawMode);
            public:
                VertexRenderer(uint32_t width, uint32_t height);
                ~VertexRenderer();

                void clear() const;
                void render() const;
                void addLines(std::vector<Vertex> vertices);
                void toggleApplyScale();
            };
        };
    };
};
