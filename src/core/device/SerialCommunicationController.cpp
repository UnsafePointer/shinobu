#include "core/device/SerialCommunicationController.hpp"
#include <iostream>

using namespace Core::Device::SerialCommunication;

Controller::Controller() {

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
        std::cout << "Unhandled Serial Communication load at offset: 0x" << std::hex << (unsigned int)offset << std::endl;
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
        std::cout << "Unhandled Serial Communication load at offset: 0x" << std::hex << (unsigned int)offset << " with value: 0x" << std::hex << (unsigned int)value << std::endl;
        return;
    }
}
