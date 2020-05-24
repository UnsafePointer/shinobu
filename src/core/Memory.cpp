#include "core/Memory.hpp"
#include <iostream>
#include "core/device/SerialCommunicationController.hpp"
#include "core/device/PictureProcessingUnit.hpp"
#include "core/ROM.hpp"

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

BankController::BankController(std::unique_ptr<Core::ROM::Cartridge> &cartridge, std::unique_ptr<Core::ROM::BOOT::ROM> &bootROM) : cartridge(cartridge), bootROM(bootROM), WRAMBank00(), WRAMBank01_N(), serialCommController(std::make_unique<Device::SerialCommunication::Controller>()), PPU(std::make_unique<Device::PictureProcessingUnit::Processor>()) {

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
        std::cout << "Unhandled Sprite attribute table load at address: 0x" << std::hex << address << std::endl;
        return 0;
    }
    offset = NotUsable.contains(address);
    if (offset) {
        std::cout << "Unhandled Not Usable load at address: 0x" << std::hex << address << std::endl;
        return 0;
    }
    offset = I_ORegisters.contains(address);
    if (offset) {
        offset = Device::SerialCommunication::AddressRange.contains(address);
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
        std::cout << "Unhandled I/O Register load at address: 0x" << std::hex << address << std::endl;
        return 0;
    }
    offset = HighRAM.contains(address);
    if (offset) {
        std::cout << "Unhandled HRAM load at address: 0x" << std::hex << (unsigned int)address << std::endl;
        return 0;
    }
    std::cout << "Unhandled load at address: 0x" << std::hex << (unsigned int)address << std::endl;
    exit(1);
    return 0;
}

void BankController::storeInternal(uint16_t address, uint8_t value) {
    std::optional<uint32_t> offset = VideoRAM.contains(address);
    if (offset) {
        std::cout << "Unhandled Video RAM write at address: 0x" << std::hex << address << " with value: 0x" << std::hex << (unsigned int)value << std::endl;
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
        offset = Device::SerialCommunication::AddressRange.contains(address);
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
        std::cout << "Unhandled I/O Register write at address: 0x" << std::hex << address << " with value: 0x" << std::hex << (unsigned int)value << std::endl;
        return;
    }
    offset = HighRAM.contains(address);
    if (offset) {
        std::cout << "Unhandled HRAM write at address: 0x" << std::hex << address << " with value: 0x" << std::hex << (unsigned int)value << std::endl;
        return;
    }
    offset = InterruptsEnableRegister.contains(address);
    if (offset) {
        std::cout << "Unhandled Interrupt Enable Register write at address: 0x" << std::hex << address << " with value: 0x" << std::hex << (unsigned int)value << std::endl;
        return;
    }
    std::cout << "Unhandled store at address: 0x" << std::hex << address << " with value: 0x" << std::hex << (unsigned int)value << std::endl;
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

Controller::Controller(std::unique_ptr<Core::ROM::Cartridge> &cartridge) : cartridge(cartridge), bootROM(std::make_unique<Core::ROM::BOOT::ROM>()) {

}

Controller::~Controller() {

}

void Controller::initialize() {
    bootROM->initialize();
    if (!cartridge->isOpen()) {
        std::cout << "ROM file not open, unable to initialize memory." << std::endl;
        return;
    }
    Core::ROM::Type cartridgeType = cartridge->header.cartridgeType;
    switch (cartridgeType) {
    case Core::ROM::ROM:
        bankController = std::make_unique<ROM::Controller>(cartridge, bootROM);
        break;
    case Core::ROM::MBC1:
    case Core::ROM::MBC1_RAM:
    case Core::ROM::MBC1_RAM_BATTERY:
        bankController = std::make_unique<MBC1::Controller>(cartridge, bootROM);
        break;
    default:
        std::cout << "Unhandled cartridge type: 0x" << std::hex << (unsigned int)cartridgeType << std::endl;
        break;
    }
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
