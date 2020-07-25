#include "core/device/DirectMemoryAccess.hpp"

using namespace Core::Device::DirectMemoryAccess;

Controller::Controller(Common::Logs::Level logLevel) : logger(logLevel, "  [DMA]: "), memoryController(nullptr) {

}

Controller::~Controller() {

}

void Controller::setMemoryController(std::unique_ptr<Core::Memory::Controller> &memoryController) {
    this->memoryController = memoryController.get();
}

void Controller::execute(uint8_t value) {
    uint16_t source = value;
    source <<= 8;
    if (source >= 0xFE00) {
        source -= 0x2000;
    }
    uint16_t sourceEnd = source + 0x9F;
    uint16_t destination = 0xFE00;
    while (source <= sourceEnd) {
        uint8_t value = memoryController->load(source, false);
        memoryController->store(destination, value, false);
        source++;
        destination++;
    }
}
