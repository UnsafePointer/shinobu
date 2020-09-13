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

                std::string openShaderSource(std::filesystem::path filePath) const;
                GLuint compileShader(std::string source, GLenum shaderType) const;
                GLuint linkProgram(std::vector<GLuint> shaders) const;
            public:
                Program(std::filesystem::path vertexShaderSrcPath, std::filesystem::path fragmentShaderSrcPath);
                Program(std::string vertexShaderSrc, std::string fragmentShaderSrc);
                ~Program();

                void useProgram() const;
                GLuint findProgramAttribute(std::string attribute) const;
                GLuint findProgramUniform(std::string uniform) const;
            };
        };
    };
};
