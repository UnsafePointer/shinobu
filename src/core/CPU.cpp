#include "core/CPU.hpp"

using namespace Core;

CPU::Processor::Processor(std::unique_ptr<Memory::Controller> &memory) : registers(), stackPointer(), programCounter(), memory(memory) {
}

CPU::Processor::~Processor() {
}

void CPU::Processor::initialize() {
    registers.af = 0x01B0;
    registers.bc = 0x0013;
    registers.de = 0x00D8;
    registers.hl = 0x014D;
    stackPointer = 0xFFFE;
    programCounter = 0x0100;
    // TODO: I/O registers
    // memory->store(0xFF05, 0x00);
    // memory->store(0xFF06, 0x00);
    // memory->store(0xFF07, 0x00);
    // memory->store(0xFF10, 0x80);
    // memory->store(0xFF11, 0xBF);
    // memory->store(0xFF12, 0xF3);
    // memory->store(0xFF14, 0xBF);
    // memory->store(0xFF16, 0x3F);
    // memory->store(0xFF17, 0x00);
    // memory->store(0xFF19, 0xBF);
    // memory->store(0xFF1A, 0x7F);
    // memory->store(0xFF1B, 0xFF);
    // memory->store(0xFF1C, 0x9F);
    // memory->store(0xFF1E, 0xBF);
    // memory->store(0xFF20, 0xFF);
    // memory->store(0xFF21, 0x00);
    // memory->store(0xFF22, 0x00);
    // memory->store(0xFF23, 0xBF);
    // memory->store(0xFF24, 0x77);
    // memory->store(0xFF25, 0xF3);
    // memory->store(0xFF26, 0xF1);
    // memory->store(0xFF40, 0x91);
    // memory->store(0xFF42, 0x00);
    // memory->store(0xFF43, 0x00);
    // memory->store(0xFF45, 0x00);
    // memory->store(0xFF47, 0xFC);
    // memory->store(0xFF48, 0xFF);
    // memory->store(0xFF49, 0xFF);
    // memory->store(0xFF4A, 0x00);
    // memory->store(0xFF4B, 0x00);
    // TODO: Interrupt Enable Register
    // memory->store(0xFFFF, 0x00);
}
