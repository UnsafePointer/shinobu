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
            class TextureRenderer {
                uint32_t width;
                uint32_t height;
                bool applyScale;

                std::unique_ptr<Texture> texture;
                std::unique_ptr<Program> program;
                std::unique_ptr<Buffer<Texel>> buffer;
            public:
                TextureRenderer(uint32_t width, uint32_t height, bool applyScale);
                ~TextureRenderer();

                void clear() const;
                void render() const;
                void addTextureData(std::vector<GLfloat> textureData) const;
                void toggleApplyScale();
            };
        };
    };
};
