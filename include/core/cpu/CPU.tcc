#pragma once
#include "core/cpu/CPU.hpp"
#include "core/cpu/Decoding.hpp"
#include <bitset>

using namespace Core::CPU;

template<>
void Instructions::NOP(std::unique_ptr<Core::CPU::Processor> &processor, Instruction instruction) {
    processor->advanceProgramCounter(instruction);
}

template<>
void Instructions::JP_U16(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)instruction;
    uint16_t destinaton = processor->memory->loadDoubleWord(processor->registers.pc + 1);
    processor->memory->step(4);
    processor->registers.pc = destinaton;
}

template<>
void Instructions::DI(std::unique_ptr<Processor> &processor, Instruction instruction) {
    processor->shouldClearIME = true;
    processor->advanceProgramCounter(instruction);
}

template<>
void Instructions::LD_RR_NN(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t RR = Instructions::RPTable[instruction.code.p];
    uint16_t value = processor->memory->loadDoubleWord(processor->registers.pc + 1);
    processor->registers._value16[RR] = value;
    processor->advanceProgramCounter(instruction);
}

template<>
void Instructions::RST_N(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t N = instruction.code.y * 8;
    processor->memory->step(4);
    processor->pushIntoStack(processor->registers.pc + 1);
    processor->registers.pc = N;
}

template<>
void Instructions::INC_R(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t R = Instructions::RTable[instruction.code.y];
    uint8_t augend;
    uint8_t addend = 1;
    uint8_t result;
    if (R != 0xFF) {
        augend = processor->registers._value8[R];
    } else {
        augend = processor->memory->load(processor->registers.hl);
    }
    result = augend + addend;
    processor->registers.flag.calculateZero(result);
    processor->registers.flag.n = 0;
    processor->registers.flag.calculateAdditionHalfCarry(augend, addend, 0x0);
    if (R != 0xFF) {
        processor->registers._value8[R] = result;
    } else {
        processor->memory->store(processor->registers.hl, result);
    }
    processor->advanceProgramCounter(instruction);
}

template<>
void Instructions::RET(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)instruction;
    uint16_t address = processor->popFromStack();
    processor->memory->step(4);
    processor->registers.pc = address;
}

template<>
void Instructions::LD_NN_A(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint16_t address = processor->memory->loadDoubleWord(processor->registers.pc + 1);
    processor->memory->store(address, processor->registers.a);
    processor->advanceProgramCounter(instruction);
}

template<>
void Instructions::LD_U8(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t R = Instructions::RTable[instruction.code.y];
    uint8_t value = processor->memory->load(processor->registers.pc + 1);
    if (R != 0xFF) {
        processor->registers._value8[R] = value;
    } else {
        processor->memory->store(processor->registers.hl, value);
    }
    processor->advanceProgramCounter(instruction);
}

template<>
void Instructions::LDH_N_A(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t value = processor->memory->load(processor->registers.pc + 1);
    uint16_t address = 0xFF00 | value;
    processor->memory->store(address, processor->registers.a);
    processor->advanceProgramCounter(instruction);
}

template<>
void Instructions::DEC_RR(std::unique_ptr<Processor> &processor, Instruction instruction) {
    processor->memory->step(4);
    uint8_t RR = RPTable[instruction.code.p];
    processor->registers._value16[RR]--;
    processor->advanceProgramCounter(instruction);
}

template<>
void Instructions::CALL_NN(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint16_t address = processor->memory->loadDoubleWord(processor->registers.pc + 1);
    processor->advanceProgramCounter(instruction);
    processor->memory->step(4);
    processor->pushIntoStack(processor->registers.pc);
    processor->registers.pc = address;
}

template<>
void Instructions::LD_R_R(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t R = RTable[instruction.code.y];
    uint8_t R2 = RTable[instruction.code.z];
    if (R != 0xFF) {
        if (R2 != 0xFF) {
            processor->registers._value8[R] = processor->registers._value8[R2];
        } else {
            uint8_t value = processor->memory->load(processor->registers.hl);
            processor->registers._value8[R] = value;
        }
    } else {
        uint8_t value = processor->registers._value8[R2];
        processor->memory->store(processor->registers.hl, value);
    }
    processor->advanceProgramCounter(instruction);
}

template<>
void Instructions::JR_I8(std::unique_ptr<Processor> &processor, Instruction instruction) {
    int8_t value = processor->memory->load(processor->registers.pc + 1);
    processor->memory->step(4);
    processor->advanceProgramCounter(instruction);
    processor->registers.pc += value;
}

template<>
void Instructions::LD_INDIRECT(std::unique_ptr<Processor> &processor, Instruction instruction) {
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
}

template<>
void Instructions::PUSH_RR(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t RR = Instructions::RP2Table[instruction.code.p];
    processor->memory->step(4);
    processor->pushIntoStack(processor->registers._value16[RR]);
    processor->advanceProgramCounter(instruction);
}

template<>
void Instructions::POP_RR(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t RR = RP2Table[instruction.code.p];
    uint16_t value = processor->popFromStack();
    if (RR == 0x0) {
        processor->registers.a = (value & 0xFF00) >> 8;
        processor->registers.flag.zero = (value & 0xFF) & 0x80 ? 1 : 0;
        processor->registers.flag.n = (value & 0xFF) & 0x40 ? 1 : 0;
        processor->registers.flag.halfcarry = (value & 0xFF) & 0x20 ? 1 : 0;
        processor->registers.flag.carry = (value & 0xFF) & 0x10 ? 1 : 0;
    } else {
        processor->registers._value16[RR] = value;
    }
    processor->advanceProgramCounter(instruction);
}

template<>
void Instructions::INC_RR(std::unique_ptr<Processor> &processor, Instruction instruction) {
    processor->memory->step(4);
    uint8_t RR = RPTable[instruction.code.p];
    processor->registers._value16[RR]++;
    processor->advanceProgramCounter(instruction);
}

template<>
void Instructions::EI(std::unique_ptr<Processor> &processor, Instruction instruction) {
    processor->shouldSetIME = true;
    processor->advanceProgramCounter(instruction);
}

template<>
void Instructions::OR(std::unique_ptr<Processor> &processor, Instruction instruction) {
    processor->executeArithmetic(instruction, [](uint8_t operand1, uint8_t operand2) {
        uint8_t result = operand1 | operand2;
        Flag flags = Flag();
        flags.calculateZero(result);
        flags.n = 0;
        flags.halfcarry = 0;
        flags.carry = 0;
        return std::tuple(result, flags);
    });
}

template<>
void Instructions::JR_CC_I8(std::unique_ptr<Processor> &processor, Instruction instruction) {
    std::function<bool(Flag&)> compare = CCTable[instruction.code.y - 4];
    int8_t value = processor->memory->load(processor->registers.pc + 1);
    processor->advanceProgramCounter(instruction);
    if (compare(processor->registers.flag)) {
        processor->memory->step(4);
        processor->registers.pc += value;
    }
}

template<>
void Instructions::STOP(std::unique_ptr<Processor> &processor, Instruction instruction) {
    processor->advanceProgramCounter(instruction);
    processor->memory->handleSpeedSwitch();
}

template<>
void Instructions::CALL_CC_NN(std::unique_ptr<Processor> &processor, Instruction instruction) {
    std::function<bool(Flag&)> compare = CCTable[instruction.code.y];
    uint16_t address = processor->memory->loadDoubleWord(processor->registers.pc + 1);
    processor->advanceProgramCounter(instruction);
    if (compare(processor->registers.flag)) {
        processor->memory->step(4);
        processor->pushIntoStack(processor->registers.pc);
        processor->registers.pc = address;
    }
}

template<>
void Instructions::ADD(std::unique_ptr<Processor> &processor, Instruction instruction) {
    processor->executeArithmetic(instruction, [](uint8_t operand1, uint8_t operand2) {
        uint8_t result = operand1 + operand2;
        Flag flags = Flag();
        flags.calculateZero(result);
        flags.n = 0;
        flags.calculateAdditionHalfCarry(operand1, operand2, 0x0);
        flags.calculateAdditionCarry(operand1, operand2, 0x0);
        return std::tuple(result, flags);
    });
}

template<>
void Instructions::LD_NN_SP(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint16_t address = processor->memory->loadDoubleWord(processor->registers.pc + 1);
    processor->advanceProgramCounter(instruction);
    processor->memory->storeDoubleWord(address, processor->registers.sp);
}

template<>
void Instructions::RLCA(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t result = (processor->registers.a & 0x80) >> 7;
    processor->registers.flag.zero = 0;
    processor->registers.flag.n = 0;
    processor->registers.flag.halfcarry = 0;
    processor->registers.flag.carry = result;
    processor->registers.a <<= 1;
    processor->registers.a |= result;
    processor->advanceProgramCounter(instruction);
}

template<>
void Instructions::LD_A_NN(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)instruction;
    uint16_t address = processor->memory->loadDoubleWord(processor->registers.pc + 1);
    uint8_t value = processor->memory->load(address);
    processor->registers.a = value;
    processor->advanceProgramCounter(instruction);
}

template<>
void Instructions::SBC_A(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t carry = processor->registers.flag.carry;
    processor->executeArithmetic(instruction, [carry](uint8_t minuend, uint8_t subtrahend) {
        uint8_t result = minuend - (subtrahend + carry);
        Flag flags = Flag();
        flags.calculateZero(result);
        flags.n = 1;
        flags.calculateSubtractionHalfCarry(minuend, subtrahend, carry);
        flags.calculateSubtractionCarry(minuend, subtrahend, carry);
        return std::tuple(result, flags);
    });
}

template<>
void Instructions::DEC_R(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t R = Instructions::RTable[instruction.code.y];
    uint8_t minuend;
    uint8_t subtrahend = 1;
    uint8_t result;
    if (R != 0xFF) {
        minuend = processor->registers._value8[R];
    } else {
        minuend = processor->memory->load(processor->registers.hl);
    }
    result = minuend - subtrahend;
    processor->registers.flag.calculateZero(result);
    processor->registers.flag.n = 1;
    processor->registers.flag.calculateSubtractionHalfCarry(minuend, subtrahend, 0x0);
    if (R != 0xFF) {
        processor->registers._value8[R] = result;
    } else {
        processor->memory->store(processor->registers.hl, result);
    }
    processor->advanceProgramCounter(instruction);
}

template<>
void Instructions::XOR_A(std::unique_ptr<Processor> &processor, Instruction instruction) {
    processor->executeArithmetic(instruction, [](uint8_t operand1, uint8_t operand2) {
        uint8_t result = operand1 ^ operand2;
        Flag flags = Flag();
        flags.calculateZero(result);
        flags.n = 0;
        flags.halfcarry = 0;
        flags.carry = 0;
        return std::tuple(result, flags);
    });
}

template<>
void Instructions::ADC_A(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t carry = processor->registers.flag.carry;
    processor->executeArithmetic(instruction, [carry](uint8_t operand1, uint8_t operand2) {
        uint8_t result = operand1 + operand2 + carry;
        Flag flags = Flag();
        flags.calculateZero(result);
        flags.n = 0;
        flags.calculateAdditionHalfCarry(operand1, operand2, carry);
        flags.calculateAdditionCarry(operand1, operand2, carry);
        return std::tuple(result, flags);
    });
}

template<>
void Instructions::JP_HL(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)instruction;
    processor->registers.pc = processor->registers.hl;
}

template<>
void Instructions::RRA(std::unique_ptr<Processor> &processor, Instruction instruction) {
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
}

template<>
void Instructions::RET_CC(std::unique_ptr<Processor> &processor, Instruction instruction) {
    std::function<bool(Flag&)> compare = CCTable[instruction.code.y];
    processor->memory->step(4);
    if (compare(processor->registers.flag)) {
        uint16_t address = processor->popFromStack();
        processor->memory->step(4);
        processor->registers.pc = address;
        return;
    }
    processor->advanceProgramCounter(instruction);
}

template<>
void Instructions::RLC(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t R = Instructions::RTable[instruction.code.z];
    if (R != 0xFF) {
        uint8_t lastBit = (processor->registers._value8[R] & 0x80) >> 7;
        processor->registers._value8[R] <<= 1;
        processor->registers._value8[R] |= lastBit;
        processor->registers.flag.calculateZero(processor->registers._value8[R]);
        processor->registers.flag.n = 0;
        processor->registers.flag.halfcarry = 0;
        processor->registers.flag.carry = lastBit;
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
    }
    processor->advanceProgramCounter(instruction);
}

template<>
void Instructions::CP_A(std::unique_ptr<Processor> &processor, Instruction instruction) {
    processor->executeArithmetic(instruction, [](uint8_t operand1, uint8_t operand2) {
        uint8_t result = operand1 - operand2;
        Flag flags = Flag();
        flags.calculateZero(result);
        flags.n = 1;
        flags.calculateSubtractionHalfCarry(operand1, operand2, 0x0);
        flags.calculateSubtractionCarry(operand1, operand2, 0x0);
        return std::tuple(result, flags);
    }, false);
}

template<>
void Instructions::LDH_A_N(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t value = processor->memory->load(processor->registers.pc + 1);
    uint16_t address = 0xFF00 | value;
    processor->registers.a = processor->memory->load(address);
    processor->advanceProgramCounter(instruction);
}

template<>
void Instructions::BIT(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t R = RTable[instruction.code.z];
    if (R != 0xFF) {
        std::bitset<8> bits = std::bitset<8>(processor->registers._value8[R]);
        bool isSet = bits.test(instruction.code.y);
        processor->registers.flag.zero = isSet ? 0 : 1;
        processor->registers.flag.n = 0;
        processor->registers.flag.halfcarry = 1;
    } else {
        uint8_t value = processor->memory->load(processor->registers.hl);
        std::bitset<8> bits = std::bitset<8>(value);
        bool isSet = bits.test(instruction.code.y);
        processor->registers.flag.zero = isSet ? 0 : 1;
        processor->registers.flag.n = 0;
        processor->registers.flag.halfcarry = 1;
    }
    processor->advanceProgramCounter(instruction);
}

template<>
void Instructions::LDH_C_A(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint16_t address = 0xFF00 | processor->registers.c;
    processor->memory->store(address, processor->registers.a);
    processor->advanceProgramCounter(instruction);
}

template<>
void Instructions::LDH_A_C(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint16_t address = 0xFF00 | processor->registers.c;
    processor->registers.a = processor->memory->load(address);
    processor->advanceProgramCounter(instruction);
}

template<>
void Instructions::RL(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t R = Instructions::RTable[instruction.code.z];
    if (R != 0xFF) {
        uint8_t lastBit = (processor->registers._value8[R] & 0x80) >> 7;
        uint8_t carry = processor->registers.flag.carry;
        processor->registers._value8[R] <<= 1;
        processor->registers._value8[R] |= carry;
        processor->registers.flag.calculateZero(processor->registers._value8[R]);
        processor->registers.flag.n = 0;
        processor->registers.flag.halfcarry = 0;
        processor->registers.flag.carry = lastBit;
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
    }
    processor->advanceProgramCounter(instruction);
}

template<>
void Instructions::RLA(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t result = (processor->registers.a & 0x80) >> 7;
    uint8_t carry = processor->registers.flag.carry;
    processor->registers.flag.zero = 0;
    processor->registers.flag.n = 0;
    processor->registers.flag.halfcarry = 0;
    processor->registers.flag.carry = result;
    processor->registers.a <<= 1;
    processor->registers.a |= carry;
    processor->advanceProgramCounter(instruction);
}

template<>
void Instructions::SUB(std::unique_ptr<Processor> &processor, Instruction instruction) {
    processor->executeArithmetic(instruction, [](uint8_t operand1, uint8_t operand2) {
        uint8_t result = operand1 - operand2;
        Flag flags = Flag();
        flags.calculateZero(result);
        flags.n = 1;
        flags.calculateSubtractionHalfCarry(operand1, operand2, 0x0);
        flags.calculateSubtractionCarry(operand1, operand2, 0x0);
        return std::tuple(result, flags);
    });
}

template<>
void Instructions::AND(std::unique_ptr<Processor> &processor, Instruction instruction) {
    processor->executeArithmetic(instruction, [](uint8_t operand1, uint8_t operand2) {
        uint8_t result = operand1 & operand2;
        Flag flags = Flag();
        flags.calculateZero(result);
        flags.n = 0;
        flags.halfcarry = 1;
        flags.carry = 0;
        return std::tuple(result, flags);
    });
}

template<>
void Instructions::SET(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t R = RTable[instruction.code.z];
    if (R != 0xFF) {
        std::bitset<8> bits = std::bitset<8>(processor->registers._value8[R]);
        bits.set(instruction.code.y);
        processor->registers._value8[R] = bits.to_ulong();
    } else {
        uint8_t value = processor->memory->load(processor->registers.hl);
        std::bitset<8> bits = std::bitset<8>(value);
        bits.set(instruction.code.y);
        processor->memory->store(processor->registers.hl, bits.to_ulong());
    }
    processor->advanceProgramCounter(instruction);
}

template<>
void Instructions::ADD_HL_RR(std::unique_ptr<Processor> &processor, Instruction instruction) {
    processor->memory->step(4);
    uint8_t RR = Instructions::RPTable[instruction.code.p];
    uint16_t augend = processor->registers.hl;
    uint16_t addend = processor->registers._value16[RR];
    processor->registers.hl += processor->registers._value16[RR];
    processor->registers.flag.n = 0;
    processor->registers.flag.halfcarry = ((((uint32_t)augend & 0xFFF) + ((uint32_t)addend & 0xFFF)) & 0x1000) == 0x1000;
    processor->registers.flag.carry = ((((uint32_t)augend & 0xFFFF) + ((uint32_t)addend & 0xFFFF)) & 0x10000) == 0x10000;
    processor->advanceProgramCounter(instruction);
}

template<>
void Instructions::RES(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t R = RTable[instruction.code.z];
    if (R != 0xFF) {
        std::bitset<8> bits = std::bitset<8>(processor->registers._value8[R]);
        bits.reset(instruction.code.y);
        processor->registers._value8[R] = bits.to_ulong();
    } else {
        uint8_t value = processor->memory->load(processor->registers.hl);
        std::bitset<8> bits = std::bitset<8>(value);
        bits.reset(instruction.code.y);
        processor->memory->store(processor->registers.hl, bits.to_ulong());
    }
    processor->advanceProgramCounter(instruction);
}

template<>
void Instructions::SRA(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t R = Instructions::RTable[instruction.code.z];
    if (R != 0xFF) {
        uint8_t lastBitMask = processor->registers._value8[R] & 0x80;
        uint8_t firstBit = (processor->registers._value8[R] & 0x1);
        processor->registers._value8[R] >>= 1;
        processor->registers._value8[R] |= lastBitMask;
        processor->registers.flag.calculateZero(processor->registers._value8[R]);
        processor->registers.flag.n = 0;
        processor->registers.flag.halfcarry = 0;
        processor->registers.flag.carry = firstBit;
    } else {
        uint8_t value = processor->memory->load(processor->registers.hl);
        uint8_t lastBitMask = value & 0x80;
        uint8_t firstBit = (value & 0x1);
        value >>= 1;
        value |= lastBitMask;
        processor->memory->store(processor->registers.hl, value);
        processor->registers.flag.calculateZero(value);
        processor->registers.flag.n = 0;
        processor->registers.flag.halfcarry = 0;
        processor->registers.flag.carry = firstBit;
    }
    processor->advanceProgramCounter(instruction);
}

template<>
void Instructions::SWAP(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t R = Instructions::RTable[instruction.code.z];
    if (R != 0xFF) {
        uint8_t lsb = (processor->registers._value8[R] & 0x0F);
        lsb <<= 4;
        uint8_t msb = (processor->registers._value8[R] & 0xF0);
        msb >>= 4;
        processor->registers._value8[R] = msb | lsb;
        processor->registers.flag.calculateZero(processor->registers._value8[R]);
        processor->registers.flag.n = 0;
        processor->registers.flag.halfcarry = 0;
        processor->registers.flag.carry = 0;
    } else {
        uint8_t value = processor->memory->load(processor->registers.hl);
        uint8_t lsb = (value & 0x0F);
        lsb <<= 4;
        uint8_t msb = (value & 0xF0);
        msb >>= 4;
        value = msb | lsb;
        processor->memory->store(processor->registers.hl, value);
        processor->registers.flag.calculateZero(value);
        processor->registers.flag.n = 0;
        processor->registers.flag.halfcarry = 0;
        processor->registers.flag.carry = 0;
    }
    processor->advanceProgramCounter(instruction);
}

template<>
void Instructions::JP_CC_NN(std::unique_ptr<Processor> &processor, Instruction instruction) {
    std::function<bool(Flag&)> compare = CCTable[instruction.code.y];
    uint16_t address = processor->memory->loadDoubleWord(processor->registers.pc + 1);
    processor->advanceProgramCounter(instruction);
    if (compare(processor->registers.flag)) {
        processor->memory->step(4);
        processor->registers.pc = address;
        return;
    }
}

template<>
void Instructions::LD_HL_SP_I8(std::unique_ptr<Processor> &processor, Instruction instruction) {
    int8_t value = processor->memory->load(processor->registers.pc + 1);
    processor->memory->step(4);
    processor->advanceProgramCounter(instruction);
    uint16_t result = processor->registers.sp + value;
    processor->registers.flag.zero = 0;
    processor->registers.flag.n = 0;
    if (value >= 0) {
        processor->registers.flag.calculateAdditionHalfCarry(processor->registers.sp, value, 0x0);
        processor->registers.flag.calculateAdditionCarry(processor->registers.sp, value, 0x0);
    } else {
        processor->registers.flag.calculateSubtractionHalfCarry(result, processor->registers.sp, 0x0);
        processor->registers.flag.calculateSubtractionCarry(result, processor->registers.sp, 0x0);
    }
    processor->registers.hl = result;
}

template<>
void Instructions::SLA(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t R = Instructions::RTable[instruction.code.z];
    if (R != 0xFF) {
        uint8_t lastBit = (processor->registers._value8[R] & 0x80) >> 7;
        processor->registers._value8[R] <<= 1;
        processor->registers.flag.calculateZero(processor->registers._value8[R]);
        processor->registers.flag.n = 0;
        processor->registers.flag.halfcarry = 0;
        processor->registers.flag.carry = lastBit;
    } else {
        uint8_t value = processor->memory->load(processor->registers.hl);
        uint8_t lastBit = (value & 0x80) >> 7;
        value <<= 1;
        processor->memory->store(processor->registers.hl, value);
        processor->registers.flag.calculateZero(value);
        processor->registers.flag.n = 0;
        processor->registers.flag.halfcarry = 0;
        processor->registers.flag.carry = lastBit;
    }
    processor->advanceProgramCounter(instruction);
}

template<>
void Instructions::RR(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t R = Instructions::RTable[instruction.code.z];
    if (R != 0xFF) {
        uint8_t firstBit = (processor->registers._value8[R] & 0x1);
        uint8_t carryMask = processor->registers.flag.carry << 7;
        processor->registers._value8[R] >>= 1;
        processor->registers._value8[R] |= carryMask;
        processor->registers.flag.calculateZero(processor->registers._value8[R]);
        processor->registers.flag.n = 0;
        processor->registers.flag.halfcarry = 0;
        processor->registers.flag.carry = firstBit;
    } else {
        uint8_t value = processor->memory->load(processor->registers.hl);
        uint8_t firstBit = (value & 0x1);
        uint8_t carryMask = processor->registers.flag.carry << 7;
        value >>= 1;
        value |= carryMask;
        processor->memory->store(processor->registers.hl, value);
        processor->registers.flag.calculateZero(value);
        processor->registers.flag.n = 0;
        processor->registers.flag.halfcarry = 0;
        processor->registers.flag.carry = firstBit;
    }
    processor->advanceProgramCounter(instruction);
}

template<>
void Instructions::RRC(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t R = Instructions::RTable[instruction.code.z];
    if (R != 0xFF) {
        uint8_t firstBit = (processor->registers._value8[R] & 0x1);
        uint8_t firstBitMask = firstBit << 7;
        processor->registers._value8[R] >>= 1;
        processor->registers._value8[R] |= firstBitMask;
        processor->registers.flag.calculateZero(processor->registers._value8[R]);
        processor->registers.flag.n = 0;
        processor->registers.flag.halfcarry = 0;
        processor->registers.flag.carry = firstBit;
    } else {
        uint8_t value = processor->memory->load(processor->registers.hl);
        uint8_t firstBit = (value & 0x1);
        uint8_t firstBitMask = firstBit << 7;
        value >>= 1;
        value |= firstBitMask;
        processor->memory->store(processor->registers.hl, value);
        processor->registers.flag.calculateZero(value);
        processor->registers.flag.n = 0;
        processor->registers.flag.halfcarry = 0;
        processor->registers.flag.carry = firstBit;
    }
    processor->advanceProgramCounter(instruction);
}

template<>
void Instructions::LD_SP_HL(std::unique_ptr<Processor> &processor, Instruction instruction) {
    processor->memory->step(4);
    processor->advanceProgramCounter(instruction);
    processor->registers.sp = processor->registers.hl;
}

template<>
void Instructions::ADD_SP_I8(std::unique_ptr<Processor> &processor, Instruction instruction) {
    int8_t value = processor->memory->load(processor->registers.pc + 1);
    processor->memory->step(8);
    processor->advanceProgramCounter(instruction);
    uint16_t result = processor->registers.sp + value;
    processor->registers.flag.zero = 0;
    processor->registers.flag.n = 0;
    if (value >= 0) {
        processor->registers.flag.calculateAdditionHalfCarry(processor->registers.sp, value, 0x0);
        processor->registers.flag.calculateAdditionCarry(processor->registers.sp, value, 0x0);
    } else {
        processor->registers.flag.calculateSubtractionHalfCarry(result, processor->registers.sp, 0x0);
        processor->registers.flag.calculateSubtractionCarry(result, processor->registers.sp, 0x0);
    }
    processor->registers.sp = result;
}

template<>
void Instructions::RETI(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)instruction;
    uint16_t address = processor->popFromStack();
    processor->memory->step(4);
    processor->registers.pc = address;
    processor->interrupt->updateIME(true);
}

template<>
void Instructions::DAA(std::unique_ptr<Processor> &processor, Instruction instruction) {
    if (!processor->registers.flag.n) {
        if (processor->registers.flag.carry || processor->registers.a > 0x99) {
            processor->registers.a += 0x60;
            processor->registers.flag.carry = 1;
        }
        if (processor->registers.flag.halfcarry || (processor->registers.a & 0x0f) > 0x09) {
            processor->registers.a += 0x6;
        }
    } else {
        if (processor->registers.flag.carry) {
            processor->registers.a -= 0x60;
        }
        if (processor->registers.flag.halfcarry) {
            processor->registers.a -= 0x6;
        }
    }
    processor->registers.flag.calculateZero(processor->registers.a);
    processor->registers.flag.halfcarry = 0;
    processor->advanceProgramCounter(instruction);
}

template<>
void Instructions::CPL(std::unique_ptr<Processor> &processor, Instruction instruction) {
    processor->registers.a = ~processor->registers.a;
    processor->registers.flag.n = 1;
    processor->registers.flag.halfcarry = 1;
    processor->advanceProgramCounter(instruction);
}

template<>
void Instructions::SCF(std::unique_ptr<Processor> &processor, Instruction instruction) {
    processor->registers.flag.n = 0;
    processor->registers.flag.halfcarry = 0;
    processor->registers.flag.carry = 1;
    processor->advanceProgramCounter(instruction);
}

template<>
void Instructions::CCF(std::unique_ptr<Processor> &processor, Instruction instruction) {
    processor->registers.flag.n = 0;
    processor->registers.flag.halfcarry = 0;
    processor->registers.flag.carry = !processor->registers.flag.carry;
    processor->advanceProgramCounter(instruction);
}

template<>
void Instructions::RRCA(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t lastBit = processor->registers.a & 0x1;
    uint8_t lastBitMask = lastBit << 7;
    processor->registers.a >>= 1;
    processor->registers.a |= lastBitMask;
    processor->registers.flag.zero = 0;
    processor->registers.flag.n = 0;
    processor->registers.flag.halfcarry = 0;
    processor->registers.flag.carry = lastBit;
    processor->advanceProgramCounter(instruction);
}

template<>
void Instructions::SRL(std::unique_ptr<Processor> &processor, Instruction instruction) {
    uint8_t R = Instructions::RTable[instruction.code.z];
    if (R != 0xFF) {
        uint8_t firstBit = (processor->registers._value8[R] & 0x1);
        processor->registers._value8[R] >>= 1;
        processor->registers.flag.calculateZero(processor->registers._value8[R]);
        processor->registers.flag.n = 0;
        processor->registers.flag.halfcarry = 0;
        processor->registers.flag.carry = firstBit;
    } else {
        uint8_t value = processor->memory->load(processor->registers.hl);
        uint8_t firstBit = (value & 0x1);
        value >>= 1;
        processor->memory->store(processor->registers.hl, value);
        processor->registers.flag.calculateZero(value);
        processor->registers.flag.n = 0;
        processor->registers.flag.halfcarry = 0;
        processor->registers.flag.carry = firstBit;
    }
    processor->advanceProgramCounter(instruction);
}

template<>
void Instructions::HALT(std::unique_ptr<Processor> &processor, Instruction instruction) {
    processor->halted = true;
    processor->advanceProgramCounter(instruction);
}

template<>
void Instructions::HALTED(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)instruction;
    processor->memory->step(4);
}
