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
                Framebuffer(Texture &texture);
                Framebuffer(GLuint texture, GLsizei width, GLsizei height);
                ~Framebuffer();
            };
        }
    };
};
