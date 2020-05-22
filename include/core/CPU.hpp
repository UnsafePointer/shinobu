#pragma once
#include <cstdint>
#include <memory>
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

                Instruction(uint8_t value) : _value(value) {}
            };

            const std::vector<uint8_t> RPTable = { 0x1, 0x2, 0x3, 0x4 };
            const std::vector<uint8_t> RTable = { 0x3, 0x2, 0x5, 0x4, 0x7, 0x6, 0xFF, 0x1 };

            typedef uint8_t (*InstructionHandler) (std::unique_ptr<Processor> &processor, Instruction instruction);

            uint8_t NOP(std::unique_ptr<Processor> &processor, Instruction instruction);
            uint8_t JP_U16(std::unique_ptr<Processor> &processor, Instruction instruction);
            uint8_t DI(std::unique_ptr<Processor> &processor, Instruction instruction);
            uint8_t LD_RR_NN(std::unique_ptr<Processor> &processor, Instruction instruction);
            uint8_t RST_N(std::unique_ptr<Processor> &processor, Instruction instruction);
            uint8_t INC_R(std::unique_ptr<Processor> &processor, Instruction instruction);
            uint8_t RET(std::unique_ptr<Processor> &processor, Instruction instruction);
            uint8_t LD_NN_A(std::unique_ptr<Processor> &processor, Instruction instruction);
            uint8_t LD_U8(std::unique_ptr<Processor> &processor, Instruction instruction);
            uint8_t LDH_N_A(std::unique_ptr<Processor> &processor, Instruction instruction);
            uint8_t DEC_RR(std::unique_ptr<Processor> &processor, Instruction instruction);
            uint8_t CALL_NN(std::unique_ptr<Processor> &processor, Instruction instruction);
            uint8_t LD_R_R(std::unique_ptr<Processor> &processor, Instruction instruction);

            const std::vector<InstructionHandler> instructionHandlerTable = {
            //    +0       +1        +2      +3      +4      +5      +6      +7      +8      +9      +A       +B      +C      +D       +E      +F
            /*0+*/ NOP,    LD_RR_NN, NULL,   NULL,   INC_R,  NULL,   NULL,   LD_U8,  NULL,   NULL,   NULL,    DEC_RR, INC_R,  NULL,    LD_U8,  NULL,
            /*1+*/NULL,    LD_RR_NN, NULL,   NULL,   INC_R,  NULL,   NULL,   LD_U8,  NULL,   NULL,   NULL,    DEC_RR, INC_R,  NULL,    LD_U8,  NULL,
            /*2+*/NULL,    LD_RR_NN, NULL,   NULL,   INC_R,  NULL,   NULL,   LD_U8,  NULL,   NULL,   NULL,    DEC_RR, INC_R,  NULL,    LD_U8,  NULL,
            /*3+*/NULL,    LD_RR_NN, NULL,   NULL,   INC_R,  NULL,   NULL,   LD_U8,  NULL,   NULL,   NULL,    DEC_RR, INC_R,  NULL,    LD_U8,  NULL,
            /*4+*/LD_R_R,  LD_R_R,   LD_R_R, LD_R_R, LD_R_R, LD_R_R, LD_R_R, LD_R_R, LD_R_R, LD_R_R, LD_R_R,  LD_R_R, LD_R_R, LD_R_R,  LD_R_R, LD_R_R,
            /*5+*/LD_R_R,  LD_R_R,   LD_R_R, LD_R_R, LD_R_R, LD_R_R, LD_R_R, LD_R_R, LD_R_R, LD_R_R, LD_R_R,  LD_R_R, LD_R_R, LD_R_R,  LD_R_R, LD_R_R,
            /*6+*/LD_R_R,  LD_R_R,   LD_R_R, LD_R_R, LD_R_R, LD_R_R, LD_R_R, LD_R_R, LD_R_R, LD_R_R, LD_R_R,  LD_R_R, LD_R_R, LD_R_R,  LD_R_R, LD_R_R,
            /*7+*/LD_R_R,  LD_R_R,   LD_R_R, LD_R_R, LD_R_R, LD_R_R, NULL,   LD_R_R, LD_R_R, LD_R_R, LD_R_R,  LD_R_R, LD_R_R, LD_R_R,  LD_R_R, LD_R_R,
            /*8+*/NULL,    NULL,     NULL,   NULL,   NULL,   NULL,   NULL,   NULL,   NULL,   NULL,   NULL,    NULL,   NULL,   NULL,    NULL,   NULL,
            /*9+*/NULL,    NULL,     NULL,   NULL,   NULL,   NULL,   NULL,   NULL,   NULL,   NULL,   NULL,    NULL,   NULL,   NULL,    NULL,   NULL,
            /*A+*/NULL,    NULL,     NULL,   NULL,   NULL,   NULL,   NULL,   NULL,   NULL,   NULL,   NULL,    NULL,   NULL,   NULL,    NULL,   NULL,
            /*B+*/NULL,    NULL,     NULL,   NULL,   NULL,   NULL,   NULL,   NULL,   NULL,   NULL,   NULL,    NULL,   NULL,   NULL,    NULL,   NULL,
            /*C+*/NULL,    NULL,     NULL,   JP_U16, NULL,   NULL,   RST_N,  NULL,   NULL,   RET,    NULL,    NULL,   NULL,   CALL_NN, NULL,   RST_N,
            /*D+*/NULL,    NULL,     NULL,   NULL,   NULL,   NULL,   RST_N,  NULL,   NULL,   NULL,   NULL,    NULL,   NULL,   NULL,    NULL,   RST_N,
            /*E+*/LDH_N_A, NULL,     NULL,   NULL,   NULL,   NULL,   RST_N,  NULL,   NULL,   NULL,   LD_NN_A, NULL,   NULL,   NULL,    NULL,   RST_N,
            /*F+*/NULL,    NULL,     NULL,   DI,     NULL,   NULL,   RST_N,  NULL,   NULL,   NULL,   NULL,    NULL,   NULL,   NULL,    NULL,   RST_N,
            };
        };

        class Processor {
            Registers registers;
            std::unique_ptr<Memory::Controller> &memory;

            void pushIntoStack(uint16_t value);
            uint16_t popFromStack();

            friend uint8_t Instructions::NOP(std::unique_ptr<Processor> &processor, Instruction instruction);
            friend uint8_t Instructions::JP_U16(std::unique_ptr<Processor> &processor, Instruction instruction);
            friend uint8_t Instructions::DI(std::unique_ptr<Processor> &processor, Instruction instruction);
            friend uint8_t Instructions::LD_RR_NN(std::unique_ptr<Processor> &processor, Instruction instruction);
            friend uint8_t Instructions::RST_N(std::unique_ptr<Processor> &processor, Instruction instruction);
            friend uint8_t Instructions::INC_R(std::unique_ptr<Processor> &processor, Instruction instruction);
            friend uint8_t Instructions::RET(std::unique_ptr<Processor> &processor, Instruction instruction);
            friend uint8_t Instructions::LD_NN_A(std::unique_ptr<Processor> &processor, Instruction instruction);
            friend uint8_t Instructions::LD_U8(std::unique_ptr<Processor> &processor, Instruction instruction);
            friend uint8_t Instructions::LDH_N_A(std::unique_ptr<Processor> &processor, Instruction instruction);
            friend uint8_t Instructions::DEC_RR(std::unique_ptr<Processor> &processor, Instruction instruction);
            friend uint8_t Instructions::CALL_NN(std::unique_ptr<Processor> &processor, Instruction instruction);
            friend uint8_t Instructions::LD_R_R(std::unique_ptr<Processor> &processor, Instruction instruction);
        public:
            Processor(std::unique_ptr<Memory::Controller> &memory);
            ~Processor();

            void initialize();
            uint8_t fetchInstruction() const;
            Instructions::InstructionHandler decodeInstruction(uint8_t code) const;
        };
    };
};
