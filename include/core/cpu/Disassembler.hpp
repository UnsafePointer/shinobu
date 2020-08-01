#pragma once
#include "common/Logger.hpp"
#include <memory>
#include "core/cpu/Instructions.hpp"

namespace Core {
    namespace CPU {
        class Processor;

        namespace Disassembler {
            class Disassembler {
                Common::Logs::Logger logger;
                std::unique_ptr<Processor> &processor;

                bool enabled;
            public:
                Disassembler(Common::Logs::Level logLevel, std::unique_ptr<Processor> &processor);
                ~Disassembler();

                void disassembleWhileExecuting(Instructions::Instruction instruction) const;
                std::string disassemble(Instructions::Instruction instruction);
                void toggleEnabled();
                bool canDisassemble() const;
            };
        };
    };
};
