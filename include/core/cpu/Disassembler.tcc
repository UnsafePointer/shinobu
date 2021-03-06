#pragma once
#include "core/cpu/CPU.hpp"
#include "core/cpu/Decoding.hpp"
#include "common/Formatter.hpp"

using namespace Core::CPU;

template<>
std::string Instructions::NOP(std::unique_ptr<Core::CPU::Processor> &processor, Instruction instruction) {
    (void)instruction;
    (void)processor;
    return "NOP";
}

template<>
std::string Instructions::JP_U16(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)instruction;
    uint16_t destinaton = processor->memory->loadDoubleWord(processor->registers.pc + 1, false);
    return Common::Formatter::format("JP $%04x", destinaton);
}

template<>
std::string Instructions::DI(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)processor;
    (void)instruction;
    return "DI";
}

template<>
std::string Instructions::LD_RR_NN(std::unique_ptr<Processor> &processor, Instruction instruction) {
    std::string RR = Disassembler::RPTable[instruction.code.p];
    uint16_t value = processor->memory->loadDoubleWord(processor->registers.pc + 1,false);
    return Common::Formatter::format("LD %s,$%04x", RR.c_str(), value);
}

template<>
std::string Instructions::RST_N(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)processor;
    uint8_t N = instruction.code.y * 8;
    return Common::Formatter::format("RET $%04x", N);
}

template<>
std::string Instructions::INC_R(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)processor;
    (void)instruction;
    std::string R = Disassembler::RTable[instruction.code.y];
    return Common::Formatter::format("INC %s", R.c_str());
}

template<>
std::string Instructions::RET(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)processor;
    (void)instruction;
    return "RET";
}

template<>
std::string Instructions::LD_NN_A(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)instruction;
    uint16_t address = processor->memory->loadDoubleWord(processor->registers.pc + 1, false);
    return Common::Formatter::format("LD ($%04x),A", address);
}

template<>
std::string Instructions::LD_U8(std::unique_ptr<Processor> &processor, Instruction instruction) {
    std::string R = Disassembler::RTable[instruction.code.y];
    uint8_t value = processor->memory->load(processor->registers.pc + 1, false);
    return Common::Formatter::format("LD %s,$%02x", R.c_str(), value);
}

template<>
std::string Instructions::LDH_N_A(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)instruction;
    uint8_t value = processor->memory->load(processor->registers.pc + 1, false);
    return Common::Formatter::format("LD ($FF00+$%02x),A", value);
}

template<>
std::string Instructions::DEC_RR(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)processor;
    (void)instruction;
    std::string RR = Disassembler::RPTable[instruction.code.p];
    return Common::Formatter::format("DEC %s", RR.c_str());
}

template<>
std::string Instructions::CALL_NN(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)instruction;
    uint16_t address = processor->memory->loadDoubleWord(processor->registers.pc + 1, false);
    return Common::Formatter::format("CALL $%04x", address);
}

template<>
std::string Instructions::LD_R_R(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)processor;
    std::string R = Disassembler::RTable[instruction.code.y];
    std::string R2 = Disassembler::RTable[instruction.code.z];
    return Common::Formatter::format("LD %s,%s", R.c_str(), R2.c_str());
}

template<>
std::string Instructions::JR_I8(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)instruction;
    int8_t value = processor->memory->load(processor->registers.pc + 1, false);
    uint16_t destination = processor->registers.pc + 2 + value;
    return Common::Formatter::format("JR $%04x", destination);
}

template<>
std::string Instructions::LD_INDIRECT(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)processor;
    if (instruction.code.q) {
        switch (instruction.code.p) {
        case 0:
            return Common::Formatter::format("LD A,(BC)");
        case 1:
            return Common::Formatter::format("LD A,(DE)");
        case 2:
            return Common::Formatter::format("LD A,(HL+)");
        case 3:
            return Common::Formatter::format("LD A,(HL-)");
        }
    } else {
        switch (instruction.code.p) {
        case 0:
            return Common::Formatter::format("LD (BC),A");
        case 1:
            return Common::Formatter::format("LD (DE),A");
        case 2:
            return Common::Formatter::format("LD (HL+),A");
        case 3:
            return Common::Formatter::format("LD (HL-),A");
        }
    }
    return "";
}

template<>
std::string Instructions::PUSH_RR(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)processor;
    std::string RR = Disassembler::RP2Table[instruction.code.p];
    return Common::Formatter::format("PUSH %s", RR.c_str());
}

template<>
std::string Instructions::POP_RR(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)processor;
    (void)instruction;
    std::string RR = Disassembler::RP2Table[instruction.code.p];
    return Common::Formatter::format("POP %s", RR.c_str());
}

template<>
std::string Instructions::INC_RR(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)processor;
    std::string RR = Disassembler::RPTable[instruction.code.p];
    return Common::Formatter::format("INC %s", RR.c_str());
}

template<>
std::string Instructions::EI(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)processor;
    (void)instruction;
    return "EI";
}

template<>
std::string Instructions::OR(std::unique_ptr<Processor> &processor, Instruction instruction) {
    return processor->disassembleArithmetic(instruction, "OR");
}

template<>
std::string Instructions::JR_CC_I8(std::unique_ptr<Processor> &processor, Instruction instruction) {
    std::string compare = Disassembler::CCTable[instruction.code.y - 4];
    int8_t immediate = processor->memory->load(processor->registers.pc + 1, false);
    uint16_t destinationAddress = processor->registers.pc + immediate + 2;
    return Common::Formatter::format("JR %s,$%04x", compare.c_str(), destinationAddress);
}

template<>
std::string Instructions::STOP(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)processor;
    (void)instruction;
    return "STOP";
}

template<>
std::string Instructions::CALL_CC_NN(std::unique_ptr<Processor> &processor, Instruction instruction) {
    std::string compare = Disassembler::CCTable[instruction.code.y];
    uint16_t destination = processor->memory->loadDoubleWord(processor->registers.pc + 1, false);
    return Common::Formatter::format("CALL %s,$%04x", compare.c_str(), destination);
}

template<>
std::string Instructions::ADD(std::unique_ptr<Processor> &processor, Instruction instruction) {
    return processor->disassembleArithmetic(instruction, "ADD");
}

template<>
std::string Instructions::LD_NN_SP(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)instruction;
    uint16_t address = processor->memory->loadDoubleWord(processor->registers.pc + 1, false);
    return Common::Formatter::format("LD ($%04x),SP", address);
}

template<>
std::string Instructions::RLCA(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)processor;
    (void)instruction;
    return "RLCA";
}

template<>
std::string Instructions::LD_A_NN(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)instruction;
    uint16_t address = processor->memory->loadDoubleWord(processor->registers.pc + 1, false);
    return Common::Formatter::format("LD A,$%04x", address);
}

template<>
std::string Instructions::SBC_A(std::unique_ptr<Processor> &processor, Instruction instruction) {
    return processor->disassembleArithmetic(instruction, "SBC");
}

template<>
std::string Instructions::DEC_R(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)processor;
    (void)instruction;
    std::string R = Disassembler::RTable[instruction.code.y];
    return Common::Formatter::format("DEC %s", R.c_str());
}

template<>
std::string Instructions::XOR_A(std::unique_ptr<Processor> &processor, Instruction instruction) {
    return processor->disassembleArithmetic(instruction, "XOR");
}

template<>
std::string Instructions::ADC_A(std::unique_ptr<Processor> &processor, Instruction instruction) {
    return processor->disassembleArithmetic(instruction, "ADC");
}

template<>
std::string Instructions::JP_HL(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)instruction;
    return Common::Formatter::format("JP $%04x", processor->registers.hl);
}

template<>
std::string Instructions::RRA(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)processor;
    (void)instruction;
    return "RRA";
}

template<>
std::string Instructions::RET_CC(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)processor;
    std::string compare = Disassembler::CCTable[instruction.code.y];
    return Common::Formatter::format("RET %s", compare.c_str());
}

template<>
std::string Instructions::RLC(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)processor;
    std::string R = Disassembler::RTable[instruction.code.z];
    return Common::Formatter::format("RLC %s", R.c_str());
}

template<>
std::string Instructions::CP_A(std::unique_ptr<Processor> &processor, Instruction instruction) {
    return processor->disassembleArithmetic(instruction, "CP");
}

template<>
std::string Instructions::LDH_A_N(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)instruction;
    uint8_t value = processor->memory->load(processor->registers.pc + 1, false);
    return Common::Formatter::format("LD A,($FF00+$%02x)", value);
}

template<>
std::string Instructions::BIT(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)processor;
    std::string R = Disassembler::RTable[instruction.code.z];
    return Common::Formatter::format("BIT %d,%s", instruction.code.y, R.c_str());
}

template<>
std::string Instructions::LDH_C_A(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)processor;
    (void)instruction;
    return "LD ($FF00+C),A";
}

template<>
std::string Instructions::LDH_A_C(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)processor;
    (void)instruction;
    return "LD A,($FF00+C)";
}

template<>
std::string Instructions::RL(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)processor;
    std::string R = Disassembler::RTable[instruction.code.z];
    return Common::Formatter::format("RL %s", R.c_str());
}

template<>
std::string Instructions::RLA(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)processor;
    (void)instruction;
    return "RLA";
}

template<>
std::string Instructions::SUB(std::unique_ptr<Processor> &processor, Instruction instruction) {
    return processor->disassembleArithmetic(instruction, "SUB");
}

template<>
std::string Instructions::AND(std::unique_ptr<Processor> &processor, Instruction instruction) {
    return processor->disassembleArithmetic(instruction, "AND");
}

template<>
std::string Instructions::SET(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)processor;
    std::string R = Disassembler::RTable[instruction.code.z];
    return Common::Formatter::format("SET %d,%s", instruction.code.y, R.c_str());
}

template<>
std::string Instructions::ADD_HL_RR(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)processor;
    std::string RR = Disassembler::RPTable[instruction.code.p];
    return Common::Formatter::format("ADD HL,%s", RR.c_str());
}

template<>
std::string Instructions::RES(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)processor;
    std::string R = Disassembler::RTable[instruction.code.z];
    return Common::Formatter::format("RES %d,%s", instruction.code.y, R.c_str());
}

template<>
std::string Instructions::SRA(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)processor;
    std::string R = Disassembler::RTable[instruction.code.z];
    return Common::Formatter::format("SRA %s", R.c_str());
}

template<>
std::string Instructions::SWAP(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)processor;
    std::string R = Disassembler::RTable[instruction.code.z];
    return Common::Formatter::format("SWAP %s", R.c_str());
}

template<>
std::string Instructions::JP_CC_NN(std::unique_ptr<Processor> &processor, Instruction instruction) {
    std::string compare = Disassembler::CCTable[instruction.code.y];
    uint16_t destination = processor->memory->loadDoubleWord(processor->registers.pc + 1, false);
    return Common::Formatter::format("JP %s,$%04x", compare.c_str(), destination);
}

template<>
std::string Instructions::LD_HL_SP_I8(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)instruction;
    int8_t value = processor->memory->load(processor->registers.pc + 1, false);
    uint16_t result = processor->registers.sp + value;
    return Common::Formatter::format("LD HL,$%04x", result);
}

template<>
std::string Instructions::SLA(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)processor;
    std::string R = Disassembler::RTable[instruction.code.z];
    return Common::Formatter::format("SLA %s", R.c_str());
}

template<>
std::string Instructions::RR(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)processor;
    std::string R = Disassembler::RTable[instruction.code.z];
    return Common::Formatter::format("RR %s", R.c_str());
}

template<>
std::string Instructions::RRC(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)processor;
    std::string R = Disassembler::RTable[instruction.code.z];
    return Common::Formatter::format("RRC %s", R.c_str());
}

template<>
std::string Instructions::LD_SP_HL(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)processor;
    (void)instruction;
    return Common::Formatter::format("LD SP,HL");
}

template<>
std::string Instructions::ADD_SP_I8(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)instruction;
    int8_t value = processor->memory->load(processor->registers.pc + 1, false);
    return Common::Formatter::format("ADD SP,$%02x", value);
}

template<>
std::string Instructions::RETI(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)processor;
    (void)instruction;
    return "RETI";
}

template<>
std::string Instructions::DAA(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)processor;
    (void)instruction;
    return "DAA";
}

template<>
std::string Instructions::CPL(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)processor;
    (void)instruction;
    return "CPL";
}

template<>
std::string Instructions::SCF(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)processor;
    (void)instruction;
    return "SCF";
}

template<>
std::string Instructions::CCF(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)processor;
    (void)instruction;
    return "CCF";
}

template<>
std::string Instructions::RRCA(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)processor;
    (void)instruction;
    return "RRCA";
}

template<>
std::string Instructions::SRL(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)processor;
    std::string R = Disassembler::RTable[instruction.code.z];
    return Common::Formatter::format("SRL %s", R.c_str());
}

template<>
std::string Instructions::HALT(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)processor;
    (void)instruction;
    return "HALT";
}

template<>
std::string Instructions::HALTED(std::unique_ptr<Processor> &processor, Instruction instruction) {
    (void)processor;
    (void)instruction;
    return "HALTED";
}
