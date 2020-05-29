#include "common/Formatter.hpp"
#include <cstdarg>

std::string _format(const char *fmt, va_list args) {
    char buffer[4096];

    const auto result = vsnprintf(buffer, sizeof(buffer), fmt, args);

    if (result < 0) {
        return {};
    }

    const size_t length = result;
    if (length < sizeof(buffer)) {
        return { buffer, length };
    }

    std::string formatted(length, '\0');
    vsnprintf(formatted.data(), length + 1, fmt, args);

    return formatted;
}

std::string Common::Formatter::format(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    std::string formatted = _format(fmt, args);
    va_end(args);
    return formatted;
}
