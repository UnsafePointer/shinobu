#include "core/device/Timer.hpp"

using namespace Core::Device::Timer;

Controller::Controller(Common::Logs::Level logLevel, std::unique_ptr<Core::Device::Interrupt::Controller> &interrupt) : logger(logLevel, "  [Timer]: "), interrupt(interrupt) {

}

Controller::~Controller() {

}
