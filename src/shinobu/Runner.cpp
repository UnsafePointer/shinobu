#include "shinobu/Runner.hpp"
#include <algorithm>
#include <iostream>

using namespace Shinobu;

Runner::Runner() : ROMFilePath() {

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
            std::cout << "Incorrect arguments passed. See README.md for usage." << std::endl;
            exit(1);
        }
        ROMFilePath = std::filesystem::current_path() / std::string(path);
        if (!std::filesystem::exists(ROMFilePath)) {
            std::cout << "The filepath provided with the --rom flag doesn't exist." << std::endl;
        }
        argumentFound = true;
    }
    if (!argumentFound) {
        std::cout << "Incorrect arguments passed. See README.md for usage." << std::endl;
    }
}

void Runner::configureEmulator(Emulator &emulator) {
    emulator.setROMFilePath(ROMFilePath);
}
