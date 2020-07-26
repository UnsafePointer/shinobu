#include "core/cpu/CPU.hpp"
#include <iostream>
#include "core/cpu/Table.hpp"
#include "core/cpu/Decoding.hpp"

using namespace Core::CPU;

Processor::Processor(Common::Logs::Level logLevel, std::unique_ptr<Memory::Controller> &memory, std::unique_ptr<Device::Interrupt::Controller> &interrupt) : logger(logLevel, "  [CPU]: "), registers(), memory(memory), interrupt(interrupt), shouldSetIME(false), shouldClearIME(false), halted(false) {
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

std::string Processor::disassembleArithmetic(Instructions::Instruction instruction, std::string operation) {
    if (instruction.code.x == 2) {
        std::string R = Instructions::Disassembler::RTable[instruction.code.z];
        return Common::Formatter::format("%s A,%s", operation.c_str(), R.c_str());
    } else {
        uint8_t value = memory->load(registers.pc + 1);
        return Common::Formatter::format("%s A,$%02x", operation.c_str(), value);
    }
}

uint8_t Processor::executeArithmetic(Instructions::Instruction instruction, std::function<std::tuple<uint8_t, Flag>(uint8_t,uint8_t)> operation, bool useAccumulator) {
    advanceProgramCounter(instruction);
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
        } else {
            uint8_t HLValue = memory->load(registers.hl);
            uint8_t result;
            Flag flags;
            std::tie(result, flags) = operation(registers.a, HLValue);
            if (useAccumulator) {
                registers.a = result;
            }
            registers.flag = flags;
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
    } else {
        logger.logError("Invalid instruction decoding");
        return 0;
    }
    return 0;
}

void Processor::initialize() {
    if (memory->hasBootROM()) {
        registers.pc = 0x0000;
    } else {
        registers.af = 0x01B0;
        registers.bc = 0x0013;
        registers.de = 0x00D8;
        registers.hl = 0x014D;
        registers.pc = 0x0100;
        registers.sp = 0xFFFE;
        memory->store(0xFF05, 0x00, false);
        memory->store(0xFF06, 0x00, false);
        memory->store(0xFF07, 0x00, false);
        memory->store(0xFF10, 0x80, false);
        memory->store(0xFF11, 0xBF, false);
        memory->store(0xFF12, 0xF3, false);
        memory->store(0xFF14, 0xBF, false);
        memory->store(0xFF16, 0x3F, false);
        memory->store(0xFF17, 0x00, false);
        memory->store(0xFF19, 0xBF, false);
        memory->store(0xFF1A, 0x7F, false);
        memory->store(0xFF1B, 0xFF, false);
        memory->store(0xFF1C, 0x9F, false);
        memory->store(0xFF1E, 0xBF, false);
        memory->store(0xFF20, 0xFF, false);
        memory->store(0xFF21, 0x00, false);
        memory->store(0xFF22, 0x00, false);
        memory->store(0xFF23, 0xBF, false);
        memory->store(0xFF24, 0x77, false);
        memory->store(0xFF25, 0xF3, false);
        memory->store(0xFF26, 0xF1, false);
        memory->store(0xFF40, 0x91, false);
        memory->store(0xFF42, 0x00, false);
        memory->store(0xFF43, 0x00, false);
        memory->store(0xFF45, 0x00, false);
        memory->store(0xFF47, 0xFC, false);
        memory->store(0xFF48, 0xFF, false);
        memory->store(0xFF49, 0xFF, false);
        memory->store(0xFF4A, 0x00, false);
        memory->store(0xFF4B, 0x00, false);
        memory->store(0xFFFF, 0x00, false);
    }
}

Instructions::Instruction Processor::fetchInstruction() const {
    memory->beginCurrentInstruction();
    if (halted) {
        return Core::CPU::Instructions::Instruction(0x76, false);
    }
    uint8_t code = memory->load(registers.pc);
    if (code == Instructions::InstructionPrefix) {
        uint16_t immediateAddress = registers.pc + 1;
        code = memory->load(immediateAddress);
        return Instructions::Instruction(code, true);
    }
    return Instructions::Instruction(code, false);
}

void Processor::checkPendingInterrupts(Instructions::Instruction lastInstruction) {
    if (shouldSetIME && lastInstruction.code._value != 0xFB) {
        interrupt->updateIME(true);
        shouldSetIME = false;
    }
    if (shouldClearIME && lastInstruction.code._value != 0xF3) {
        interrupt->updateIME(false);
        shouldClearIME = false;
    }
    interrupt->serveInterrupts();
}

void Processor::executeInterrupt(Device::Interrupt::Interrupt interrupt) {
    halted = false;
    uint16_t address = Device::Interrupt::VECTOR[interrupt];
    memory->step(4);
    memory->step(4);
    pushIntoStack(registers.pc);
    memory->step(4);
    registers.pc = address;
}

bool Processor::isHalted() const {
    return halted;
}

void Processor::unhalt() {
    halted = false;
}

template<typename T>
Instructions::InstructionHandler<T> Processor::decodeInstruction(Instructions::Instruction instruction) const {
    if (halted) {
        return Instructions::HALTED;
    }
    std::vector<Instructions::InstructionHandler<T>> table;
    if (instruction.isPrefixed) {
        table = Instructions::PrefixedInstructionHandlerTable<T>;
    } else {
        table = Instructions::InstructionHandlerTable<T>;
    }
    if (instruction.code._value > table.size()) {
        logger.logError("Unhandled instruction with code: %02x", instruction.code._value);
    }
    Instructions::InstructionHandler<T> handler = table[instruction.code._value];
    if (handler == nullptr) {
        logger.logError("Unhandled instruction with code: %02x", instruction.code._value);
    }
    return handler;
}

template Instructions::InstructionHandler<void> Processor::decodeInstruction<void>(Instructions::Instruction instruction) const;
template Instructions::InstructionHandler<std::string> Processor::decodeInstruction<std::string>(Instructions::Instruction instruction) const;
