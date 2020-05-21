#pragma once
#include <filesystem>
#include <memory>
#include "core/CPU.hpp"
#include "core/ROM.hpp"

namespace Shinobu {
    class Emulator {
        std::unique_ptr<Core::CPU::Processor> processor;
        std::unique_ptr<Core::ROM::Cartridge> cartridge;
    public:
        Emulator();
        ~Emulator();

        void setROMFilePath(std::filesystem::path &filePath);
        void powerUp();
        void start();
    };
};
