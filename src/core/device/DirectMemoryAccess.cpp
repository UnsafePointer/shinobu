#include "core/device/DirectMemoryAccess.hpp"

using namespace Core::Device::DirectMemoryAccess;

Request::Request(uint8_t value) {
    uint16_t source = value;
    source <<= 8;
    if (source >= 0xFE00) {
        source -= 0x2000;
    }
    startSourceAddress = source;
    startDestinationAddress = 0xFE00;
    currentSourceAddress = source;
    currentDestinationAddress = 0xFE00;
    remainingTransfers = 0xA0;
    preparing = true;
}

Controller::Controller(Common::Logs::Level logLevel) : logger(logLevel, "  [DMA]: "), memoryController(nullptr), requests() {

}

Controller::~Controller() {

}

void Controller::setMemoryController(std::unique_ptr<Core::Memory::Controller> &memoryController) {
    this->memoryController = memoryController.get();
}

void Controller::execute(uint8_t value) {
    Request request = Request(value);
    requests.push_back(request);
}

void Controller::step(uint8_t cycles) {
    uint8_t steps = cycles / 4;
    while (steps > 0) {
        if (requests.empty()) {
            break;
        }

        steps--;

        Request &request = requests.front();
        if (request.preparing) {
            request.preparing = false;
            continue;
        }

        uint8_t value = memoryController->load(request.currentSourceAddress, false, true);
        memoryController->store(request.currentDestinationAddress, value, false, true);

        request.currentSourceAddress++;
        request.currentDestinationAddress++;
        request.remainingTransfers--;

        if (request.remainingTransfers <= 0) {
            requests.pop_back();
        }
    }
}

bool Controller::isActive() const {
    return !requests.empty();
}
