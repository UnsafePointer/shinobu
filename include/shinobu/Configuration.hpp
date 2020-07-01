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
            Common::Logs::Level disassembler;
            Common::Logs::Level interrupt;
            Common::Logs::Level timer;
            Common::Logs::Level openGL;
            Common::Logs::Level joypad;
            Common::Logs::Level sound;
            bool trace;
            bool useImGuiFrontend;
            bool mute;

            Manager();
        public:
            static Manager* getInstance();

            Common::Logs::Level CPULogLevel() const;
            Common::Logs::Level memoryLogLevel() const;
            Common::Logs::Level ROMLogLevel() const;
            Common::Logs::Level PPULogLevel() const;
            Common::Logs::Level serialLogLevel() const;
            Common::Logs::Level disassemblerLogLevel() const;
            Common::Logs::Level interruptLogLevel() const;
            Common::Logs::Level timerLogLevel() const;
            Common::Logs::Level openGLLogLevel() const;
            Common::Logs::Level joypadLogLevel() const;
            Common::Logs::Level soundLogLevel() const;
            bool shouldTraceLogs() const;
            bool shouldUseImGuiFrontend() const;
            bool shouldMute() const;
            void setupConfigurationFile() const;
            void loadConfiguration();
        };
    };
};
