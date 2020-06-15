#include "shinobu/frontend/opengl/Buffer.hpp"
#include <stddef.h>
#include <iostream>
#include "shinobu/frontend/opengl/Program.hpp"
#include "shinobu/frontend/opengl/VertexArrayObject.hpp"
#include "shinobu/frontend/opengl/Vertex.hpp"

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
void Buffer<T>::addData(std::vector<T> data) {
    uint remainingCapacity = capacity - size;
    if (data.size() > remainingCapacity) {
        // TODO: Use proper logging
        std::cout << "Renderer buffer out of memory." << std::endl;
        exit(1);
    }
    bind();

    uint offset = size * sizeof(T);
    uint dataSize = data.size() * sizeof(T);
    glBufferSubData(GL_ARRAY_BUFFER, offset, dataSize, data.data());

    size += data.size();
}

template <class T>
uint32_t Buffer<T>::remainingCapacity() const {
    int remainingCapacity = capacity - size;
    return remainingCapacity;
}

template <class T>
void Buffer<T>::draw() {
    vao->bind();
    program->useProgram();
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)size);
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

template class Buffer<Vertex>;
