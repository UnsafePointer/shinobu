#pragma once
#include <cstdint>
#include <string>
#include <sstream>
#include <filesystem>

namespace Common {
    namespace Logs {
        enum Level : uint8_t {
            NoLog = 0,
            Warning = 1,
            Message = 2,
        };

        Level levelWithValue(std::string value);

        const uint32_t BUFFER_SIZE_LIMIT = 8192;
        const std::filesystem::path filePath = std::filesystem::current_path() / "shinobu.log";

        class Logger {
            Level level;
            std::string prefix;

            void traceMessage(std::string message) const;
            void flush() const;
        public:
            Logger(Level level, std::string prefix);
            Level logLevel();
            void logDebug(const char *fmt, ...) const;
            void logMessage(const char *fmt, ...) const;
            void logWarning(const char *fmt, ...) const;
            void logError(const char *fmt, ...) const;
        };
    };
};
