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
            uint16_t _value[4];
            struct {
                uint16_t af;
                uint16_t bc;
                uint16_t de;
                uint16_t hl;
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

            Registers() : _value() {} ;
        };

        class Processor;

        namespace Instructions {
            typedef uint8_t (*InstructionHandler) (std::unique_ptr<Processor> &processor, uint8_t code);

            uint8_t NOP(std::unique_ptr<Processor> &processor, uint8_t code);

            const std::vector<InstructionHandler> table = {
                NOP,
            };
        };

        class Processor {
            Registers registers;
            uint16_t stackPointer;
            uint16_t programCounter;
            std::unique_ptr<Memory::Controller> &memory;
        public:
            Processor(std::unique_ptr<Memory::Controller> &memory);
            ~Processor();

            void initialize();
            uint8_t fetchInstruction() const;
            Instructions::InstructionHandler decodeInstruction(uint8_t code) const;
        };
    };
};
