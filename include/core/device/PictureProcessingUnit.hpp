#pragma once
#include "core/Memory.hpp"
#include "common/Logger.hpp"

namespace Core {
    namespace Device {
        namespace PictureProcessingUnit {
            const Core::Memory::Range AddressRange = Core::Memory::Range(0xFF40, 0x6);

            class Processor {
                Common::Logs::Logger logger;
            public:
                Processor(Common::Logs::Level logLevel);
                ~Processor();

                uint8_t load(uint16_t offset);
                void store(uint16_t offset, uint8_t value);
            };
        };
    };
};
