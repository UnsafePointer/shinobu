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
    currentSourceAddress = source;
    remainingTransfers = 0xA0;
    currentDestinationAddress = 0xFE00;
}

void Controller::step(uint8_t cycles) {
    uint8_t steps = cycles / 4;
    while (steps > 0) {
        if (remainingTransfers <= 0) {
            break;
        }

        uint8_t value = memoryController->load(currentSourceAddress, false);
        memoryController->store(currentDestinationAddress, value, false);

        currentSourceAddress++;
        currentDestinationAddress++;
        remainingTransfers--;

        steps--;
    }
}
