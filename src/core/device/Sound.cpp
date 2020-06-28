#include "core/device/Sound.hpp"
#include "common/Timing.hpp"

using namespace Core::Device::Sound;

Controller::Controller(Common::Logs::Level logLevel) : logger(logLevel, "  [Sound]: "), squareOne(), squareTwo(), wave(), noise(), control(), waveTable() {}
Controller::~Controller() {}

void Shared::Channel::loadLengthCounter(uint8_t value) {
    lengthCounter = value;
}

void Shared::Channel::step(uint8_t cycles) {
    if (!_NRX4.lengthEnable || lengthCounter == 0) {
        return;
    }
    lengthCounterSteps += cycles;
    if (lengthCounterSteps >= SoundControllerLengthStep) {
        lengthCounterSteps %= SoundControllerLengthStep;
        lengthCounterSteps += SoundControllerLengthStep;
        lengthCounter--;
        if (lengthCounter <= 0) {
            enabled = false;
        }
    }
}

uint8_t Controller::load(uint16_t offset) {
    switch (offset) {
    case 0x0:
        return squareOne._NR10._value | 0x80;
    case 0x1:
        return squareOne._NR11._value | 0x3F;
    case 0x2:
        return squareOne._NR12._value | 0x00;
    case 0x3:
        return squareOne._NR13 | 0xFF;
    case 0x4:
        return squareOne._NRX4._value | 0xBF;
    case 0x5:
        return squareTwo._NR20 | 0xFF;
    case 0x6:
        return squareTwo._NR21._value | 0x3F;
    case 0x7:
        return squareTwo._NR22._value | 0x00;
    case 0x8:
        return squareTwo._NR23 | 0xFF;
    case 0x9:
        return squareTwo._NRX4._value | 0xBF;
    case 0xA:
        return wave._NR30._value | 0x7F;
    case 0xB:
        return wave._NR31 | 0xFF;
    case 0xC:
        return wave._NR32._value | 0x9F;
    case 0xD:
        return wave._NR33 | 0xFF;
    case 0xE:
        return wave._NRX4._value | 0xBF;
    case 0xF:
        return noise._NR40 | 0xFF;
    case 0x10:
        return noise._NR41._value | 0xFF;
    case 0x11:
        return noise._NR42._value | 0x00;
    case 0x12:
        return noise._NR43._value | 0x00;
    case 0x13:
        return noise._NRX4._value | 0xBF;
    case 0x14:
        return control._NR50._value | 0x00;
    case 0x15:
        return control._NR51._value | 0x00;
    case 0x16:
        control._NR52.squareOneLengthStatus = squareOne.lengthCounter != 0;
        control._NR52.squareTwoLengthStatus = squareTwo.lengthCounter != 0;
        control._NR52.waveLengthStatus = wave.lengthCounter != 0;
        control._NR52.noiseLengthStatus = noise.lengthCounter != 0;
        return control._NR52._value | 0x70;
    default:
        logger.logWarning("Unhandled sound controller load at offset: %02x", offset);
        return 0xFF;
    }
}

void Controller::store(uint16_t offset, uint8_t value) {
    if (!control._NR52.power && offset != 0x16) {
        return;
    }
    switch (offset) {
    case 0x0:
        squareOne._NR10._value = value;
        return;
    case 0x1:
        squareOne._NR11._value = value;
        squareOne.loadLengthCounter(squareOne._NR11.lengthLoad);
        return;
    case 0x2:
        squareOne._NR12._value = value;
        return;
    case 0x3:
        squareOne._NR13 = value;
        return;
    case 0x4:
        squareOne._NRX4._value = value;
        return;
    case 0x5:
        squareTwo._NR20 = value;
        return;
    case 0x6:
        squareTwo._NR21._value = value;
        squareTwo.loadLengthCounter(squareTwo._NR21.lengthLoad);
        return;
    case 0x7:
        squareTwo._NR22._value = value;
        return;
    case 0x8:
        squareTwo._NR23 = value;
        return;
    case 0x9:
        squareTwo._NRX4._value = value;
        return;
    case 0xA:
        wave._NR30._value = value;
        return;
    case 0xB:
        wave._NR31 = value;
        wave.loadLengthCounter(wave._NR31);
        return;
    case 0xC:
        wave._NR32._value = value;
        return;
    case 0xD:
        wave._NR33 = value;
        return;
    case 0xE:
        wave._NRX4._value = value;
        return;
    case 0xF:
        noise._NR40 = value;
        return;
    case 0x10:
        noise._NR41._value = value;
        noise.loadLengthCounter(noise._NR41.lengthLoad);
        return;
    case 0x11:
        noise._NR42._value = value;
        return;
    case 0x12:
        noise._NR43._value = value;
        return;
    case 0x13:
        noise._NRX4._value = value;
        return;
    case 0x14:
        control._NR50._value = value;
        return;
    case 0x15:
        control._NR51._value = value;
        return;
    case 0x16:
        control._NR52._value = value;
        if (!control._NR52.power) {
            powerOff();
        }
        return;
    default:
        logger.logWarning("Unhandled sound controller load at offset: %02x", offset);
        return;
    }
}

uint8_t Controller::waveTableLoad(uint16_t offset) const {
    return waveTable[offset];
}

void Controller::waveTableStore(uint16_t offset, uint8_t value) {
    waveTable[offset] = value;
}

void Controller::step(uint8_t cycles) {
    squareOne.step(cycles);
    squareTwo.step(cycles);
    wave.step(cycles);
    noise.step(cycles);
}

void Controller::powerOff() {
    squareOne._NR10._value = 0x0;
    squareOne._NR11._value = 0x0;
    squareOne.loadLengthCounter(squareOne._NR11.lengthLoad);
    squareOne._NR12._value = 0x0;
    squareOne._NR13 = 0x0;
    squareOne._NRX4._value = 0x0;
    squareTwo._NR20 = 0x0;
    squareTwo._NR21._value = 0x0;
    squareTwo.loadLengthCounter(squareTwo._NR21.lengthLoad);
    squareTwo._NR22._value = 0x0;
    squareTwo._NR23 = 0x0;
    squareTwo._NRX4._value = 0x0;
    wave._NR30._value = 0x0;
    wave._NR31 = 0x0;
    wave.loadLengthCounter(wave._NR31);
    wave._NR32._value = 0x0;
    wave._NR33 = 0x0;
    wave._NRX4._value = 0x0;
    noise._NR40 = 0x0;
    noise._NR41._value = 0x0;
    noise.loadLengthCounter(noise._NR41.lengthLoad);
    noise._NR42._value = 0x0;
    noise._NR43._value = 0x0;
    noise._NRX4._value = 0x0;
    control._NR50._value = 0x0;
    control._NR51._value = 0x0;
}
