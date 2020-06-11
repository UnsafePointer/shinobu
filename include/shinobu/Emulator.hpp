#pragma once
#include <filesystem>
#include <memory>
#include "core/cpu/CPU.hpp"
#include "core/ROM.hpp"
#include "core/Memory.hpp"
#include "core/device/PictureProcessingUnit.hpp"
#include "common/Logger.hpp"
#include "core/device/Interrupt.hpp"
#include "core/device/Timer.hpp"

namespace Shinobu {
    class Emulator {
        Common::Logs::Logger disassembler;
        Common::Logs::Logger tracer;

        std::unique_ptr<Core::CPU::Processor> processor;
        std::unique_ptr<Core::ROM::Cartridge> cartridge;
        std::unique_ptr<Core::Memory::Controller> memoryController;
        std::unique_ptr<Core::Device::PictureProcessingUnit::Processor> PPU;
        std::unique_ptr<Core::Device::Interrupt::Controller> interrupt;
        std::unique_ptr<Core::Device::Timer::Controller> timer;

        bool shouldSkipBootROM;
    public:
        Emulator(Common::Logs::Level disassemblerLogLevel, Common::Logs::Level tracerLogLevel);
        ~Emulator();

        void setROMFilePath(std::filesystem::path &filePath);
        void setShouldSkipBootROM(bool skipBootROM);
        void powerUp();
        void start();
    };
};
