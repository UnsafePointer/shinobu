#pragma once
#include <cstdint>
#include "core/Memory.hpp"
#include "common/Logger.hpp"

namespace Core {
    namespace Device {
        namespace SerialDataTransfer {
            union ControlRegister {
                uint8_t _value;
                struct {
                    uint8_t SIOCLK : 1;
                    uint8_t unused : 6;
                    uint8_t SIOEN : 1;
                };

                ControlRegister() : _value(0x0) {}
            };

            const Core::Memory::Range AddressRange = Core::Memory::Range(0xFF01, 0x2);

            class Controller {
                Common::Logs::Logger logger;
                uint8_t data;
                ControlRegister control;
                std::string ttyBuffer;

                void checkTTY(char c);
            public:
                Controller(Common::Logs::Level logLevel);
                ~Controller();

                uint8_t load(uint16_t offset);
                void store(uint16_t offset, uint8_t value);
            };
        };
    };
};
