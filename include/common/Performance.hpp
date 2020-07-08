#pragma once
#include <cstdint>

namespace Common {
    namespace Performance {
        struct Frame {
            uint32_t averageFrameTime;
            uint32_t elapsedTime;
        };
    };
};
