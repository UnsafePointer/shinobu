#include "core/device/PictureProcessingUnit.hpp"
#include <iostream>

using namespace Core::Device::PictureProcessingUnit;

Processor::Processor() {

}

Processor::~Processor() {

}

uint8_t Processor::load(uint16_t offset) {
    switch (offset) {
    default:
        std::cout << "Unhandled Picture Processing Unit load at offset: 0x" << std::hex << (unsigned int)offset << std::endl;
        return 0;
    }
}

void Processor::store(uint16_t offset, uint8_t value) {
    switch (offset) {
    default:
        std::cout << "Unhandled Picture Processing Unit at offset: 0x" << std::hex << (unsigned int)offset << " with value: 0x" << std::hex << (unsigned int)value << std::endl;
        return;
    }
}
