#include "shinobu/frontend/opengl/Buffer.hpp"
#include <stddef.h>
#include <iostream>
#include "shinobu/frontend/opengl/Program.hpp"
#include "shinobu/frontend/opengl/VertexArrayObject.hpp"
#include "shinobu/frontend/opengl/Vertex.hpp"
#include <stdexcept>

using namespace Shinobu::Frontend::OpenGL;

template <class T>
Buffer<T>::Buffer(std::unique_ptr<Program> &program, uint32_t capacity) : vao(std::make_unique<VertexArrayObject>()), program(program), capacity(capacity), size(0) {
    glGenBuffers(1, &vbo);

    vao->bind();
    bind();

    GLsizeiptr bufferSize = sizeof(T) * capacity;
    glBufferData(GL_ARRAY_BUFFER, bufferSize, nullptr, GL_DYNAMIC_DRAW);
    enableAttributes();
}

template <class T>
Buffer<T>::~Buffer() {
    glDeleteBuffers(1, &vbo);
}

template <class T>
void Buffer<T>::bind() const {
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
}

template <class T>
void Buffer<T>::clean() {
    bind();
    GLsizeiptr bufferSize = sizeof(T) * capacity;
    glBufferData(GL_ARRAY_BUFFER, bufferSize, nullptr, GL_DYNAMIC_DRAW);
    size = 0;
}

template <class T>
void Buffer<T>::addData(T *data, uint32_t dataSize) {
    unsigned int remainingCapacity = capacity - size;
    if (dataSize > remainingCapacity) {
        // TODO: Use proper logging
        std::string message = "Renderer buffer out of memory.";
        std::cout << message << std::endl;
        throw std::runtime_error(message);
    }
    bind();

    unsigned int offset = size * sizeof(T);
    unsigned int dataSizeInBuffer = dataSize * sizeof(T);
    glBufferSubData(GL_ARRAY_BUFFER, offset, dataSizeInBuffer, data);

    size += dataSize;
}

template <class T>
uint32_t Buffer<T>::remainingCapacity() const {
    int remainingCapacity = capacity - size;
    return remainingCapacity;
}

template <class T>
void Buffer<T>::draw(GLenum mode) {
    vao->bind();
    program->useProgram();
    glDrawArrays(mode, 0, (GLsizei)size);
    clean();
}

template <>
void Buffer<Vertex>::enableAttributes() const {
    GLuint positionIdx = program->findProgramAttribute("position");
    glVertexAttribPointer(positionIdx, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(struct Vertex, position));
    glEnableVertexAttribArray(positionIdx);

    GLuint colorIdx = program->findProgramAttribute("color");
    glVertexAttribPointer(colorIdx, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(struct Vertex, color));
    glEnableVertexAttribArray(colorIdx);
}

template <>
void Buffer<Texel>::enableAttributes() const {
    GLuint positionIdx = program->findProgramAttribute("position");
    glVertexAttribPointer(positionIdx, 2, GL_FLOAT, GL_FALSE, sizeof(Texel), (void*)offsetof(struct Texel, position));
    glEnableVertexAttribArray(positionIdx);

    GLuint texturePositionIdx = program->findProgramAttribute("texturePosition");
    glVertexAttribPointer(texturePositionIdx, 2, GL_FLOAT, GL_FALSE, sizeof(Texel), (void*)offsetof(struct Texel, texturePosition));
    glEnableVertexAttribArray(texturePositionIdx);
}

template class Shinobu::Frontend::OpenGL::Buffer<Vertex>;
template class Shinobu::Frontend::OpenGL::Buffer<Texel>;
