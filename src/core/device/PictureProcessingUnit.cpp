#include "core/device/PictureProcessingUnit.hpp"
#include <iostream>

using namespace Core::Device::PictureProcessingUnit;

Processor::Processor(Common::Logs::Level logLevel) : logger(logLevel, "  [PPU]: ") {

}

Processor::~Processor() {

}

uint8_t Processor::load(uint16_t offset) {
    switch (offset) {
    default:
        logger.logWarning("Unhandled Picture Processing Unit load at offset: %04x", offset);
        return 0;
    }
}

void Processor::store(uint16_t offset, uint8_t value) {
    switch (offset) {
    default:
        logger.logWarning("Unhandled Picture Processing Unit at offset: %04x with value: %04x", offset, value);
        return;
    }
}
