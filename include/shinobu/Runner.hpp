#pragma once
#include <string>
#include <filesystem>
#include <shinobu/Emulator.hpp>
#include <common/Logger.hpp>

namespace Shinobu {
    class Runner {
        Common::Logs::Logger logger;
        std::filesystem::path ROMFilePath;
        bool skipBootROM;
        bool disassemble;

    public:
        Runner();
        ~Runner();

        void configure(int argc, char* argv[]);
        void configureEmulator(Emulator &emulator);
    };
};
