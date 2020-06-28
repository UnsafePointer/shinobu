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
    case 0xA:
        return wave._NR30._value;
    case 0xB:
        return wave._NR31;
    case 0xC:
        return wave._NR32._value;
    case 0xD:
        return wave._NR33;
    case 0xE:
        return wave._NR34._value;
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
    case 0xA:
        wave._NR30._value = value;
        return;
    case 0xB:
        wave._NR31 = value;
        return;
    case 0xC:
        wave._NR32._value = value;
        return;
    case 0xD:
        wave._NR33 = value;
        return;
    case 0xE:
        wave._NR34._value = value;
        return;
    default:
        logger.logWarning("Unhandled sound controller load at offset: %02x", offset);
        return;
    }
}
