#pragma once
#include <cstdint>

namespace Shinobu {
    namespace Frontend {
        namespace Performance {
            struct Frame {
                uint32_t averageFrameTime;
                uint32_t elapsedTime;
            };
        };
    };
};
