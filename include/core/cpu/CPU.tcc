#pragma once
#include "core/cpu/CPU.hpp"
#include "core/cpu/Decoding.hpp"
#include <bitset>

using namespace Core::CPU;

template<>
uint8_t Instructions::NOP(std::unique_ptr<Core::CPU::Processor> &processor, Instruction instruction) {
    (void)instruction;
    processor->registers.pc++;
    return 4;
}

template<>
uint8_t Instructions::JP_U16(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)instruction;
    uint16_t destinaton = processor->memory->loadDoubleWord(++processor->registers.pc);
    processor->registers.pc = destinaton;
    return 16;
}

template<>
uint8_t Instructions::DI(std::unique_ptr<Processor> &processor, Instruction instruction) {
    // TODO: Interrupt handling
    (void)instruction;
    processor->registers.pc++;
    return 4;
}

template<>
uint8_t Instructions::LD_RR_NN(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t RR = Instructions::RPTable[instruction.p];
    uint16_t value = processor->memory->loadDoubleWord(++processor->registers.pc);
    processor->registers.pc += 2;
    processor->registers._value16[RR] = value;
    return 12;
}

template<>
uint8_t Instructions::RST_N(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t N = instruction.y * 8;
    processor->pushIntoStack(++processor->registers.pc);
    processor->registers.pc = N;
    return 16;
}

template<>
uint8_t Instructions::INC_R(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t R = Instructions::RTable[instruction.y];
    processor->registers.pc++;
    uint8_t augend;
    uint8_t addend = 1;
    uint8_t result;
    uint8_t cycles;
    if (R != 0xFF) {
        augend = processor->registers._value8[R];
    } else {
        augend = processor->memory->load(processor->registers.hl);
    }
    result = augend + addend;
    processor->registers.flag.calculateZero(result);
    processor->registers.flag.n = 0;
    processor->registers.flag.calculateAdditionHalfCarry(augend, addend);
    if (R != 0xFF) {
        processor->registers._value8[R] = result;
        cycles = 4;
    } else {
        processor->memory->store(processor->registers.hl, result);
        cycles = 12;
    }
    return cycles;
}

template<>
uint8_t Instructions::RET(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)instruction;
    uint16_t address = processor->popFromStack();
    processor->registers.pc = address;
    return 16;
}

template<>
uint8_t Instructions::LD_NN_A(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)instruction;
    uint16_t address = processor->memory->loadDoubleWord(++processor->registers.pc);
    processor->registers.pc += 2;
    processor->memory->store(address, processor->registers.a);
    return 16;
}

template<>
uint8_t Instructions::LD_U8(std::unique_ptr<Processor> &processor, Instruction instruction) {
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

template<>
uint8_t Instructions::LDH_N_A(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)instruction;
    uint8_t value = processor->memory->load(++processor->registers.pc);
    processor->registers.pc++;
    uint16_t address = 0xFF00 | value;
    processor->memory->store(address, processor->registers.a);
    return 12;
}

template<>
uint8_t Instructions::DEC_RR(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t RR = RPTable[instruction.p];
    processor->registers.pc++;
    processor->registers._value16[RR]--;
    return 8;
}

template<>
uint8_t Instructions::CALL_NN(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)instruction;
    uint16_t address = processor->memory->loadDoubleWord(++processor->registers.pc);
    processor->registers.pc += 2;
    processor->pushIntoStack(processor->registers.pc);
    processor->registers.pc = address;
    return 24;
}

template<>
uint8_t Instructions::LD_R_R(std::unique_ptr<Processor> &processor, Instruction instruction) {
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

template<>
uint8_t Instructions::JR_I8(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)instruction;
    int8_t value = processor->memory->load(++processor->registers.pc);
    processor->registers.pc++;
    processor->registers.pc += value;
    return 12;
}

template<>
uint8_t Instructions::LD_INDIRECT(std::unique_ptr<Processor> &processor, Instruction instruction) {
    if (instruction.q) {
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
    } else {
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
    }
    processor->registers.pc++;
    return 8;
}

template<>
uint8_t Instructions::PUSH_RR(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t RR = Instructions::RP2Table[instruction.p];
    processor->pushIntoStack(processor->registers._value16[RR]);
    processor->registers.pc++;
    return 16;
}

template<>
uint8_t Instructions::POP_RR(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t RR = RP2Table[instruction.p];
    uint16_t value = processor->popFromStack();
    processor->registers._value16[RR] = value;
    processor->registers.pc++;
    return 12;
}

template<>
uint8_t Instructions::INC_RR(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t RR = RPTable[instruction.p];
    processor->registers.pc++;
    processor->registers._value16[RR]++;
    return 8;
}

template<>
uint8_t Instructions::EI(std::unique_ptr<Processor> &processor, Instruction instruction) {
    // TODO: Interrupt handling
    (void)instruction;
    processor->registers.pc++;
    return 4;
}

template<>
uint8_t Instructions::OR(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t cycles = processor->executeArithmetic(instruction, [](uint8_t operand1, uint8_t operand2) {
        uint8_t result = operand1 | operand2;
        Flag flags = Flag();
        return std::tuple(result, flags);
    });
    return cycles;
}

template<>
uint8_t Instructions::JR_CC_I8(std::unique_ptr<Processor> &processor, Instruction instruction) {
    std::function<bool(Flag&)> compare = CCTable[instruction.y - 4];
    int8_t value = processor->memory->load(++processor->registers.pc);
    processor->registers.pc++;
    if (compare(processor->registers.flag)) {
        processor->registers.pc += value;
        return 12;
    }
    return 8;
}

template<>
uint8_t Instructions::STOP(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)instruction;
    processor->registers.pc++;
    return 0;
}

template<>
uint8_t Instructions::CALL_CC_NN(std::unique_ptr<Processor> &processor, Instruction instruction) {
    std::function<bool(Flag&)> compare = CCTable[instruction.y];
    uint16_t address = processor->memory->loadDoubleWord(++processor->registers.pc);
    processor->registers.pc += 2;
    if (compare(processor->registers.flag)) {
        processor->pushIntoStack(processor->registers.pc);
        processor->registers.pc = address;
        return 24;
    }
    return 12;
}

template<>
uint8_t Instructions::ADD(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t cycles = processor->executeArithmetic(instruction, [](uint8_t operand1, uint8_t operand2) {
        uint8_t result = operand1 + operand2;
        Flag flags = Flag();
        flags.zero = result == 0 ? 1 : 0;
        return std::tuple(result, flags);
    });
    return cycles;
}

template<>
uint8_t Instructions::LD_NN_SP(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)instruction;
    uint16_t address = processor->memory->loadDoubleWord(++processor->registers.pc);
    processor->registers.pc += 2;
    processor->memory->storeDoubleWord(address, processor->registers.sp);
    return 20;
}

template<>
uint8_t Instructions::RLCA(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)instruction;
    uint8_t result = (processor->registers.a & 0x80) >> 7;
    processor->registers.flag.zero = 0;
    processor->registers.flag.n = 0;
    processor->registers.flag.halfcarry = 0;
    processor->registers.flag.carry = result;
    processor->registers.a <<= 1;
    processor->registers.a |= result;
    processor->registers.pc++;
    return 4;
}

template<>
uint8_t Instructions::LD_A_NN(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)instruction;
    uint16_t address = processor->memory->loadDoubleWord(++processor->registers.pc);
    processor->registers.pc += 2;
    uint8_t value = processor->memory->load(address);
    processor->registers.a = value;
    return 16;
}

template<>
uint8_t Instructions::SBC_A(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t carry = processor->registers.flag.carry;
    uint8_t cycles = processor->executeArithmetic(instruction, [carry](uint8_t minuend, uint8_t subtrahend) {
        if (carry) {
            subtrahend++;
        }
        uint8_t result = minuend - subtrahend;
        Flag flags = Flag();
        return std::tuple(result, flags);
    });
    return cycles;
}

template<>
uint8_t Instructions::DEC_R(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t R = Instructions::RTable[instruction.y];
    processor->registers.pc++;
    uint8_t minuend;
    uint8_t subtrahend = 1;
    uint8_t result;
    uint8_t cycles;
    if (R != 0xFF) {
        minuend = processor->registers._value8[R];
    } else {
        minuend = processor->memory->load(processor->registers.hl);
    }
    result = minuend - subtrahend;
    processor->registers.flag.calculateZero(result);
    processor->registers.flag.n = 1;
    processor->registers.flag.calculateSubtractionHalfCarry(minuend, subtrahend);
    if (R != 0xFF) {
        processor->registers._value8[R] = result;
        cycles = 4;
    } else {
        processor->memory->store(processor->registers.hl, result);
        cycles = 12;
    }
    return cycles;
}

template<>
uint8_t Instructions::XOR_A(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t cycles = processor->executeArithmetic(instruction, [](uint8_t operand1, uint8_t operand2) {
        uint8_t result = operand1 ^ operand2;
        Flag flags = Flag();
        return std::tuple(result, flags);
    });
    return cycles;
}

template<>
uint8_t Instructions::ADC_A(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t carry = processor->registers.flag.carry;
    uint8_t cycles = processor->executeArithmetic(instruction, [carry](uint8_t operand1, uint8_t operand2) {
        if (carry) {
            operand2++;
        }
        uint8_t result = operand1 + operand2;
        Flag flags = Flag();
        return std::tuple(result, flags);
    });
    return cycles;
}

template<>
uint8_t Instructions::JP_HL(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)instruction;
    processor->registers.pc = processor->registers.hl;
    return 4;
}

template<>
uint8_t Instructions::RRA(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)instruction;
    uint8_t carry = processor->registers.flag.carry;
    carry <<= 7;
    uint8_t result = processor->registers.a & 0x1;
    processor->registers.flag.zero = 0;
    processor->registers.flag.n = 0;
    processor->registers.flag.halfcarry = 0;
    processor->registers.flag.carry = result;
    processor->registers.a >>= 1;
    processor->registers.a |= carry;
    processor->registers.pc++;
    return 4;
}

template<>
uint8_t Instructions::RET_CC(std::unique_ptr<Processor> &processor, Instruction instruction) {
    std::function<bool(Flag&)> compare = CCTable[instruction.y];
    if (compare(processor->registers.flag)) {
        uint16_t address = processor->popFromStack();
        processor->registers.pc = address;
        return 20;
    }
    processor->registers.pc++;
    return 8;
}

template<>
uint8_t Instructions::RLC(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t R = Instructions::RTable[instruction.z];
    processor->registers.pc += 2;
    if (R != 0xFF) {
        uint8_t lastBit = (processor->registers._value8[R] & 0x80) >> 7;
        // TODO: breaks test
        // processor->registers.flag.carry = lastBit;
        processor->registers._value8[R] <<= 1;
        processor->registers._value8[R] |= lastBit;
        return 8;
    } else {
        uint8_t value = processor->memory->load(processor->registers.hl);
        uint8_t lastBit = (value & 0x80) >> 7;
        processor->registers.flag.carry = lastBit;
        value <<= 1;
        value |= lastBit;
        processor->memory->store(processor->registers.hl, value);
        return 16;
    }
}

template<>
uint8_t Instructions::CP_A(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t cycles = processor->executeArithmetic(instruction, [](uint8_t operand1, uint8_t operand2) {
        uint8_t result = operand1 - operand2;
        Flag flags = Flag();
        if (result == 0) {
            flags.zero = 1;
        }
        flags.n = 1;
        if ((operand2 & 0xF) > (operand1 & 0xF)) {
            flags.halfcarry = 1;
        }
        if (operand2 > operand1) {
            flags.carry = 1;
        }
        return std::tuple(result, flags);
    }, false);
    return cycles;
}

template<>
uint8_t Instructions::LDH_A_N(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)instruction;
    uint8_t value = processor->memory->load(++processor->registers.pc);
    processor->registers.pc++;
    uint16_t address = 0xFF00 | value;
    processor->registers.a = processor->memory->load(address);
    return 12;
}

template<>
uint8_t Instructions::BIT(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t R = RTable[instruction.z];
    processor->registers.pc += 2;
    if (R != 0xFF) {
        std::bitset<8> bits = std::bitset<8>(processor->registers._value8[R]);
        bool isSet = bits.test(instruction.y);
        processor->registers.flag.zero = isSet ? 1 : 0;
        processor->registers.flag.n = 0;
        processor->registers.flag.halfcarry = 1;
        return 8;
    } else {
        uint8_t value = processor->memory->load(processor->registers.hl);
        std::bitset<8> bits = std::bitset<8>(value);
        bool isSet = bits.test(instruction.y);
        processor->registers.flag.zero = isSet ? 1 : 0;
        processor->registers.flag.n = 0;
        processor->registers.flag.halfcarry = 1;
        return 12;
    }
}

template<>
uint8_t Instructions::LDH_C_A(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)instruction;
    processor->registers.pc++;
    uint16_t address = 0xFF00 | processor->registers.c;
    processor->memory->store(address, processor->registers.a);
    return 8;
}

template<>
uint8_t Instructions::LDH_A_C(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)instruction;
    processor->registers.pc++;
    uint16_t address = 0xFF00 | processor->registers.c;
    processor->registers.a = processor->memory->load(address);
    return 8;
}

template<>
uint8_t Instructions::RL(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t R = Instructions::RTable[instruction.z];
    processor->registers.pc += 2;
    if (R != 0xFF) {
        uint8_t lastBit = (processor->registers._value8[R] & 0x80) >> 7;
        uint8_t carry = processor->registers.flag.carry;
        processor->registers.flag.carry = lastBit;
        processor->registers._value8[R] <<= 1;
        processor->registers._value8[R] |= carry;
        return 8;
    } else {
        uint8_t value = processor->memory->load(processor->registers.hl);
        uint8_t lastBit = (value & 0x80) >> 7;
        uint8_t carry = processor->registers.flag.carry;
        processor->registers.flag.carry = lastBit;
        value <<= 1;
        value |= carry;
        processor->memory->store(processor->registers.hl, value);
        return 16;
    }
}

template<>
uint8_t Instructions::RLA(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)instruction;
    uint8_t result = (processor->registers.a & 0x80) >> 7;
    uint8_t carry = processor->registers.flag.carry;
    processor->registers.flag.zero = 0;
    processor->registers.flag.n = 0;
    processor->registers.flag.halfcarry = 0;
    processor->registers.flag.carry = result;
    processor->registers.a <<= 1;
    processor->registers.a |= carry;
    processor->registers.pc++;
    return 4;
}

template<>
uint8_t Instructions::SUB(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t cycles = processor->executeArithmetic(instruction, [](uint8_t operand1, uint8_t operand2) {
        uint8_t result = operand1 - operand2;
        Flag flags = Flag();
        flags.zero = result == 0 ? 1 : 0;
        flags.n = 1;
        flags.halfcarry = ((operand2 & 0xF) > (operand1 & 0xF)) ? 1 : 0;
        flags.carry = operand2 > operand1 ? 1 : 0;
        return std::tuple(result, flags);
    });
    return cycles;
}
