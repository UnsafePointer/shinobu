#pragma once
#include <string>
#include <filesystem>
#include <shinobu/Emulator.hpp>
#include <common/Logger.hpp>

namespace Shinobu {
    class Runner {
        Common::Logs::Logger logger;
        std::filesystem::path ROMFilePath;

        bool checkOption(char** begin, char** end, const std::string &option) const;
        char* getOptionValue(char** begin, char** end, const std::string &option) const;
    public:
        Runner();
        ~Runner();

        void configure(int argc, char* argv[]);
        void configureEmulator(Emulator &emulator);
    };
};
