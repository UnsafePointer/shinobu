#pragma once
#include <cstdint>
#include "core/Memory.hpp"
#include "common/Logger.hpp"
#include <vector>

namespace Core {
    namespace Device {
        namespace DirectMemoryAccess {
            struct Request {
                uint16_t startSourceAddress;
                uint16_t startDestinationAddress;
                uint16_t currentSourceAddress;
                uint16_t currentDestinationAddress;
                uint8_t remainingTransfers;
                bool preparing;
                bool canceling;

                Request(uint8_t value);
            };

            class Controller {
                Common::Logs::Logger logger;
                Core::Memory::Controller *memoryController;

                std::vector<Request> requests;
            public:
                Controller(Common::Logs::Level logLevel);
                ~Controller();

                void setMemoryController(std::unique_ptr<Core::Memory::Controller> &memoryController);
                void execute(uint8_t value);
                void step(uint8_t cycles);
                bool isActive() const;
            };
        };
    };
};
