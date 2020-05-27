#include "shinobu/Emulator.hpp"

using namespace Shinobu;

Emulator::Emulator() : cartridge(std::make_unique<Core::ROM::Cartridge>()) {
    memoryController = std::make_unique<Core::Memory::Controller>(cartridge);
    processor = std::make_unique<Core::CPU::Processor>(memoryController);
}

Emulator::~Emulator() {

}

void Emulator::setROMFilePath(std::filesystem::path &filePath) {
    cartridge->open(filePath);
}

void Emulator::powerUp() {
    // TODO: implement power up sequence: https://gbdev.io/pandocs/#power-up-sequence
    memoryController->initialize();
    processor->initialize();
}

void Emulator::start() {
    while (true) {
        uint8_t code = processor->fetchInstruction();
        Core::CPU::Instructions::InstructionHandler handler;
        Core::CPU::Instructions::Instruction instruction;
        if (code == Core::CPU::Instructions::InstructionPrefix) {
            uint8_t prefixedCode = processor->fetchPrefixedInstruction();
            handler = processor->decodeInstruction(prefixedCode, true);
            instruction = Core::CPU::Instructions::Instruction(prefixedCode);
        } else {
            instruction = Core::CPU::Instructions::Instruction(code);
            handler = processor->decodeInstruction(code, false);
        }
        handler(processor, instruction);
    }
}
