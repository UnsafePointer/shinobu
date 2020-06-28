#include "core/device/Sound.hpp"

using namespace Core::Device::Sound;

Controller::Controller(Common::Logs::Level logLevel) : logger(logLevel, "  [Sound]: "), squareOne(), squareTwo() {}
Controller::~Controller() {}
