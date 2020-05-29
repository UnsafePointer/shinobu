#include "common/Logger.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdarg>
#include "common/Formatter.hpp"

using namespace Common::Logs;

Level Common::Logs::levelWithValue(std::string value) {
    if (value.compare("WAR") == 0) {
        return Level::Warning;
    } else if (value.compare("MSG") == 0) {
        return Level::Message;
    } else if (value.compare("NOLOG") == 0) {
        return Level::NoLog;
    }
    return Level::NoLog;
}

Logger::Logger(Level level, std::string prefix) : level(level), prefix(prefix) {

}

Level Logger::logLevel() {
    return level;
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
