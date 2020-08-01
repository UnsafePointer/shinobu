#pragma once
#include <string>
#include <filesystem>
#include <shinobu/Emulator.hpp>
#include <common/Logger.hpp>

namespace Shinobu {
    namespace Program {
        class ArgumentParser {
            Common::Logs::Logger logger;

            void printUsage() const;
        public:
            ArgumentParser();
            ~ArgumentParser();

            Shinobu::Program::Configuration parse(int argc, char* argv[]) const ;
        };
    };
};
