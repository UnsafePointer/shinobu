#include "shinobu/frontend/opengl/VertexRenderer.hpp"
#include "shinobu/frontend/opengl/debug/Debugger.hpp"
#include "shinobu/frontend/opengl/Framebuffer.hpp"
#include "shinobu/frontend/opengl/Shaders.hpp"
#include "common/System.hpp"

using namespace Shinobu::Frontend::OpenGL;

VertexRenderer::VertexRenderer(uint32_t width, uint32_t height) : width(width), height(height), currentDrawMode(GL_TRIANGLES) {
    program = std::make_unique<Program>(Shaders::vertex, Shaders::fragment);
    program->useProgram();

    GLuint widthUniform = program->findProgramUniform("width");
    glUniform1f(widthUniform, width);
    GLuint heightUniform = program->findProgramUniform("height");
    glUniform1f(heightUniform, height);

    buffer = std::make_unique<Buffer<Vertex>>(program, 1024);

    Debug::Debugger *debugger = Debug::Debugger::getInstance();
    debugger->checkForOpenGLErrors();
}

VertexRenderer::~VertexRenderer() {
}

void VertexRenderer::addLines(std::vector<Vertex> vertices) {
    checkForceDraw(vertices.size(), GL_LINES);
    currentDrawMode = GL_LINES;
    buffer->addData(vertices.data(), vertices.size());
}

void VertexRenderer::clear() const {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void VertexRenderer::checkForceDraw(uint32_t verticesToRender, GLenum drawMode) {
    if (this->currentDrawMode != drawMode) {
        render();
    } else if (buffer->remainingCapacity() < verticesToRender) {
        render();
    }
    return;
}

void VertexRenderer::render() const {
    buffer->draw(currentDrawMode);
}
