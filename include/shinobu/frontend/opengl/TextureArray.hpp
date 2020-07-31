#pragma once
#include <vector>
#include <tuple>
#include <glad/glad.h>

namespace Shinobu {
    namespace Frontend {
        namespace OpenGL {
            class TextureArray {
                std::vector<GLuint> textures;
                unsigned int capacity;
                GLsizei width;
                GLsizei height;
            public:
                TextureArray(unsigned int capacity, GLsizei width, GLsizei height);
                ~TextureArray();

                GLuint getTextureAtIndex(unsigned int index);
                std::pair<GLsizei,GLsizei> getDimensions() const;
            };
        };
    };
};
