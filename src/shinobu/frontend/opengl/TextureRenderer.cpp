#include "shinobu/frontend/opengl/TextureRenderer.hpp"
#include "shinobu/frontend/opengl/debug/Debugger.hpp"
#include "shinobu/frontend/opengl/Framebuffer.hpp"
#include "shinobu/frontend/opengl/Shaders.hpp"

using namespace Shinobu::Frontend::OpenGL;

TextureRenderer::TextureRenderer(uint32_t width, uint32_t height, bool applyScale) : width(width), height(height), applyScale(applyScale) {
    program = std::make_unique<Program>(Shaders::textureVertex, Shaders::textureFragment);
    program->useProgram();

    GLuint applyScaleUniform = program->findProgramUniform("applyScale");
    glUniform1i(applyScaleUniform, applyScale);

    texture = std::make_unique<Texture>(width, height);
    buffer = std::make_unique<Buffer<Texel>>(program, 4);

    Debug::Debugger *debugger = Debug::Debugger::getInstance();
    debugger->checkForOpenGLErrors();
}

TextureRenderer::~TextureRenderer() {
}

void TextureRenderer::addTextureData(std::vector<GLfloat> textureData) const {
    texture->bind(GL_TEXTURE0);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB,  GL_FLOAT, textureData.data());
    Debug::Debugger *debugger = Debug::Debugger::getInstance();
    debugger->checkForOpenGLErrors();
}

void TextureRenderer::clear() const {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void TextureRenderer::render() const {
    std::array<Texel, 4> data = {
        Texel({{-1.0f, -1.0f}, {0.0f, 1.0f}}),
        Texel({{1.0f, -1.0f}, {1.0f, 1.0f}}),
        Texel({{-1.0f, 1.0f}, {0.0f, 0.0f}}),
        Texel({{1.0f, 1.0f}, {1.0f, 0.0f}}),
    };
    buffer->addData(data.data(), data.size());
    buffer->draw(GL_TRIANGLE_STRIP);
    Debug::Debugger *debugger = Debug::Debugger::getInstance();
    debugger->checkForOpenGLErrors();
}

void TextureRenderer::toggleApplyScale() {
    applyScale = !applyScale;
    GLuint applyScaleUniform = program->findProgramUniform("applyScale");
    glUniform1i(applyScaleUniform, applyScale);
}
