#include "shinobu/frontend/opengl/debug/Debugger.hpp"
#include <glad/glad.h>
#include <vector>
#include <string>
#include <sstream>
#include "shinobu/Configuration.hpp"

using namespace Shinobu::Frontend::OpenGL::Debug;

Debugger::Debugger(Common::Logs::Level logLevel) : logger(logLevel, "  [OpenGL]: ") {}

Debugger* Debugger::instance = nullptr;

Debugger* Debugger::getInstance() {
    if (instance == nullptr) {
        Configuration::Manager *configurationManager = Configuration::Manager::getInstance();
        instance = new Debugger(configurationManager->openGLLogLevel());
    }
    return instance;
}

std::string Debugger::debugSourceDescription(Source source) const {
    switch (source) {
        case API: {
            return "API";
        }
        case WindowSystem: {
            return "WindowSystem";
        }
        case ShaderCompiler: {
            return "ShaderCompiler";
        }
        case ThirdParty: {
            return "ThirdParty";
        }
        case Application: {
            return "Application";
        }
        case OtherSource: {
            return "Other";
        }
        default: {
            logger.logError("Invalid source");
            return "";
        }
    }
}

std::string Debugger::debugTypeDescription(Type type) const {
    switch (type) {
        case Error: {
            return "Error";
        }
        case DeprecatedBehaviour: {
            return "DeprecatedBehaviour";
        }
        case UndefinedBehaviour: {
            return "UndefinedBehaviour";
        }
        case Portability: {
            return "Portability";
        }
        case Performance: {
            return "Performance";
        }
        case OtherType: {
            return "Other";
        }
        default: {
            logger.logError("Invalid type");
            return "";
        }
    }
}

std::string Debugger::debugSeverityDescription(Severity severity) const {
    switch (severity) {
        case High: {
            return "High";
        }
        case Medium: {
            return "Medium";
        }
        case Low: {
            return "Low";
        }
        case Notification: {
            return "Notification";
        }
        default: {
            logger.logError("Invalid severity");
            return "";
        }
    }
}

void Debugger::checkForOpenGLErrors() const {
    bool highSeverityFound = false;
    while (true) {
        std::vector<GLchar> buffer(4096);
        GLenum severity;
        GLenum source;
        GLsizei messageSize;
        GLenum type;
        GLenum id;
        GLuint count = glGetDebugMessageLog(1, (GLsizei)buffer.size(), &source, &type, &id, &severity, &messageSize, &buffer[0]);
        if (count == 0) {
            break;
        }
        buffer.resize(messageSize);
        std::string message = std::string(buffer.begin(), buffer.end());
        Source debugSource = Source(source);
        Type debugType = Type(type);
        Severity debugSeverity = Severity(severity);

        logger.logWarning("[%s] | %s | %s | %04X | %s", debugSeverityDescription(debugSeverity).c_str(),
            debugSourceDescription(debugSource).c_str(), debugTypeDescription(debugType).c_str(), id, message.c_str());
        if (debugSeverity == Severity::High) {
            highSeverityFound = true;
        }
    }
    if (highSeverityFound) {
        logger.logError("Encountered high severity OpenGL error");
    }
}
