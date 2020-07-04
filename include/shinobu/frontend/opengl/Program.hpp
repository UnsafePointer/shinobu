#pragma once
#include <glad/glad.h>
#include <string>
#include <vector>
#include <filesystem>

namespace Shinobu {
    namespace Frontend {
        namespace OpenGL {
            class Program {
                GLuint program;

                std::string openShaderSource(std::filesystem::path filePath, GLenum shaderType) const;
                GLuint compileShader(std::filesystem::path, GLenum shaderType) const;
                GLuint linkProgram(std::vector<GLuint> shaders) const;
            public:
                Program(std::filesystem::path vertexShaderSrcPath, std::filesystem::path fragmentShaderSrcPath);
                ~Program();

                void useProgram() const;
                GLuint findProgramAttribute(std::string attribute) const;
                GLuint findProgramUniform(std::string uniform) const;
            };
        };
    };
};
