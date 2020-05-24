#pragma once
#include "core/Memory.hpp"

namespace Core {
    namespace Device {
        namespace PictureProcessingUnit {
            const Core::Memory::Range AddressRange = Core::Memory::Range(0xFF40, 0x6);

            class Processor {

            public:
                Processor();
                ~Processor();

                uint8_t load(uint16_t offset);
                void store(uint16_t offset, uint8_t value);
            };
        };
    };
};
