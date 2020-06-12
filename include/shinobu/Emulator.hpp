#pragma once
#include <filesystem>
#include <memory>
#include "core/cpu/CPU.hpp"
#include "core/ROM.hpp"
#include "core/Memory.hpp"
#include "core/device/PictureProcessingUnit.hpp"
#include "core/device/Interrupt.hpp"
#include "core/device/Timer.hpp"
#include "core/cpu/Disassembler.hpp"

namespace Shinobu {
    class Emulator {
        std::unique_ptr<Core::CPU::Processor> processor;
        std::unique_ptr<Core::ROM::Cartridge> cartridge;
        std::unique_ptr<Core::Memory::Controller> memoryController;
        std::unique_ptr<Core::Device::PictureProcessingUnit::Processor> PPU;
        std::unique_ptr<Core::Device::Interrupt::Controller> interrupt;
        std::unique_ptr<Core::Device::Timer::Controller> timer;
        std::unique_ptr<Core::CPU::Disassembler::Disassembler> disassembler;

        bool shouldSkipBootROM;
    public:
        Emulator();
        ~Emulator();

        void setROMFilePath(std::filesystem::path &filePath);
        void setShouldSkipBootROM(bool skipBootROM);
        void powerUp();
        void start();
    };
};
