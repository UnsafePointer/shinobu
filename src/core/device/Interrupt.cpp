#include "core/device/Interrupt.hpp"
#include "core/cpu/CPU.hpp"

using namespace Core::Device::Interrupt;

Controller::Controller(Common::Logs::Level logLevel) : logger(logLevel, "  [Interrupt]: "), IME(), enable(), flag() {

}

Controller::~Controller() {

}

void Controller::clearInterrupt(Interrupt interrupt) {
    uint8_t interruptMask = ~(0x1 << interrupt);
    flag._value &= interruptMask;
}

void Controller::requestInterrupt(Interrupt interrupt) {
    uint8_t interruptMask = 0x1 << interrupt;
    flag._value |= interruptMask;
}

bool Controller::shouldExecute(Interrupt interrupt) const {
    uint8_t interruptMask = 0x1 << interrupt;
    return ((flag._value & interruptMask) && (enable._value & interruptMask));
}

uint8_t Controller::loadEnable() const {
    return enable._value;
}

void Controller::storeEnable(uint8_t value) {
    enable._value = value;
}

uint8_t Controller::loadFlag() const {
    return flag._value;
}

void Controller::storeFlag(uint8_t value) {
    flag._value = value;
}
