#include "core/Memory.hpp"
#include <iostream>
#include "core/device/SerialDataTransfer.hpp"
#include "core/device/PictureProcessingUnit.hpp"
#include "core/ROM.hpp"
#include "shinobu/Configuration.hpp"

using namespace Core::Memory;

Range::Range(uint32_t start, uint32_t length) : start(start), length(length) {

}

Range::~Range() {

}

std::optional<uint32_t> Range::contains(uint32_t address) const {
    if (address >= start && address < (start + length)) {
        uint32_t offset = address - start;
        return { offset };
    } else {
        return std::nullopt;
    }
 }

BankController::BankController(Common::Logs::Level logLevel, std::unique_ptr<Core::ROM::Cartridge> &cartridge, std::unique_ptr<Core::ROM::BOOT::ROM> &bootROM, std::unique_ptr<Core::Device::PictureProcessingUnit::Processor> &PPU) : logger(logLevel, "  [Memory]: "), cartridge(cartridge), bootROM(bootROM), WRAMBank00(), WRAMBank01_N(), PPU(PPU), HRAM() {
    Shinobu::Configuration::Manager *configurationManager = Shinobu::Configuration::Manager::getInstance();
    serialCommController = std::make_unique<Device::SerialDataTransfer::Controller>(configurationManager->serialLogLevel());
}

BankController::~BankController() {

}

uint8_t BankController::loadInternal(uint16_t address) const {
    std::optional<uint32_t> offset = WorkRAMBank00.contains(address);
    if (offset) {
        return WRAMBank00[*offset];
    }
    offset = WorkRAMBank01_N.contains(address);
    if (offset) {
        return WRAMBank01_N[*offset];
    }
    offset = EchoRAM.contains(address);
    if (offset) {
        return WRAMBank00[*offset];
    }
    offset = SpriteAttributeTable.contains(address);
    if (offset) {
        logger.logWarning("Unhandled Sprite attribute table load at address: %04x", address);
        return 0;
    }
    offset = NotUsable.contains(address);
    if (offset) {
        logger.logWarning("Unhandled Not Usable load at address: %04x", address);
        return 0;
    }
    offset = I_ORegisters.contains(address);
    if (offset) {
        offset = Device::SerialDataTransfer::AddressRange.contains(address);
        if (offset) {
            return serialCommController->load(*offset);
        }
        offset = Device::PictureProcessingUnit::AddressRange.contains(address);
        if (offset) {
            return PPU->load(*offset);;
        }
        offset = Core::ROM::BOOT::AddressRange.contains(address);
        if (offset) {
            return bootROM->loadLockRegister();
        }
        logger.logWarning("Unhandled I/O Register load at address: %04x", address);
        return 0;
    }
    offset = HighRAM.contains(address);
    if (offset) {
        return HRAM[*offset];
    }
    logger.logError("Unhandled load at address: %04x", address);
    return 0;
}

void BankController::storeInternal(uint16_t address, uint8_t value) {
    std::optional<uint32_t> offset = VideoRAM.contains(address);
    if (offset) {
        logger.logWarning("Unhandled Video RAM write at address: %04x with value: %02x", address, value);
        return;
    }
    offset = WorkRAMBank00.contains(address);
    if (offset) {
        WRAMBank00[*offset] = value;
        return;
    }
    offset = WorkRAMBank01_N.contains(address);
    if (offset) {
        WRAMBank01_N[*offset] = value;
        return;
    }
    offset = EchoRAM.contains(address);
    if (offset) {
        WRAMBank00[*offset] = value;
        return;
    }
    offset = I_ORegisters.contains(address);
    if (offset) {
        offset = Device::SerialDataTransfer::AddressRange.contains(address);
        if (offset) {
            serialCommController->store(*offset, value);
            return;
        }
        offset = Device::PictureProcessingUnit::AddressRange.contains(address);
        if (offset) {
            PPU->store(*offset, value);
            return;
        }
        offset = Core::ROM::BOOT::AddressRange.contains(address);
        if (offset) {
            bootROM->storeLockRegister(value);
            return;
        }
        logger.logWarning("Unhandled I/O Register write at address: %04x with value: %02x", address, value);
        return;
    }
    offset = HighRAM.contains(address);
    if (offset) {
        HRAM[*offset] = value;
        return;
    }
    offset = InterruptsEnableRegister.contains(address);
    if (offset) {
        logger.logWarning("Unhandled Interrupt Enable Register write at address: %04x with value: %02x", address, value);
        return;
    }
    logger.logError("Unhandled store at address: %04x with value: %02x", address, value);
    exit(1);
}

uint8_t ROM::Controller::load(uint16_t address) const {
    std::optional<uint32_t> offset = ROMRange.contains(address);
    if (offset) {
        return cartridge->load(*offset);
    }
    return loadInternal(address);
}

void ROM::Controller::store(uint16_t address, uint8_t value) {
    storeInternal(address, value);
    return;
}

uint8_t MBC1::Controller::load(uint16_t address) const {
    std::optional<uint32_t> offset = ROMBank00.contains(address);
    if (offset) {
        uint32_t upperMask = mode.mode ? _BANK2.bank2 << 5 : 0x0;
        uint32_t physicalAddress = (upperMask << 14) | (address & 0x1FFF);
        return cartridge->load(physicalAddress);
    }
    offset = ROMBank01_N.contains(address);
    if (offset) {
        uint32_t upperMask = _BANK2.bank2 << 5 | _BANK1.bank1;
        uint32_t physicalAddress = (upperMask << 14) | (address & 0x1FFF);
        return cartridge->load(physicalAddress);
    }
    offset = ExternalRAM.contains(address);
    if (offset) {
        uint32_t upperMask = mode.mode ? _BANK2.bank2 : 0x0;
        uint32_t physicalAddress = (upperMask << 13) | (address & 0xFFF);
        return cartridge->load(physicalAddress);
    }
    return loadInternal(address);
}

void MBC1::Controller::store(uint16_t address, uint8_t value) {
    std::optional<uint32_t> offset = RAMGRange.contains(address);
    if (offset) {
        _RAMG._value = value;
        return;
    }
    offset = BANK1Range.contains(address);
    if (offset) {
        _BANK1._value = value;
        if (_BANK1.bank1 == 0) {
            _BANK1.bank1 = 0;
        }
        return;
    }
    offset = BANK2Range.contains(address);
    if (offset) {
        _BANK2._value = value;
        return;
    }
    offset = ModeRange.contains(address);
    if (offset) {
        mode._value = value;
        return;
    }
    storeInternal(address, value);
    return;
}

Controller::Controller(Common::Logs::Level logLevel, std::unique_ptr<Core::ROM::Cartridge> &cartridge, std::unique_ptr<Core::Device::PictureProcessingUnit::Processor> &PPU) : logger(logLevel, "  [Memory]: "), cartridge(cartridge), PPU(PPU) {
    Shinobu::Configuration::Manager *configurationManager = Shinobu::Configuration::Manager::getInstance();
    bootROM = std::make_unique<Core::ROM::BOOT::ROM>(configurationManager->ROMLogLevel());
}

Controller::~Controller() {

}

void Controller::initialize(bool skipBootROM) {
    bootROM->initialize(skipBootROM);
    if (!cartridge->isOpen()) {
        logger.logWarning("ROM file not open, unable to initialize memory.");
        return;
    }
    Core::ROM::Type cartridgeType = cartridge->header.cartridgeType;
    switch (cartridgeType) {
    case Core::ROM::ROM:
        bankController = std::make_unique<ROM::Controller>(logger.logLevel(), cartridge, bootROM, PPU);
        break;
    case Core::ROM::MBC1:
    case Core::ROM::MBC1_RAM:
    case Core::ROM::MBC1_RAM_BATTERY:
        bankController = std::make_unique<MBC1::Controller>(logger.logLevel(), cartridge, bootROM, PPU);
        break;
    default:
        logger.logError("Unhandled cartridge type: %02x", cartridgeType);
        break;
    }
}

bool Controller::hasBootROM() const {
    return bootROM->hasBootROM();
}

uint8_t Controller::load(uint16_t address) const {
    if (bootROM->isLocked()) {
        std::optional<uint32_t> offset = Core::ROM::BOOT::AddressRange.contains(address);
        if (offset) {
            return bootROM->load(*offset);
        }
    }
    return bankController->load(address);
}

void Controller::store(uint16_t address, uint8_t value) {
    if (bootROM->isLocked()) {
        std::optional<uint32_t> offset = Core::ROM::BOOT::AddressRange.contains(address);
        if (offset) {
            return;
        }
    }
    bankController->store(address, value);
}

uint16_t Controller::loadDoubleWord(uint16_t address) const {
    uint16_t lsb = load(address);
    uint16_t msb = load(address + 1);
    return (msb << 8) | lsb;
}

void Controller::storeDoubleWord(uint16_t address, uint16_t value) {
    uint8_t lsb = value & 0xFF;
    store(address, lsb);
    uint8_t msb = value >> 8;
    store(address + 1, msb);
}
