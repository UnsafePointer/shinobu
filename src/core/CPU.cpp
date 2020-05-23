#include "core/CPU.hpp"
#include <iostream>

using namespace Core;

CPU::Processor::Processor(std::unique_ptr<Memory::Controller> &memory) : registers(), memory(memory) {
}

CPU::Processor::~Processor() {
}

void CPU::Processor::pushIntoStack(uint16_t value) {
    registers.sp -= 2;
    memory->storeDoubleWord(registers.sp, value);
}

uint16_t CPU::Processor::popFromStack() {
    uint16_t value = memory->loadDoubleWord(registers.sp);
    registers.sp += 2;
    return value;
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
    processor->registers._value16[RR] = value;
    return 12;
}

uint8_t CPU::Instructions::RST_N(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t N = instruction.y * 8;
    processor->pushIntoStack(++processor->registers.pc);
    processor->registers.pc = N;
    return 16;
}

uint8_t CPU::Instructions::INC_R(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t R = Instructions::RTable[instruction.y];
    processor->registers.pc++;
    if (R != 0xFF) {
        processor->registers._value8[R]++;
        return 4;
    } else {
        uint8_t value = processor->memory->load(processor->registers.hl);
        processor->memory->store(processor->registers.hl, ++value);
        return 12;
    }
}

uint8_t CPU::Instructions::RET(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)instruction;
    uint16_t address = processor->popFromStack();
    processor->registers.pc = address;
    return 16;
}

uint8_t CPU::Instructions::LD_NN_A(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)instruction;
    uint16_t address = processor->memory->loadDoubleWord(++processor->registers.pc);
    processor->registers.pc++;
    processor->memory->store(address, processor->registers.a);
    return 16;
}

uint8_t CPU::Instructions::LD_U8(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t R = Instructions::RTable[instruction.y];
    uint8_t value = processor->memory->load(++processor->registers.pc);
    processor->registers.pc++;
    if (R != 0xFF) {
        processor->registers._value8[R] = value;
        return 8;
    } else {
        processor->memory->store(processor->registers.hl, value);
        return 12;
    }
}

uint8_t CPU::Instructions::LDH_N_A(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)instruction;
    uint8_t value = processor->memory->load(++processor->registers.pc);
    processor->registers.pc++;
    uint16_t address = 0xFF00 | value;
    processor->memory->store(address, processor->registers.a);
    return 12;
}

uint8_t CPU::Instructions::DEC_RR(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t RR = RPTable[instruction.p];
    processor->registers.pc++;
    processor->registers._value16[RR]--;
    return 8;
}

uint8_t CPU::Instructions::CALL_NN(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)instruction;
    uint16_t address = processor->memory->loadDoubleWord(++processor->registers.pc);
    processor->pushIntoStack(++processor->registers.pc);
    processor->registers.pc = address;
    return 24;
}

uint8_t CPU::Instructions::LD_R_R(std::unique_ptr<Processor> &processor, Instruction instruction) {
    processor->registers.pc++;
    uint8_t R = RTable[instruction.y];
    uint8_t R2 = RTable[instruction.z];
    if (R != 0xFF) {
        if (R2 != 0xFF) {
            processor->registers._value8[R] = processor->registers._value8[R2];
            return 4;
        } else {
            uint8_t value = processor->memory->load(processor->registers.hl);
            processor->registers._value8[R] = value;
            return 8;
        }
    } else {
        uint8_t value = processor->registers._value8[R];
        processor->memory->store(processor->registers.hl, value);
        return 8;
    }
}

uint8_t CPU::Instructions::JR_I8(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)instruction;
    int8_t value = processor->memory->load(++processor->registers.pc);
    processor->registers.pc++;
    processor->registers.pc += value;
    return 12;
}

uint8_t CPU::Instructions::LD_INDIRECT(std::unique_ptr<Processor> &processor, Instruction instruction) {
    if (instruction.q) {
        switch (instruction.p) {
        case 0:
            processor->memory->store(processor->registers.bc, processor->registers.a);
            break;
        case 1:
            processor->memory->store(processor->registers.de, processor->registers.a);
            break;
        case 2:
            processor->memory->store(processor->registers.hl, processor->registers.a);
            processor->registers.hl++;
            break;
        case 3:
            processor->memory->store(processor->registers.hl, processor->registers.a);
            processor->registers.hl--;
            break;
        }
    } else {
        switch (instruction.p) {
        case 0:
            processor->registers.a = processor->memory->load(processor->registers.bc);
            break;
        case 1:
            processor->registers.a = processor->memory->load(processor->registers.de);
            break;
        case 2:
            processor->registers.a = processor->memory->load(processor->registers.hl);
            processor->registers.hl++;
            break;
        case 3:
            processor->registers.a = processor->memory->load(processor->registers.hl);
            processor->registers.hl--;
            break;
        }
    }
    processor->registers.pc++;
    return 8;
}

uint8_t CPU::Instructions::PUSH_RR(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t RR = RP2Table[instruction.p];
    processor->pushIntoStack(processor->registers._value16[RR]);
    processor->registers.pc++;
    return 16;
}

uint8_t CPU::Instructions::POP_RR(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t RR = RP2Table[instruction.p];
    uint16_t value = processor->popFromStack();
    processor->registers._value16[RR] = value;
    processor->registers.pc++;
    return 12;
}

uint8_t CPU::Instructions::INC_RR(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t RR = RPTable[instruction.p];
    processor->registers.pc++;
    processor->registers._value16[RR]++;
    return 8;
}

uint8_t CPU::Instructions::EI(std::unique_ptr<Processor> &processor, Instruction instruction) {
    // TODO: Interrupt handling
    (void)instruction;
    processor->registers.pc++;
    return 4;
}

uint8_t CPU::Instructions::OR(std::unique_ptr<Processor> &processor, Instruction instruction) {
    processor->registers.pc++;
    if (instruction.x == 2) {
        uint8_t R = RTable[instruction.z];
        if (R != 0xFF) {
            processor->registers.a = processor->registers.a | processor->registers._value8[R];
            return 4;
        } else {
            uint8_t value = processor->memory->load(processor->registers.hl);
            processor->registers.a = processor->registers.a | value;
            return 8;
        }
    } else if (instruction.x == 3) {
        uint8_t value = processor->memory->load(processor->registers.pc);
        processor->registers.pc++;
        processor->registers.a = processor->registers.a | value;
        return 8;
    } else {
        return 0;
    }
}

uint8_t CPU::Instructions::JR_CC_I8(std::unique_ptr<Processor> &processor, Instruction instruction) {
    std::function<bool(Flag&)> compare = CCTable[instruction.y - 4];
    int8_t value = processor->memory->load(++processor->registers.pc);
    processor->registers.pc++;
    if (compare(processor->registers.flag)) {
        processor->registers.pc += value;
        return 12;
    }
    return 8;
}

uint8_t CPU::Instructions::STOP(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)instruction;
    processor->registers.pc++;
    return 0;
}

uint8_t CPU::Instructions::CALL_CC_NN(std::unique_ptr<Processor> &processor, Instruction instruction) {
    std::function<bool(Flag&)> compare = CCTable[instruction.y];
    uint16_t address = processor->memory->loadDoubleWord(++processor->registers.pc);
    if (compare(processor->registers.flag)) {
        processor->pushIntoStack(++processor->registers.pc);
        processor->registers.pc = address;
        return 24;
    }
    processor->registers.pc++;
    return 12;
}

uint8_t CPU::Instructions::ADD(std::unique_ptr<Processor> &processor, Instruction instruction) {
    processor->registers.pc++;
    if (instruction.x == 2) {
        uint8_t R = RTable[instruction.z];
        if (R != 0xFF) {
            processor->registers.a = processor->registers.a + processor->registers._value8[R];
            return 4;
        } else {
            uint8_t value = processor->memory->load(processor->registers.hl);
            processor->registers.a = processor->registers.a + value;
            return 8;
        }
    } else if (instruction.x == 3) {
        uint8_t value = processor->memory->load(processor->registers.pc);
        processor->registers.pc++;
        processor->registers.a = processor->registers.a + value;
        return 8;
    } else {
        return 0;
    }
}

uint8_t CPU::Instructions::LD_NN_SP(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)instruction;
    uint16_t address = processor->memory->loadDoubleWord(++processor->registers.pc);
    processor->registers.pc++;
    processor->memory->storeDoubleWord(address, processor->registers.sp);
    return 20;
}
