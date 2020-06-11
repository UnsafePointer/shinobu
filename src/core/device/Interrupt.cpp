#include "core/device/Interrupt.hpp"

using namespace Core::Device::Interrupt;

Controller::Controller(Common::Logs::Level logLevel) : logger(logLevel, "  [Interrupt]: "), IME(), enable(), flag() {

}

Controller::~Controller() {

}

void Controller::updateIME(bool value) {
    IME = value;
}
