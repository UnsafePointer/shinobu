#include "shinobu/Emulator.hpp"
#include <iostream>
#include "common/Formatter.hpp"
#include "shinobu/Configuration.hpp"
#include "core/device/PictureProcessingUnit.hpp"

using namespace Shinobu;

Emulator::Emulator(Common::Logs::Level logLevel) : logger(logLevel, "  [Disassembler]: ") {
    Configuration::Manager *configurationManager = Configuration::Manager::getInstance();

    PPU = std::make_unique<Core::Device::PictureProcessingUnit::Processor>(configurationManager->PPULogLevel());
    cartridge = std::make_unique<Core::ROM::Cartridge>(configurationManager->ROMLogLevel());
    memoryController = std::make_unique<Core::Memory::Controller>(configurationManager->memoryLogLevel(), cartridge, PPU);
    processor = std::make_unique<Core::CPU::Processor>(configurationManager->CPULogLevel(), memoryController);
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
        Core::CPU::Instructions::InstructionHandler<uint8_t> handler;
        Core::CPU::Instructions::InstructionHandler<std::string> disassemblerHandler;
        Core::CPU::Instructions::Instruction instruction;
        if (code == Core::CPU::Instructions::InstructionPrefix) {
            uint8_t prefixedCode = processor->fetchPrefixedInstruction();
            handler = processor->decodeInstruction<uint8_t>(prefixedCode, true);
            disassemblerHandler = processor->decodeInstruction<std::string>(prefixedCode, true);
            instruction = Core::CPU::Instructions::Instruction(prefixedCode, true);
        } else {
            instruction = Core::CPU::Instructions::Instruction(code, false);
            handler = processor->decodeInstruction<uint8_t>(code, false);
            disassemblerHandler = processor->decodeInstruction<std::string>(code, false);
        }
        std::string disassembledInstruction = disassemblerHandler(processor, instruction);
        std::string separator = std::string(20 - disassembledInstruction.length(), ' ');
        logger.logMessage("%s%s; $%04x", disassembledInstruction.c_str(), separator.c_str(), processor->programCounter());
        uint8_t cycles = handler(processor, instruction);
        PPU->step(cycles);
    }
}
