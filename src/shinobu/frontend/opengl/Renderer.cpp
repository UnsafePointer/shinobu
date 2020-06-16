#include "shinobu/frontend/opengl/Renderer.hpp"
#include "shinobu/frontend/opengl/debug/Debugger.hpp"
#include "shinobu/frontend/opengl/Framebuffer.hpp"

using namespace Shinobu::Frontend::OpenGL;

Renderer::Renderer(uint32_t width, uint32_t height, uint32_t scale) : mode(GL_TRIANGLES), width(width), height(height), scale(scale) {
    program = std::make_unique<Program>("glsl/vertex.glsl", "glsl/fragment.glsl");
    program->useProgram();

    GLuint widthUniform = program->findProgramUniform("width");
    glUniform1f(widthUniform, width);
    GLuint heightUniform = program->findProgramUniform("height");
    glUniform1f(heightUniform, height);

    buffer = std::make_unique<Buffer<Vertex>>(program, 1024);
    framebufferTexture = std::make_unique<Texture>(width * scale, height * scale);

    Debug::Debugger *debugger = Debug::Debugger::getInstance();
    debugger->checkForOpenGLErrors();
}

Renderer::~Renderer() {
}

std::vector<Vertex> Renderer::verticesForPixel(Vertex pixel) const {
    Vertex v1 = Vertex({pixel.position, pixel.color});
    Vertex v2 = Vertex({pixel.position.x + 1, pixel.position.y, pixel.color});
    Vertex v3 = Vertex({pixel.position.x + 1, pixel.position.y + 1, pixel.color.r, pixel.color.g, pixel.color.b});
    Vertex v4 = Vertex({pixel.position, pixel.color.r, pixel.color.g, pixel.color.b});
    Vertex v5 = Vertex({pixel.position.x, pixel.position.y + 1, pixel.color.r, pixel.color.g, pixel.color.b});
    Vertex v6 = Vertex({pixel.position.x + 1, pixel.position.y + 1, pixel.color.r, pixel.color.g, pixel.color.b});
    return {v1, v2, v3, v4, v5, v6};
}

void Renderer::addPixels(std::vector<Vertex> pixels) {
    for (const auto& pixel : pixels) {
        std::vector<Vertex> verticesForPixel = this->verticesForPixel(pixel);
        checkForceDraw(verticesForPixel.size(), GL_TRIANGLES);
        mode = GL_TRIANGLES;
        buffer->addData(verticesForPixel);
    }
}

void Renderer::addViewPort(std::vector<Vertex> vertices) {
    checkForceDraw(vertices.size(), GL_LINE_LOOP);
    mode = GL_LINE_LOOP;
    glLineWidth(scale);
    buffer->addData(vertices);
}

void Renderer::checkForceDraw(uint32_t verticesToRender, GLenum mode) {
    if (this->mode != mode) {
        render();
    } else if (buffer->remainingCapacity() < verticesToRender) {
        render();
    }
    return;
}

void Renderer::render() {
    Framebuffer framebuffer = Framebuffer(framebufferTexture);
    buffer->draw(mode);
}

GLuint Renderer::framebufferTextureObject() const {
    return framebufferTexture->getObject();
}
