#include "shinobu/Runner.hpp"
#include <algorithm>
#include <iostream>

using namespace Shinobu;

Runner::Runner() : logger(Common::Logs::Level::Message, ""), ROMFilePath(), skipBootROM(false) {

}

Runner::~Runner() {

}

bool Runner::checkOption(char** begin, char** end, const std::string &option) const {
    return find(begin, end, option) != end;
}

char* Runner::getOptionValue(char** begin, char** end, const std::string &option) const {
    char **iterator = find(begin, end, option);
    if (iterator != end && iterator++ != end) {
        return *iterator;
    }
    return NULL;
}

void Runner::configure(int argc, char* argv[]) {
    if (argc <= 1) {
        return;
    }
    bool argumentFound = false;
    if (checkOption(argv, argv + argc, "--rom")) {
        char *path = getOptionValue(argv, argv + argc, "--rom");
        if (path == NULL) {
            logger.logError("Incorrect arguments passed. See README.md for usage.");
        }
        ROMFilePath = std::filesystem::current_path() / std::string(path);
        if (!std::filesystem::exists(ROMFilePath)) {
            logger.logError("The filepath provided with the --rom flag doesn't exist.");
        }
        argumentFound = true;
    }
    if (checkOption(argv, argv + argc, "--skip-boot")) {
        skipBootROM = true;
        argumentFound = true;
    }
    if (!argumentFound) {
        logger.logError("Incorrect arguments passed. See README.md for usage.");
    }
}

void Runner::configureEmulator(Emulator &emulator) {
    emulator.setROMFilePath(ROMFilePath);
    emulator.setShouldSkipBootROM(skipBootROM);
}
