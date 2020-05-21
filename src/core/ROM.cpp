#include "core/ROM.hpp"
#include <iostream>
#include <cstring>

using namespace Core;

ROM::Cartridge::Cartridge() : file(), header() {

}

ROM::Cartridge::~Cartridge() {
    if (file.is_open()) {
        file.close();
    }
}

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
    std::streampos size = file.tellg();
    std::cout << "Opened file path of size: " << std::dec << size << std::endl;
}

bool ROM::Cartridge::isOpen() const {
    return file.is_open();
}

void ROM::Cartridge::readHeader() {
    if (!file.is_open()) {
        std::cout << "ROM file not open, unable to read the header." << std::endl;
        return;
    }
    file.seekg(HEADER_START_ADDRESS, file.beg);
    file.read(reinterpret_cast<char *>(&header), sizeof(Header));
    std::cout << "ROM header information: " << std::endl;
    std::cout << "Cartridge type: 0x" << std::hex << header.cartridgeType << std::endl;
    std::cout << "ROM Size: 0x" << std::hex << header._ROMSize << std::endl;
    std::cout << "RAM Size: 0x" << std::hex << header._RAMSize << std::endl;
}