#pragma once
#include "core/cpu/CPU.hpp"
#include "core/cpu/Decoding.hpp"
#include <bitset>

using namespace Core::CPU;

template<>
uint8_t Instructions::NOP(std::unique_ptr<Core::CPU::Processor> &processor, Instruction instruction) {
    processor->advanceProgramCounter(instruction);
    return 4;
}

template<>
uint8_t Instructions::JP_U16(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)instruction;
    uint16_t destinaton = processor->memory->loadDoubleWord(processor->registers.pc + 1);
    processor->registers.pc = destinaton;
    return 16;
}

template<>
uint8_t Instructions::DI(std::unique_ptr<Processor> &processor, Instruction instruction) {
    // TODO: Interrupt handling
    processor->advanceProgramCounter(instruction);
    return 4;
}

template<>
uint8_t Instructions::LD_RR_NN(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t RR = Instructions::RPTable[instruction.code.p];
    uint16_t value = processor->memory->loadDoubleWord(processor->registers.pc + 1);
    processor->registers._value16[RR] = value;
    processor->advanceProgramCounter(instruction);
    return 12;
}

template<>
uint8_t Instructions::RST_N(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t N = instruction.code.y * 8;
    processor->pushIntoStack(processor->registers.pc + 1);
    processor->registers.pc = N;
    return 16;
}

template<>
uint8_t Instructions::INC_R(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t R = Instructions::RTable[instruction.code.y];
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
    processor->advanceProgramCounter(instruction);
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
    uint16_t address = processor->memory->loadDoubleWord(processor->registers.pc + 1);
    processor->memory->store(address, processor->registers.a);
    processor->advanceProgramCounter(instruction);
    return 16;
}

template<>
uint8_t Instructions::LD_U8(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t R = Instructions::RTable[instruction.code.y];
    uint8_t value = processor->memory->load(processor->registers.pc + 1);
    uint8_t cycles;
    if (R != 0xFF) {
        processor->registers._value8[R] = value;
        cycles = 8;
    } else {
        processor->memory->store(processor->registers.hl, value);
        cycles = 12;
    }
    processor->advanceProgramCounter(instruction);
    return cycles;
}

template<>
uint8_t Instructions::LDH_N_A(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t value = processor->memory->load(processor->registers.pc + 1);
    uint16_t address = 0xFF00 | value;
    processor->memory->store(address, processor->registers.a);
    processor->advanceProgramCounter(instruction);
    return 12;
}

template<>
uint8_t Instructions::DEC_RR(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t RR = RPTable[instruction.code.p];
    processor->registers._value16[RR]--;
    processor->advanceProgramCounter(instruction);
    return 8;
}

template<>
uint8_t Instructions::CALL_NN(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint16_t address = processor->memory->loadDoubleWord(processor->registers.pc + 1);
    processor->advanceProgramCounter(instruction);
    processor->pushIntoStack(processor->registers.pc);
    processor->registers.pc = address;
    return 24;
}

template<>
uint8_t Instructions::LD_R_R(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t R = RTable[instruction.code.y];
    uint8_t R2 = RTable[instruction.code.z];
    uint8_t cycles;
    if (R != 0xFF) {
        if (R2 != 0xFF) {
            processor->registers._value8[R] = processor->registers._value8[R2];
            cycles = 4;
        } else {
            uint8_t value = processor->memory->load(processor->registers.hl);
            processor->registers._value8[R] = value;
            cycles = 8;
        }
    } else {
        uint8_t value = processor->registers._value8[R];
        processor->memory->store(processor->registers.hl, value);
        cycles = 8;
    }
    processor->advanceProgramCounter(instruction);
    return cycles;
}

template<>
uint8_t Instructions::JR_I8(std::unique_ptr<Processor> &processor, Instruction instruction) {
    int8_t value = processor->memory->load(processor->registers.pc + 1);
    processor->advanceProgramCounter(instruction);
    processor->registers.pc += value;
    return 12;
}

template<>
uint8_t Instructions::LD_INDIRECT(std::unique_ptr<Processor> &processor, Instruction instruction) {
    if (instruction.code.q) {
        switch (instruction.code.p) {
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
        switch (instruction.code.p) {
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
    processor->advanceProgramCounter(instruction);
    return 8;
}

template<>
uint8_t Instructions::PUSH_RR(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t RR = Instructions::RP2Table[instruction.code.p];
    processor->pushIntoStack(processor->registers._value16[RR]);
    processor->advanceProgramCounter(instruction);
    return 16;
}

template<>
uint8_t Instructions::POP_RR(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t RR = RP2Table[instruction.code.p];
    uint16_t value = processor->popFromStack();
    processor->registers._value16[RR] = value;
    if (RR == 0x3) {
        processor->registers.flag.zero = (value & 0xFF) & 0x80 ? 1 : 0;
        processor->registers.flag.n = (value & 0xFF) & 0x40 ? 1 : 0;
        processor->registers.flag.halfcarry = (value & 0xFF) & 0x20 ? 1 : 0;
        processor->registers.flag.carry = (value & 0xFF) & 0x10 ? 1 : 0;
    }
    processor->advanceProgramCounter(instruction);
    return 12;
}

template<>
uint8_t Instructions::INC_RR(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t RR = RPTable[instruction.code.p];
    processor->registers._value16[RR]++;
    processor->advanceProgramCounter(instruction);
    return 8;
}

template<>
uint8_t Instructions::EI(std::unique_ptr<Processor> &processor, Instruction instruction) {
    // TODO: Interrupt handling
    processor->advanceProgramCounter(instruction);
    return 4;
}

template<>
uint8_t Instructions::OR(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t cycles = processor->executeArithmetic(instruction, [](uint8_t operand1, uint8_t operand2) {
        uint8_t result = operand1 | operand2;
        Flag flags = Flag();
        flags.calculateZero(result);
        flags.n = 0;
        flags.halfcarry = 0;
        flags.carry = 0;
        return std::tuple(result, flags);
    });
    return cycles;
}

template<>
uint8_t Instructions::JR_CC_I8(std::unique_ptr<Processor> &processor, Instruction instruction) {
    std::function<bool(Flag&)> compare = CCTable[instruction.code.y - 4];
    int8_t value = processor->memory->load(processor->registers.pc + 1);
    processor->advanceProgramCounter(instruction);
    if (compare(processor->registers.flag)) {
        processor->registers.pc += value;
        return 12;
    }
    return 8;
}

template<>
uint8_t Instructions::STOP(std::unique_ptr<Processor> &processor, Instruction instruction) {
    processor->advanceProgramCounter(instruction);
    return 0;
}

template<>
uint8_t Instructions::CALL_CC_NN(std::unique_ptr<Processor> &processor, Instruction instruction) {
    std::function<bool(Flag&)> compare = CCTable[instruction.code.y];
    uint16_t address = processor->memory->loadDoubleWord(processor->registers.pc + 1);
    processor->advanceProgramCounter(instruction);
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
        flags.calculateZero(result);
        flags.n = 0;
        flags.calculateAdditionHalfCarry(operand1, operand2);
        flags.calculateAdditionCarry(operand1, operand2);
        return std::tuple(result, flags);
    });
    return cycles;
}

template<>
uint8_t Instructions::LD_NN_SP(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint16_t address = processor->memory->loadDoubleWord(processor->registers.pc + 1);
    processor->advanceProgramCounter(instruction);
    processor->memory->storeDoubleWord(address, processor->registers.sp);
    return 20;
}

template<>
uint8_t Instructions::RLCA(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t result = (processor->registers.a & 0x80) >> 7;
    processor->registers.flag.zero = 0;
    processor->registers.flag.n = 0;
    processor->registers.flag.halfcarry = 0;
    processor->registers.flag.carry = result;
    processor->registers.a <<= 1;
    processor->registers.a |= result;
    processor->advanceProgramCounter(instruction);
    return 4;
}

template<>
uint8_t Instructions::LD_A_NN(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)instruction;
    uint16_t address = processor->memory->loadDoubleWord(processor->registers.pc + 1);
    uint8_t value = processor->memory->load(address);
    processor->registers.a = value;
    processor->advanceProgramCounter(instruction);
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
        flags.calculateZero(result);
        flags.n = 1;
        flags.calculateSubtractionHalfCarry(minuend, subtrahend);
        flags.calculateSubtractionCarry(minuend, subtrahend);
        return std::tuple(result, flags);
    });
    return cycles;
}

template<>
uint8_t Instructions::DEC_R(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t R = Instructions::RTable[instruction.code.y];
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
    processor->advanceProgramCounter(instruction);
    return cycles;
}

template<>
uint8_t Instructions::XOR_A(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t cycles = processor->executeArithmetic(instruction, [](uint8_t operand1, uint8_t operand2) {
        uint8_t result = operand1 ^ operand2;
        Flag flags = Flag();
        flags.calculateZero(result);
        flags.n = 0;
        flags.halfcarry = 0;
        flags.carry = 0;
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
        flags.calculateZero(result);
        flags.n = 0;
        flags.calculateAdditionHalfCarry(operand1, operand2);
        flags.calculateAdditionCarry(operand1, operand2);
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
    uint8_t carry = processor->registers.flag.carry;
    carry <<= 7;
    uint8_t result = processor->registers.a & 0x1;
    processor->registers.flag.zero = 0;
    processor->registers.flag.n = 0;
    processor->registers.flag.halfcarry = 0;
    processor->registers.flag.carry = result;
    processor->registers.a >>= 1;
    processor->registers.a |= carry;
    processor->advanceProgramCounter(instruction);
    return 4;
}

template<>
uint8_t Instructions::RET_CC(std::unique_ptr<Processor> &processor, Instruction instruction) {
    std::function<bool(Flag&)> compare = CCTable[instruction.code.y];
    if (compare(processor->registers.flag)) {
        uint16_t address = processor->popFromStack();
        processor->registers.pc = address;
        return 20;
    }
    processor->advanceProgramCounter(instruction);
    return 8;
}

template<>
uint8_t Instructions::RLC(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t R = Instructions::RTable[instruction.code.z];
    uint8_t cycles;
    if (R != 0xFF) {
        uint8_t lastBit = (processor->registers._value8[R] & 0x80) >> 7;
        processor->registers._value8[R] <<= 1;
        processor->registers._value8[R] |= lastBit;
        processor->registers.flag.calculateZero(processor->registers._value8[R]);
        processor->registers.flag.n = 0;
        processor->registers.flag.halfcarry = 0;
        processor->registers.flag.carry = lastBit;
        cycles = 8;
    } else {
        uint8_t value = processor->memory->load(processor->registers.hl);
        uint8_t lastBit = (value & 0x80) >> 7;
        value <<= 1;
        value |= lastBit;
        processor->memory->store(processor->registers.hl, value);
        processor->registers.flag.calculateZero(value);
        processor->registers.flag.n = 0;
        processor->registers.flag.halfcarry = 0;
        processor->registers.flag.carry = lastBit;
        cycles = 16;
    }
    processor->advanceProgramCounter(instruction);
    return cycles;
}

template<>
uint8_t Instructions::CP_A(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t cycles = processor->executeArithmetic(instruction, [](uint8_t operand1, uint8_t operand2) {
        uint8_t result = operand1 - operand2;
        Flag flags = Flag();
        flags.calculateZero(result);
        flags.n = 1;
        flags.calculateSubtractionHalfCarry(operand1, operand2);
        flags.calculateSubtractionCarry(operand1, operand2);
        return std::tuple(result, flags);
    }, false);
    return cycles;
}

template<>
uint8_t Instructions::LDH_A_N(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t value = processor->memory->load(processor->registers.pc + 1);
    uint16_t address = 0xFF00 | value;
    processor->registers.a = processor->memory->load(address);
    processor->advanceProgramCounter(instruction);
    return 12;
}

template<>
uint8_t Instructions::BIT(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t R = RTable[instruction.code.z];
    uint8_t cycles;
    if (R != 0xFF) {
        std::bitset<8> bits = std::bitset<8>(processor->registers._value8[R]);
        bool isSet = bits.test(instruction.code.y);
        processor->registers.flag.zero = isSet ? 1 : 0;
        processor->registers.flag.n = 0;
        processor->registers.flag.halfcarry = 1;
        cycles = 8;
    } else {
        uint8_t value = processor->memory->load(processor->registers.hl);
        std::bitset<8> bits = std::bitset<8>(value);
        bool isSet = bits.test(instruction.code.y);
        processor->registers.flag.zero = isSet ? 1 : 0;
        processor->registers.flag.n = 0;
        processor->registers.flag.halfcarry = 1;
        cycles = 12;
    }
    processor->advanceProgramCounter(instruction);
    return cycles;
}

template<>
uint8_t Instructions::LDH_C_A(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint16_t address = 0xFF00 | processor->registers.c;
    processor->memory->store(address, processor->registers.a);
    processor->advanceProgramCounter(instruction);
    return 8;
}

template<>
uint8_t Instructions::LDH_A_C(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint16_t address = 0xFF00 | processor->registers.c;
    processor->registers.a = processor->memory->load(address);
    processor->advanceProgramCounter(instruction);
    return 8;
}

template<>
uint8_t Instructions::RL(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t R = Instructions::RTable[instruction.code.z];
    uint8_t cycles;
    if (R != 0xFF) {
        uint8_t lastBit = (processor->registers._value8[R] & 0x80) >> 7;
        uint8_t carry = processor->registers.flag.carry;
        processor->registers._value8[R] <<= 1;
        processor->registers._value8[R] |= carry;
        processor->registers.flag.calculateZero(processor->registers._value8[R]);
        processor->registers.flag.n = 0;
        processor->registers.flag.halfcarry = 0;
        processor->registers.flag.carry = lastBit;
        cycles = 8;
    } else {
        uint8_t value = processor->memory->load(processor->registers.hl);
        uint8_t lastBit = (value & 0x80) >> 7;
        uint8_t carry = processor->registers.flag.carry;
        value <<= 1;
        value |= carry;
        processor->memory->store(processor->registers.hl, value);
        processor->registers.flag.calculateZero(value);
        processor->registers.flag.n = 0;
        processor->registers.flag.halfcarry = 0;
        processor->registers.flag.carry = lastBit;
        cycles = 16;
    }
    processor->advanceProgramCounter(instruction);
    return cycles;
}

template<>
uint8_t Instructions::RLA(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t result = (processor->registers.a & 0x80) >> 7;
    uint8_t carry = processor->registers.flag.carry;
    processor->registers.flag.zero = 0;
    processor->registers.flag.n = 0;
    processor->registers.flag.halfcarry = 0;
    processor->registers.flag.carry = result;
    processor->registers.a <<= 1;
    processor->registers.a |= carry;
    processor->advanceProgramCounter(instruction);
    return 4;
}

template<>
uint8_t Instructions::SUB(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t cycles = processor->executeArithmetic(instruction, [](uint8_t operand1, uint8_t operand2) {
        uint8_t result = operand1 - operand2;
        Flag flags = Flag();
        flags.calculateZero(result);
        flags.n = 1;
        flags.calculateSubtractionHalfCarry(operand1, operand2);
        flags.calculateSubtractionCarry(operand1, operand2);
        return std::tuple(result, flags);
    });
    return cycles;
}

template<>
uint8_t Instructions::AND(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t cycles = processor->executeArithmetic(instruction, [](uint8_t operand1, uint8_t operand2) {
        uint8_t result = operand1 & operand2;
        Flag flags = Flag();
        flags.calculateZero(result);
        flags.n = 0;
        flags.halfcarry = 1;
        flags.carry = 0;
        return std::tuple(result, flags);
    });
    return cycles;
}

template<>
uint8_t Instructions::SET(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t R = RTable[instruction.code.z];
    uint8_t cycles;
    if (R != 0xFF) {
        std::bitset<8> bits = std::bitset<8>(processor->registers._value8[R]);
        bits.set(instruction.code.y);
        processor->registers._value8[R] = bits.to_ulong();
        cycles = 8;
    } else {
        uint8_t value = processor->memory->load(processor->registers.hl);
        std::bitset<8> bits = std::bitset<8>(value);
        bits.set(instruction.code.y);
        processor->memory->store(processor->registers.hl, bits.to_ulong());
        cycles = 16;
    }
    processor->advanceProgramCounter(instruction);
    return cycles;
}

template<>
uint8_t Instructions::ADD_HL_RR(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t RR = Instructions::RPTable[instruction.code.p];
    uint16_t augend = processor->registers.hl;
    uint16_t addend = processor->registers._value16[RR];
    processor->registers.hl += processor->registers._value16[RR];
    processor->registers.flag.n = 0;
    processor->registers.flag.halfcarry = ((((uint32_t)augend & 0xFFF) + ((uint32_t)addend & 0xFFF)) & 0x1000) == 0x1000;
    processor->registers.flag.carry = ((((uint32_t)augend & 0xFFFF) + ((uint32_t)addend & 0xFFFF)) & 0x10000) == 0x10000;
    processor->advanceProgramCounter(instruction);
    return 8;
}

template<>
uint8_t Instructions::RES(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t R = RTable[instruction.code.z];
    uint8_t cycles;
    if (R != 0xFF) {
        std::bitset<8> bits = std::bitset<8>(processor->registers._value8[R]);
        bits.reset(instruction.code.y);
        processor->registers._value8[R] = bits.to_ulong();
        cycles = 8;
    } else {
        uint8_t value = processor->memory->load(processor->registers.hl);
        std::bitset<8> bits = std::bitset<8>(value);
        bits.reset(instruction.code.y);
        processor->memory->store(processor->registers.hl, bits.to_ulong());
        cycles = 16;
    }
    processor->advanceProgramCounter(instruction);
    return cycles;
}

template<>
uint8_t Instructions::SRA(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t R = Instructions::RTable[instruction.code.z];
    uint8_t cycles;
    if (R != 0xFF) {
        uint8_t firstBit = (processor->registers._value8[R] & 0x1);
        processor->registers._value8[R] >>= 1;
        processor->registers.flag.calculateZero(processor->registers._value8[R]);
        processor->registers.flag.n = 0;
        processor->registers.flag.halfcarry = 0;
        processor->registers.flag.carry = firstBit;
        cycles = 8;
    } else {
        uint8_t value = processor->memory->load(processor->registers.hl);
        uint8_t firstBit = (value & 0x1);
        value >>= 1;
        processor->memory->store(processor->registers.hl, value);
        processor->registers.flag.calculateZero(value);
        processor->registers.flag.n = 0;
        processor->registers.flag.halfcarry = 0;
        processor->registers.flag.carry = firstBit;
        cycles = 16;
    }
    processor->advanceProgramCounter(instruction);
    return cycles;
}

template<>
uint8_t Instructions::SWAP(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t R = Instructions::RTable[instruction.code.z];
    uint8_t cycles;
    if (R != 0xFF) {
        uint8_t lsb = (processor->registers._value8[R] & 0x00FF);
        lsb <<= 4;
        uint8_t msb = (processor->registers._value8[R] & 0xFF00);
        msb >>= 4;
        processor->registers._value8[R] = msb | lsb;
        processor->registers.flag.calculateZero(processor->registers._value8[R]);
        processor->registers.flag.n = 0;
        processor->registers.flag.halfcarry = 0;
        processor->registers.flag.carry = 0;
        cycles = 8;
    } else {
        uint8_t value = processor->memory->load(processor->registers.hl);
        uint8_t lsb = (value & 0x00FF);
        lsb <<= 4;
        uint8_t msb = (value & 0xFF00);
        msb >>= 4;
        value = msb | lsb;
        processor->memory->store(processor->registers.hl, value);
        processor->registers.flag.calculateZero(value);
        processor->registers.flag.n = 0;
        processor->registers.flag.halfcarry = 0;
        processor->registers.flag.carry = 0;
        cycles = 16;
    }
    processor->advanceProgramCounter(instruction);
    return cycles;
}

template<>
uint8_t Instructions::JP_CC_NN(std::unique_ptr<Processor> &processor, Instruction instruction) {
    std::function<bool(Flag&)> compare = CCTable[instruction.code.y];
    uint16_t address = processor->memory->loadDoubleWord(processor->registers.pc + 1);
    processor->advanceProgramCounter(instruction);
    if (compare(processor->registers.flag)) {
        processor->registers.pc = address;
        return 16;
    }
    return 12;
}

template<>
uint8_t Instructions::LD_HL_SP_I8(std::unique_ptr<Processor> &processor, Instruction instruction) {
    int8_t value = processor->memory->load(processor->registers.pc + 1);
    processor->advanceProgramCounter(instruction);
    processor->registers.hl = processor->registers.sp + value;
    return 12;
}

template<>
uint8_t Instructions::SLA(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t R = Instructions::RTable[instruction.code.z];
    uint8_t cycles;
    if (R != 0xFF) {
        uint8_t lastBit = (processor->registers._value8[R] & 0x80) >> 7;
        processor->registers._value8[R] <<= 1;
        processor->registers.flag.calculateZero(processor->registers._value8[R]);
        processor->registers.flag.n = 0;
        processor->registers.flag.halfcarry = 0;
        processor->registers.flag.carry = lastBit;
        cycles = 8;
    } else {
        uint8_t value = processor->memory->load(processor->registers.hl);
        uint8_t lastBit = (processor->registers._value8[R] & 0x80) >> 7;
        value <<= 1;
        processor->memory->store(processor->registers.hl, value);
        processor->registers.flag.calculateZero(value);
        processor->registers.flag.n = 0;
        processor->registers.flag.halfcarry = 0;
        processor->registers.flag.carry = lastBit;
        cycles = 16;
    }
    processor->advanceProgramCounter(instruction);
    return cycles;
}

template<>
uint8_t Instructions::RR(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t R = Instructions::RTable[instruction.code.z];
    uint8_t cycles;
    if (R != 0xFF) {
        uint8_t firstBit = (processor->registers._value8[R] & 0x1);
        uint8_t carryMask = processor->registers.flag.carry << 7;
        processor->registers._value8[R] >>= 1;
        processor->registers._value8[R] |= carryMask;
        processor->registers.flag.calculateZero(processor->registers._value8[R]);
        processor->registers.flag.n = 0;
        processor->registers.flag.halfcarry = 0;
        processor->registers.flag.carry = firstBit;
        cycles = 8;
    } else {
        uint8_t value = processor->memory->load(processor->registers.hl);
        uint8_t firstBit = (processor->registers._value8[R] & 0x1);
        uint8_t carryMask = processor->registers.flag.carry << 7;
        value >>= 1;
        value |= carryMask;
        processor->memory->store(processor->registers.hl, value);
        processor->registers.flag.calculateZero(value);
        processor->registers.flag.n = 0;
        processor->registers.flag.halfcarry = 0;
        processor->registers.flag.carry = firstBit;
        cycles = 16;
    }
    processor->advanceProgramCounter(instruction);
    return cycles;
}

template<>
uint8_t Instructions::RRC(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t R = Instructions::RTable[instruction.code.z];
    uint8_t cycles;
    if (R != 0xFF) {
        uint8_t firstBit = (processor->registers._value8[R] & 0x1);
        uint8_t firstBitMask = firstBit << 7;
        processor->registers._value8[R] >>= 1;
        processor->registers._value8[R] |= firstBitMask;
        processor->registers.flag.calculateZero(processor->registers._value8[R]);
        processor->registers.flag.n = 0;
        processor->registers.flag.halfcarry = 0;
        processor->registers.flag.carry = firstBit;
        cycles = 8;
    } else {
        uint8_t value = processor->memory->load(processor->registers.hl);
        uint8_t firstBit = (processor->registers._value8[R] & 0x1);
        uint8_t firstBitMask = firstBit << 7;
        value >>= 1;
        value |= firstBitMask;
        processor->memory->store(processor->registers.hl, value);
        processor->registers.flag.calculateZero(value);
        processor->registers.flag.n = 0;
        processor->registers.flag.halfcarry = 0;
        processor->registers.flag.carry = firstBit;
        cycles = 16;
    }
    processor->advanceProgramCounter(instruction);
    return cycles;
}

template<>
uint8_t Instructions::LD_SP_HL(std::unique_ptr<Processor> &processor, Instruction instruction) {
    processor->advanceProgramCounter(instruction);
    processor->registers.sp = processor->registers.hl;
    return 8;
}

template<>
uint8_t Instructions::ADD_SP_I8(std::unique_ptr<Processor> &processor, Instruction instruction) {
    int8_t value = processor->memory->load(processor->registers.pc + 1);
    processor->advanceProgramCounter(instruction);
    processor->registers.sp += value;
    return 16;
}

template<>
uint8_t Instructions::RETI(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)instruction;
    uint16_t address = processor->popFromStack();
    processor->registers.pc = address;
    // TODO: Interrupt handling
    return 16;
}
