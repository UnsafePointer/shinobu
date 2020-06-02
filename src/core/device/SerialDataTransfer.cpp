#include "core/device/SerialDataTransfer.hpp"
#include <iostream>

using namespace Core::Device::SerialDataTransfer;

Controller::Controller(Common::Logs::Level logLevel) : logger(logLevel, "  [Serial]: ") {

}

Controller::~Controller() {

}

uint8_t Controller::load(uint16_t offset) {
    switch (offset) {
    case 0x0:
        return data;
    case 0x1:
        control.unused = 0x3F; // Read as 1
        return control._value;
    default:
        logger.logWarning("Unhandled Serial Communication load at offset: %04x", offset);
        return 0;
    }
}

void Controller::store(uint16_t offset, uint8_t value) {
    switch (offset) {
    case 0x0:
        data = value;
        return;
    case 0x1:
        control._value = value;
        return;
    default:
        logger.logWarning("Unhandled Serial Communication load at offset: %04x with value: %04x", offset, value);
        return;
    }
}
