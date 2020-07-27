#pragma once
#include <cstdint>
#include "common/Logger.hpp"
#include <memory>
#include "core/Memory.hpp"

namespace Core {
    namespace CPU {
        class Processor;
    };

    namespace Device {
        namespace Interrupt {
            enum Interrupt {
                VBLANK = 0,
                LCDSTAT = 1,
                TIMER = 2,
                SERIAL = 3,
                JOYPAD = 4
            };

            static const Interrupt ALL[] = { VBLANK, LCDSTAT, TIMER, SERIAL, JOYPAD };
            static const uint16_t VECTOR[] = { 0x40, 0x48, 0x50, 0x58, 0x60 };

            const Core::Memory::Range EnableAddressRange = Core::Memory::Range(0xFFFF, 0x1);

            union Enable {
                uint8_t _value;
                struct {
                    uint8_t VBlank : 1;
                    uint8_t LCDStatus : 1;
                    uint8_t timer : 1;
                    uint8_t serial : 1;
                    uint8_t joypad : 1;
                    uint8_t unusued : 3;
                };

                Enable() : _value(0x0) {}
            };

            const Core::Memory::Range FlagAddressRange = Core::Memory::Range(0xFF0F, 0x1);

            union Flag {
                uint8_t _value;
                struct {
                    uint8_t VBlank : 1;
                    uint8_t LCDStatus : 1;
                    uint8_t timer : 1;
                    uint8_t serial : 1;
                    uint8_t joypad : 1;
                    uint8_t unusued : 3;
                };

                Flag() : _value(0x0) {}
            };

            class Controller {
                friend class Core::CPU::Processor;

                Common::Logs::Logger logger;

                bool IME;
                Enable enable;
                Flag flag;
            public:
                Controller(Common::Logs::Level logLevel);
                ~Controller();

                void requestInterrupt(Interrupt interrupt);
                void clearInterrupt(Interrupt interrupt);
                bool shouldExecute(Interrupt interrupt) const;
                uint8_t loadEnable() const;
                void storeEnable(uint8_t value);
                uint8_t loadFlag() const;
                void storeFlag(uint8_t value);
            };
        };
    };
};
