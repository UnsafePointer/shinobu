#pragma once
#include <cstdint>
#include "core/Memory.hpp"
#include "common/Logger.hpp"

namespace Core {
    namespace Device {
        namespace DirectMemoryAccess {
            class Controller {
                Common::Logs::Logger logger;
                Core::Memory::Controller *memoryController;

                uint8_t remainingTransfers;
                uint16_t currentSourceAddress;
                uint16_t currentDestinationAddress;
            public:
                Controller(Common::Logs::Level logLevel);
                ~Controller();

                void setMemoryController(std::unique_ptr<Core::Memory::Controller> &memoryController);
                void execute(uint8_t value);
                void step(uint8_t cycles);
            };
        };
    };
};
