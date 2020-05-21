#include "shinobu/Emulator.hpp"

using namespace Shinobu;

Emulator::Emulator() : processor(std::make_unique<Core::CPU::Processor>()), cartridge(std::make_unique<Core::ROM::Cartridge>()) {

}

Emulator::~Emulator() {

}

void Emulator::setROMFilePath(std::filesystem::path &filePath) {
    cartridge->open(filePath);
    cartridge->readHeader();
}

void Emulator::powerUp() {
    // TODO: implement power up sequence: https://gbdev.io/pandocs/#power-up-sequence
    processor->initialize();
}

void Emulator::start() {
}
