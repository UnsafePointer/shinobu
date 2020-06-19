#include "core/device/JoypadInput.hpp"

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
