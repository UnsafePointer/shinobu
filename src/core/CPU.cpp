#include "core/CPU.hpp"

using namespace Core;

CPU::Processor::Processor() : registers(), stackPointer(), programCounter() {
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
    // TODO: memory initialization
}
