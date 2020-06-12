#include "shinobu/Emulator.hpp"
#include <iostream>
#include "common/Formatter.hpp"
#include "shinobu/Configuration.hpp"
#include "core/device/PictureProcessingUnit.hpp"

using namespace Shinobu;

Emulator::Emulator() : shouldSkipBootROM(false) {
    Configuration::Manager *configurationManager = Configuration::Manager::getInstance();

    interrupt = std::make_unique<Core::Device::Interrupt::Controller>(configurationManager->interruptLogLevel());
    timer = std::make_unique<Core::Device::Timer::Controller>(configurationManager->timerLogLevel(), interrupt);
    PPU = std::make_unique<Core::Device::PictureProcessingUnit::Processor>(configurationManager->PPULogLevel());
    cartridge = std::make_unique<Core::ROM::Cartridge>(configurationManager->ROMLogLevel());
    memoryController = std::make_unique<Core::Memory::Controller>(configurationManager->memoryLogLevel(), cartridge, PPU, interrupt, timer);
    processor = std::make_unique<Core::CPU::Processor>(configurationManager->CPULogLevel(), memoryController, interrupt);
    disassembler = std::make_unique<Core::CPU::Disassembler::Disassembler>(configurationManager->disassemblerLogLevel(), processor);
    interrupt->setProcessor(processor);
}

Emulator::~Emulator() {

}

void Emulator::setROMFilePath(std::filesystem::path &filePath) {
    cartridge->open(filePath);
}

void Emulator::setShouldSkipBootROM(bool skipBootROM) {
    shouldSkipBootROM = skipBootROM;
}

void Emulator::powerUp() {
    memoryController->initialize(shouldSkipBootROM);
    processor->initialize();
}

void Emulator::start() {
    while (true) {
        uint8_t cycles;
        Core::CPU::Instructions::Instruction instruction;
        if (!processor->isHalted()) {
            instruction = processor->fetchInstruction();
            disassembler->disassemble(instruction);
            Core::CPU::Instructions::InstructionHandler<uint8_t> handler = processor->decodeInstruction<uint8_t>(instruction);
            cycles = handler(processor, instruction);
        } else {
            instruction = Core::CPU::Instructions::Instruction(0x76, false);
            cycles = 4;
        }
        PPU->step(cycles);
        timer->step(cycles);
        processor->checkPendingInterrupts(instruction);
    }
}
