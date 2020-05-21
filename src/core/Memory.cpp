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

uint8_t Memory::MBC1::load(uint16_t address) {
    (void)address;
    return 0;
}

void Memory::MBC1::store(uint16_t address, uint8_t value) {
    (void)address;
    (void)value;
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
        bankController = std::make_unique<MBC1>(cartridge);
        break;
    default:
        std::cout << "Unhandled cartridge type: 0x" << std::hex << cartridgeType << std::endl;
        break;
    }
}
