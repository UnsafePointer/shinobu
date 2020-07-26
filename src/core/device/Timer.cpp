#include "core/device/Timer.hpp"
#include "common/Timing.hpp"
#include <bitset>

using namespace Core::Device::Timer;

Controller::Controller(Common::Logs::Level logLevel, std::unique_ptr<Core::Device::Interrupt::Controller> &interrupt) : logger(logLevel, "  [Timer]: "), interrupt(interrupt), DIV(), TIMA(), TMA(), control(), lastResult(), overflown() {

}

Controller::~Controller() {

}

uint8_t Controller::load(uint16_t offset) const {
    switch (offset) {
    case 0x0:
        return (DIV & 0xFF00) >> 8;
    case 0x1:
        return TIMA;
    case 0x2:
        return TMA;
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
        DIV = 0;
        return;
    case 0x1:
        TIMA = value;
        return;
    case 0x2:
        TMA = value;
        return;
    case 0x3:
        control._value = value;
        return;
    default:
        logger.logWarning("Unhandled Timer store at offset: %04x with value %02x", offset, value);
        return;
    }
}

void Controller::step(uint8_t cycles) {
    uint8_t steps = cycles / 4;
    while (steps > 0) {
        DIV += 4;
        steps--;

        if (overflown) {
            overflown = false;
            TIMA = TMA;
            interrupt->requestInterrupt(Interrupt::TIMER);
        }

        uint8_t bitPositionForCurrentClock = clocks[control._clock];
        std::bitset<16> dividerBits = std::bitset<16>(DIV);
        bool result = dividerBits.test(bitPositionForCurrentClock) & control.enable;
        if (lastResult && !result) {
            TIMA++;
            if (TIMA == 0) {
                overflown = true;
            }
        }
        lastResult = result;
    }
}
