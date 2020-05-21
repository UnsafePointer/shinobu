#pragma once
#include <filesystem>
#include <memory>
#include "core/CPU.hpp"
#include "core/ROM.hpp"
#include "core/Memory.hpp"

namespace Shinobu {
    class Emulator {
        std::unique_ptr<Core::CPU::Processor> processor;
        std::unique_ptr<Core::ROM::Cartridge> cartridge;
        std::unique_ptr<Core::Memory::Controller> memoryController;
    public:
        Emulator();
        ~Emulator();

        void setROMFilePath(std::filesystem::path &filePath);
        void powerUp();
        void start();
    };
};
