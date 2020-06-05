#pragma once
#include <filesystem>
#include <memory>
#include "core/cpu/CPU.hpp"
#include "core/ROM.hpp"
#include "core/Memory.hpp"
#include "core/device/PictureProcessingUnit.hpp"
#include "common/Logger.hpp"

namespace Shinobu {
    class Emulator {
        Common::Logs::Logger logger;

        std::unique_ptr<Core::CPU::Processor> processor;
        std::unique_ptr<Core::ROM::Cartridge> cartridge;
        std::unique_ptr<Core::Memory::Controller> memoryController;
        std::unique_ptr<Core::Device::PictureProcessingUnit::Processor> PPU;

        bool shouldSkipBootROM;
    public:
        Emulator(Common::Logs::Level logLevel);
        ~Emulator();

        void setROMFilePath(std::filesystem::path &filePath);
        void setShouldSkipBootROM(bool skipBootROM);
        void powerUp();
        void start();
    };
};
