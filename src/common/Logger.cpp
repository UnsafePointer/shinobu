#include "common/Logger.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdarg>
#include "common/Formatter.hpp"
#include "shinobu/Configuration.hpp"
#include <stdexcept>

using namespace Common::Logs;

std::stringstream stream = std::stringstream();
uint16_t bufferSize = 0;

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

void Logger::flush() const {
    std::ofstream logfile = std::ofstream();
    logfile.open(filePath, std::ios::out | std::ios::app);
    logfile << stream.str();
    logfile.close();
    stream.str(std::string());
    bufferSize = 0;
}

void Logger::traceMessage(std::string message) const {
    stream << message << std::endl;
    bufferSize += message.length();
    if (bufferSize < BUFFER_SIZE_LIMIT) {
        return;
    }
    flush();
}

void Logger::logDebug(const char *fmt, ...) const {
    va_list args;
    va_start(args, fmt);
    std::string formatted = Common::Formatter::format(fmt, args);
    va_end(args);

    formatted.insert(0, prefix);
    std::cout << formatted << std::endl;
    traceMessage(formatted);
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
    traceMessage(formatted);
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
    traceMessage(formatted);
}

void Logger::logError(const char *fmt, ...) const {
    va_list args;
    va_start(args, fmt);
    std::string formatted = Common::Formatter::format(fmt, args);
    va_end(args);

    formatted.insert(0, prefix);
    std::cout << formatted << std::endl;
    traceMessage(formatted);
    flush();
    throw std::runtime_error(formatted);
}
