#pragma once
#include <cstdint>
#include "core/Memory.hpp"
#include "common/Logger.hpp"
#include <vector>
#include "core/ROM.hpp"

namespace Core {
    namespace Device {
        namespace DirectMemoryAccess {
            namespace HDMA {
                const Core::Memory::Range AddressRange = Core::Memory::Range(0xFF51, 0x5);

                static const uint8_t Done = 0xFF;

                enum Mode {
                    GeneralPurpose = 0x0,
                    HBlank = 0x1,
                };

                union HDMA5 {
                    uint8_t _value;
                    struct {
                        uint8_t length : 7;
                        uint8_t _mode : 1;
                    };

                    HDMA5() : _value(0) {};
                    Mode mode() { return Mode(_mode); }
                };

                struct Request {
                    uint16_t startSourceAddress;
                    uint16_t startDestinationAddress;
                    uint16_t currentSourceAddress;
                    uint16_t currentDestinationAddress;
                    uint16_t remainingTransfers;
                    Mode mode;

                    Request(uint8_t HDMA1, uint8_t HDMA2, uint8_t HDMA3, uint8_t HDMA4, HDMA::HDMA5 _HDMA5);
                };
            };

            namespace DMA {
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
            };

            class Controller {
                Common::Logs::Logger logger;
                Core::Memory::Controller *memoryController;

                std::vector<DMA::Request> requests;

                uint8_t HDMA1;
                uint8_t HDMA2;
                uint8_t HDMA3;
                uint8_t HDMA4;
                HDMA::HDMA5 _HDMA5;

                Core::ROM::CGBFlag cgbFlag;

                void executeHDMA();
            public:
                Controller(Common::Logs::Level logLevel);
                ~Controller();

                void setMemoryController(std::unique_ptr<Core::Memory::Controller> &memoryController);
                void execute(uint8_t value);
                void step(uint8_t cycles);
                bool isActive() const;
                uint8_t HDMALoad(uint16_t offset) const;
                void HDMAStore(uint16_t offset, uint8_t value);
            };
        };
    };
};
