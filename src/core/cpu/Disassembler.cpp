#include "core/cpu/Disassembler.hpp"
#include "core/cpu/CPU.hpp"
#include "common/Formatter.hpp"

using namespace Core::CPU::Disassembler;

Disassembler::Disassembler(Common::Logs::Level logLevel, std::unique_ptr<Processor> &processor) : logger(logLevel, ""), processor(processor), enabled(logLevel != Common::Logs::NoLog) {

}

Disassembler::~Disassembler() {

}

void Disassembler::disassembleWhileExecuting(Instructions::Instruction instruction) const {
    if (!enabled) {
        return;
    }
    Core::CPU::Instructions::InstructionHandler<std::string> disassemblerHandler = processor->decodeInstruction<std::string>(instruction);
    std::string disassembledInstruction = disassemblerHandler(processor, instruction);
    logger.logDebug("A: %02X F: %02X B: %02X C: %02X D: %02X E: %02X H: %02X L: %02X SP: %04X PC: 00:%04X | %s",
        processor->registers.a,
        processor->registers.f,
        processor->registers.b,
        processor->registers.c,
        processor->registers.d,
        processor->registers.e,
        processor->registers.h,
        processor->registers.l,
        processor->registers.sp,
        processor->registers.pc,
        disassembledInstruction.c_str());
}

std::string Disassembler::disassemble(Instructions::Instruction instruction) {
    std::string disassembledInstruction;
    Core::CPU::Instructions::InstructionHandler<std::string> disassemblerHandler = processor->decodeInstruction<std::string>(instruction);
    if (disassemblerHandler == nullptr) {
        instruction = Instructions::Instruction(0x0, false);
        disassembledInstruction = Common::Formatter::format("00:%04X | ???: %02x", processor->registers.pc, instruction.code._value);
    } else {
        disassembledInstruction = disassemblerHandler(processor, instruction);
    }
    std::string separator = std::string(20 - disassembledInstruction.length(), ' ');
    disassembledInstruction = Common::Formatter::format("%s%s; 00:%04X", disassembledInstruction.c_str(), separator.c_str(), processor->registers.pc);
    processor->advanceProgramCounter(instruction);
    return disassembledInstruction;
}

void Disassembler::toggleEnabled() {
    enabled = !enabled;
}

bool Disassembler::canDisassemble() const {
    return processor->registers.pc <= 0x3FFF;
}

void Disassembler::configure() const {
    processor->registers.pc = 0x0150;
}
