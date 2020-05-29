#include "common/Logger.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdarg>
#include "common/Formatter.hpp"

using namespace Common::Logs;

Logger::Logger(Level level, std::string prefix) : level(level), prefix(prefix) {

}

void Logger::logDebug(const char *fmt, ...) const {
    va_list args;
    va_start(args, fmt);
    std::string formatted = Common::Formatter::format(fmt, args);
    va_end(args);

    formatted.insert(0, prefix);
    std::cout << formatted << std::endl;
}

void Logger::logMessage(const char *fmt, ...) const {
    if (level < Message) {
        return;
    }
    va_list args;
    va_start(args, fmt);
    std::string formatted = Common::Formatter::format(fmt, args);
    va_end(args);

    formatted.insert(0, prefix);
    std::cout << formatted << std::endl;
}

void Logger::logWarning(const char *fmt, ...) const {
    if (level < Warning) {
        return;
    }
    va_list args;
    va_start(args, fmt);
    std::string formatted = Common::Formatter::format(fmt, args);
    va_end(args);

    formatted.insert(0, prefix);
    std::cout << formatted << std::endl;
}

void Logger::logError(const char *fmt, ...) const {
    va_list args;
    va_start(args, fmt);
    std::string formatted = Common::Formatter::format(fmt, args);
    va_end(args);

    formatted.insert(0, prefix);
    std::cout << formatted << std::endl;
    exit(1);
}
