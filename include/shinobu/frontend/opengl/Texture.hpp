#pragma once
#include <tuple>
#include <glad/glad.h>

namespace Shinobu {
    namespace Frontend {
        namespace OpenGL {
            class Texture {
                GLuint object;
                GLsizei width;
                GLsizei height;
            public:
                Texture(GLsizei width, GLsizei height);
                ~Texture();

                std::pair<uint32_t,uint32_t> dimensions() const;
                GLuint getObject() const;
                void bind(GLenum texture) const;
            };
        };
    };
};
