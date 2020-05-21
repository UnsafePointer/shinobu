#include "core/Memory.hpp"
#include <iostream>

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

Memory::BankController::BankController(std::unique_ptr<ROM::Cartridge> &cartridge) : cartridge(cartridge) {

}

Memory::BankController::~BankController() {

}

uint8_t Memory::MBC1::Controller::load(uint16_t address) {
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
    std::cout << "Unhandled MBC1 load at address: 0x" << std::hex << address;
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
    std::cout << "Unhandled MBC1 store at address: 0x" << std::hex << address << " with value: 0x" << std::hex << value;
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
        std::cout << "Unhandled cartridge type: 0x" << std::hex << cartridgeType << std::endl;
        break;
    }
}
