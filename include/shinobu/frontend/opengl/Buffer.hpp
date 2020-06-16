#pragma once
#include <glad/glad.h>
#include <memory>
#include <vector>

namespace Shinobu {
    namespace Frontend {
        namespace OpenGL {
            class VertexArrayObject;
            class Program;

            template <class T>
            class Buffer {
                std::unique_ptr<VertexArrayObject> vao;
                GLuint vbo;
                std::unique_ptr<Program> &program;
                uint32_t capacity;
                uint32_t size;

                void enableAttributes() const;
            public:
                Buffer(std::unique_ptr<Program> &program, uint32_t capacity);
                ~Buffer();

                void bind() const;
                void clean();
                void addData(std::vector<T> data);
                uint32_t remainingCapacity() const;
                void draw(GLenum mode);
            };
        };
    };
};
