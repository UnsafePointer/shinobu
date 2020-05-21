#pragma once
#include <cstdint>
#include <memory>
#include <filesystem>
#include <optional>
#include "ROM.hpp"

namespace Core {
    namespace Memory {
        class Range {
            const uint32_t start;
            const uint32_t length;
        public:
            Range(uint32_t start, uint32_t length);
            ~Range();

            std::optional<uint32_t> contains(uint32_t address) const;
        };

        class BankController {
        protected:
            std::unique_ptr<ROM::Cartridge> &cartridge;
        public:
            BankController(std::unique_ptr<ROM::Cartridge> &cartridge);
            ~BankController();
            virtual uint8_t load(uint16_t address) = 0 ;
            virtual void store(uint16_t address, uint8_t value) = 0;
        };

        namespace MBC1 {
            class Controller : public BankController {
            public:
                Controller(std::unique_ptr<ROM::Cartridge> &cartridge) : BankController(cartridge) {};
                uint8_t load(uint16_t address) override;
                void store(uint16_t address, uint8_t value) override;
            };
        };

        class Controller {
            std::unique_ptr<ROM::Cartridge> &cartridge;
            std::unique_ptr<BankController> bankController;
        public:
            Controller(std::unique_ptr<ROM::Cartridge> &cartridge);
            ~Controller();

            void initialize();
        };
    };
};
