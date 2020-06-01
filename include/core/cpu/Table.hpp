#pragma once
#include <memory>
#include "core/cpu/CPU.hpp"
#include "core/cpu/CPU.tcc"
#include "core/cpu/Disassembler.tcc"

namespace Core {
    namespace CPU {
        namespace Instructions {
            template <typename T>
            const std::vector<InstructionHandler<T>> InstructionHandlerTable = {
            //    +0         +1        +2           +3      +4          +5       +6      +7      +8        +9      +A           +B      +C          +D          +E      +F
            /*0+*/ NOP,      LD_RR_NN, LD_INDIRECT, INC_RR, INC_R,      DEC_R,   LD_U8,  RLCA,   LD_NN_SP, NULL,   LD_INDIRECT, DEC_RR, INC_R,      DEC_R,      LD_U8,  NULL,
            /*1+*/ STOP,     LD_RR_NN, LD_INDIRECT, INC_RR, INC_R,      DEC_R,   LD_U8,  RLA,    JR_I8,    NULL,   LD_INDIRECT, DEC_RR, INC_R,      DEC_R,      LD_U8,  RRA,
            /*2+*/ JR_CC_I8, LD_RR_NN, LD_INDIRECT, INC_RR, INC_R,      DEC_R,   LD_U8,  NULL,   JR_CC_I8, NULL,   LD_INDIRECT, DEC_RR, INC_R,      DEC_R,      LD_U8,  NULL,
            /*3+*/ JR_CC_I8, LD_RR_NN, LD_INDIRECT, INC_RR, INC_R,      DEC_R,   LD_U8,  NULL,   JR_CC_I8, NULL,   LD_INDIRECT, DEC_RR, INC_R,      DEC_R,      LD_U8,  NULL,
            /*4+*/ LD_R_R,   LD_R_R,   LD_R_R,      LD_R_R, LD_R_R,     LD_R_R,  LD_R_R, LD_R_R, LD_R_R,   LD_R_R, LD_R_R,      LD_R_R, LD_R_R,     LD_R_R,     LD_R_R, LD_R_R,
            /*5+*/ LD_R_R,   LD_R_R,   LD_R_R,      LD_R_R, LD_R_R,     LD_R_R,  LD_R_R, LD_R_R, LD_R_R,   LD_R_R, LD_R_R,      LD_R_R, LD_R_R,     LD_R_R,     LD_R_R, LD_R_R,
            /*6+*/ LD_R_R,   LD_R_R,   LD_R_R,      LD_R_R, LD_R_R,     LD_R_R,  LD_R_R, LD_R_R, LD_R_R,   LD_R_R, LD_R_R,      LD_R_R, LD_R_R,     LD_R_R,     LD_R_R, LD_R_R,
            /*7+*/ LD_R_R,   LD_R_R,   LD_R_R,      LD_R_R, LD_R_R,     LD_R_R,  NULL,   LD_R_R, LD_R_R,   LD_R_R, LD_R_R,      LD_R_R, LD_R_R,     LD_R_R,     LD_R_R, LD_R_R,
            /*8+*/ ADD,      ADD,      ADD,         ADD,    ADD,        ADD,     ADD,    ADD,    ADC_A,    ADC_A,  ADC_A,       ADC_A,  ADC_A,      ADC_A,      ADC_A,  ADC_A,
            /*9+*/ SUB,      SUB,      SUB,         SUB,    SUB,        SUB,     SUB,    SUB,    SBC_A,    SBC_A,  SBC_A,       SBC_A,  SBC_A,      SBC_A,      SBC_A,  SBC_A,
            /*A+*/ AND,      AND,      AND,         AND,    AND,        AND,     AND,    AND,    XOR_A,    XOR_A,  XOR_A,       XOR_A,  XOR_A,      XOR_A,      XOR_A,  XOR_A,
            /*B+*/ OR,       OR,       OR,          OR,     OR,         OR,      OR,     OR,     CP_A,     CP_A,   CP_A,        CP_A,   CP_A,       CP_A,       CP_A,   CP_A,
            /*C+*/ RET_CC,   POP_RR,   NULL,        JP_U16, CALL_CC_NN, PUSH_RR, ADD,    RST_N,  RET_CC,   RET,    NULL,        NULL,   CALL_CC_NN, CALL_NN,    ADC_A,  RST_N,
            /*D+*/ RET_CC,   POP_RR,   NULL,        NULL,   CALL_CC_NN, PUSH_RR, SUB,    RST_N,  RET_CC,   NULL,   NULL,        NULL,   CALL_CC_NN, NULL,       SBC_A,  RST_N,
            /*E+*/ LDH_N_A,  POP_RR,   LDH_C_A,     NULL,   NULL,       PUSH_RR, AND,    RST_N,  NULL,     JP_HL,  LD_NN_A,     NULL,   NULL,       NULL,       XOR_A,  RST_N,
            /*F+*/ LDH_A_N,  POP_RR,   LDH_A_C,     DI,     NULL,       PUSH_RR, OR,     RST_N,  NULL,     NULL,   LD_A_NN,     EI,     NULL,       NULL,       CP_A,   RST_N,
            };

            template <typename T>
            const std::vector<InstructionHandler<T>> PrefixedInstructionHandlerTable = {
            //    +0    +1    +2    +3    +4    +5    +6    +7    +8    +9    +A    +B    +C    +D    +E    +F
            /*0+*/RLC,  RLC,  RLC,  RLC,  RLC,  RLC,  RLC,  RLC,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
            /*1+*/RL,   RL,   RL,   RL,   RL,   RL,   RL,   RL,   NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
            /*2+*/NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
            /*3+*/NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
            /*4+*/BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,
            /*5+*/BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,
            /*6+*/BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,
            /*7+*/BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,
            /*8+*/NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
            /*9+*/NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
            /*A+*/NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
            /*B+*/NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
            /*C+*/NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
            /*D+*/NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
            /*E+*/NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
            /*F+*/NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
            };
        };
    };
};
