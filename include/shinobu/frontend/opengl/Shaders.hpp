#pragma once

namespace Shinobu {
    namespace Frontend {
        namespace OpenGL {
            namespace Shaders {
                const char *fragment = "\n"
                "#version 330 core\n"
                "out vec4 FragColor;\n"
                "\n"
                "in vec3 fragmentColor;\n"
                "\n"
                "void main() {\n"
                "    FragColor = vec4(fragmentColor, 1.0);\n"
                "}\n"
                "\n";

                const char *vertex = "\n"
                "#version 330 core\n"
                "in vec2 position;\n"
                "in vec3 color;\n"
                "\n"
                "uniform float width;\n"
                "uniform float height;\n"
                "out vec3 fragmentColor;\n"
                "\n"
                "void main() {\n"
                "    float x_pos = (float(position.x) / (width / 2.0)) - 1.0;\n"
                "    float y_pos = (float(position.y) / (height / 2.0)) - 1.0;\n"
                "\n"
                "    gl_Position = vec4(x_pos, y_pos, 0.0, 1.0);\n"
                "    fragmentColor = color;\n"
                "}\n"
                "\n";
            };
        };
    };
};
