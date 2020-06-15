#pragma once
#include <memory>
#include <glad/glad.h>

namespace Shinobu {
    namespace Frontend {
        namespace OpenGL {
            class Texture;

            class Framebuffer {
                GLuint object;
            public:
                Framebuffer(std::unique_ptr<Texture> &texture);
                ~Framebuffer();
            };
        }
    };
};
