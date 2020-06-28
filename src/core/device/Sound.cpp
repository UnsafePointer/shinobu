#include "core/device/Sound.hpp"
#include "common/Timing.hpp"

using namespace Core::Device::Sound;

Controller::Controller(Common::Logs::Level logLevel) : logger(logLevel, "  [Sound]: "), apu(), buffer(), time() {
    apu.treble_eq(-20.0);
	buffer.bass_freq(461);
}

Controller::~Controller() {}

blip_time_t Controller::clock() {
    return time += 4;
}

uint8_t Controller::load(uint16_t address) {
    return apu.read_register(clock(), address);
}

void Controller::store(uint16_t address, uint8_t value) {
    apu.write_register(clock(), address, value);
}

void Controller::endFrame() {
	time = 0;
	bool stereo = apu.end_frame(CyclesPerFrame);
	buffer.end_frame(CyclesPerFrame, stereo);
}
