#include "core/ROM.hpp"
#include <iostream>
#include <cstring>

using namespace Core::ROM;

BOOT::ROM::ROM() : lockRegister(), data() {

}

BOOT::ROM::~ROM() {

}

void BOOT::ROM::initialize() {
    if (!std::filesystem::exists(DEFAULT_BOOT_ROM_FILE_PATH)) {
        std::cout << "Couldn't find BOOT_ROM at path: " << DEFAULT_BOOT_ROM_FILE_PATH.string() << std::endl;
        return;
    }
    std::ifstream bootROMFile = std::ifstream();
    bootROMFile.open(DEFAULT_BOOT_ROM_FILE_PATH, std::ios::binary | std::ios::ate);
    if (!bootROMFile.is_open()) {
        std::cout << "Unable to open BOOT_ROM at path: " << DEFAULT_BOOT_ROM_FILE_PATH.string() << std::endl;
        exit(1);
    }
    std::streampos fileSize = bootROMFile.tellg();
    std::cout << "Opened BOOT ROM file of size: 0x" << std::hex << fileSize << std::endl;
    bootROMFile.seekg(0, bootROMFile.beg);
    bootROMFile.read(reinterpret_cast<char *>(&data[0]), fileSize);
    bootROMFile.close();
}

uint8_t BOOT::ROM::load(uint16_t offset) const {
    return data[offset];
}

bool BOOT::ROM::isLocked() const {
    return lockRegister.BOOT_OFF == 0x0;
}

uint8_t BOOT::ROM::loadLockRegister() const {
    return lockRegister._value;
}

void BOOT::ROM::storeLockRegister(uint8_t value) {
    lockRegister._value = value;
    lockRegister.unused = 0x3F; // Read as 1
}

Cartridge::Cartridge() : file(), memory(), header() {

}

Cartridge::~Cartridge() { }

void Cartridge::open(std::filesystem::path &filePath) {
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

bool Cartridge::isOpen() const {
    return !memory.empty();
}

uint8_t Cartridge::load(uint16_t address) const {
    if (address > memory.size()) {
        std::cout << "ROM load out of bounds with address: 0x" << std::hex<< address << std::endl;
        exit(1);
    }
    return memory[address];
}
