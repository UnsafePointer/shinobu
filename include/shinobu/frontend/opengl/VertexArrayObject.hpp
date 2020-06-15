#pragma once
#include <glad/glad.h>

namespace Shinobu {
    namespace Frontend {
        namespace OpenGL {
            class VertexArrayObject {
                GLuint object;
            public:
                VertexArrayObject();
                ~VertexArrayObject();

                void bind() const;
            };
        };
    };
};
