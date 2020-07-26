#include "core/cpu/Disassembler.hpp"
#include "core/cpu/CPU.hpp"

using namespace Core::CPU::Disassembler;

Disassembler::Disassembler(Common::Logs::Level logLevel, std::unique_ptr<Processor> &processor) : logger(logLevel, ""), processor(processor), enabled(logLevel != Common::Logs::NoLog) {

}

Disassembler::~Disassembler() {

}

void Disassembler::disassemble(Instructions::Instruction instruction) const {
    if (!enabled) {
        return;
    }
    Core::CPU::Instructions::InstructionHandler<std::string> disassemblerHandler = processor->decodeInstruction<std::string>(instruction);
    std::string disassembledInstruction = disassemblerHandler(processor, instruction);
    std::string separator = std::string(20 - disassembledInstruction.length(), ' ');
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

void Disassembler::toggleEnabled() {
    enabled = !enabled;
}
