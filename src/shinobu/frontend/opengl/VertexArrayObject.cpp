#include "shinobu/frontend/opengl/VertexArrayObject.hpp"

using namespace Shinobu::Frontend::OpenGL;

VertexArrayObject::VertexArrayObject() {
    glGenVertexArrays(1, &object);
}

VertexArrayObject::~VertexArrayObject() {
    glDeleteVertexArrays(1, &object);
}

void VertexArrayObject::bind() const {
    glBindVertexArray(object);
}
