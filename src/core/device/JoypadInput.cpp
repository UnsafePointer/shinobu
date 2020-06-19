#include "core/device/JoypadInput.hpp"
#include <SDL2/SDL.h>
#include "core/device/Interrupt.hpp"

using namespace Core::Device::JoypadInput;

Controller::Controller(Common::Logs::Level logLevel, std::unique_ptr<Core::Device::Interrupt::Controller> &interrupt) : logger(logLevel, "  [Joypad]: "), interrupt(interrupt), joypad() {}

Controller::~Controller() {}

uint8_t Controller::load() const {
    return joypad._value;
}

void Controller::store(uint8_t value) {
    joypad._value &= ~0x30;
    joypad._value |= value & 0x30;
}

void Controller::updateJoypad() {
    bool shouldTriggerInterrupt = false;
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    if (!joypad.selectDirectionKeys && joypad.selectButtonKeys) {
        if (state[SDL_SCANCODE_RIGHT]) {
            shouldTriggerInterrupt = !joypad.p10;
            joypad.p10 = 0x0;
        } else {
            joypad.p10 = 0x1;
        }
        if (state[SDL_SCANCODE_LEFT]) {
            shouldTriggerInterrupt = !joypad.p11;
            joypad.p11 = 0x0;
        } else {
            joypad.p11 = 0x1;
        }
        if (state[SDL_SCANCODE_UP]) {
            shouldTriggerInterrupt = !joypad.p12;
            joypad.p12 = 0x0;
        } else {
            joypad.p12 = 0x1;
        }
        if (state[SDL_SCANCODE_DOWN]) {
            shouldTriggerInterrupt = !joypad.p13;
            joypad.p13 = 0x0;
        } else {
            joypad.p13 = 0x1;
        }
    } else if (!joypad.selectButtonKeys && joypad.selectDirectionKeys) {
        if (state[SDL_SCANCODE_A]) {
            shouldTriggerInterrupt = !joypad.p10;
            joypad.p10 = 0x0;
        } else {
            joypad.p10 = 0x1;
        }
        if (state[SDL_SCANCODE_S]) {
            shouldTriggerInterrupt = !joypad.p11;
            joypad.p11 = 0x0;
        } else {
            joypad.p11 = 0x1;
        }
        if (state[SDL_SCANCODE_SPACE]) {
            shouldTriggerInterrupt = !joypad.p12;
            joypad.p12 = 0x0;
        } else {
            joypad.p12 = 0x1;
        }
        if (state[SDL_SCANCODE_RETURN]) {
            shouldTriggerInterrupt = !joypad.p13;
            joypad.p13 = 0x0;
        } else {
            joypad.p13 = 0x1;
        }
    }
    if (shouldTriggerInterrupt) {
        interrupt->requestInterrupt(Interrupt::JOYPAD);
    }
}
