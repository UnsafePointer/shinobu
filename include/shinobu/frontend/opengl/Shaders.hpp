#pragma once

namespace Shinobu {
    namespace Frontend {
        namespace OpenGL {
            namespace Shaders {
                const std::string fragment = "\n"
                "#version 330 core\n"
                "out vec4 FragColor;\n"
                "\n"
                "in vec3 fragmentColor;\n"
                "\n"
                "void main() {\n"
                "    FragColor = vec4(fragmentColor, 1.0);\n"
                "}\n"
                "\n";

                const std::string vertex = "\n"
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

                const std::string textureFragment = "\n"
                "#version 330 core\n"
                "out vec4 FragColor;\n"
                "\n"
                "in vec2 fragmentTexturePosition;\n"
                "\n"
                "uniform sampler2D textureToSample;\n"
                "\n"
                "uniform int applyScale;"
                "\n"
                "#define COLOR_LOW 0.8\n"
                "#define COLOR_HIGH 1.0\n"
                "#define SCANLINE_DEPTH 0.1\n"
                "\n"
                "vec4 scale(sampler2D image, vec2 position, vec2 input_resolution)\n"
                "{\n"
                "    vec2 pos = fract(position * input_resolution);\n"
                "    vec2 sub_pos = fract(position * input_resolution * 6);\n"
                "\n"
                "    vec4 center = texture(image, position);\n"
                "    vec4 left = texture(image, position - vec2(1.0 / input_resolution.x, 0));\n"
                "    vec4 right = texture(image, position + vec2(1.0 / input_resolution.x, 0));\n"
                "\n"
                "    if (pos.y < 1.0 / 6.0) {\n"
                "        center = mix(center, texture(image, position + vec2(0, -1.0 / input_resolution.y)), 0.5 - sub_pos.y / 2.0);\n"
                "        left =   mix(left,   texture(image, position + vec2(-1.0 / input_resolution.x, -1.0 / input_resolution.y)), 0.5 - sub_pos.y / 2.0);\n"
                "        right =  mix(right,  texture(image, position + vec2( 1.0 / input_resolution.x, -1.0 / input_resolution.y)), 0.5 - sub_pos.y / 2.0);\n"
                "        center *= sub_pos.y * SCANLINE_DEPTH + (1 - SCANLINE_DEPTH);\n"
                "        left *= sub_pos.y * SCANLINE_DEPTH + (1 - SCANLINE_DEPTH);\n"
                "        right *= sub_pos.y * SCANLINE_DEPTH + (1 - SCANLINE_DEPTH);\n"
                "    }\n"
                "    else if (pos.y > 5.0 / 6.0) {\n"
                "        center = mix(center, texture(image, position + vec2(0, 1.0 / input_resolution.y)), sub_pos.y / 2.0);\n"
                "        left =   mix(left,   texture(image, position + vec2(-1.0 / input_resolution.x, 1.0 / input_resolution.y)), sub_pos.y / 2.0);\n"
                "        right =  mix(right,  texture(image, position + vec2( 1.0 / input_resolution.x, 1.0 / input_resolution.y)), sub_pos.y / 2.0);\n"
                "        center *= (1.0 - sub_pos.y) * SCANLINE_DEPTH + (1 - SCANLINE_DEPTH);\n"
                "        left *= (1.0 - sub_pos.y) * SCANLINE_DEPTH + (1 - SCANLINE_DEPTH);\n"
                "        right *= (1.0 - sub_pos.y) * SCANLINE_DEPTH + (1 - SCANLINE_DEPTH);\n"
                "    }\n"
                "\n"
                "\n"
                "    vec4 midleft = mix(left, center, 0.5);\n"
                "    vec4 midright = mix(right, center, 0.5);\n"
                "\n"
                "    vec4 ret;\n"
                "    if (pos.x < 1.0 / 6.0) {\n"
                "        ret = mix(vec4(COLOR_HIGH * center.r, COLOR_LOW * center.g, COLOR_HIGH * left.b, 1),\n"
                "                  vec4(COLOR_HIGH * center.r, COLOR_LOW * center.g, COLOR_LOW  * left.b, 1),\n"
                "                  sub_pos.x);\n"
                "    }\n"
                "    else if (pos.x < 2.0 / 6.0) {\n"
                "        ret = mix(vec4(COLOR_HIGH * center.r, COLOR_LOW  * center.g, COLOR_LOW * left.b, 1),\n"
                "                  vec4(COLOR_HIGH * center.r, COLOR_HIGH * center.g, COLOR_LOW * midleft.b, 1),\n"
                "                  sub_pos.x);\n"
                "    }\n"
                "    else if (pos.x < 3.0 / 6.0) {\n"
                "        ret = mix(vec4(COLOR_HIGH * center.r  , COLOR_HIGH * center.g, COLOR_LOW * midleft.b, 1),\n"
                "                  vec4(COLOR_LOW  * midright.r, COLOR_HIGH * center.g, COLOR_LOW * center.b, 1),\n"
                "                  sub_pos.x);\n"
                "    }\n"
                "    else if (pos.x < 4.0 / 6.0) {\n"
                "        ret = mix(vec4(COLOR_LOW * midright.r, COLOR_HIGH * center.g , COLOR_LOW  * center.b, 1),\n"
                "                  vec4(COLOR_LOW * right.r   , COLOR_HIGH  * center.g, COLOR_HIGH * center.b, 1),\n"
                "                  sub_pos.x);\n"
                "    }\n"
                "    else if (pos.x < 5.0 / 6.0) {\n"
                "        ret = mix(vec4(COLOR_LOW * right.r, COLOR_HIGH * center.g  , COLOR_HIGH * center.b, 1),\n"
                "                  vec4(COLOR_LOW * right.r, COLOR_LOW  * midright.g, COLOR_HIGH * center.b, 1),\n"
                "                  sub_pos.x);\n"
                "    }\n"
                "    else {\n"
                "        ret = mix(vec4(COLOR_LOW  * right.r, COLOR_LOW * midright.g, COLOR_HIGH * center.b, 1),\n"
                "                  vec4(COLOR_HIGH * right.r, COLOR_LOW * right.g  ,  COLOR_HIGH * center.b, 1),\n"
                "                  sub_pos.x);\n"
                "    }\n"
                "\n"
                "    return ret;\n"
                "}\n"
                "\n"
                "void main() {\n"
                "    if (applyScale == 1) {\n"
                "        vec2 input_resolution = textureSize(textureToSample, 0);\n"
                "        FragColor = scale(textureToSample, fragmentTexturePosition, input_resolution);\n"
                "    } else {\n"
                "        FragColor = texture(textureToSample, fragmentTexturePosition);\n"
                "    }\n"
                "}\n"
                "\n";

                const std::string textureVertex = "\n"
                "#version 330 core\n"
                "in vec2 position;\n"
                "in vec2 texturePosition;\n"
                "\n"
                "out vec2 fragmentTexturePosition;\n"
                "\n"
                "void main() {\n"
                "    gl_Position = vec4(position.x, position.y, 0.0, 1.0);\n"
                "    fragmentTexturePosition = texturePosition;\n"
                "}\n"
                "\n";
            };
        };
    };
};
