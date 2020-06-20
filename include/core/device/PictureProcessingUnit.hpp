#pragma once
#include "core/Memory.hpp"
#include "common/Logger.hpp"
#include <vector>
#include "shinobu/frontend/opengl/Vertex.hpp"
#include "core/device/Interrupt.hpp"
#include <unordered_map>

namespace Shinobu {
    class Emulator;
    namespace Frontend {
        class Renderer;
    };
};

namespace Core {
    namespace Device {
        namespace PictureProcessingUnit {
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

                Sprite() : y(0), x(0), tileNumber(0), attributes(0) {}
                Sprite(uint8_t y, uint8_t x, uint8_t tileNumber, SpriteAttributes attributes) : y(y), x(x), tileNumber(tileNumber), attributes(attributes) {}
            };

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
                Background_WindowTileMapLocation backgroundTileMapDisplaySelect() const { return Background_WindowTileMapLocation(_backgroundTileMapDisplaySelect); }
                Background_WindowTileDataLocation background_WindowTileDataSelect() const { return Background_WindowTileDataLocation(_background_WindowTileDataSelect); }
                Background_WindowTileMapLocation windowTimeMapDisplaySelect() const { return Background_WindowTileMapLocation(_windowTimeMapDisplaySelect); }
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
                LCDCMode mode() { return LCDCMode(_mode); }
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

                Palette() : _value(0) {}
                Shinobu::Frontend::OpenGL::Color colorWithIndex(uint8_t index) const {
                    const std::array<Shinobu::Frontend::OpenGL::Color, 4> colors = {
                        {
                            { 255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f },
                            { 170.0f / 255.0f, 170.0f / 255.0f, 170.0f / 255.0f },
                            { 85.0f / 255.0f, 85.0f / 255.0f, 85.0f / 255.0f },
                            { 0.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f },
                        }
                     };
                     return colors[index];
                }
            };

            const Core::Memory::Range AddressRange = Core::Memory::Range(0xFF40, 0x9);
            const Core::Memory::Range DMATransferRange = Core::Memory::Range(0xFF46, 0x1);

            enum LCDCSTATInterruptCondition {
                Mode2 = 0,
                Mode1 = 1,
                Mode0 = 2,
                Coincidence = 3,
            };

            class Processor {
                friend class Shinobu::Emulator;

                Common::Logs::Logger logger;
                std::unique_ptr<Core::Device::Interrupt::Controller> &interrupt;
                std::array<uint8_t, 0x2000> memory;
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
                uint32_t steps;
                std::unordered_map<LCDCSTATInterruptCondition, bool> interruptConditions;

                Shinobu::Frontend::Renderer *renderer;
                std::vector<Shinobu::Frontend::OpenGL::Vertex> scanlines;

                bool isAnyConditionMet();

                std::array<Shinobu::Frontend::OpenGL::Color, 8> getTileRowPixelsWithData(uint8_t lower, uint8_t upper) const;
                std::vector<Shinobu::Frontend::OpenGL::Vertex> getTileByIndex(uint16_t index) const;
                std::vector<Shinobu::Frontend::OpenGL::Vertex> translateTileOwnCoordinatesToTileDataViewerCoordinates(std::vector<Shinobu::Frontend::OpenGL::Vertex> tile, uint16_t tileX, uint16_t tileY) const;
                std::vector<Shinobu::Frontend::OpenGL::Vertex> translateTileOwnCoordinatesToBackgroundMapViewerCoordinates(std::vector<Shinobu::Frontend::OpenGL::Vertex> tile, uint16_t tileX, uint16_t tileY) const;
                std::vector<Shinobu::Frontend::OpenGL::Vertex> translateSpriteOwnCoordinatesToSpriteViewerCoordinates(std::vector<Shinobu::Frontend::OpenGL::Vertex> tile, uint16_t position) const;

                std::vector<Sprite> getSpriteData() const;
                void renderScanline();
            public:
                Processor(Common::Logs::Level logLevel, std::unique_ptr<Core::Device::Interrupt::Controller> &interrupt);
                ~Processor();

                void setRenderer(Shinobu::Frontend::Renderer *renderer);

                uint8_t load(uint16_t offset) const;
                void store(uint16_t offset, uint8_t value);
                void VRAMStore(uint16_t offset, uint8_t value);
                void OAMStore(uint16_t offset, uint8_t value);
                void step(uint8_t cycles);
                std::vector<Shinobu::Frontend::OpenGL::Vertex> getTileDataPixels() const;
                std::vector<Shinobu::Frontend::OpenGL::Vertex> getBackgroundMap01Pixels() const;
                std::vector<Shinobu::Frontend::OpenGL::Vertex> getScrollingViewPort() const;
                std::vector<Shinobu::Frontend::OpenGL::Vertex> getLCDOutput() const;
                std::pair<std::vector<Sprite>, std::vector<Shinobu::Frontend::OpenGL::Vertex>> getSprites() const;
            };
        };
    };
};
