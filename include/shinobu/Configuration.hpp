#pragma once
#include <yaml/Yaml.hpp>
#include <filesystem>
#include <common/Logger.hpp>
#include "shinobu/frontend/Renderer.hpp"

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
            Common::Logs::Level DMA;
            Shinobu::Frontend::Kind frontend;
            bool mute;
            bool launchFullscreen;
            int scale;
            int palette;
            bool overrideCGBFlag;
            bool windowLineCounter;
            std::string dmgBootstrapROM;
            std::string cgbBootstrapROM;
            bool colorCorrection;

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
            Common::Logs::Level DMALogLevel() const;
            Shinobu::Frontend::Kind frontendKind() const;
            bool shouldMute() const;
            bool shouldLaunchFullscreen() const;
            int overlayScale() const;
            int paletteIndex() const;
            bool shouldOverrideCGBFlag() const;
            bool shouldEmulateWindowLineCounter() const;
            std::string DMGBootstrapROM() const;
            std::string CGBBootstrapROM() const;
            bool shouldCorrectColors() const;
            void setupConfigurationFile() const;
            void loadConfiguration();
        };
    };
};
