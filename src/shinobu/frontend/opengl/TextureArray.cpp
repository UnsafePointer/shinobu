#include "shinobu/frontend/opengl/TextureArray.hpp"

using namespace Shinobu::Frontend::OpenGL;

TextureArray::TextureArray(unsigned int capacity, GLsizei width, GLsizei height) : textures({}), capacity(capacity), width(width), height(height) {
    for (unsigned int i = 0; i < capacity; i++) {
        GLuint object;
        glGenTextures(1, &object);
        glBindTexture(GL_TEXTURE_2D, object);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB5_A1, width, height);
        textures.push_back(object);
    }
}

TextureArray::~TextureArray() {
    for (unsigned int i = 0; i < capacity; i++) {
        GLuint object = textures[i];
        glDeleteTextures(1, &object);
    }
}

GLuint TextureArray::getTextureAtIndex(unsigned int index) {
    return textures[index];
}

std::pair<GLsizei,GLsizei> TextureArray::getDimensions() const {
    return { width, height };
}
