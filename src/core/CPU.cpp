#include "core/CPU.hpp"
#include <iostream>

using namespace Core;

CPU::Processor::Processor(std::unique_ptr<Memory::Controller> &memory) : registers(), memory(memory) {
}

CPU::Processor::~Processor() {
}

void CPU::Processor::initialize() {
    registers.af = 0x01B0;
    registers.bc = 0x0013;
    registers.de = 0x00D8;
    registers.hl = 0x014D;
    registers.pc = 0x0100;
    registers.sp = 0xFFFE;
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

uint8_t CPU::Processor::fetchInstruction() const {
    return memory->load(registers.pc);
}

CPU::Instructions::InstructionHandler CPU::Processor::decodeInstruction(uint8_t code) const {
    // TODO: Remove these checks once table is complete
    if (code > CPU::Instructions::instructionHandlerTable.size()) {
        std::cout << "Unhandled instruction with code: 0x" << std::hex << (unsigned int)code << std::endl;
        exit(1);
    }
    CPU::Instructions::InstructionHandler handler = CPU::Instructions::instructionHandlerTable[code];
    if (handler == nullptr) {
        std::cout << "Unhandled instruction with code: 0x" << std::hex << (unsigned int)code << std::endl;
        exit(1);
    }
    return handler;
}

uint8_t CPU::Instructions::NOP(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)instruction;
    processor->registers.pc++;
    return 4;
}

uint8_t CPU::Instructions::JP_U16(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)instruction;
    uint16_t destinaton = processor->memory->loadDoubleWord(++processor->registers.pc);
    processor->registers.pc = destinaton;
    return 16;
}

uint8_t CPU::Instructions::DI(std::unique_ptr<Processor> &processor, Instruction instruction) {
    // TODO: Interrupt handling
    (void)instruction;
    processor->registers.pc++;
    return 4;
}

uint8_t CPU::Instructions::LD_RR_NN(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t RR = Instructions::RPTable[instruction.p];
    uint16_t value = processor->memory->loadDoubleWord(++processor->registers.pc);
    processor->registers.pc++;
    processor->registers._value[RR] = value;
    return 12;
}
