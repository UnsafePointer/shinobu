#include "shinobu/Emulator.hpp"
#include <iostream>
#include "common/Formatter.hpp"
#include "shinobu/Configuration.hpp"
#include "core/device/PictureProcessingUnit.hpp"

using namespace Shinobu;

Emulator::Emulator(Common::Logs::Level disassemblerLogLevel, Common::Logs::Level tracerLogLevel) : disassembler(disassemblerLogLevel, "  [Disassembler]: "), tracer(tracerLogLevel, ""), shouldSkipBootROM(false) {
    Configuration::Manager *configurationManager = Configuration::Manager::getInstance();

    interrupt = std::make_unique<Core::Device::Interrupt::Controller>(configurationManager->interruptLogLevel());
    timer = std::make_unique<Core::Device::Timer::Controller>(configurationManager->timerLogLevel(), interrupt);
    PPU = std::make_unique<Core::Device::PictureProcessingUnit::Processor>(configurationManager->PPULogLevel());
    cartridge = std::make_unique<Core::ROM::Cartridge>(configurationManager->ROMLogLevel());
    memoryController = std::make_unique<Core::Memory::Controller>(configurationManager->memoryLogLevel(), cartridge, PPU, interrupt);
    processor = std::make_unique<Core::CPU::Processor>(configurationManager->CPULogLevel(), memoryController, interrupt);
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
    // TODO: implement power up sequence: https://gbdev.io/pandocs/#power-up-sequence
    memoryController->initialize(shouldSkipBootROM);
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
        disassembler.logMessage("%s%s; $%04x", disassembledInstruction.c_str(), separator.c_str(), processor->registers.pc);
        tracer.logMessage("A: %02X F: %02X B: %02X C: %02X D: %02X E: %02X H: %02X L: %02X SP: %04X PC: 00:%04X | %s",
            processor->registers.a,
            processor->registers.f,
            processor->registers.b,
            processor->registers.c,
            processor->registers.d,
            processor->registers.e,
            processor->registers.h,
            processor->registers.l,
            processor->registers.sp,
            processor->registers.pc,
            disassembledInstruction.c_str());
        uint8_t cycles = handler(processor, instruction);
        PPU->step(cycles);
        processor->checkPendingInterrupts(instruction);
    }
}
