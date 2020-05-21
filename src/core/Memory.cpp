#include "core/Memory.hpp"
#include <iostream>

using namespace Core;

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
