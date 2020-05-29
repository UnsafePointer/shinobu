#pragma once
#include <cstdarg>
#include <string>

namespace Common {
    namespace Formatter {
        std::string format(const char *fmt, va_list args);
        std::string format(const char *fmt, ...);
    };
};
