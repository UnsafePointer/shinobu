#pragma once
#include "core/Memory.hpp"
#include "common/Logger.hpp"
#include <vector>

namespace Shinobu {
    class Emulator;
};

namespace Core {
    namespace Device {
        namespace PictureProcessingUnit {
            enum Background_WindowTileMapLocation : uint8_t {
                _9800_9BFF = 0,
                _9C00_9FFF = 1,
            };
            enum Background_WindowTileDataLocation : uint8_t {
                _8800_97FF = 0,
                _8000_8FFF = 1,
            };
            enum SpriteSize : uint8_t {
                _8x8 = 0,
                _8x16 = 1,
            };

            union LCDControl {
                uint8_t _value;
                struct {
                    uint8_t background_WindowDisplayEnable : 1;
                    uint8_t spriteDisplayEnable : 1;
                    uint8_t _spriteSize : 1;
                    uint8_t _backgroundTileMapDisplaySelect : 1;
                    uint8_t _background_WindowTileDataSelect : 1;
                    uint8_t windowDisplayEnable : 1;
                    uint8_t _windowTimeMapDisplaySelect : 1;
                    uint8_t LCDDisplayEnable : 1;
                };

                LCDControl() : _value() {}

                SpriteSize spriteSize() { return SpriteSize(_spriteSize); }
                Background_WindowTileMapLocation backgroundTileMapDisplaySelect() { return Background_WindowTileMapLocation(_backgroundTileMapDisplaySelect); }
                Background_WindowTileDataLocation background_WindowTileDataSelect() { return Background_WindowTileDataLocation(_background_WindowTileDataSelect); }
                Background_WindowTileMapLocation windowTimeMapDisplaySelect() { return Background_WindowTileMapLocation(_windowTimeMapDisplaySelect); }
            };

            enum LCDCMode : uint8_t {
                HBlank = 0,
                VBlank = 1,
                SearchingOAM = 2,
                TransferingData = 3,
            };

            union LCDStatus {
                uint8_t _value;
                struct {
                    uint8_t _mode : 2;
                    uint8_t coincidence : 1;
                    uint8_t mode0InterruptEnable : 1;
                    uint8_t mode1InterruptEnable : 1;
                    uint8_t mode2InterruptEnable : 1;
                    uint8_t coincidenceInterruptEnable : 1;
                };

                LCDStatus() : _value(0) {}
                LCDCMode mode() { return LCDCMode(_mode); }
             };

            const Core::Memory::Range AddressRange = Core::Memory::Range(0xFF40, 0x6);

            class Processor {
                friend class Shinobu::Emulator;

                Common::Logs::Logger logger;
                std::vector<uint8_t> memory;
                LCDControl control;
                LCDStatus status;
                uint8_t scrollY;
                uint8_t scrollX;
                uint8_t LY;
                uint8_t LYC;
                uint32_t steps;
            public:
                Processor(Common::Logs::Level logLevel);
                ~Processor();

                uint8_t load(uint16_t offset) const;
                void store(uint16_t offset, uint8_t value);
                void VRAMStore(uint16_t offset, uint8_t value);
                void step(uint8_t cycles);
            };
        };
    };
};
