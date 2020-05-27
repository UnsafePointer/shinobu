#pragma once
#include <cstdint>
#include <functional>
#include <memory>
#include <tuple>
#include <vector>
#include "Memory.hpp"

namespace Core {
    namespace CPU {
        /*
        Bit  Name  Set Clr  Expl.
        3-0  -     -   -    Not used (always zero)
        4    cy    C   NC   Carry Flag
        5    h     -   -    Half Carry Flag (BCD)
        6    n     -   -    Add/Sub-Flag (BCD)
        7    zf    Z   NZ   Zero Flag
        */
        union Flag {
            struct {
                uint8_t unused : 4;
                uint8_t carry : 1;
                uint8_t halfcarry : 1;
                uint8_t n : 1;
                uint8_t zero : 1;
            };
            uint8_t _value;

            Flag() : _value() {}
        };

        /*
        16bit Hi   Lo   Name/Function
        AF    A    -    Accumulator & Flags
        BC    B    C    BC
        DE    D    E    DE
        HL    H    L    HL
        SP    -    -    Stack Pointer
        PC    -    -    Program Counter/Pointer
        */
        union Registers {
            uint16_t _value16[6];
            uint8_t _value8[12];
            struct {
                uint16_t af;
                uint16_t bc;
                uint16_t de;
                uint16_t hl;
                uint16_t sp;
                uint16_t pc;
            };
            struct {
                uint8_t f;
                uint8_t a;
                uint8_t c;
                uint8_t b;
                uint8_t e;
                uint8_t d;
                uint8_t l;
                uint8_t h;
            };
            Flag flag;

            Registers() : _value16() {} ;
        };

        class Processor;

        namespace Instructions {
            // https://gb-archive.github.io/salvage/decoding_gbz80_opcodes/Decoding%20Gamboy%20Z80%20Opcodes.html
            union Instruction {
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

                Instruction() : _value() {}
                Instruction(uint8_t value) : _value(value) {}
            };

            const uint8_t InstructionPrefix = 0xCB;

            const std::vector<uint8_t> RPTable = { 0x1, 0x2, 0x3, 0x4 };
            const std::vector<uint8_t> RP2Table = { 0x1, 0x2, 0x3, 0x0 };
            const std::vector<uint8_t> RTable = { 0x3, 0x2, 0x5, 0x4, 0x7, 0x6, 0xFF, 0x1 };
            const std::vector<std::function<bool(Flag&)>> CCTable = {
                [](Flag& flags) {
                    return flags.zero == 0;
                },
                [](Flag& flags) {
                    return flags.zero == 1;
                },
                [](Flag& flags) {
                    return flags.carry == 0;
                },
                [](Flag& flags) {
                    return flags.carry == 1;
                },
            };

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

            typedef uint8_t (*InstructionHandler) (std::unique_ptr<Core::CPU::Processor> &processor, Core::CPU::Instructions::Instruction instruction);
        };

        class Processor {
            Registers registers;
            std::unique_ptr<Memory::Controller> &memory;

            void pushIntoStack(uint16_t value);
            uint16_t popFromStack();

            uint8_t executeArithmetic(Instructions::Instruction instruction, std::function<std::tuple<uint8_t, Flag>(uint8_t,uint8_t)> operation, bool useAccumulator = true);

            template<typename T>
            friend T CPU::Instructions::NOP(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            friend T CPU::Instructions::JP_U16(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            friend T CPU::Instructions::DI(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            friend T CPU::Instructions::LD_RR_NN(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            friend T CPU::Instructions::RST_N(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            friend T CPU::Instructions::INC_R(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            friend T CPU::Instructions::RET(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            friend T CPU::Instructions::LD_NN_A(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            friend T CPU::Instructions::LD_U8(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            friend T CPU::Instructions::LDH_N_A(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            friend T CPU::Instructions::DEC_RR(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            friend T CPU::Instructions::CALL_NN(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            friend T CPU::Instructions::LD_R_R(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            friend T CPU::Instructions::JR_I8(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            friend T CPU::Instructions::LD_INDIRECT(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            friend T CPU::Instructions::PUSH_RR(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            friend T CPU::Instructions::POP_RR(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            friend T CPU::Instructions::INC_RR(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            friend T CPU::Instructions::EI(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            friend T CPU::Instructions::OR(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            friend T CPU::Instructions::JR_CC_I8(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            friend T CPU::Instructions::STOP(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            friend T CPU::Instructions::CALL_CC_NN(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            friend T CPU::Instructions::ADD(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            friend T CPU::Instructions::LD_NN_SP(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            friend T CPU::Instructions::RLCA(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            friend T CPU::Instructions::LD_A_NN(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            friend T CPU::Instructions::SBC_A(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            friend T CPU::Instructions::DEC_R(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            friend T CPU::Instructions::XOR_A(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            friend T CPU::Instructions::ADC_A(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            friend T CPU::Instructions::JP_HL(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            friend T CPU::Instructions::RRA(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            friend T CPU::Instructions::RET_CC(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            friend T CPU::Instructions::RLC(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            friend T CPU::Instructions::CP_A(std::unique_ptr<Processor> &processor, Instruction instruction);
            template<typename T>
            friend T CPU::Instructions::LDH_A_N(std::unique_ptr<Processor> &processor, Instruction instruction);
        public:
            Processor(std::unique_ptr<Memory::Controller> &memory);
            ~Processor();

            void initialize();
            uint8_t fetchInstruction() const;
            uint8_t fetchPrefixedInstruction() const;
            Instructions::InstructionHandler decodeInstruction(uint8_t code, bool isPrefixed) const;
        };
    };
};
