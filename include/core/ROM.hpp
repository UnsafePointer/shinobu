#pragma once
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <vector>
#include <array>
#include "core/Memory.hpp"
#include "common/Logger.hpp"

namespace Core {
    const uint16_t HEADER_START_ADDRESS = 0x100;

    namespace ROM {
        enum CGBFlag {
            DMG = 0x0,
            DMG_CGB = 0x80,
            CGB = 0xC0,
        };

        namespace BOOT {
            union Lock {
                uint8_t _value;
                struct {
                    uint8_t BOOT_OFF : 1;
                    uint8_t unused : 7;
                };

                Lock() : _value() {}
            };

            const Core::Memory::Range AddressRange = Core::Memory::Range(0x0, 0x100);
            const Core::Memory::Range BootROMRegisterRange = Core::Memory::Range(0xFF50, 0x1);

            const std::filesystem::path DEFAULT_DMG_BOOT_ROM_FILE_PATH = "DMG_ROM.BIN";
            const std::filesystem::path DEFAULT_CGB_BOOT_ROM_FILE_PATH = "CGB_ROM.BIN";

            class ROM {
                Common::Logs::Logger logger;

                Lock lockRegister;
                std::vector<uint8_t> data;
                bool initialized;
            public:
                ROM(Common::Logs::Level logLevel);
                ~ROM();

                void initialize(bool skip, Core::ROM::CGBFlag cgbFlag);
                uint8_t load(uint16_t offset) const;
                bool isLocked() const;
                bool hasBootROM() const;
                uint8_t loadLockRegister() const;
                void storeLockRegister(uint8_t value);
            };
        };

        enum Type : uint8_t {
            ROM = 0x0,
            MBC1 = 0x1,
            MBC1_RAM = 0x2,
            MBC1_RAM_BATTERY = 0x3,
            MBC2 = 0x5,
            MBC2_RAM = 0x6,
            MBC2_RAM_BATTERY = 0x7,
            ROM_RAM = 0x8,
            ROM_RAM_BATTERY = 0x9,
            MMM01 = 0xB,
            MMM01_RAM = 0xC,
            MMM01_RAM_BATTERY = 0xD,
            MBC3_TIMER_BATTERY = 0xF,
            MBC3_TIMER_RAM_BATTERY = 0x10,
            MBC3 = 0x11,
            MBC3_RAM = 0x12,
            MBC3_RAM_BATTERY = 0x13,
            MBC5 = 0x19,
            MBC5_RAM = 0x1A,
            MBC5_RAM_BATTERY = 0x1B,
            MBC5_RUMBLE = 0x1C,
            MBC5_RUMBLE_RAM = 0x1D,
            MBC5_RUMBLE_RAM_BATTERY = 0x1E,
            MBC6 = 0x20,
            MBC7_SENSOR_RUMBLE_RAM_BATTERY = 0x22,
            POCKET_CAMERA = 0xFC,
            BANDAI_TAMA5 = 0xFD,
            HUC3 = 0xFE,
            HUC1_RAM_BATTERY = 0xFF,
        };

        namespace ROMSize {
            enum Size : uint8_t {
                _32KB = 0x0,
                _64KB = 0x1,
                _128KB = 0x2,
                _256KB = 0x3,
                _512KB = 0x4,
                _1MB = 0x5,
                _2MB = 0x6,
                _4MB = 0x7,
                _8MB = 0x8,
                _1_1MB = 0x52,
                _1_2MB = 0x53,
                _1_5MB = 0x54,
            };
        };

        namespace RAMSize {
            enum Size : uint8_t {
                _0KB = 0x0,
                _2KB = 0x1,
                _8KB = 0x2,
                _32KB = 0x3,
                _128KB = 0x4,
                _64KB = 0x5,
            };
        };

        union Title {
            uint8_t _value[0x10];
            struct {
                uint8_t padding[0xC];
                uint8_t manufacturerCode[0x3];
                uint8_t _cgbFlag;
            };

            Title() : _value() {}

            CGBFlag cgbFlag() const { return CGBFlag(_cgbFlag); }
        };

        enum DestinationCode : uint8_t {
            Japanese = 0x0,
            NonJapanese = 0x1,
        };

        struct Header {
            uint8_t entryPoint[0x4];
            uint8_t nintendoLogo[0x30];
            Title title;
            uint8_t newLicenseeCode[0x2];
            uint8_t sgbFlag;
            Type cartridgeType;
            ROMSize::Size _ROMSize;
            RAMSize::Size _RAMSize;
            DestinationCode destinationCode;
            uint8_t oldLicenseeCode;
            uint8_t maskROMVersionNumber;
            uint8_t headerChecksum;
            uint8_t globalChecksum[2];
        };

        class Cartridge {
            Common::Logs::Logger logger;

            std::filesystem::path filePath;
            std::vector<uint8_t> memory;
            Header header;
        public:
            Cartridge(Common::Logs::Level logLevel);
            ~Cartridge();

            void open(std::filesystem::path &filePath);
            bool isOpen() const;
            std::filesystem::path saveFilePath() const;
            uint8_t load(uint32_t address) const;
            uint32_t RAMSize() const;
            uint32_t ROMSize() const;
            Type type() const;
            CGBFlag cgbFlag() const;
        };
    }
}
