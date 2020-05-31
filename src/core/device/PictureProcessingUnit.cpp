#include "core/device/PictureProcessingUnit.hpp"
#include <iostream>
#include "common/Timing.hpp"

using namespace Core::Device::PictureProcessingUnit;

Processor::Processor(Common::Logs::Level logLevel) : logger(logLevel, "  [PPU]: "), control(), status(), scrollY(), scrollX(), LY(), LYC(), steps() {

}

Processor::~Processor() {

}

uint8_t Processor::load(uint16_t offset) {
    switch (offset) {
    case 0x0:
        return control._value;
    case 0x1:
        return status._value;
    case 0x2:
        return scrollY;
    case 0x3:
        return scrollX;
    case 0x4:
        return LY;
    case 0x5:
        return LYC;
    default:
        logger.logWarning("Unhandled Picture Processing Unit load at offset: %04x", offset);
        return 0;
    }
}

void Processor::store(uint16_t offset, uint8_t value) {
    switch (offset) {
    case 0x0:
        control._value = value;
        return;
    case 0x1: {
        status._value = value;
        // TODO: update status mode and coincidence
        return;
    }
    case 0x2:
        scrollY = value;
        return;
    case 0x3:
        scrollX = value;
        return;
    case 0x4:
        logger.logWarning("Attempting to write read-only LY register with value: %02x", value);
        return;
    case 0x5:
        LYC = value;
        return;
    default:
        logger.logWarning("Unhandled Picture Processing Unit at offset: %04x with value: %04x", offset, value);
        return;
    }
}

void Processor::step(uint8_t cycles) {
    steps += cycles;
    if (steps >= CyclesPerScanline) {
        LY++;
        if (LY >= TotalScanlines) {
            LY = 0;
        }
        steps = 0;
    }
    return;
}
