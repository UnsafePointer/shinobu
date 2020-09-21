#pragma once
#include <string>

namespace Shinobu {
    namespace Configuration {
        namespace Sentry {
            class Manager {
                static Manager *instance;

                bool initialized;

                Manager();
            public:
                static Manager* getInstance();

                void initialize(std::string dsn);
                void shutdown();
            };
        };
    };
};
