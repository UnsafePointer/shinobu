#pragma once
#include <vector>
#include <cstdint>

namespace Core {
    namespace CPU {
        class Processor;

        namespace Instructions {
            // https://gb-archive.github.io/salvage/decoding_gbz80_opcodes/Decoding%20Gamboy%20Z80%20Opcodes.html
            union Code {
                uint8_t _value;
                struct {
                    uint8_t z : 3;
                    uint8_t y : 3;
                    uint8_t x : 2;
                };
                struct {
                    uint8_t _padding : 3;
                    uint8_t q : 1;
                    uint8_t p : 2;
                    uint8_t _unused : 3;
                };

                Code(uint8_t value) : _value(value) {}
            };

            struct Instruction {
                Code code;
                bool isPrefixed;

                Instruction() : code(0x0), isPrefixed(false) {}
                Instruction(uint8_t value, bool isPrefixed) : code(value), isPrefixed(isPrefixed) {}
            };

            const uint8_t InstructionPrefix = 0xCB;

            template<typename T>
            T NOP(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T JP_U16(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T DI(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T LD_RR_NN(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T RST_N(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T INC_R(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T RET(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T LD_NN_A(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T LD_U8(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T LDH_N_A(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T DEC_RR(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T CALL_NN(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T LD_R_R(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T JR_I8(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T LD_INDIRECT(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T PUSH_RR(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T POP_RR(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T INC_RR(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T EI(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T OR(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T JR_CC_I8(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T STOP(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T CALL_CC_NN(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T ADD(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T LD_NN_SP(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T RLCA(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T LD_A_NN(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T SBC_A(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T DEC_R(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T XOR_A(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T ADC_A(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T JP_HL(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T RRA(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T RET_CC(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T RLC(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T CP_A(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T LDH_A_N(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T BIT(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T LDH_C_A(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T LDH_A_C(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T RL(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T RLA(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T SUB(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T AND(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T SET(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T ADD_HL_RR(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T RES(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T SRA(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T SWAP(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T JP_CC_NN(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T LD_HL_SP_I8(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T SLA(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T RR(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T RRC(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T LD_SP_HL(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T ADD_SP_I8(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T RETI(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T DAA(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T CPL(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T SCF(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T CCF(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T RRCA(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T SRL(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T HALT(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            T HALTED(std::unique_ptr<Processor> &processor, Instruction instruction);

            template<typename T>
            using InstructionHandler = T (*) (std::unique_ptr<Core::CPU::Processor> &processor, Core::CPU::Instructions::Instruction instruction);
        };
    };
};
