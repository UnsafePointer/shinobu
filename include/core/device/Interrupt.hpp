#pragma once
#include <cstdint>
#include "common/Logger.hpp"

namespace Core {
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
                Common::Logs::Logger logger;


                bool IME;
                Enable enable;
                Flag flag;

                void clearInterrupt(Interrupt interrupt);
                void executeInterrupt(Interrupt interrupt);
            public:
                Controller(Common::Logs::Level logLevel);
                ~Controller();

                void updateIME(bool value);
                void requestInterrupt(Interrupt interrupt);
                void serveInterrupts();
            };
        };
    };
};
