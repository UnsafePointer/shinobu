#pragma once
#include <cstdint>
#include <string>
#include <sstream>

namespace Common {
    namespace Logs {
        enum Level : uint8_t {
            NoLog = 0,
            Warning = 1,
            Message = 2,
        };

        class Logger {
            Level level;
            std::string prefix;
        public:
            Logger(Level level, std::string prefix);
            void logDebug(const char *fmt, ...) const;
            void logMessage(const char *fmt, ...) const;
            void logWarning(const char *fmt, ...) const;
            void logError(const char *fmt, ...) const;
        };
    };
};
