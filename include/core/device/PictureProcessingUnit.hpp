#pragma once
#include "core/Memory.hpp"
#include "common/Logger.hpp"
#include <vector>
#include "shinobu/frontend/opengl/Vertex.hpp"
#include "core/device/Interrupt.hpp"
#include <unordered_map>
#include "shinobu/frontend/Palette.hpp"
#include "core/ROM.hpp"

namespace Shinobu {
    class Emulator;
    namespace Frontend {
        class Renderer;
    };
};

namespace Core {
    namespace Device {
        namespace PictureProcessingUnit {
            union BackgroundMapAttributes {
                uint8_t _value;
                struct {
                    uint8_t paletteNumber : 3;
                    uint8_t VRAMBankNumber : 1;
                    uint8_t unused : 1;
                    uint8_t xFlip : 1;
                    uint8_t yFlip : 1;
                    uint8_t _priority : 1;
                };
                BackgroundMapAttributes() : _value() {}
                BackgroundMapAttributes(uint8_t value) : _value(value) {}
            };

            union PaletteData {
                uint16_t _value;
                struct {
                    uint16_t red : 5;
                    uint16_t green : 5;
                    uint16_t blue : 5;
                    uint16_t unused : 1;
                };

                PaletteData(uint8_t low, uint8_t high) : _value((high << 8) | low) {}
            };

            union SpriteAttributes {
                uint8_t _value;
                struct {
                    uint8_t CGBPalette : 3;
                    uint8_t tileVRAMBank : 1;
                    uint8_t DMGPalette : 1;
                    uint8_t xFlip : 1;
                    uint8_t yFlip : 1;
                    uint8_t _priority : 1;
                };
                SpriteAttributes(uint8_t value) : _value(value) {}
            };

            struct Sprite {
                uint8_t y;
                uint8_t x;
                uint8_t tileNumber;
                SpriteAttributes attributes;
                uint16_t offset;

                Sprite() : y(0), x(0), tileNumber(0), attributes(0), offset(0) {}
                Sprite(uint8_t y, uint8_t x, uint8_t tileNumber, SpriteAttributes attributes, uint16_t offset) : y(y), x(x), tileNumber(tileNumber), attributes(attributes), offset(offset) {}

                int16_t positionY() const { return ((int16_t)y) - 16; }
                int16_t positionX() const { return ((int16_t)x) - 8; }
            };

            bool compareSpritesByPriority(const Sprite &a, const Sprite &b);

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
                    uint8_t _windowTileMapDisplaySelect : 1;
                    uint8_t LCDDisplayEnable : 1;
                };

                LCDControl() : _value() {}

                SpriteSize spriteSize() const { return SpriteSize(_spriteSize); }
                Background_WindowTileMapLocation backgroundTileMapDisplaySelect() const { return Background_WindowTileMapLocation(_backgroundTileMapDisplaySelect); }
                Background_WindowTileDataLocation background_WindowTileDataSelect() const { return Background_WindowTileDataLocation(_background_WindowTileDataSelect); }
                Background_WindowTileMapLocation windowTileMapDisplaySelect() const { return Background_WindowTileMapLocation(_windowTileMapDisplaySelect); }
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

                LCDStatus() : _value(0x3) {}
                LCDCMode mode() const { return LCDCMode(_mode); }
                void setMode(LCDCMode mode) { _mode = mode; }
             };

            union Palette {
                uint8_t _value;
                struct {
                    uint8_t color0 : 2;
                    uint8_t color1 : 2;
                    uint8_t color2 : 2;
                    uint8_t color3 : 2;
                };

                Palette() : _value(0) {};
            };

            struct WindowXPosition {
                uint8_t _value;

                WindowXPosition() : _value(0) {};

                int16_t position() const { return std::max(((int16_t)_value) - 7, 0); }
            };

            const Core::Memory::Range AddressRange = Core::Memory::Range(0xFF40, 0xC);
            const Core::Memory::Range VBKAddressRange = Core::Memory::Range(0xFF4F, 0x1);
            const Core::Memory::Range HDMARange = Core::Memory::Range(0xFF51, 0x5);
            const Core::Memory::Range ColorPaletteRange = Core::Memory::Range(0xFF68, 0x4);

            enum LCDCSTATInterruptCondition : uint8_t {
                None = 0,
                Mode2 = 1 << 0,
                Mode1 = 1 << 1,
                Mode0 = 1 << 2,
                Coincidence = 1 << 3,
            };

            union VBK {
                uint8_t _value;
                struct {
                    uint8_t bank : 1;
                    uint8_t unused : 7;
                };

                VBK() : _value(0) {};
            };

            union BGPI {
                uint8_t _value;
                struct {
                    uint8_t index : 6;
                    uint8_t unused : 1;
                    uint8_t autoIncrement : 1;
                };

                BGPI() : _value(0) {};
            };

            union OBPI {
                uint8_t _value;
                struct {
                    uint8_t index : 6;
                    uint8_t unused : 1;
                    uint8_t autoIncrement : 1;
                };

                OBPI() : _value(0) {};
            };

            union HDMA5 {
                uint8_t _value;
                struct {
                    uint8_t length : 7;
                    uint8_t mode : 1;
                };

                HDMA5() : _value(0) {};
            };

            class Processor {
                friend class Shinobu::Emulator;

                Common::Logs::Logger logger;
                std::unique_ptr<Core::Device::Interrupt::Controller> &interrupt;
                std::unique_ptr<Shinobu::Frontend::Palette::Selector> &paletteSelector;
                std::array<uint8_t, 0x4000> memory;
                std::array<uint8_t, 0xA0> spriteAttributeTable;
                LCDControl control;
                LCDStatus status;
                uint8_t scrollY;
                uint8_t scrollX;
                uint8_t LY;
                uint8_t LYC;
                Palette backgroundPalette;
                Palette object0Palette;
                Palette object1Palette;
                uint8_t windowYPosition;
                WindowXPosition windowXPosition;
                uint8_t windowLineCounter;
                uint32_t steps;
                uint8_t interruptConditions;

                Shinobu::Frontend::Renderer *renderer;
                std::vector<std::vector<Shinobu::Frontend::OpenGL::Vertex>> scanlines;

                Core::Memory::Controller *memoryController;
                uint8_t DMA;

                bool shouldNextFrameBeBlank;

                Core::ROM::CGBFlag cgbFlag;
                VBK _VBK;
                std::array<uint8_t, 0x40> backgroundPaletteData;
                BGPI _BGPI;
                std::array<uint8_t, 0x40> objectPaletteData;
                OBPI _OBPI;

                uint8_t HDMA1;
                uint8_t HDMA2;
                uint8_t HDMA3;
                uint8_t HDMA4;
                HDMA5 _HDMA5;

                uint16_t physicalAddressForAddress(uint16_t address) const;

                std::array<uint8_t, 8> getTileRowPixelsColorIndicesWithData(uint8_t lower, uint8_t upper) const;
                std::vector<Shinobu::Frontend::OpenGL::Vertex> getTileByIndex(uint16_t index, std::array<Shinobu::Frontend::OpenGL::Color, 4> paletteColors) const;
                std::vector<Shinobu::Frontend::OpenGL::Vertex> translateTileOwnCoordinatesToTileDataViewerCoordinates(std::vector<Shinobu::Frontend::OpenGL::Vertex> tile, uint16_t tileX, uint16_t tileY) const;
                std::vector<Shinobu::Frontend::OpenGL::Vertex> translateTileOwnCoordinatesToBackgroundMapViewerCoordinates(std::vector<Shinobu::Frontend::OpenGL::Vertex> tile, uint16_t tileX, uint16_t tileY) const;
                std::vector<Shinobu::Frontend::OpenGL::Vertex> translateSpriteOwnCoordinatesToSpriteViewerCoordinates(std::vector<Shinobu::Frontend::OpenGL::Vertex> tile, uint16_t position) const;

                std::vector<Sprite> getSpriteData() const;
                void renderScanline();
                uint8_t getColorIndexForSpriteAtScreenHorizontalPosition(Sprite sprite, uint16_t screenPositionX) const;
                std::pair<uint8_t, BackgroundMapAttributes> getColorIndexForBackgroundAtScreenHorizontalPosition(uint16_t screenPositionX) const;
                std::vector<std::vector<Shinobu::Frontend::OpenGL::Vertex>> blankScanlines() const;

                std::array<Shinobu::Frontend::OpenGL::Color, 4> cgbPaletteAtIndex(uint8_t index, bool isBackground) const;
            public:
                Processor(Common::Logs::Level logLevel, std::unique_ptr<Core::Device::Interrupt::Controller> &interrupt, std::unique_ptr<Shinobu::Frontend::Palette::Selector> &paletteSelector);
                ~Processor();

                void setRenderer(Shinobu::Frontend::Renderer *renderer);
                void setMemoryController(std::unique_ptr<Core::Memory::Controller> &memoryController);
                void setCGBFlag(Core::ROM::CGBFlag cgbFlag);

                uint8_t load(uint16_t offset) const;
                void store(uint16_t offset, uint8_t value);
                uint8_t VRAMLoad(uint16_t offset) const;
                void VRAMStore(uint16_t offset, uint8_t value);
                uint8_t OAMLoad(uint16_t offset) const;
                void OAMStore(uint16_t offset, uint8_t value);
                uint8_t VBKLoad(uint16_t offset) const;
                void VBKStore(uint16_t offset, uint8_t value);
                uint8_t colorPaletteLoad(uint16_t offset) const;
                void colorPaletteStore(uint16_t offset, uint8_t value);
                uint8_t HDMALoad(uint16_t offset) const;
                void HDMAStore(uint16_t offset, uint8_t value);
                void step(uint8_t cycles);
                std::vector<Shinobu::Frontend::OpenGL::Vertex> getTileDataPixels() const;
                std::vector<Shinobu::Frontend::OpenGL::Vertex> getBackgroundMap01Pixels() const;
                std::vector<Shinobu::Frontend::OpenGL::Vertex> getScrollingViewPort() const;
                std::vector<std::vector<Shinobu::Frontend::OpenGL::Vertex>> getLCDOutput();
                std::pair<std::vector<Sprite>, std::vector<Shinobu::Frontend::OpenGL::Vertex>> getSprites() const;
                uint8_t VRAMBank() const;
            };
        };
    };
};
