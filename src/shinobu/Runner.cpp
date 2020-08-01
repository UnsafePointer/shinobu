#include "shinobu/Runner.hpp"
#include <algorithm>
#include <iostream>
#include <unistd.h>

using namespace Shinobu;

Runner::Runner() : logger(Common::Logs::Level::Message, ""), ROMFilePath(), skipBootROM(false), disassemble(false) {

}

Runner::~Runner() {

}

void Runner::configure(int argc, char* argv[]) {
    int c;
    while ((c = getopt(argc, argv, "sd")) != -1) {
        switch (c) {
        case 's':
            skipBootROM = true;
            break;
        case 'd':
            disassemble = true;
            break;
        case '?':
            if (isprint(optopt)) {
                logger.logError("Unknown option -%c", optopt);
            } else {
                logger.logError("Unknown option character %x", optopt);
            }
            break;
        default:
            logger.logError("Unexpected error while parsing options.");
        }
    }
    if (optind >= argc) {
        logger.logError("Missing argument: ROM filepath");
    }
    char *path = argv[optind];
    ROMFilePath = std::filesystem::current_path() / std::string(path);
    if (!std::filesystem::exists(ROMFilePath)) {
        logger.logError("The filepath provided as argument: %s doesn't exist.", ROMFilePath.c_str());
    }
}

Shinobu::Program::Configuration Runner::configuration() {
    Shinobu::Program::Configuration config = { ROMFilePath, skipBootROM, disassemble };
    return config;
}
