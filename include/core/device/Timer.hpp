#pragma once
#include <cstdint>
#include <memory>
#include "core/device/Interrupt.hpp"

namespace Core {
    namespace Device {
        namespace Timer {
            class Controller {
                Common::Logs::Logger logger;
                std::unique_ptr<Core::Device::Interrupt::Controller> &interrupt;

            public:
                Controller(Common::Logs::Level logLevel, std::unique_ptr<Core::Device::Interrupt::Controller> &interrupt);
                ~Controller();
            };
        };
    };
};
