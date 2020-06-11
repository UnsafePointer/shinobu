#include "core/device/Interrupt.hpp"
#include "core/cpu/CPU.hpp"

using namespace Core::Device::Interrupt;

Controller::Controller(Common::Logs::Level logLevel) : logger(logLevel, "  [Interrupt]: "), processor(nullptr), IME(), enable(), flag() {

}

Controller::~Controller() {

}

void Controller::setProcessor(std::unique_ptr<Core::CPU::Processor> &processor) {
    this->processor = processor.get();
}

void Controller::updateIME(bool value) {
    IME = value;
}

void Controller::clearInterrupt(Interrupt interrupt) {
    uint8_t interruptMask = ~(0x1 << interrupt);
    flag._value &= interruptMask;
}

void Controller::requestInterrupt(Interrupt interrupt) {
    processor->unhalt();
    uint8_t interruptMask = 0x1 << interrupt;
    flag._value |= interruptMask;
}

void Controller::serveInterrupts() {
    if (!IME) {
        return;
    }
    if (flag._value == 0) {
        return;
    }

    for (const auto& interrupt : ALL) {
        uint8_t interruptMask = 0x1 << interrupt;
        if ((flag._value & interruptMask) && (enable._value & interruptMask)) {
            executeInterrupt(interrupt);
        }
    }
}

void Controller::executeInterrupt(Interrupt interrupt) {
    processor->executeInterrupt(interrupt);
    IME = false;
    clearInterrupt(interrupt);
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
