#pragma once
#include <cstdint>
#include "core/Memory.hpp"
#include "common/Logger.hpp"
#include <vector>
#include "core/ROM.hpp"

namespace Core {
    namespace Device {
        namespace DirectMemoryAccess {
            const Core::Memory::Range HDMARange = Core::Memory::Range(0xFF51, 0x5);

            union HDMA5 {
                uint8_t _value;
                struct {
                    uint8_t length : 7;
                    uint8_t mode : 1;
                };

                HDMA5() : _value(0) {};
            };

            struct Request {
                uint16_t startSourceAddress;
                uint16_t startDestinationAddress;
                uint16_t currentSourceAddress;
                uint16_t currentDestinationAddress;
                uint8_t remainingTransfers;
                bool active;
                bool preparing;
                bool canceling;

                Request(uint8_t value);
            };

            class Controller {
                Common::Logs::Logger logger;
                Core::Memory::Controller *memoryController;

                std::vector<Request> requests;

                uint8_t HDMA1;
                uint8_t HDMA2;
                uint8_t HDMA3;
                uint8_t HDMA4;
                HDMA5 _HDMA5;

                Core::ROM::CGBFlag cgbFlag;
            public:
                Controller(Common::Logs::Level logLevel);
                ~Controller();

                void setMemoryController(std::unique_ptr<Core::Memory::Controller> &memoryController);
                void execute(uint8_t value);
                void step(uint8_t cycles);
                bool isActive() const;
                uint8_t HDMALoad(uint16_t offset) const;
                void HDMAStore(uint16_t offset, uint8_t value);
                void executeHDMA(uint16_t source, uint16_t destination, uint16_t length);
            };
        };
    };
};
