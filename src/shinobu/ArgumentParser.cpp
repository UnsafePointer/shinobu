#include "shinobu/ArgumentParser.hpp"
#include <algorithm>
#include <iostream>
#include <unistd.h>

using namespace Shinobu::Program;

ArgumentParser::ArgumentParser() : logger(Common::Logs::Level::Message, "") {

}

ArgumentParser::~ArgumentParser() {

}

void Shinobu::Program::ArgumentParser::printUsage() const {
    logger.logDebug("Usage: shinobu [-s] [-d] [-h] filepath");
    logger.logDebug("");
    logger.logDebug("  -s   skip BOOT ROM, only supported by DMG emulation");
    logger.logDebug("  -d   disassemble, a `filepath.s` file will be created");
    logger.logDebug("  -h   print this message");
    logger.logDebug("");
}

Shinobu::Program::Configuration ArgumentParser::parse(int argc, char* argv[]) const {
    int c;
    bool skipBootROM = false;
    bool disassemble = false;
    std::filesystem::path ROMFilePath;
    while ((c = getopt(argc, argv, "sdh")) != -1) {
        switch (c) {
        case 's':
            skipBootROM = true;
            break;
        case 'd':
            disassemble = true;
            break;
        case 'h':
            printUsage();
            exit(0);
            break;
        case '?':
            printUsage();
            exit(1);
            break;
        default:
            logger.logDebug("Unexpected error while parsing options.");
            exit(1);
        }
    }
    if (optind >= argc) {
        printUsage();
        logger.logDebug("Missing argument: ROM filepath");
        exit(1);
    }
    char *path = argv[optind];
    ROMFilePath = std::filesystem::current_path() / std::string(path);
    if (!std::filesystem::exists(ROMFilePath)) {
        logger.logDebug("The filepath provided as argument: %s doesn't exist.", ROMFilePath.c_str());
        exit(1);
    }
    return { ROMFilePath, skipBootROM, disassemble };
}
