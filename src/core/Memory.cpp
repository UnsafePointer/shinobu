#include "core/Memory.hpp"
#include <iostream>
#include "core/device/SerialCommunicationController.hpp"

using namespace Core;

Core::Memory::Range::Range(uint32_t start, uint32_t length) : start(start), length(length) {

}

Core::Memory::Range::~Range() {

}

std::optional<uint32_t> Core::Memory::Range::contains(uint32_t address) const {
    if (address >= start && address < (start + length)) {
        uint32_t offset = address - start;
        return { offset };
    } else {
        return std::nullopt;
    }
 }

Memory::BankController::BankController(std::unique_ptr<ROM::Cartridge> &cartridge) : cartridge(cartridge), WRAMBank01_N(), serialCommController(std::make_unique<Device::SerialCommunication::Controller>()) {

}

Memory::BankController::~BankController() {

}

uint8_t Memory::MBC1::Controller::load(uint16_t address) const {
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
    offset = WorkRAMBank01_N.contains(address);
    if (offset) {
        return WRAMBank01_N[*offset];
    }
    std::cout << "Unhandled MBC1 load at address: 0x" << std::hex << (unsigned int)address;
    exit(1);
    return 0;
}

void Memory::MBC1::Controller::store(uint16_t address, uint8_t value) {
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
    offset = VideoRAM.contains(address);
    if (offset) {
        std::cout << "Unhandled Video RAM write at address: 0x" << std::hex << address << " with value: 0x" << std::hex << (unsigned int)value << std::endl;
        return;
    }
    offset = WorkRAMBank01_N.contains(address);
    if (offset) {
        WRAMBank01_N[*offset] = value;
        return;
    }
    offset = I_ORegisters.contains(address);
    if (offset) {
        offset = Device::SerialCommunication::AddressRange.contains(address);
        if (offset) {
            serialCommController->store(*offset, value);
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
    std::cout << "Unhandled MBC1 store at address: 0x" << std::hex << address << " with value: 0x" << std::hex << (unsigned int)value << std::endl;
    exit(1);
}

Memory::Controller::Controller(std::unique_ptr<ROM::Cartridge> &cartridge) : cartridge(cartridge) {

}

Memory::Controller::~Controller() {

}

void Memory::Controller::initialize() {
    if (!cartridge->isOpen()) {
        std::cout << "ROM file not open, unable to initialize memory." << std::endl;
        return;
    }
    ROM::Type cartridgeType = cartridge->header.cartridgeType;
    switch (cartridgeType) {
    case ROM::MBC1:
    case ROM::MBC1_RAM:
    case ROM::MBC1_RAM_BATTERY:
        bankController = std::make_unique<MBC1::Controller>(cartridge);
        break;
    default:
        std::cout << "Unhandled cartridge type: 0x" << std::hex << (unsigned int)cartridgeType << std::endl;
        break;
    }
}

uint8_t Memory::Controller::load(uint16_t address) const {
    return bankController->load(address);
}

void Memory::Controller::store(uint16_t address, uint8_t value) {
    bankController->store(address, value);
}

uint16_t Memory::Controller::loadDoubleWord(uint16_t address) const {
    uint16_t lsb = load(address);
    uint16_t msb = load(address + 1);
    return (msb << 8) | lsb;
}

void Memory::Controller::storeDoubleWord(uint16_t address, uint16_t value) {
    uint8_t lsb = value & 0xFF;
    store(address, lsb);
    uint8_t msb = value >> 8;
    store(address + 1, msb);
}
