#pragma once
#include <yaml/Yaml.hpp>
#include <filesystem>
#include <common/Logger.hpp>

namespace Shinobu {
    namespace Configuration {

        const std::filesystem::path filePath = std::filesystem::current_path() / "shinobu.yaml";

        class Manager {
            static Manager *instance;

            Common::Logs::Logger logger;
            Common::Logs::Level CPU;
            Common::Logs::Level memory;
            Common::Logs::Level ROM;
            Common::Logs::Level PPU;
            Common::Logs::Level serial;

            Manager();
        public:
            static Manager* getInstance();

            Common::Logs::Level CPULogLevel();
            Common::Logs::Level memoryLogLevel();
            Common::Logs::Level ROMLogLevel();
            Common::Logs::Level PPULogLevel();
            Common::Logs::Level serialLogLevel();
            void setupConfigurationFile() const;
            void loadConfiguration();
        };
    };
};
