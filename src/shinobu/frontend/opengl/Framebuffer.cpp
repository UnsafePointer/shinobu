#include "shinobu/frontend/opengl/Framebuffer.hpp"
#include "shinobu/frontend/opengl/Texture.hpp"

using namespace Shinobu::Frontend::OpenGL;

Framebuffer::Framebuffer(Texture &texture) {
    glGenFramebuffers(1, &object);
    glBindFramebuffer(GL_FRAMEBUFFER, object);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.getObject(), 0);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    uint32_t width, height = 0;
    std::tie(width, height) = texture.dimensions();
    glViewport(0, 0, width, height);
}

Framebuffer::~Framebuffer() {
    glDeleteFramebuffers(1, &object);
}
