#include "core/device/DirectMemoryAccess.hpp"
#include <algorithm>

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
    canceling = false;
}

Controller::Controller(Common::Logs::Level logLevel) : logger(logLevel, "  [DMA]: "), memoryController(nullptr), requests() {

}

Controller::~Controller() {

}

void Controller::setMemoryController(std::unique_ptr<Core::Memory::Controller> &memoryController) {
    this->memoryController = memoryController.get();
}

void Controller::execute(uint8_t value) {
    for (auto& request : requests) {
        request.canceling = true;
    }
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

        for (auto& request : requests) {
            if (request.preparing) {
                request.preparing = false;
                continue;
            }

            uint8_t value = memoryController->load(request.currentSourceAddress, false, true);
            memoryController->store(request.currentDestinationAddress, value, false, true);

            request.currentSourceAddress++;
            request.currentDestinationAddress++;
            request.remainingTransfers--;
        }

        requests.erase(std::remove_if(requests.begin(), requests.end(), [](Request request) {
            return request.remainingTransfers <= 0 || request.canceling;
        }), requests.end());
    }
}

bool Controller::isActive() const {
    if (requests.empty()) {
        return false;
    }
    bool active = false;
    for (const auto& request : requests) {
        if (!request.preparing) {
            active = true;
            break;
        }
    }
    return active;
}
