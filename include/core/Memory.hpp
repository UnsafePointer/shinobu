#pragma once
#include <cstdint>
#include <memory>
#include <filesystem>
#include <optional>
#include <vector>
#include <common/Logger.hpp>
#include <chrono>

namespace Core {
    namespace Device {
        namespace SerialDataTransfer {
            class Controller;
        };
        namespace PictureProcessingUnit {
            class Processor;
        };
        namespace Interrupt {
            class Controller;
        };
        namespace Timer {
            class Controller;
        };
        namespace JoypadInput {
            class Controller;
        };
        namespace Sound {
            class Controller;
        };
    }
    namespace ROM {
        class Cartridge;
        namespace BOOT {
        class ROM;
        }
    };

    namespace Memory {
        class Range {
            const uint32_t start;
            const uint32_t length;
        public:
            Range(uint32_t start, uint32_t length);
            ~Range();

            std::optional<uint32_t> contains(uint32_t address) const;
        };
        // https://gbdev.io/pandocs/#memory-map
        const Range ROMBank00 = Range(0x0, 0x4000);
        const Range ROMBank01_N = Range(0x4000, 0x4000);
        const Range VideoRAM = Range(0x8000, 0x2000);
        const Range ExternalRAM = Range(0xA000, 0x2000);
        const Range WorkRAMBank00 = Range(0xC000, 0x1000);
        const Range WorkRAMBank01_N = Range(0xD000, 0x1000);
        const Range EchoRAM = Range(0xE000, 0x1E00);
        const Range SpriteAttributeTable = Range(0xFE00, 0xA0);
        const Range NotUsable = Range(0xFEA0, 0x60);
        const Range I_ORegisters = Range(0xFF00, 0x80);
        const Range HighRAM = Range(0xFF80, 0x7F);

        class BankController {
        protected:
            Common::Logs::Logger logger;

            std::unique_ptr<Core::ROM::Cartridge> &cartridge;
            std::unique_ptr<Core::ROM::BOOT::ROM> &bootROM;
            std::array<uint8_t, 0x1000> WRAMBank00;
            std::array<uint8_t, 0x1000> WRAMBank01_N;
            std::unique_ptr<Core::Device::SerialDataTransfer::Controller> serialCommController;
            std::unique_ptr<Core::Device::PictureProcessingUnit::Processor> &PPU;
            std::unique_ptr<Core::Device::Sound::Controller> &sound;
            std::array<uint8_t, 0x7F> HRAM;
            std::vector<uint8_t> externalRAM;
            std::unique_ptr<Core::Device::Interrupt::Controller> &interrupt;
            std::unique_ptr<Core::Device::Timer::Controller> &timer;
            std::unique_ptr<Core::Device::JoypadInput::Controller> &joypad;

            void executeDMA(uint8_t value);
            uint8_t loadInternal(uint16_t address) const;
            void storeInternal(uint16_t address, uint8_t value);
        public:
            BankController(Common::Logs::Level logLevel,
                           std::unique_ptr<Core::ROM::Cartridge> &cartridge,
                           std::unique_ptr<Core::ROM::BOOT::ROM> &bootROM,
                           std::unique_ptr<Core::Device::PictureProcessingUnit::Processor> &PPU,
                           std::unique_ptr<Core::Device::Sound::Controller> &sound,
                           std::unique_ptr<Core::Device::Interrupt::Controller> &interrupt,
                           std::unique_ptr<Core::Device::Timer::Controller> &timer,
                           std::unique_ptr<Core::Device::JoypadInput::Controller> &joypad);
            ~BankController();

            void loadExternalRAMFromSaveFile();
            void saveExternalRAM();
            virtual uint8_t load(uint16_t address) const = 0;
            virtual void store(uint16_t address, uint8_t value) = 0;
        };

        namespace ROM {
            const Range ROMRange = Range(0x0, 0x8000);
            class Controller : public BankController {
            public:
                Controller(Common::Logs::Level logLevel,
                           std::unique_ptr<Core::ROM::Cartridge> &cartridge,
                           std::unique_ptr<Core::ROM::BOOT::ROM> &bootROM,
                           std::unique_ptr<Core::Device::PictureProcessingUnit::Processor> &PPU,
                           std::unique_ptr<Core::Device::Sound::Controller> &sound,
                           std::unique_ptr<Core::Device::Interrupt::Controller> &interrupt,
                           std::unique_ptr<Core::Device::Timer::Controller> &timer,
                           std::unique_ptr<Core::Device::JoypadInput::Controller> &joypad) : BankController(logLevel, cartridge, bootROM, PPU, sound, interrupt, timer, joypad) {};
                uint8_t load(uint16_t address) const override;
                void store(uint16_t address, uint8_t value) override;
            };
        };

        namespace MBC1 {
            // https://gbdev.io/pandocs/#mbc1
            const Range RAMGRange = Range(0x0, 0x2000);
            const Range BANK1Range = Range(0x2000, 0x2000);
            const Range BANK2Range = Range(0x4000, 0x2000);
            const Range ModeRange = Range(0x6000, 0x2000);

            union RAMG {
                uint8_t _value;
                struct {
                    uint8_t enableAccess : 4;
                    uint8_t unused : 4;
                };

                RAMG() : _value() {}
            };

            union BANK1 {
                uint8_t _value;
                struct {
                    uint8_t bank1 : 5;
                    uint8_t unused : 3;
                };

                BANK1() : _value(0x1) {}
            };

            union BANK2 {
                uint8_t _value;
                struct {
                    uint8_t bank2 : 2;
                    uint8_t unused : 6;
                };

                BANK2() : _value() {}
            };

            union Mode {
                uint8_t _value;
                struct {
                    uint8_t mode : 1;
                    uint8_t unused : 7;
                };

                Mode() : _value() {}
            };

            class Controller : public BankController {
                RAMG _RAMG;
                BANK1 _BANK1;
                BANK2 _BANK2;
                Mode mode;
            public:
                Controller(Common::Logs::Level logLevel,
                           std::unique_ptr<Core::ROM::Cartridge> &cartridge,
                           std::unique_ptr<Core::ROM::BOOT::ROM> &bootROM,
                           std::unique_ptr<Core::Device::PictureProcessingUnit::Processor> &PPU,
                           std::unique_ptr<Core::Device::Sound::Controller> &sound,
                           std::unique_ptr<Core::Device::Interrupt::Controller> &interrupt,
                           std::unique_ptr<Core::Device::Timer::Controller> &timer,
                           std::unique_ptr<Core::Device::JoypadInput::Controller> &joypad) : BankController(logLevel, cartridge, bootROM, PPU, sound, interrupt, timer, joypad) {};
                uint8_t load(uint16_t address) const override;
                void store(uint16_t address, uint8_t value) override;
            };
        };

        namespace MBC3 {
            // https://gbdev.io/pandocs/#mbc3
            const Range RAMG_TimerEnableRange = Range(0x0, 0x2000);
            const Range ROMBANKRange = Range(0x2000, 0x2000);
            const Range RAMBANK_RTCRegisterRange = Range(0x4000, 0x2000);
            const Range LatchClockDataRange = Range(0x6000, 0x2000);

            union ROMBANK {
                uint8_t _value;
                struct {
                    uint8_t bank1 : 7;
                    uint8_t unused : 1;
                };

                ROMBANK() : _value(0x1) {}
            };

            union RAMBANK {
                uint8_t _value;
                struct {
                    uint8_t bank2 : 2;
                    uint8_t unused : 6;
                };

                RAMBANK() : _value() {}
            };

            union RTCDH {
                uint8_t _value;
                struct {
                    uint8_t dayCounterMSB : 1;
                    uint8_t unused : 5;
                    uint8_t halt : 1;
                    uint8_t dayCounterCarry : 1;
                };

                RTCDH() : _value() {}
            };

            class Controller : public BankController {
                MBC1::RAMG _RAMG;
                ROMBANK _ROMBANK;
                RAMBANK _RAMBANK_RTCRegister;
                uint8_t latchClockData;
                uint8_t _RTCS;
                uint8_t _RTCM;
                uint8_t _RTCH;
                uint8_t _RTCDL;
                RTCDH _RTCDH;
                std::chrono::time_point<std::chrono::system_clock> lastTimePoint;
                std::chrono::milliseconds calculationReminder;
                bool hasRTC;

                void calculateTime(bool overrideHalt = false);
            public:
                Controller(Common::Logs::Level logLevel,
                           std::unique_ptr<Core::ROM::Cartridge> &cartridge,
                           std::unique_ptr<Core::ROM::BOOT::ROM> &bootROM,
                           std::unique_ptr<Core::Device::PictureProcessingUnit::Processor> &PPU,
                           std::unique_ptr<Core::Device::Sound::Controller> &sound,
                           std::unique_ptr<Core::Device::Interrupt::Controller> &interrupt,
                           std::unique_ptr<Core::Device::Timer::Controller> &timer,
                           std::unique_ptr<Core::Device::JoypadInput::Controller> &joypad, bool hasRTC) : BankController(logLevel, cartridge, bootROM, PPU, sound, interrupt, timer, joypad),
                            _RAMG(), _ROMBANK(), _RAMBANK_RTCRegister(), latchClockData(), _RTCS(), _RTCM(), _RTCH(), _RTCDL(), _RTCDH(), lastTimePoint(std::chrono::system_clock::now()), calculationReminder(), hasRTC(hasRTC) {};
                uint8_t load(uint16_t address) const override;
                void store(uint16_t address, uint8_t value) override;

                // http://bgb.bircd.org/rtcsave.html
                std::vector<uint8_t> clockData();
                void loadClockData(std::vector<uint8_t> clockData);
            };
        };

        class Controller {
            Common::Logs::Logger logger;

            std::unique_ptr<Core::ROM::Cartridge> &cartridge;
            std::unique_ptr<BankController> bankController;
            std::unique_ptr<Core::ROM::BOOT::ROM> bootROM;
            std::unique_ptr<Core::Device::PictureProcessingUnit::Processor> &PPU;
            std::unique_ptr<Core::Device::Sound::Controller> &sound;
            std::unique_ptr<Core::Device::Interrupt::Controller> &interrupt;
            std::unique_ptr<Core::Device::Timer::Controller> &timer;
            std::unique_ptr<Core::Device::JoypadInput::Controller> &joypad;
        public:
            Controller(Common::Logs::Level logLevel,
                       std::unique_ptr<Core::ROM::Cartridge> &cartridge,
                       std::unique_ptr<Core::Device::PictureProcessingUnit::Processor> &PPU,
                       std::unique_ptr<Core::Device::Sound::Controller> &sound,
                       std::unique_ptr<Core::Device::Interrupt::Controller> &interrupt,
                       std::unique_ptr<Core::Device::Timer::Controller> &timer,
                       std::unique_ptr<Core::Device::JoypadInput::Controller> &joypad);
            ~Controller();

            void initialize(bool skipBootROM);
            bool hasBootROM() const;
            void saveExternalRAM() const;
            uint8_t load(uint16_t address) const;
            void store(uint16_t address, uint8_t value);
            uint16_t loadDoubleWord(uint16_t address) const;
            void storeDoubleWord(uint16_t address, uint16_t value);
        };
    };
};
