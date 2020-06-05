#include "core/cpu/CPU.hpp"
#include <iostream>
#include "core/cpu/Table.hpp"
#include "core/cpu/Decoding.hpp"

using namespace Core::CPU;

Processor::Processor(Common::Logs::Level logLevel, std::unique_ptr<Memory::Controller> &memory) : logger(logLevel, "  [CPU]: "), registers(), memory(memory) {
}

Processor::~Processor() {
}

void Processor::pushIntoStack(uint16_t value) {
    registers.sp -= 2;
    memory->storeDoubleWord(registers.sp, value);
}

uint16_t Processor::popFromStack() {
    uint16_t value = memory->loadDoubleWord(registers.sp);
    registers.sp += 2;
    return value;
}

void Processor::advanceProgramCounter(Instructions::Instruction instruction) {
    if (instruction.isPrefixed) {
        registers.pc += 2;
    } else {
        uint8_t length = Instructions::InstructionSizeTable[instruction.code._value];
        registers.pc += length;
    }
}

uint8_t Processor::executeArithmetic(Instructions::Instruction instruction, std::function<std::tuple<uint8_t, Flag>(uint8_t,uint8_t)> operation, bool useAccumulator) {
    advanceProgramCounter(instruction);
    uint8_t cycles;
    if (instruction.code.x == 2) {
        uint8_t R = Instructions::RTable[instruction.code.z];
        if (R != 0xFF) {
            uint8_t RValue = registers._value8[R];
            uint8_t result;
            Flag flags;
            std::tie(result, flags) = operation(registers.a, RValue);
            if (useAccumulator) {
                registers.a = result;
            }
            registers.flag = flags;
            cycles = 4;
        } else {
            uint8_t HLValue = memory->load(registers.hl);
            uint8_t result;
            Flag flags;
            std::tie(result, flags) = operation(registers.a, HLValue);
            if (useAccumulator) {
                registers.a = result;
            }
            registers.flag = flags;
            cycles = 8;
        }
    } else if (instruction.code.x == 3) {
        uint8_t NValue = memory->load(registers.pc - 1); // PC is already at next instruction
        int8_t result;
        Flag flags;
        std::tie(result, flags) = operation(registers.a, NValue);
        if (useAccumulator) {
            registers.a = result;
        }
        registers.flag = flags;
        cycles = 8;
    } else {
        logger.logError("Invalid instruction decoding");
        return 0;
    }
    return cycles;
}

void Processor::initialize() {
    registers.af = 0x01B0;
    registers.bc = 0x0013;
    registers.de = 0x00D8;
    registers.hl = 0x014D;
    if (memory->hasBootROM()) {
        registers.pc = 0x0000;
    } else {
        registers.pc = 0x0100;
    }
    registers.sp = 0xFFFE;
    memory->store(0xFF05, 0x00);
    memory->store(0xFF06, 0x00);
    memory->store(0xFF07, 0x00);
    memory->store(0xFF10, 0x80);
    memory->store(0xFF11, 0xBF);
    memory->store(0xFF12, 0xF3);
    memory->store(0xFF14, 0xBF);
    memory->store(0xFF16, 0x3F);
    memory->store(0xFF17, 0x00);
    memory->store(0xFF19, 0xBF);
    memory->store(0xFF1A, 0x7F);
    memory->store(0xFF1B, 0xFF);
    memory->store(0xFF1C, 0x9F);
    memory->store(0xFF1E, 0xBF);
    memory->store(0xFF20, 0xFF);
    memory->store(0xFF21, 0x00);
    memory->store(0xFF22, 0x00);
    memory->store(0xFF23, 0xBF);
    memory->store(0xFF24, 0x77);
    memory->store(0xFF25, 0xF3);
    memory->store(0xFF26, 0xF1);
    memory->store(0xFF40, 0x91);
    memory->store(0xFF42, 0x00);
    memory->store(0xFF43, 0x00);
    memory->store(0xFF45, 0x00);
    memory->store(0xFF47, 0xFC);
    memory->store(0xFF48, 0xFF);
    memory->store(0xFF49, 0xFF);
    memory->store(0xFF4A, 0x00);
    memory->store(0xFF4B, 0x00);
    // TODO: Interrupt Enable Register
    // memory->store(0xFFFF, 0x00);
}

uint8_t Processor::fetchInstruction() const {
    return memory->load(registers.pc);
}

uint8_t Processor::fetchPrefixedInstruction() const {
    uint8_t immediateAddress = registers.pc + 1;
    return memory->load(immediateAddress);
}

uint16_t Processor::programCounter() const {
    return registers.pc;
}

template<typename T>
Instructions::InstructionHandler<T> Processor::decodeInstruction(uint8_t code,  bool isPrefixed) const {
    // TODO: Remove these checks once table is complete
    std::string prefixed = "";
    if (isPrefixed) {
        prefixed = "prefixed ";
    }
    std::vector<Instructions::InstructionHandler<T>> table;
    if (isPrefixed) {
        table = Instructions::PrefixedInstructionHandlerTable<T>;
    } else {
        table = Instructions::InstructionHandlerTable<T>;
    }
    if (code > table.size()) {
        logger.logError("Unhandled %sinstruction with code: %02x", prefixed.c_str(), code);
    }
    Instructions::InstructionHandler<T> handler = table[code];
    if (handler == nullptr) {
        logger.logError("Unhandled %sinstruction with code: %02x", prefixed.c_str(), code);
    }
    return handler;
}

template Instructions::InstructionHandler<uint8_t> Processor::decodeInstruction<uint8_t>(uint8_t code,  bool isPrefixed) const;
template Instructions::InstructionHandler<std::string> Processor::decodeInstruction<std::string>(uint8_t code,  bool isPrefixed) const;
