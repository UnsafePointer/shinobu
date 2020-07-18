#include "core/ROM.hpp"
#include <iostream>
#include <cstring>

using namespace Core::ROM;

BOOT::ROM::ROM(Common::Logs::Level logLevel) : logger(logLevel, "  [BOOTROM]: "), lockRegister(), data(), initialized(false) {

}

BOOT::ROM::~ROM() {

}

void BOOT::ROM::initialize(bool skip) {
    if (skip) {
        logger.logWarning("Skipping boot ROM");
        return;
    }
    if (!std::filesystem::exists(DEFAULT_BOOT_ROM_FILE_PATH)) {
        logger.logWarning("Couldn't find BOOT_ROM at path: %s", DEFAULT_BOOT_ROM_FILE_PATH.string().c_str());
        return;
    }
    std::ifstream bootROMFile = std::ifstream();
    bootROMFile.open(DEFAULT_BOOT_ROM_FILE_PATH, std::ios::binary | std::ios::ate);
    if (!bootROMFile.is_open()) {
        logger.logError("Unable to open BOOT_ROM at path: %s", DEFAULT_BOOT_ROM_FILE_PATH.string().c_str());
    }
    std::streampos fileSize = bootROMFile.tellg();
    logger.logMessage("Opened BOOT ROM file of size: %x", fileSize);
    bootROMFile.seekg(0, bootROMFile.beg);
    bootROMFile.read(reinterpret_cast<char *>(&data[0]), fileSize);
    bootROMFile.close();
    initialized = true;
}

uint8_t BOOT::ROM::load(uint16_t offset) const {
    return data[offset];
}

bool BOOT::ROM::isLocked() const {
    return hasBootROM() && lockRegister.BOOT_OFF == 0x0;
}

bool BOOT::ROM::hasBootROM() const {
    return initialized;
}

uint8_t BOOT::ROM::loadLockRegister() const {
    return lockRegister._value;
}

void BOOT::ROM::storeLockRegister(uint8_t value) {
    lockRegister._value = value;
    lockRegister.unused = 0x3F; // Read as 1
}

Cartridge::Cartridge(Common::Logs::Level logLevel) : logger(logLevel, "  [ROM]: "), filePath(), memory(), header() {

}

Cartridge::~Cartridge() { }

void Cartridge::open(std::filesystem::path &filePath) {
    if (!std::filesystem::exists(filePath)) {
        logger.logWarning("No ROM file set.");
        return;
    }

    std::ifstream file = std::ifstream();
    file.open(filePath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        logger.logError("Unable to load ROM file at path: %s", filePath.string().c_str());
    }
    this->filePath = filePath;

    std::streampos fileSize = file.tellg();
    logger.logMessage("Opened file path of size: %x", fileSize);

    file.seekg(HEADER_START_ADDRESS, file.beg);
    file.read(reinterpret_cast<char *>(&header), sizeof(Header));

    memory.resize(fileSize);
    file.seekg(0, file.beg);
    file.read(reinterpret_cast<char *>(&memory[0]), fileSize);

    logger.logMessage("ROM header information: ");
    logger.logMessage("Cartridge type: %x", header.cartridgeType);
    logger.logMessage("ROM Size: %x", header._ROMSize);
    logger.logMessage("RAM Size: %x", header._RAMSize);

    file.close();
}

bool Cartridge::isOpen() const {
    return !memory.empty();
}

std::filesystem::path Cartridge::saveFilePath() const {
    return std::filesystem::path(filePath).replace_extension(".sav");
}

uint8_t Cartridge::load(uint32_t address) const {
    if (address > memory.size()) {
        logger.logWarning("ROM load out of bounds with address: %04x", address);
        return 0xFF;
    }
    return memory[address];
}

uint32_t Cartridge::RAMSize() const {
    switch (header._RAMSize) {
    case RAMSize::Size::_0KB:
        return 0;
    case RAMSize::Size::_2KB:
        return 1024 * 2;
    case RAMSize::Size::_8KB:
        return 1024 * 8;
    case RAMSize::Size::_32KB:
        return 1024 * 32;
    case RAMSize::Size::_128KB:
        return 1024 * 128;
    case RAMSize::Size::_64KB:
        return 1024 * 64;
    default:
        return 0;
    }
}

uint32_t Cartridge::ROMSize() const {
    return memory.size();
}

Type Cartridge::type() const {
    return header.cartridgeType;
}
