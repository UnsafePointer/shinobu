#include "shinobu/frontend/opengl/Texture.hpp"

using namespace Shinobu::Frontend::OpenGL;

Texture::Texture(GLsizei width, GLsizei height) : width(width), height(height) {
    glGenTextures(1, &object);
    glBindTexture(GL_TEXTURE_2D, object);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB5_A1, width, height);
}

Texture::~Texture() {
    glDeleteTextures(1, &object);
}

std::pair<uint32_t,uint32_t> Texture::dimensions() const {
    return { width, height };
}

GLuint Texture::getObject() const {
    return object;
}

void Texture::bind(GLenum texture) const {
    glActiveTexture(texture);
    glBindTexture(GL_TEXTURE_2D, object);
}
