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

    uint32_t bufferCapacity =  width * 6;

    buffer = std::make_unique<Buffer<Vertex>>(program, bufferCapacity);

    Debug::Debugger *debugger = Debug::Debugger::getInstance();
    debugger->checkForOpenGLErrors();
}

Renderer::~Renderer() {
}

std::array<Vertex, 6> Renderer::verticesForPixel(Vertex pixel) const {
    Vertex v1 = Vertex({pixel.position, pixel.color});
    Vertex v2 = Vertex({pixel.position.x + 1, pixel.position.y, pixel.color});
    Vertex v3 = Vertex({pixel.position.x + 1, pixel.position.y + 1, pixel.color.r, pixel.color.g, pixel.color.b});
    Vertex v4 = Vertex({pixel.position, pixel.color.r, pixel.color.g, pixel.color.b});
    Vertex v5 = Vertex({pixel.position.x, pixel.position.y + 1, pixel.color.r, pixel.color.g, pixel.color.b});
    Vertex v6 = Vertex({pixel.position.x + 1, pixel.position.y + 1, pixel.color.r, pixel.color.g, pixel.color.b});
    return {v1, v2, v3, v4, v5, v6};
}

void Renderer::addPixels(std::vector<Vertex> pixels) {
    uint32_t step = width;
    uint32_t index = 0;
    while (index < pixels.size()) {
        std::vector<Vertex> vertices = std::vector<Vertex>();
        for (uint32_t i = index; i < index + step; i++) {
            Vertex pixel = pixels[i];
            std::array<Vertex, 6> verticesForPixel = this->verticesForPixel(pixel);
            vertices.insert(vertices.end(), verticesForPixel.begin(), verticesForPixel.end());
        }
        checkForceDraw(vertices.size(), GL_TRIANGLES);
        mode = GL_TRIANGLES;
        buffer->addData(vertices.data(), vertices.size());
        index += step;
    }
}

void Renderer::addViewPort(std::vector<Vertex> vertices) {
    checkForceDraw(vertices.size(), GL_LINES);
    mode = GL_LINES;
    glLineWidth(scale);
    buffer->addData(vertices.data(), vertices.size());
}

void Renderer::checkForceDraw(uint32_t verticesToRender, GLenum mode) {
    if (this->mode != mode) {
        render();
    } else if (buffer->remainingCapacity() < verticesToRender) {
        render();
    }
    return;
}

void Renderer::clear() const {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::render() {
    buffer->draw(mode);
}

