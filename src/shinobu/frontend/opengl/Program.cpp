#include "shinobu/frontend/opengl/Program.hpp"
#include <fstream>
#include "shinobu/frontend/opengl/debug/Debugger.hpp"

using namespace Shinobu::Frontend::OpenGL;

Program::Program(std::string vertexShaderSrcPath, std::string fragmentShaderSrcPath) {
    GLuint vertexShader = compileShader(vertexShaderSrcPath, GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShader(fragmentShaderSrcPath, GL_FRAGMENT_SHADER);
    program = linkProgram({vertexShader, fragmentShader});
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

Program::~Program() {
    glDeleteProgram(program);
}

void Program::useProgram() const {
    glUseProgram(program);
}

std::string Program::openShaderSource(std::string filePath) const {
    std::ifstream file(filePath);
    std::string source;

    file.seekg(0, std::ios::end);
    source.reserve(file.tellg());
    file.seekg(0, std::ios::beg);

    source.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return source;
}

GLuint Program::compileShader(std::string filePath, GLenum shaderType) const {
    std::string source = openShaderSource(filePath);
    GLuint shader = glCreateShader(shaderType);
    const GLchar *src = source.c_str();
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);
    GLint status = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        Debug::Debugger *debugger = Debug::Debugger::getInstance();
        debugger->checkForOpenGLErrors();
    }
    return shader;
}

GLuint Program::linkProgram(std::vector<GLuint> shaders) const {
    GLuint program = glCreateProgram();
    for(std::vector<GLuint>::iterator it = shaders.begin(); it != shaders.end(); ++it) {
        glAttachShader(program, *it);
    }
    glLinkProgram(program);
    GLint status = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        Debug::Debugger *debugger = Debug::Debugger::getInstance();
        debugger->checkForOpenGLErrors();
    }
    return program;
}

GLuint Program::findProgramAttribute(std::string attribute) const {
    const GLchar *attrib = attribute.c_str();
    GLint index = glGetAttribLocation(program, attrib);
    Debug::Debugger *debugger = Debug::Debugger::getInstance();
    debugger->checkForOpenGLErrors();
    return index;
}

GLuint Program::findProgramUniform(std::string uniform) const {
    const GLchar *attrib = uniform.c_str();
    GLint index = glGetUniformLocation(program, attrib);
    Debug::Debugger *debugger = Debug::Debugger::getInstance();
    debugger->checkForOpenGLErrors();
    return index;
}
