#pragma once

namespace Common {
    namespace Logs {
        class Logger;
    };
};

namespace Shinobu {
    namespace Frontend {
        namespace SDL2 {
            void handleSDL2Error(int errorCode, Common::Logs::Logger logger);
        };
    };
};
