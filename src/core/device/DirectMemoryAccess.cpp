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
    active = false;
    preparing = true;
    canceling = false;
}

Controller::Controller(Common::Logs::Level logLevel) : logger(logLevel, "  [DMA]: "), memoryController(nullptr), requests(), HDMA1(), HDMA2(), HDMA3(), HDMA4(), _HDMA5() {

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

        requests.erase(std::remove_if(requests.begin(), requests.end(), [](Request request) {
            return request.remainingTransfers <= 0 || request.canceling;
        }), requests.end());

        steps--;

        for (auto& request : requests) {
            if (request.preparing) {
                request.preparing = false;
                continue;
            }
            request.active = true;

            uint8_t value = memoryController->load(request.currentSourceAddress, false, true);
            memoryController->store(request.currentDestinationAddress, value, false, true);

            request.currentSourceAddress++;
            request.currentDestinationAddress++;
            request.remainingTransfers--;
        }
    }
}

bool Controller::isActive() const {
    if (requests.empty()) {
        return false;
    }
    bool active = false;
    for (const auto& request : requests) {
        if (request.active) {
            active = true;
            break;
        }
    }
    return active;
}

uint8_t Controller::HDMALoad(uint16_t offset) const {
    if (cgbFlag == Core::ROM::CGBFlag::DMG) {
        logger.logWarning("Attempting to load HDMA register at offset: %04x on DMG mode", offset);
        return 0xFF;
    }
    switch (offset) {
    case 0x0:
        return HDMA1;
    case 0x1:
        return HDMA2;
    case 0x2:
        return HDMA3;
    case 0x3:
        return HDMA4;
    case 0x4:
        return _HDMA5._value;
    default:
        logger.logWarning("Unhandled HDMA register load at offset: %04x", offset);
        return 0xFF;
    }
}

void Controller::HDMAStore(uint16_t offset, uint8_t value) {
    if (cgbFlag == Core::ROM::CGBFlag::DMG) {
        logger.logWarning("Attempting to load HDMA register at offset: %04x with value: %02x on DMG mode", offset, value);
        return;
    }
    switch (offset) {
    case 0x0:
        HDMA1 = value;
        break;
    case 0x1:
        HDMA2 = value;
        break;
    case 0x2:
        HDMA3 = value;
        break;
    case 0x3:
        HDMA4 = value;
        break;
    case 0x4: {
        _HDMA5._value = value;
        uint16_t source = (((uint16_t)HDMA1) << 8) | HDMA2;
        source &= 0xFFF0;
        uint16_t destination = (((uint16_t)HDMA3) << 8) | HDMA4;
        destination &= 0x1FF0;
        destination += 0x8000;
        uint16_t length = (((uint16_t)_HDMA5.length) + 1) * 0x10;
        executeHDMA(source, destination, length);
        _HDMA5._value = 0xFF;
        break;
    }
    default:
        logger.logWarning("Unhandled HDMA register store at offset: %04x with value %02x", offset, value);
        break;
    }
}

void Controller::executeHDMA(uint16_t source, uint16_t destination, uint16_t length) {
    uint16_t sourceEnd = source + length;
    while (source <= sourceEnd) {
        uint8_t value = memoryController->load(source, true, true);
        memoryController->store(destination, value, true, true);
        source++;
        destination++;
    }
}
