#include "shinobu/ArgumentParser.hpp"
#include <algorithm>
#include <iostream>
#include <unistd.h>

using namespace Shinobu::Program;

ArgumentParser::ArgumentParser() : logger(Common::Logs::Level::Message, "") {

}

ArgumentParser::~ArgumentParser() {

}

Shinobu::Program::Configuration ArgumentParser::parse(int argc, char* argv[]) const {
    int c;
    bool skipBootROM = false;
    bool disassemble = false;
    std::filesystem::path ROMFilePath;
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
    return { ROMFilePath, skipBootROM, disassemble };
}
