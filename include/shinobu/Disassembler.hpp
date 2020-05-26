#pragma once
#include <memory>
#include <vector>
#include <string>

namespace Core::CPU {
    namespace Instructions {
        union Instruction;
    };
    class Processor;
};


namespace Shinobu {
    namespace Disassembler {
        typedef std::string (*InstructionHandler) (std::unique_ptr<Core::CPU::Processor> &processor, Core::CPU::Instructions::Instruction instruction);

        std::string NOP(std::unique_ptr<Core::CPU::Processor> &processor, Core::CPU::Instructions::Instruction instruction);
        std::string JP_U16(std::unique_ptr<Core::CPU::Processor> &processor, Core::CPU::Instructions::Instruction instruction);
        std::string DI(std::unique_ptr<Core::CPU::Processor> &processor, Core::CPU::Instructions::Instruction instruction);
        std::string LD_RR_NN(std::unique_ptr<Core::CPU::Processor> &processor, Core::CPU::Instructions::Instruction instruction);
        std::string RST_N(std::unique_ptr<Core::CPU::Processor> &processor, Core::CPU::Instructions::Instruction instruction);
        std::string INC_R(std::unique_ptr<Core::CPU::Processor> &processor, Core::CPU::Instructions::Instruction instruction);
        std::string RET(std::unique_ptr<Core::CPU::Processor> &processor, Core::CPU::Instructions::Instruction instruction);
        std::string LD_NN_A(std::unique_ptr<Core::CPU::Processor> &processor, Core::CPU::Instructions::Instruction instruction);
        std::string LD_U8(std::unique_ptr<Core::CPU::Processor> &processor, Core::CPU::Instructions::Instruction instruction);
        std::string LDH_N_A(std::unique_ptr<Core::CPU::Processor> &processor, Core::CPU::Instructions::Instruction instruction);
        std::string DEC_RR(std::unique_ptr<Core::CPU::Processor> &processor, Core::CPU::Instructions::Instruction instruction);
        std::string CALL_NN(std::unique_ptr<Core::CPU::Processor> &processor, Core::CPU::Instructions::Instruction instruction);
        std::string LD_R_R(std::unique_ptr<Core::CPU::Processor> &processor, Core::CPU::Instructions::Instruction instruction);
        std::string JR_I8(std::unique_ptr<Core::CPU::Processor> &processor, Core::CPU::Instructions::Instruction instruction);
        std::string LD_INDIRECT(std::unique_ptr<Core::CPU::Processor> &processor, Core::CPU::Instructions::Instruction instruction);
        std::string PUSH_RR(std::unique_ptr<Core::CPU::Processor> &processor, Core::CPU::Instructions::Instruction instruction);
        std::string POP_RR(std::unique_ptr<Core::CPU::Processor> &processor, Core::CPU::Instructions::Instruction instruction);
        std::string INC_RR(std::unique_ptr<Core::CPU::Processor> &processor, Core::CPU::Instructions::Instruction instruction);
        std::string EI(std::unique_ptr<Core::CPU::Processor> &processor, Core::CPU::Instructions::Instruction instruction);
        std::string OR(std::unique_ptr<Core::CPU::Processor> &processor, Core::CPU::Instructions::Instruction instruction);
        std::string JR_CC_I8(std::unique_ptr<Core::CPU::Processor> &processor, Core::CPU::Instructions::Instruction instruction);
        std::string STOP(std::unique_ptr<Core::CPU::Processor> &processor, Core::CPU::Instructions::Instruction instruction);
        std::string CALL_CC_NN(std::unique_ptr<Core::CPU::Processor> &processor, Core::CPU::Instructions::Instruction instruction);
        std::string ADD(std::unique_ptr<Core::CPU::Processor> &processor, Core::CPU::Instructions::Instruction instruction);
        std::string LD_NN_SP(std::unique_ptr<Core::CPU::Processor> &processor, Core::CPU::Instructions::Instruction instruction);
        std::string RLCA(std::unique_ptr<Core::CPU::Processor> &processor, Core::CPU::Instructions::Instruction instruction);
        std::string LD_A_NN(std::unique_ptr<Core::CPU::Processor> &processor, Core::CPU::Instructions::Instruction instruction);
        std::string SBC_A(std::unique_ptr<Core::CPU::Processor> &processor, Core::CPU::Instructions::Instruction instruction);
        std::string DEC_R(std::unique_ptr<Core::CPU::Processor> &processor, Core::CPU::Instructions::Instruction instruction);
        std::string XOR_A(std::unique_ptr<Core::CPU::Processor> &processor, Core::CPU::Instructions::Instruction instruction);
        std::string ADC_A(std::unique_ptr<Core::CPU::Processor> &processor, Core::CPU::Instructions::Instruction instruction);
        std::string JP_HL(std::unique_ptr<Core::CPU::Processor> &processor, Core::CPU::Instructions::Instruction instruction);
        std::string RRA(std::unique_ptr<Core::CPU::Processor> &processor, Core::CPU::Instructions::Instruction instruction);
        std::string RET_CC(std::unique_ptr<Core::CPU::Processor> &processor, Core::CPU::Instructions::Instruction instruction);
        std::string RLC(std::unique_ptr<Core::CPU::Processor> &processor, Core::CPU::Instructions::Instruction instruction);
        std::string CP_A(std::unique_ptr<Core::CPU::Processor> &processor, Core::CPU::Instructions::Instruction instruction);
        std::string LDH_A_N(std::unique_ptr<Core::CPU::Processor> &processor, Core::CPU::Instructions::Instruction instruction);

        const std::vector<InstructionHandler> InstructionHandlerTable = {
        //    +0         +1        +2           +3      +4          +5       +6      +7      +8        +9      +A           +B      +C          +D          +E      +F
        /*0+*/ NOP,      LD_RR_NN, LD_INDIRECT, INC_RR, INC_R,      DEC_R,   LD_U8,  RLCA,   LD_NN_SP, NULL,   LD_INDIRECT, DEC_RR, INC_R,      DEC_R,      LD_U8,  NULL,
        /*1+*/ STOP,     LD_RR_NN, LD_INDIRECT, INC_RR, INC_R,      DEC_R,   LD_U8,  NULL,   JR_I8,    NULL,   LD_INDIRECT, DEC_RR, INC_R,      DEC_R,      LD_U8,  RRA,
        /*2+*/ JR_CC_I8, LD_RR_NN, LD_INDIRECT, INC_RR, INC_R,      DEC_R,   LD_U8,  NULL,   JR_CC_I8, NULL,   LD_INDIRECT, DEC_RR, INC_R,      DEC_R,      LD_U8,  NULL,
        /*3+*/ JR_CC_I8, LD_RR_NN, LD_INDIRECT, INC_RR, INC_R,      DEC_R,   LD_U8,  NULL,   JR_CC_I8, NULL,   LD_INDIRECT, DEC_RR, INC_R,      DEC_R,      LD_U8,  NULL,
        /*4+*/ LD_R_R,   LD_R_R,   LD_R_R,      LD_R_R, LD_R_R,     LD_R_R,  LD_R_R, LD_R_R, LD_R_R,   LD_R_R, LD_R_R,      LD_R_R, LD_R_R,     LD_R_R,     LD_R_R, LD_R_R,
        /*5+*/ LD_R_R,   LD_R_R,   LD_R_R,      LD_R_R, LD_R_R,     LD_R_R,  LD_R_R, LD_R_R, LD_R_R,   LD_R_R, LD_R_R,      LD_R_R, LD_R_R,     LD_R_R,     LD_R_R, LD_R_R,
        /*6+*/ LD_R_R,   LD_R_R,   LD_R_R,      LD_R_R, LD_R_R,     LD_R_R,  LD_R_R, LD_R_R, LD_R_R,   LD_R_R, LD_R_R,      LD_R_R, LD_R_R,     LD_R_R,     LD_R_R, LD_R_R,
        /*7+*/ LD_R_R,   LD_R_R,   LD_R_R,      LD_R_R, LD_R_R,     LD_R_R,  NULL,   LD_R_R, LD_R_R,   LD_R_R, LD_R_R,      LD_R_R, LD_R_R,     LD_R_R,     LD_R_R, LD_R_R,
        /*8+*/ ADD,      ADD,      ADD,         ADD,    ADD,        ADD,     ADD,    ADD,    ADC_A,    ADC_A,  ADC_A,       ADC_A,  ADC_A,      ADC_A,      ADC_A,  ADC_A,
        /*9+*/ NULL,     NULL,     NULL,        NULL,   NULL,       NULL,    NULL,   NULL,   SBC_A,    SBC_A,  SBC_A,       SBC_A,  SBC_A,      SBC_A,      SBC_A,  SBC_A,
        /*A+*/ NULL,     NULL,     NULL,        NULL,   NULL,       NULL,    NULL,   NULL,   XOR_A,    XOR_A,  XOR_A,       XOR_A,  XOR_A,      XOR_A,      XOR_A,  XOR_A,
        /*B+*/ OR,       OR,       OR,          OR,     OR,         OR,      OR,     OR,     CP_A,     CP_A,   CP_A,        CP_A,   CP_A,       CP_A,       CP_A,   CP_A,
        /*C+*/ RET_CC,   POP_RR,   NULL,        JP_U16, CALL_CC_NN, PUSH_RR, RST_N,  ADD,    RET_CC,   RET,    NULL,        NULL,   CALL_CC_NN, CALL_NN,    ADC_A,  RST_N,
        /*D+*/ RET_CC,   POP_RR,   NULL,        NULL,   CALL_CC_NN, PUSH_RR, RST_N,  NULL,   RET_CC,   NULL,   NULL,        NULL,   CALL_CC_NN, NULL,       SBC_A,  RST_N,
        /*E+*/ LDH_N_A,  POP_RR,   NULL,        NULL,   NULL,       PUSH_RR, RST_N,  NULL,   NULL,     JP_HL,  LD_NN_A,     NULL,   NULL,       NULL,       XOR_A,  RST_N,
        /*F+*/ LDH_A_N,  POP_RR,   NULL,        DI,     NULL,       PUSH_RR, RST_N,  OR,     NULL,     NULL,   LD_A_NN,     EI,     NULL,       NULL,       CP_A,   RST_N,
        };

        const std::vector<InstructionHandler> PrefixedInstructionHandlerTable = {
        //    +0    +1    +2    +3    +4    +5    +6    +7    +8    +9    +A    +B    +C    +D    +E    +F
        /*0+*/RLC,  RLC,  RLC,  RLC,  RLC,  RLC,  RLC,  RLC,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        /*1+*/NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        /*2+*/NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        /*3+*/NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        /*4+*/NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        /*5+*/NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        /*6+*/NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        /*7+*/NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
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