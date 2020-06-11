#pragma once
#include <cstdint>
#include <memory>
#include "core/device/Interrupt.hpp"
#include "core/Memory.hpp"

namespace Core {
    namespace Device {
        namespace Timer {
            const Core::Memory::Range AddressRange = Core::Memory::Range(0xFF04, 0x4);

            union Control {
                uint8_t _value;
                struct {
                    uint8_t _clock : 2;
                    uint8_t enable : 1;
                    uint8_t unusued : 5;
                };

                Control() : _value(0x0) {}
            };

            class Controller {
                Common::Logs::Logger logger;
                std::unique_ptr<Core::Device::Interrupt::Controller> &interrupt;

                uint8_t divider;
                uint8_t counter;
                uint8_t modulo;
                Control control;
            public:
                Controller(Common::Logs::Level logLevel, std::unique_ptr<Core::Device::Interrupt::Controller> &interrupt);
                ~Controller();

                uint8_t load(uint16_t offset) const;
                void store(uint16_t offset, uint8_t value);
            };
        };
    };
};
