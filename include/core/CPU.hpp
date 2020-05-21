#pragma once
#include <cstdint>

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

        class Processor {
            Registers registers;
            uint16_t stackPointer;
            uint16_t programCounter;
        public:
            Processor();
            ~Processor();
        };
    };
};
