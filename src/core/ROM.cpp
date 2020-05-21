#include "core/ROM.hpp"
#include <iostream>
#include <cstring>

using namespace Core;

ROM::Cartridge::Cartridge() : file(), memory(), header() {

}

ROM::Cartridge::~Cartridge() { }

void ROM::Cartridge::open(std::filesystem::path &filePath) {
    if (!std::filesystem::exists(filePath)) {
        std::cout << "No ROM file set." << std::endl;
        return;
    }

    file.open(filePath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cout << "Unable to load ROM file at path: " << filePath.string() << std::endl;
        exit(1);
    }

    std::streampos fileSize = file.tellg();
    std::cout << "Opened file path of size: 0x" << std::hex << fileSize << std::endl;

    file.seekg(HEADER_START_ADDRESS, file.beg);
    file.read(reinterpret_cast<char *>(&header), sizeof(Header));

    memory.resize(fileSize);
    file.seekg(0, file.beg);
    file.read(reinterpret_cast<char *>(&memory[0]), fileSize);

    std::cout << "ROM header information: " << std::endl;
    std::cout << "Cartridge type: 0x" << std::hex << header.cartridgeType << std::endl;
    std::cout << "ROM Size: 0x" << std::hex << header._ROMSize << std::endl;
    std::cout << "RAM Size: 0x" << std::hex << header._RAMSize << std::endl;

    file.close();
}

bool ROM::Cartridge::isOpen() const {
    return !memory.empty();
}

uint8_t ROM::Cartridge::load(uint16_t address) {
    if (address > memory.size()) {
        std::cout << "ROM load out of bounds with address: 0x" << std::hex<< address << std::endl;
        exit(1);
    }
    return memory[address];
}
