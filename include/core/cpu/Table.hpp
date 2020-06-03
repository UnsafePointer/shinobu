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
            //    +0         +1        +2           +3      +4          +5       +6      +7      +8           +9         +A           +B      +C          +D          +E      +F
            /*0+*/ NOP,      LD_RR_NN, LD_INDIRECT, INC_RR, INC_R,      DEC_R,   LD_U8,  RLCA,   LD_NN_SP,    ADD_HL_RR, LD_INDIRECT, DEC_RR, INC_R,      DEC_R,      LD_U8,  NULL,
            /*1+*/ STOP,     LD_RR_NN, LD_INDIRECT, INC_RR, INC_R,      DEC_R,   LD_U8,  RLA,    JR_I8,       ADD_HL_RR, LD_INDIRECT, DEC_RR, INC_R,      DEC_R,      LD_U8,  RRA,
            /*2+*/ JR_CC_I8, LD_RR_NN, LD_INDIRECT, INC_RR, INC_R,      DEC_R,   LD_U8,  DAA,    JR_CC_I8,    ADD_HL_RR, LD_INDIRECT, DEC_RR, INC_R,      DEC_R,      LD_U8,  CPL,
            /*3+*/ JR_CC_I8, LD_RR_NN, LD_INDIRECT, INC_RR, INC_R,      DEC_R,   LD_U8,  SCF,    JR_CC_I8,    ADD_HL_RR, LD_INDIRECT, DEC_RR, INC_R,      DEC_R,      LD_U8,  CCF,
            /*4+*/ LD_R_R,   LD_R_R,   LD_R_R,      LD_R_R, LD_R_R,     LD_R_R,  LD_R_R, LD_R_R, LD_R_R,      LD_R_R,    LD_R_R,      LD_R_R, LD_R_R,     LD_R_R,     LD_R_R, LD_R_R,
            /*5+*/ LD_R_R,   LD_R_R,   LD_R_R,      LD_R_R, LD_R_R,     LD_R_R,  LD_R_R, LD_R_R, LD_R_R,      LD_R_R,    LD_R_R,      LD_R_R, LD_R_R,     LD_R_R,     LD_R_R, LD_R_R,
            /*6+*/ LD_R_R,   LD_R_R,   LD_R_R,      LD_R_R, LD_R_R,     LD_R_R,  LD_R_R, LD_R_R, LD_R_R,      LD_R_R,    LD_R_R,      LD_R_R, LD_R_R,     LD_R_R,     LD_R_R, LD_R_R,
            /*7+*/ LD_R_R,   LD_R_R,   LD_R_R,      LD_R_R, LD_R_R,     LD_R_R,  NULL,   LD_R_R, LD_R_R,      LD_R_R,    LD_R_R,      LD_R_R, LD_R_R,     LD_R_R,     LD_R_R, LD_R_R,
            /*8+*/ ADD,      ADD,      ADD,         ADD,    ADD,        ADD,     ADD,    ADD,    ADC_A,       ADC_A,     ADC_A,       ADC_A,  ADC_A,      ADC_A,      ADC_A,  ADC_A,
            /*9+*/ SUB,      SUB,      SUB,         SUB,    SUB,        SUB,     SUB,    SUB,    SBC_A,       SBC_A,     SBC_A,       SBC_A,  SBC_A,      SBC_A,      SBC_A,  SBC_A,
            /*A+*/ AND,      AND,      AND,         AND,    AND,        AND,     AND,    AND,    XOR_A,       XOR_A,     XOR_A,       XOR_A,  XOR_A,      XOR_A,      XOR_A,  XOR_A,
            /*B+*/ OR,       OR,       OR,          OR,     OR,         OR,      OR,     OR,     CP_A,        CP_A,      CP_A,        CP_A,   CP_A,       CP_A,       CP_A,   CP_A,
            /*C+*/ RET_CC,   POP_RR,   JP_CC_NN,    JP_U16, CALL_CC_NN, PUSH_RR, ADD,    RST_N,  RET_CC,      RET,       JP_CC_NN,    NULL,   CALL_CC_NN, CALL_NN,    ADC_A,  RST_N,
            /*D+*/ RET_CC,   POP_RR,   JP_CC_NN,    NULL,   CALL_CC_NN, PUSH_RR, SUB,    RST_N,  RET_CC,      RETI,      JP_CC_NN,    NULL,   CALL_CC_NN, NULL,       SBC_A,  RST_N,
            /*E+*/ LDH_N_A,  POP_RR,   LDH_C_A,     NULL,   NULL,       PUSH_RR, AND,    RST_N,  ADD_SP_I8,   JP_HL,     LD_NN_A,     NULL,   NULL,       NULL,       XOR_A,  RST_N,
            /*F+*/ LDH_A_N,  POP_RR,   LDH_A_C,     DI,     NULL,       PUSH_RR, OR,     RST_N,  LD_HL_SP_I8, LD_SP_HL,  LD_A_NN,     EI,     NULL,       NULL,       CP_A,   RST_N,
            };

            const std::vector<uint8_t> InstructionSizeTable = {
            //     +0 +1 +2 +3    +4    +5 +6 +7 +8 +9 +A +B     +C    +D    +E +F
            /*0+*/ 1, 3, 1, 1,    1,    1, 2, 1, 3, 1, 1, 1,     1,    1,    2, 1,
            /*1+*/ 2, 3, 1, 1,    1,    1, 2, 1, 2, 1, 1, 1,     1,    1,    2, 1,
            /*2+*/ 2, 3, 1, 1,    1,    1, 2, 1, 2, 1, 1, 1,     1,    1,    2, 1,
            /*3+*/ 2, 3, 1, 1,    1,    1, 2, 1, 2, 1, 1, 1,     1,    1,    2, 1,
            /*4+*/ 1, 1, 1, 1,    1,    1, 1, 1, 1, 1, 1, 1,     1,    1,    1, 1,
            /*5+*/ 1, 1, 1, 1,    1,    1, 1, 1, 1, 1, 1, 1,     1,    1,    1, 1,
            /*6+*/ 1, 1, 1, 1,    1,    1, 1, 1, 1, 1, 1, 1,     1,    1,    1, 1,
            /*7+*/ 1, 1, 1, 1,    1,    1, 1, 1, 1, 1, 1, 1,     1,    1,    1, 1,
            /*8+*/ 1, 1, 1, 1,    1,    1, 1, 1, 1, 1, 1, 1,     1,    1,    1, 1,
            /*9+*/ 1, 1, 1, 1,    1,    1, 1, 1, 1, 1, 1, 1,     1,    1,    1, 1,
            /*A+*/ 1, 1, 1, 1,    1,    1, 1, 1, 1, 1, 1, 1,     1,    1,    1, 1,
            /*B+*/ 1, 1, 1, 1,    1,    1, 1, 1, 1, 1, 1, 1,     1,    1,    1, 1,
            /*C+*/ 1, 1, 3, 3,    3,    1, 2, 1, 1, 1, 3, 1,     3,    3,    2, 1,
            /*D+*/ 1, 1, 3, NULL, 3,    1, 2, 1, 1, 1, 3, NULL,  3,    NULL, 2, 1,
            /*E+*/ 2, 1, 1, NULL, NULL, 1, 2, 1, 2, 1, 3, NULL,  NULL, NULL, 2, 1,
            /*F+*/ 2, 1, 1, 1,    NULL, 1, 2, 1, 2, 1, 3, 1,     NULL, NULL, 2, 1,
            };

            template <typename T>
            const std::vector<InstructionHandler<T>> PrefixedInstructionHandlerTable = {
            //    +0    +1    +2    +3    +4    +5    +6    +7    +8    +9    +A    +B    +C    +D    +E    +F
            /*0+*/RLC,  RLC,  RLC,  RLC,  RLC,  RLC,  RLC,  RLC,  RRC,  RRC,  RRC,  RRC,  RRC,  RRC,  RRC,  RRC,
            /*1+*/RL,   RL,   RL,   RL,   RL,   RL,   RL,   RL,   RR,   RR,   RR,   RR,   RR,   RR,   RR,   RR,
            /*2+*/SLA,  SLA,  SLA,  SLA,  SLA,  SLA,  SLA,  SLA,  SRA,  SRA,  SRA,  SRA,  SRA,  SRA,  SRA,  SRA,
            /*3+*/SWAP, SWAP, SWAP, SWAP, SWAP, SWAP, SWAP, SWAP, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
            /*4+*/BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,
            /*5+*/BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,
            /*6+*/BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,
            /*7+*/BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,  BIT,
            /*8+*/RES,  RES,  RES,  RES,  RES,  RES,  RES,  RES,  RES,  RES,  RES,  RES,  RES,  RES,  RES,  RES,
            /*9+*/RES,  RES,  RES,  RES,  RES,  RES,  RES,  RES,  RES,  RES,  RES,  RES,  RES,  RES,  RES,  RES,
            /*A+*/RES,  RES,  RES,  RES,  RES,  RES,  RES,  RES,  RES,  RES,  RES,  RES,  RES,  RES,  RES,  RES,
            /*B+*/RES,  RES,  RES,  RES,  RES,  RES,  RES,  RES,  RES,  RES,  RES,  RES,  RES,  RES,  RES,  RES,
            /*C+*/SET,  SET,  SET,  SET,  SET,  SET,  SET,  SET,  SET,  SET,  SET,  SET,  SET,  SET,  SET,  SET,
            /*D+*/SET,  SET,  SET,  SET,  SET,  SET,  SET,  SET,  SET,  SET,  SET,  SET,  SET,  SET,  SET,  SET,
            /*E+*/SET,  SET,  SET,  SET,  SET,  SET,  SET,  SET,  SET,  SET,  SET,  SET,  SET,  SET,  SET,  SET,
            /*F+*/SET,  SET,  SET,  SET,  SET,  SET,  SET,  SET,  SET,  SET,  SET,  SET,  SET,  SET,  SET,  SET,
            };
        };
    };
};
