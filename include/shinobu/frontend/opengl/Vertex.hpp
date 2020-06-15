#pragma once
#include <glad/glad.h>

namespace Shinobu {
    namespace Frontend {
        namespace OpenGL {
            struct Point {
                GLfloat x, y;
            };

            struct Color {
                GLfloat r, g, b;
            };

            struct Vertex {
                Point position;
                Color color;
            };
        };
    };
};
