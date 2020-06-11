#include "core/device/Timer.hpp"

using namespace Core::Device::Timer;

Controller::Controller(Common::Logs::Level logLevel, std::unique_ptr<Core::Device::Interrupt::Controller> &interrupt) : logger(logLevel, "  [Timer]: "), interrupt(interrupt) {

}

Controller::~Controller() {

}

uint8_t Controller::load(uint16_t offset) const {
    switch (offset) {
    case 0x0:
        return divider;
    case 0x1:
        return counter;
    case 0x2:
        return modulo;
    case 0x3:
        return control._value;
    default:
        logger.logWarning("Unhandled Timer load at offset: %04x", offset);
        return 0;
    }
}

void Controller::store(uint16_t offset, uint8_t value) {
    switch (offset) {
    case 0x0:
        divider = value;
        return;
    case 0x1:
        counter = value;
        return;
    case 0x2:
        modulo = value;
        return;
    case 0x3:
        control._value = value;
        return;
    default:
        logger.logWarning("Unhandled Timer store at offset: %04x with value %02x", offset, value);
        return;
    }
}
