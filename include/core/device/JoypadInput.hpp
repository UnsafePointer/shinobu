#pragma once
#include <memory>
#include "common/Logger.hpp"
#include "core/Memory.hpp"
#include "shinobu/frontend/sdl2/GameController.hpp"

namespace Core {
    namespace Device {
        namespace Interrupt {
            class Controller;
        };

        namespace JoypadInput {
            union Joypad {
                uint8_t _value;
                struct {
                    uint8_t p10 : 1;
                    uint8_t p11 : 1;
                    uint8_t p12 : 1;
                    uint8_t p13 : 1;
                    uint8_t selectDirectionKeys : 1;
                    uint8_t selectButtonKeys : 1;
                    uint8_t unused : 2;
                };
                Joypad() : _value(0xFF) {}
            };

            const Core::Memory::Range AddressRange = Core::Memory::Range(0xFF00, 0x1);

            class Controller {
                Common::Logs::Logger logger;
                std::unique_ptr<Core::Device::Interrupt::Controller> &interrupt;

                Joypad joypad;
                Shinobu::Frontend::SDL2::GameController gameController;
            public:
                Controller(Common::Logs::Level logLevel, std::unique_ptr<Core::Device::Interrupt::Controller> &interrupt);
                ~Controller();

                uint8_t load() const;
                void store(uint8_t value);
                void updateJoypad();
            };
        };
    };
};
