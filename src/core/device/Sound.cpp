#include "core/device/Sound.hpp"

using namespace Core::Device::Sound;

Controller::Controller(Common::Logs::Level logLevel) : logger(logLevel, "  [Sound]: "), squareOne(), squareTwo() {}
Controller::~Controller() {}

uint8_t Controller::load(uint16_t offset) const {
    switch (offset) {
    case 0x0:
        return squareOne._NR10._value;
    case 0x1:
        return squareOne._NR10._value;
    case 0x2:
        return squareOne._NR10._value;
    case 0x3:
        return squareOne._NR10._value;
    case 0x4:
        return squareOne._NR10._value;
    case 0x6:
        return squareOne._NR10._value;
    case 0x7:
        return squareOne._NR10._value;
    case 0x8:
        return squareOne._NR10._value;
    case 0x9:
        return squareOne._NR10._value;
    default:
        logger.logWarning("Unhandled sound controller load at offset: %02x", offset);
        return 0xFF;
    }
}

void Controller::store(uint16_t offset, uint8_t value) {
    switch (offset) {
    case 0x0:
        squareOne._NR10._value = value;
        return;
    case 0x1:
        squareOne._NR10._value = value;
        return;
    case 0x2:
        squareOne._NR10._value = value;
        return;
    case 0x3:
        squareOne._NR10._value = value;
        return;
    case 0x4:
        squareOne._NR10._value = value;
        return;
    case 0x6:
        squareOne._NR10._value = value;
        return;
    case 0x7:
        squareOne._NR10._value = value;
        return;
    case 0x8:
        squareOne._NR10._value = value;
        return;
    case 0x9:
        squareOne._NR10._value = value;
        return;
    default:
        logger.logWarning("Unhandled sound controller load at offset: %02x", offset);
        return;
    }
}
