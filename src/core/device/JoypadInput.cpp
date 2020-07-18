#include "core/device/JoypadInput.hpp"
#include <SDL2/SDL.h>
#include "core/device/Interrupt.hpp"

using namespace Core::Device::JoypadInput;
using namespace Shinobu::Frontend::SDL2;

Controller::Controller(Common::Logs::Level logLevel, std::unique_ptr<Core::Device::Interrupt::Controller> &interrupt) : logger(logLevel, "  [Joypad]: "), interrupt(interrupt), joypad(), gameController(Common::Logs::Level::Warning) {}

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
    if (!joypad.selectDirectionKeys && joypad.selectButtonKeys) {
        if (gameController.isButtonPressed(Button::Right)) {
            shouldTriggerInterrupt = !joypad.p10;
            joypad.p10 = 0x0;
        } else {
            joypad.p10 = 0x1;
        }
        if (gameController.isButtonPressed(Button::Left)) {
            shouldTriggerInterrupt = !joypad.p11;
            joypad.p11 = 0x0;
        } else {
            joypad.p11 = 0x1;
        }
        if (gameController.isButtonPressed(Button::Up)) {
            shouldTriggerInterrupt = !joypad.p12;
            joypad.p12 = 0x0;
        } else {
            joypad.p12 = 0x1;
        }
        if (gameController.isButtonPressed(Button::Down)) {
            shouldTriggerInterrupt = !joypad.p13;
            joypad.p13 = 0x0;
        } else {
            joypad.p13 = 0x1;
        }
    } else if (!joypad.selectButtonKeys && joypad.selectDirectionKeys) {
        if (gameController.isButtonPressed(Button::A)) {
            shouldTriggerInterrupt = !joypad.p10;
            joypad.p10 = 0x0;
        } else {
            joypad.p10 = 0x1;
        }
        if (gameController.isButtonPressed(Button::B)) {
            shouldTriggerInterrupt = !joypad.p11;
            joypad.p11 = 0x0;
        } else {
            joypad.p11 = 0x1;
        }
        if (gameController.isButtonPressed(Button::Select)) {
            shouldTriggerInterrupt = !joypad.p12;
            joypad.p12 = 0x0;
        } else {
            joypad.p12 = 0x1;
        }
        if (gameController.isButtonPressed(Button::Start)) {
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

bool Controller::hasGameController() const {
    return gameController.hasGameController();
}
