#include "core/device/PictureProcessingUnit.hpp"
#include <iostream>
#include "common/Timing.hpp"
#include <bitset>
#include "common/System.hpp"
#include "shinobu/frontend/Renderer.hpp"
#include <algorithm>
#include "shinobu/frontend/Palette.hpp"

using namespace Core::Device::PictureProcessingUnit;
using namespace Shinobu::Frontend::Palette;

Processor::Processor(Common::Logs::Level logLevel,
                     std::unique_ptr<Core::Device::Interrupt::Controller> &interrupt,
                     std::unique_ptr<Shinobu::Frontend::Palette::Selector> &paletteSelector) : logger(logLevel, "  [PPU]: "),
                                                                                               interrupt(interrupt),
                                                                                               paletteSelector(paletteSelector),
                                                                                               memory(),
                                                                                               spriteAttributeTable(),
                                                                                               control(),
                                                                                               status(),
                                                                                               scrollY(),
                                                                                               scrollX(),
                                                                                               LY(),
                                                                                               LYC(),
                                                                                               backgroundPalette(),
                                                                                               object0Palette(),
                                                                                               object1Palette(),
                                                                                               windowYPosition(),
                                                                                               windowXPosition(),
                                                                                               windowLineCounter(),
                                                                                               steps(),
                                                                                               interruptConditions(),
                                                                                               renderer(nullptr),
                                                                                               scanlines(),
                                                                                               memoryController(nullptr),
                                                                                               DMA(),
                                                                                               shouldNextFrameBeBlank(),
                                                                                               cgbFlag(),
                                                                                               _VBK(),
                                                                                               backgroundPaletteData(),
                                                                                               _BGPI(),
                                                                                               objectPaletteData(),
                                                                                               _OBPI(),
                                                                                               HDMA1(),
                                                                                               HDMA2(),
                                                                                               HDMA3(),
                                                                                               HDMA4(),
                                                                                               _HDMA5() {

}

Processor::~Processor() {

}

void Processor::setRenderer(Shinobu::Frontend::Renderer *renderer) {
    this->renderer = renderer;
}

void Processor::setMemoryController(std::unique_ptr<Core::Memory::Controller> &memoryController) {
    this->memoryController = memoryController.get();
}

void Processor::setCGBFlag(Core::ROM::CGBFlag cgbFlag) {
    this->cgbFlag = cgbFlag;
}

uint8_t Processor::load(uint16_t offset) const {
    switch (offset) {
    case 0x0:
        return control._value;
    case 0x1:
        return status._value;
    case 0x2:
        return scrollY;
    case 0x3:
        return scrollX;
    case 0x4:
        return LY;
    case 0x5:
        return LYC;
    case 0x6:
        return DMA;
    case 0x7:
        return backgroundPalette._value;
    case 0x8:
        return object0Palette._value;
    case 0x9:
        return object1Palette._value;
    case 0xA:
        return windowYPosition;
    case 0xB:
        return windowXPosition._value;
    default:
        logger.logWarning("Unhandled Picture Processing Unit load at offset: %04x", offset);
        return 0;
    }
}

void Processor::store(uint16_t offset, uint8_t value) {
    switch (offset) {
    case 0x0: {
        uint8_t previousLCDState = control.LCDDisplayEnable;
        control._value = value;
        if (control.LCDDisplayEnable != previousLCDState) {
            logger.logWarning("LCD display enabled changed state to: %02x", control.LCDDisplayEnable);
            if (!control.LCDDisplayEnable) {
                LY = 0;
                status._mode = 0;
            } else {
                shouldNextFrameBeBlank = true;
            }
        }
        return;
    }
    case 0x1: {
        status._value &= ~0xF8;
        status._value |= value & 0xF8;
        return;
    }
    case 0x2:
        logger.logMessage("SCY write with value: %02x", value);
        scrollY = value;
        return;
    case 0x3:
        logger.logMessage("SCX write with value: %02x", value);
        scrollX = value;
        return;
    case 0x4:
        logger.logWarning("Attempting to write read-only LY register with value: %02x", value);
        return;
    case 0x5:
        LYC = value;
        return;
    case 0x6:
        DMA = value;
        memoryController->executeDMA(DMA);
        return;
    case 0x7:
        backgroundPalette._value = value;
        return;
    case 0x8:
        object0Palette._value = value;
        return;
    case 0x9:
        object1Palette._value = value;
        return;
    case 0xA:
        logger.logMessage("WY write with value: %02x", value);
        windowYPosition = value;
        return;
    case 0xB:
        logger.logMessage("WX write with value: %02x", value);
        windowXPosition._value = value;
        return;
    default:
        logger.logWarning("Unhandled Picture Processing Unit at offset: %04x with value: %04x", offset, value);
        return;
    }
}

void Processor::step(uint8_t cycles) {
    if (!control.LCDDisplayEnable) {
        return;
    }
    steps += cycles;
    bool areAnyConditionsMet = interruptConditions != LCDCSTATInterruptCondition::None;

    if (LY < 144) {
        if (steps <= 80) {
            logger.logMessage("PPU in OAM (Mode 2)");
            status.setMode(SearchingOAM);
            if (status.mode2InterruptEnable) {
                interruptConditions |= LCDCSTATInterruptCondition::Mode2;
            } else {
                interruptConditions &= ~LCDCSTATInterruptCondition::Mode2;
            }
        } else if (steps <= 289) {
            logger.logMessage("PPU in Transfering data (Mode 3)");
            status.setMode(TransferingData);
        } else {
            logger.logMessage("PPU in HBlank (Mode 0)");
            status.setMode(HBlank);
            if (status.mode0InterruptEnable) {
                status.mode0InterruptEnable |= LCDCSTATInterruptCondition::Mode0;
            } else {
                interruptConditions &= ~LCDCSTATInterruptCondition::Mode0;
            }
        }
    } else {
        logger.logMessage("PPU in VBlank (Mode 1)");
        status.setMode(VBlank);
        if (status.mode1InterruptEnable) {
            interruptConditions |= LCDCSTATInterruptCondition::Mode1;
        } else {
            interruptConditions &= ~LCDCSTATInterruptCondition::Mode1;
        }
    }

    status.coincidence = LY == LYC;
    if (status.coincidence) {
        interruptConditions |= LCDCSTATInterruptCondition::Coincidence;
    } else {
        interruptConditions &= ~LCDCSTATInterruptCondition::Coincidence;
    }

    if (steps >= CyclesPerScanline) {
        if (LY <= 143) {
            logger.logMessage("Rendering scanline: %d", LY);
            renderScanline();
        }
        LY++;
        if (LY == 144) {
            interrupt->requestInterrupt(Interrupt::VBLANK);
            renderer->update();
            scanlines.clear();
            windowLineCounter = 0;
        }
        if (LY >= TotalScanlines) {
            LY = 0;
        }
        steps %= CyclesPerScanline;
    }

    if (!areAnyConditionsMet && (interruptConditions != LCDCSTATInterruptCondition::None)) {
        interrupt->requestInterrupt(Interrupt::LCDSTAT);
    }
    return;
}

uint8_t Processor::VRAMLoad(uint16_t offset) const {
    if (status.mode() == LCDCMode::TransferingData && control.LCDDisplayEnable) {
        logger.logWarning("Attempting to load from VRAM while inaccessible with mode: %02x at offset: %04x", status.mode(), offset);
        return 0xFF;
    }
    return memory[offset];
}

void Processor::VRAMStore(uint16_t offset, uint8_t value) {
    if (status.mode() == LCDCMode::TransferingData && control.LCDDisplayEnable) {
        logger.logWarning("Attempting to store to VRAM while inaccessible with mode: %02x at offset: %04x with value: %02x", status.mode(), offset, value);
        return;
    }
    memory[offset] = value;
}

uint8_t Processor::OAMLoad(uint16_t offset) const {
    LCDCMode currentMode = status.mode();
    if ((currentMode == LCDCMode::SearchingOAM || currentMode == LCDCMode::TransferingData) && control.LCDDisplayEnable) {
        logger.logWarning("Attempting to load from OAM while inaccessible with mode: %02x at offset: %04x", currentMode, offset);
        return 0xFF;
    }
    return spriteAttributeTable[offset];
}

void Processor::OAMStore(uint16_t offset, uint8_t value) {
    LCDCMode currentMode = status.mode();
    if ((currentMode == LCDCMode::SearchingOAM || currentMode == LCDCMode::TransferingData) && control.LCDDisplayEnable) {
        logger.logWarning("Attempting to store to OAM while inaccessible with mode: %02x at offset: %04x with value: %02x", status.mode(), offset, value);
        return;
    }
    spriteAttributeTable[offset] = value;
}

uint8_t Processor::VBKLoad(uint16_t offset) const {
    (void)offset;
    if (cgbFlag == Core::ROM::CGBFlag::DMG) {
        logger.logWarning("Attempting to load VBK register on DMG mode");
        return 0xFF;
    }
    return _VBK.bank | 0xFE;
}

void Processor::VBKStore(uint16_t offset, uint8_t value) {
    (void)offset;
    if (cgbFlag == Core::ROM::CGBFlag::DMG) {
        logger.logWarning("Attempting to store VBK register on DMG mode");
        return;
    }
    _VBK._value = value;
}

uint8_t Processor::colorPaletteLoad(uint16_t offset) const {
    if (cgbFlag == Core::ROM::CGBFlag::DMG) {
        logger.logWarning("Attempting to load LCD color palette register at offset: %04x on DMG mode", offset);
        return 0xFF;
    }
    switch (offset) {
    case 0x0:
        return _BGPI._value;
    case 0x1:
        return backgroundPaletteData[_BGPI.index];
    case 0x2:
        return _OBPI._value;
    case 0x3:
        return objectPaletteData[_OBPI.index];
    default:
        logger.logWarning("Unhandled color palette load at offset: %04x", offset);
        return 0xFF;
    }
}

void Processor::colorPaletteStore(uint16_t offset, uint8_t value) {
    if (cgbFlag == Core::ROM::CGBFlag::DMG) {
        logger.logWarning("Attempting to store LCD color palette register at offset: %04x with value: %02x on DMG mode", offset, value);
        return;
    }
    switch (offset) {
    case 0x0:
        _BGPI._value = value;
        break;
    case 0x1:
        backgroundPaletteData[_BGPI.index] = value;
        if (_BGPI.autoIncrement == 0x1) {
            _BGPI.index++;
        }
        break;
    case 0x2:
        _OBPI._value = value;
        break;
    case 0x3:
        objectPaletteData[_OBPI.index] = value;
        if (_OBPI.autoIncrement == 0x1) {
            _OBPI.index++;
        }
        break;
    default:
        logger.logWarning("Unhandled color palette store at offset: %04x with value %02x", offset, value);
        break;
    }
}

uint8_t Processor::HDMALoad(uint16_t offset) const {
    if (cgbFlag == Core::ROM::CGBFlag::DMG) {
        logger.logWarning("Attempting to load HDMA register at offset: %04x on DMG mode", offset);
        return 0xFF;
    }
    switch (offset) {
    case 0x0:
        return HDMA1;
    case 0x1:
        return HDMA2;
    case 0x2:
        return HDMA3;
    case 0x3:
        return HDMA4;
    case 0x4:
        return _HDMA5._value;
    default:
        logger.logWarning("Unhandled HDMA register load at offset: %04x", offset);
        return 0xFF;
    }
}

void Processor::HDMAStore(uint16_t offset, uint8_t value) {
    if (cgbFlag == Core::ROM::CGBFlag::DMG) {
        logger.logWarning("Attempting to load HDMA register at offset: %04x with value: %02x on DMG mode", offset, value);
        return;
    }
    switch (offset) {
    case 0x0:
        HDMA1 = value;
        break;
    case 0x1:
        HDMA2 = value;
        break;
    case 0x2:
        HDMA3 = value;
        break;
    case 0x3:
        HDMA4 = value;
        break;
    case 0x4: {
        _HDMA5._value = value;
        uint16_t source = (((uint16_t)HDMA1) << 8) | HDMA2;
        source &= 0xFFF0;
        uint16_t destination = (((uint16_t)HDMA3) << 8) | HDMA4;
        source &= 0x0FF0;
        uint16_t length = (((uint16_t)_HDMA5.length) + 1) * 0x10;
        memoryController->executeHDMA(source, destination, length);
        break;
    }
    default:
        logger.logWarning("Unhandled HDMA register store at offset: %04x with value %02x", offset, value);
        break;
    }
}

uint8_t Processor::getColorIndexForSpriteAtScreenHorizontalPosition(Sprite sprite, uint16_t screenPositionX) const {
    SpriteSize spriteSize = control.spriteSize();
    uint8_t spriteHeight = spriteSize == SpriteSize::_8x16 ? 16 : 8;
    uint16_t tileIndex = sprite.tileNumber;
    if (spriteSize == SpriteSize::_8x16) {
        tileIndex = sprite.tileNumber & 0xFE;
    }
    uint16_t offset = (0x10 * tileIndex);
    uint16_t yInTile = LY - sprite.positionY();
    if (sprite.attributes.yFlip) {
        yInTile = (spriteHeight - 1) - yInTile;
        if (spriteSize == SpriteSize::_8x16 && yInTile >= 8) {
            tileIndex = sprite.tileNumber | 0x01;
        }
    }
    uint16_t lowAddress = yInTile * 2 + offset;
    uint16_t highAddress = (yInTile * 2 + 1) + offset;
    if (cgbFlag != Core::ROM::CGBFlag::DMG) {
        lowAddress = (sprite.attributes.tileVRAMBank << 13) | (lowAddress & 0x1FFF);
        highAddress = (sprite.attributes.tileVRAMBank << 13) | (highAddress & 0x1FFF);
    }
    uint8_t low = memory[lowAddress];
    uint8_t high = memory[highAddress];
    auto colorData = getTileRowPixelsColorIndicesWithData(low, high);
    uint8_t colorDataIndex = screenPositionX - sprite.positionX();
    if (sprite.attributes.xFlip) {
        colorDataIndex = 7 - colorDataIndex;
    }
    uint8_t colorIndex = colorData[colorDataIndex];
    return colorIndex;
}

std::pair<uint8_t, BackgroundMapAttributes> Processor::getColorIndexForBackgroundAtScreenHorizontalPosition(uint16_t screenPositionX) const {
    Background_WindowTileMapLocation backgroundMapLocation = control.backgroundTileMapDisplaySelect();
    uint32_t backgroundMapAddressStart;
    switch (backgroundMapLocation) {
    case _9800_9BFF:
        backgroundMapAddressStart = 0x9800 - 0x8000;
        break;
    case _9C00_9FFF:
        backgroundMapAddressStart = 0x9C00 - 0x8000;
        break;
    }
    Background_WindowTileMapLocation windowMapLocation = control.windowTileMapDisplaySelect();
    uint32_t windowMapAddressStart;
    switch (windowMapLocation) {
    case _9800_9BFF:
        windowMapAddressStart = 0x9800 - 0x8000;
        break;
    case _9C00_9FFF:
        windowMapAddressStart = 0x9C00 - 0x8000;
        break;
    }
    bool drawWindow = false;
    Background_WindowTileDataLocation tileDataLocation = control.background_WindowTileDataSelect();
    uint16_t screenPositionXWithScroll = (screenPositionX + scrollX) % TileMapResolution;
    uint16_t screenPositionYWithScroll = (LY + scrollY) % TileMapResolution;
    uint16_t tileIndexInMap = (screenPositionXWithScroll / VRAMTileDataSide) + (screenPositionYWithScroll / VRAMTileDataSide) * VRAMTileBackgroundMapSide;
    uint32_t addressStart = backgroundMapAddressStart;
    if (control.windowDisplayEnable) {
        if (LY >= windowYPosition && screenPositionX >= windowXPosition.position()) {
            addressStart = windowMapAddressStart;
            tileIndexInMap = ((screenPositionX - windowXPosition.position()) / VRAMTileDataSide) + (windowLineCounter / VRAMTileDataSide) * VRAMTileBackgroundMapSide;
            drawWindow = true;
        }
    }
    uint16_t tileIndex;
    uint16_t addressInBackgroundMap = addressStart + tileIndexInMap;
    BackgroundMapAttributes attributes;
    if (cgbFlag != Core::ROM::CGBFlag::DMG) {
        uint16_t addressInBackgroundMapAttributes = (0x1 << 13) | (addressInBackgroundMap & 0x1FFF);
        attributes = BackgroundMapAttributes(memory[addressInBackgroundMapAttributes]);
    }
    if (tileDataLocation == _8000_8FFF) {
        uint8_t indexOffset = memory[addressInBackgroundMap];
        tileIndex = indexOffset;
    } else {
        int8_t indexOffset = memory[addressInBackgroundMap];
        tileIndex = 256 + indexOffset;
    }
    uint16_t offset = (0x10 * tileIndex);
    uint16_t yInTile;
    if (drawWindow) {
        yInTile = windowLineCounter % VRAMTileDataSide;
    } else {
        yInTile = screenPositionYWithScroll % VRAMTileDataSide;
    }
    if (cgbFlag != Core::ROM::CGBFlag::DMG && attributes.yFlip) {
        yInTile = (VRAMTileDataSide - 1) - yInTile;
    }
    uint16_t lowAddress = yInTile * 2 + offset;
    uint16_t highAddress = (yInTile * 2 + 1) + offset;
    if (cgbFlag != Core::ROM::CGBFlag::DMG) {
        lowAddress = (attributes.VRAMBankNumber << 13) | (lowAddress & 0x1FFF);
        highAddress = (attributes.VRAMBankNumber << 13) | (highAddress & 0x1FFF);
    }
    uint8_t low = memory[lowAddress];
    uint8_t high = memory[highAddress];
    auto colorData = getTileRowPixelsColorIndicesWithData(low, high);
    uint8_t colorDataIndex;
    if (drawWindow) {
        colorDataIndex = (screenPositionX - windowXPosition.position()) % VRAMTileDataSide;
    } else {
        colorDataIndex = screenPositionXWithScroll % VRAMTileDataSide;
    }
    if (cgbFlag != Core::ROM::CGBFlag::DMG && attributes.xFlip) {
        colorDataIndex = (VRAMTileDataSide - 1) - colorDataIndex;
    }
    return {colorData[colorDataIndex], attributes};
}

std::vector<std::vector<Shinobu::Frontend::OpenGL::Vertex>> Processor::blankScanlines() const {
    std::vector<std::vector<Shinobu::Frontend::OpenGL::Vertex>> blankScanlines = {};
    Shinobu::Frontend::OpenGL::Color blankColor = paletteSelector->currentSelection()[0];
    for (int j = 0; j < 144; j++) {
        std::vector<Shinobu::Frontend::OpenGL::Vertex> scanline = {};
        for (int i = 0; i < HorizontalResolution; i++) {
            Shinobu::Frontend::OpenGL::Color color = blankColor;
            Shinobu::Frontend::OpenGL::Vertex vertex = { { (GLfloat)i, (GLfloat)j }, color};
            scanline.push_back(vertex);
        }
        blankScanlines.push_back(scanline);
    }
    return blankScanlines;
}

Shinobu::Frontend::Palette::palette Processor::cgbPaletteAtIndex(uint8_t index, bool isBackground) const {
    palette palette = {};
    uint16_t offset = index * 8;
    std::array<uint8_t, 64UL> paletteDataSource = backgroundPaletteData;
    if (!isBackground) {
        paletteDataSource = objectPaletteData;
    }
    for (int i = 0; i < 4; i++) {
        uint8_t low = paletteDataSource[offset + (i * 2)];
        uint8_t high = paletteDataSource[offset + (i * 2) + 1];
        PaletteData paletteData = PaletteData(low, high);
        Shinobu::Frontend::OpenGL::Color color = { paletteData.red / 32.0f, paletteData.green / 32.0f, paletteData.blue / 32.0f };
        palette[i] = color;
    }
    return palette;
}

bool Core::Device::PictureProcessingUnit::compareSpritesByPriority(const Sprite &a, const Sprite &b) {
    if (a.x == b.x) {
        return a.offset < b.offset;
    }
    return a.x < b.x;
}

std::vector<Sprite> Processor::getVisibleSprites() const {
    SpriteSize spriteSize = control.spriteSize();
    uint8_t spriteHeight = spriteSize == SpriteSize::_8x16 ? 16 : 8;
    std::vector<Sprite> visibleSprites = {};
    std::vector<Sprite> sprites = getSpriteData();
    if (control.spriteDisplayEnable) {
        for (auto const& sprite : sprites) {
            if ((LY >= sprite.positionY() && LY < (sprite.positionY() + spriteHeight)) && sprite.positionX() >= -8 && sprite.positionX() < 168) {
                visibleSprites.push_back(sprite);
            }
            if (visibleSprites.size() >= 10) {
                break;
            }
        }
    }
    return visibleSprites;
}

void Processor::DMG_renderScanline() {
    std::vector<Shinobu::Frontend::OpenGL::Vertex> scanline = {};
    const std::vector<Sprite> visibleSprites = getVisibleSprites();
    const palette colors = paletteSelector->currentSelection();
    const palette backgroundPaletteColors = { colors[backgroundPalette.color0], colors[backgroundPalette.color1], colors[backgroundPalette.color2], colors[backgroundPalette.color3] };
    const palette object0PaletteColors = { colors[object0Palette.color0], colors[object0Palette.color1], colors[object0Palette.color2], colors[object0Palette.color3] };
    const palette object1PaletteColors = { colors[object1Palette.color0], colors[object1Palette.color1], colors[object1Palette.color2], colors[object1Palette.color3] };
    for (int i = 0; i < HorizontalResolution; i++) {
        std::vector<Sprite> spritesToDraw = {};
        for (auto const& sprite : visibleSprites) {
            if (i >= sprite.positionX() && i < sprite.positionX() + 8) {
                spritesToDraw.push_back(sprite);
            }
        }
        std::sort(spritesToDraw.begin(), spritesToDraw.end(), compareSpritesByPriority);
        if (!control.background_WindowDisplayEnable && spritesToDraw.empty()) {
            Shinobu::Frontend::OpenGL::Vertex vertex = { { (GLfloat)i, (GLfloat)(VerticalResolution - 1 - LY) }, colors[0]};
            scanline.push_back(vertex);
            continue;
        }
        uint8_t colorIndex;
        Shinobu::Frontend::OpenGL::Color color;
        uint8_t spriteIndex = 0;
        if (spritesToDraw.empty()) {
            std::tie(colorIndex, std::ignore) = getColorIndexForBackgroundAtScreenHorizontalPosition(i);
            color = backgroundPaletteColors[colorIndex];
        } else {
DMG_DRAW_SPRITE:
            Sprite spriteToDraw = spritesToDraw[spriteIndex];
            if (spriteToDraw.attributes.priority() == SpritePriority::SpriteAboveBackground) {
                colorIndex = getColorIndexForSpriteAtScreenHorizontalPosition(spriteToDraw, i);
                if (colorIndex != 0) {
                    if (spriteToDraw.attributes.DMGPalette) {
                        color = object1PaletteColors[colorIndex];
                    } else {
                        color = object0PaletteColors[colorIndex];
                    }
                } else {
                    if (spriteIndex < (spritesToDraw.size() - 1)) {
                        spriteIndex++;
                        goto DMG_DRAW_SPRITE;
                    }
                    if (!control.background_WindowDisplayEnable) {
                        Shinobu::Frontend::OpenGL::Vertex vertex = { { (GLfloat)i, (GLfloat)(VerticalResolution - 1 - LY) }, colors[0]};
                        scanline.push_back(vertex);
                        continue;
                    } else {
                        std::tie(colorIndex, std::ignore) = getColorIndexForBackgroundAtScreenHorizontalPosition(i);
                        color = backgroundPaletteColors[colorIndex];
                    }
                }
            } else {
                std::tie(colorIndex, std::ignore) = getColorIndexForBackgroundAtScreenHorizontalPosition(i);
                if (colorIndex == 0) {
                    colorIndex = getColorIndexForSpriteAtScreenHorizontalPosition(spriteToDraw, i);
                    if (colorIndex != 0) {
                        if (spriteToDraw.attributes.DMGPalette) {
                            color = object1PaletteColors[colorIndex];
                        } else {
                            color = object0PaletteColors[colorIndex];
                        }
                    } else {
                        color = backgroundPaletteColors[colorIndex];
                    }
                } else {
                    color = backgroundPaletteColors[colorIndex];
                }
            }
        }
        Shinobu::Frontend::OpenGL::Vertex vertex = { { (GLfloat)i, (GLfloat)(VerticalResolution - 1 - LY) }, color};
        scanline.push_back(vertex);
    }
    if (control.windowDisplayEnable && LY >= windowYPosition && windowXPosition.position() <= 160) {
        windowLineCounter++;
    }
    scanlines.push_back(scanline);
}

void Processor::CGB_renderScanline() {
    std::vector<Shinobu::Frontend::OpenGL::Vertex> scanline = {};
    const std::vector<Sprite> visibleSprites = getVisibleSprites();
    for (int i = 0; i < HorizontalResolution; i++) {
        std::vector<Sprite> spritesToDraw = {};
        for (auto const& sprite : visibleSprites) {
            if (i >= sprite.positionX() && i < sprite.positionX() + 8) {
                spritesToDraw.push_back(sprite);
            }
        }
        std::sort(spritesToDraw.begin(), spritesToDraw.end(), compareSpritesByPriority);
        uint8_t colorIndex;
        BackgroundMapAttributes backgroundAttr;
        Shinobu::Frontend::OpenGL::Color color;
        uint8_t spriteIndex = 0;
        if (spritesToDraw.empty()) {
            std::tie(colorIndex, backgroundAttr) = getColorIndexForBackgroundAtScreenHorizontalPosition(i);
            const palette tilePalette = cgbPaletteAtIndex(backgroundAttr.paletteNumber, true);
            color = tilePalette[colorIndex];
        } else {
CGB_DRAW_SPRITE:
            Sprite spriteToDraw = spritesToDraw[spriteIndex];
            if (spriteToDraw.attributes.priority() == SpritePriority::SpriteAboveBackground) {
                colorIndex = getColorIndexForSpriteAtScreenHorizontalPosition(spriteToDraw, i);
                if (colorIndex != 0) {
                    const palette tilePalette = cgbPaletteAtIndex(spriteToDraw.attributes.CGBPalette, false);
                    color = tilePalette[colorIndex];
                } else {
                    if (spriteIndex < (spritesToDraw.size() - 1)) {
                        spriteIndex++;
                        goto CGB_DRAW_SPRITE;
                    }
                    std::tie(colorIndex, backgroundAttr) = getColorIndexForBackgroundAtScreenHorizontalPosition(i);
                    const palette tilePalette = cgbPaletteAtIndex(backgroundAttr.paletteNumber, true);
                    color = tilePalette[colorIndex];
                }
            } else {
                std::tie(colorIndex, backgroundAttr) = getColorIndexForBackgroundAtScreenHorizontalPosition(i);
                if (colorIndex == 0) {
                    colorIndex = getColorIndexForSpriteAtScreenHorizontalPosition(spriteToDraw, i);
                    if (colorIndex != 0) {
                        const palette tilePalette = cgbPaletteAtIndex(spriteToDraw.attributes.CGBPalette, false);
                        color = tilePalette[colorIndex];
                    } else {
                        const palette tilePalette = cgbPaletteAtIndex(backgroundAttr.paletteNumber, true);
                        color = tilePalette[colorIndex];
                    }
                } else {
                    const palette tilePalette = cgbPaletteAtIndex(backgroundAttr.paletteNumber, true);
                    color = tilePalette[colorIndex];
                }
            }
        }
        Shinobu::Frontend::OpenGL::Vertex vertex = { { (GLfloat)i, (GLfloat)(VerticalResolution - 1 - LY) }, color};
        scanline.push_back(vertex);
    }
    if (control.windowDisplayEnable && LY >= windowYPosition && windowXPosition.position() <= 160) {
        windowLineCounter++;
    }
    scanlines.push_back(scanline);
}

void Processor::renderScanline() {
    if (cgbFlag != Core::ROM::CGBFlag::DMG) {
        CGB_renderScanline();
    } else {
        DMG_renderScanline();
    }
}

uint16_t Processor::physicalAddressForAddress(uint16_t address) const {
    uint16_t mask = (VRAMBank() << 13);
    uint16_t physicalAddress = mask | (address & 0x1FFF);
    return physicalAddress;
}

std::array<uint8_t, 8> Processor::getTileRowPixelsColorIndicesWithData(uint8_t low, uint8_t high) const {
    std::array<uint8_t, 8> tileRowPixelsColorIndices = {};
    std::bitset<8> lowBits = std::bitset<8>(low);
    std::bitset<8> highBits = std::bitset<8>(high);
    for (int i = 7; i >= 0; i--) {
        uint8_t highMask = (uint8_t)highBits.test(i) << 0x1;
        uint8_t low = lowBits.test(i);
        uint8_t index = highMask | low;
        tileRowPixelsColorIndices[7-i] = index;
    }
    return tileRowPixelsColorIndices;
}

std::vector<Shinobu::Frontend::OpenGL::Vertex> Processor::getTileByIndex(uint16_t index, Shinobu::Frontend::Palette::palette paletteColors) const {
    std::vector<Shinobu::Frontend::OpenGL::Vertex> tile = {};
    for (int i = 0; i < VRAMTileDataSide; i++) {
        uint16_t offset = (0x10 * index);
        uint16_t lowAddress = i * 2 + offset;
        uint16_t highAddress = (i * 2 + 1) + offset;
        uint8_t low = memory[physicalAddressForAddress(lowAddress)];
        uint8_t high = memory[physicalAddressForAddress(highAddress)];
        auto colorData = getTileRowPixelsColorIndicesWithData(low, high);
        for (int j = 0; j < VRAMTileDataSide; j++) {
            Shinobu::Frontend::OpenGL::Vertex vertex = { { (GLfloat)j, (GLfloat)(7 - i) }, paletteColors[colorData[j]] };
            tile.push_back(vertex);
        }
    }
    return tile;
}

std::vector<Shinobu::Frontend::OpenGL::Vertex> Processor::translateTileOwnCoordinatesToTileDataViewerCoordinates(std::vector<Shinobu::Frontend::OpenGL::Vertex> tile, uint16_t tileX, uint16_t tileY) const {
    std::vector<Shinobu::Frontend::OpenGL::Vertex> pixels = {};
    for (const auto& tilePixel : tile) {
        uint16_t x = tilePixel.position.x + (tileX * VRAMTileDataSide);
        uint16_t y = tilePixel.position.y + (tileY * VRAMTileDataSide);
        Shinobu::Frontend::OpenGL::Vertex pixel = { { (GLfloat)x, (GLfloat)y }, tilePixel.color };
        pixels.push_back(pixel);
    }
    return pixels;
}

std::vector<Shinobu::Frontend::OpenGL::Vertex> Processor::translateTileOwnCoordinatesToBackgroundMapViewerCoordinates(std::vector<Shinobu::Frontend::OpenGL::Vertex> tile, uint16_t tileX, uint16_t tileY) const {
    std::vector<Shinobu::Frontend::OpenGL::Vertex> pixels = {};
    for (const auto& tilePixel : tile) {
        uint16_t x = tilePixel.position.x + (tileX * VRAMTileDataSide);
        uint16_t y = tilePixel.position.y + (tileY * VRAMTileDataSide);
        Shinobu::Frontend::OpenGL::Vertex pixel = { { (GLfloat)x, (GLfloat)y }, tilePixel.color };
        pixels.push_back(pixel);
    }
    return pixels;
}

std::vector<Shinobu::Frontend::OpenGL::Vertex> Processor::translateSpriteOwnCoordinatesToSpriteViewerCoordinates(std::vector<Shinobu::Frontend::OpenGL::Vertex> tile, uint16_t position) const {
    std::vector<Shinobu::Frontend::OpenGL::Vertex> pixels = {};
    for (const auto& tilePixel : tile) {
        uint16_t x = tilePixel.position.x;
        uint16_t y = tilePixel.position.y + ((39 - position) * VRAMTileDataSide);
        Shinobu::Frontend::OpenGL::Vertex pixel = { { (GLfloat)x, (GLfloat)y }, tilePixel.color };
        pixels.push_back(pixel);
    }
    return pixels;
}

std::vector<Shinobu::Frontend::OpenGL::Vertex> Processor::getTileDataPixels() const {
    const palette colors = paletteSelector->currentSelection();
    std::vector<Shinobu::Frontend::OpenGL::Vertex> pixels = {};
    uint16_t index = 0;
    for (int y = (VRAMTileDataViewerHeight - 1); y >= 0; y--) {
        for (int x = 0; x < VRAMTileDataViewerWidth; x++) {
            std::vector<Shinobu::Frontend::OpenGL::Vertex> tile = getTileByIndex(index, colors);
            tile = translateTileOwnCoordinatesToTileDataViewerCoordinates(tile, x, y);
            pixels.insert(pixels.end(), tile.begin(), tile.end());
            index++;
        }
    }
    return pixels;
}

std::vector<Shinobu::Frontend::OpenGL::Vertex> Processor::getBackgroundMap01Pixels() const {
    Background_WindowTileMapLocation backgroundMapLocation = control.backgroundTileMapDisplaySelect();
    uint32_t backgroundMapAddressStart;
    switch (backgroundMapLocation) {
    case _9800_9BFF:
        backgroundMapAddressStart = 0x9800 - 0x8000;
        break;
    case _9C00_9FFF:
        backgroundMapAddressStart = 0x9C00 - 0x8000;
        break;
    }
    const palette colors = paletteSelector->currentSelection();
    const palette paletteColors = { colors[backgroundPalette.color0], colors[backgroundPalette.color1], colors[backgroundPalette.color2], colors[backgroundPalette.color3] };
    Background_WindowTileDataLocation tileDataLocation = control.background_WindowTileDataSelect();
    std::vector<Shinobu::Frontend::OpenGL::Vertex> pixels = {};
    uint16_t index = 0;
    for (int y = (VRAMTileBackgroundMapSide - 1); y >= 0; y--) {
        for (int x = 0; x < VRAMTileBackgroundMapSide; x++) {
            std::vector<Shinobu::Frontend::OpenGL::Vertex> tile;
            if (tileDataLocation == _8000_8FFF) {
                uint8_t tileIndex = memory[physicalAddressForAddress(backgroundMapAddressStart + index)];
                tile = getTileByIndex(tileIndex, paletteColors);
            } else {
                int8_t tileIndex = memory[physicalAddressForAddress(backgroundMapAddressStart + index)];
                tile = getTileByIndex(256 + tileIndex, paletteColors);
            }
            tile = translateTileOwnCoordinatesToBackgroundMapViewerCoordinates(tile, x, y);
            pixels.insert(pixels.end(), tile.begin(), tile.end());
            index++;
        }
    }
    return pixels;
}

std::vector<Shinobu::Frontend::OpenGL::Vertex> Processor::getScrollingViewPort() const {
    Shinobu::Frontend::OpenGL::Color color = { 1.0, 0.0, 0.0 };
    Shinobu::Frontend::OpenGL::Point upperLeft = { (GLfloat)scrollX, (GLfloat)scrollY };
    Shinobu::Frontend::OpenGL::Point upperLeftTranslated = { upperLeft.x, TileMapResolution - upperLeft.y };
    std::vector<Shinobu::Frontend::OpenGL::Vertex> viewPort = {};
    Shinobu::Frontend::OpenGL::Vertex v1 = { upperLeftTranslated, color };
    Shinobu::Frontend::OpenGL::Vertex v2 = { { upperLeftTranslated.x + HorizontalResolution, upperLeftTranslated.y }, color };
    Shinobu::Frontend::OpenGL::Vertex v3 = { { upperLeftTranslated.x + HorizontalResolution, upperLeftTranslated.y - VerticalResolution }, color };
    Shinobu::Frontend::OpenGL::Vertex v4 = { { upperLeftTranslated.x, upperLeftTranslated.y - VerticalResolution }, color };
    viewPort.push_back(v1);
    viewPort.push_back(v2);
    viewPort.push_back(v2);
    viewPort.push_back(v3);
    viewPort.push_back(v3);
    viewPort.push_back(v4);
    viewPort.push_back(v4);
    viewPort.push_back(v1);
    if (v2.position.x >= TileMapResolution) {
        viewPort.push_back({ { -PixelScale, v2.position.y }, color });
        viewPort.push_back({ { v2.position.x - TileMapResolution, v2.position.y }, color });
        viewPort.push_back({ { v2.position.x - TileMapResolution, v2.position.y }, color });
        viewPort.push_back({ { v2.position.x - TileMapResolution, v3.position.y }, color });
        viewPort.push_back({ { v2.position.x - TileMapResolution, v3.position.y }, color });
        viewPort.push_back({ { -PixelScale, v3.position.y }, color });
        viewPort.push_back({ { -PixelScale, v3.position.y }, color });
        viewPort.push_back({ { -PixelScale, v2.position.y }, color });
        if (v3.position.y < 0) {
            viewPort.push_back({ { -PixelScale, TileMapResolution + PixelScale }, color });
            viewPort.push_back({ { v4.position.x - (TileMapResolution - HorizontalResolution), TileMapResolution + PixelScale }, color });
            viewPort.push_back({ { v4.position.x - (TileMapResolution - HorizontalResolution), TileMapResolution + PixelScale }, color });
            viewPort.push_back({ { v4.position.x - (TileMapResolution - HorizontalResolution), TileMapResolution + v4.position.y }, color });
            viewPort.push_back({ { v4.position.x - (TileMapResolution - HorizontalResolution), TileMapResolution + v4.position.y }, color });
            viewPort.push_back({ { -PixelScale, TileMapResolution + v4.position.y }, color });
            viewPort.push_back({ { -PixelScale, TileMapResolution + v4.position.y }, color });
            viewPort.push_back({ { -PixelScale, TileMapResolution + PixelScale }, color });

            viewPort.push_back({ { TileMapResolution + PixelScale, TileMapResolution + PixelScale }, color });
            viewPort.push_back({ { v3.position.x - HorizontalResolution, TileMapResolution + PixelScale }, color });
            viewPort.push_back({ { v3.position.x - HorizontalResolution, TileMapResolution + PixelScale }, color });
            viewPort.push_back({ { v3.position.x - HorizontalResolution, TileMapResolution + v3.position.y }, color });
            viewPort.push_back({ { v3.position.x - HorizontalResolution, TileMapResolution + v3.position.y }, color });
            viewPort.push_back({ { TileMapResolution + PixelScale, TileMapResolution + v3.position.y }, color });
            viewPort.push_back({ { TileMapResolution + PixelScale, TileMapResolution + v3.position.y }, color });
            viewPort.push_back({ { TileMapResolution + PixelScale, TileMapResolution + PixelScale }, color });
        }
    } else if (v3.position.y < 0) {
        viewPort.push_back({ { v4.position.x, TileMapResolution + PixelScale }, color });
        viewPort.push_back({ { v4.position.x, TileMapResolution + v4.position.y }, color });
        viewPort.push_back({ { v4.position.x, TileMapResolution + v4.position.y }, color });
        viewPort.push_back({ { v3.position.x, TileMapResolution + v3.position.y }, color });
        viewPort.push_back({ { v3.position.x, TileMapResolution + v3.position.y }, color });
        viewPort.push_back({ { v3.position.x, TileMapResolution + PixelScale }, color });
        viewPort.push_back({ { v3.position.x, TileMapResolution + PixelScale }, color });
        viewPort.push_back({ { v4.position.x, TileMapResolution + PixelScale }, color });
    }
    return viewPort;
}

std::vector<std::vector<Shinobu::Frontend::OpenGL::Vertex>> Processor::getLCDOutput() {
    if (shouldNextFrameBeBlank) {
        shouldNextFrameBeBlank = false;
        logger.logWarning("Rendering blank frame");
        return blankScanlines();
    }
    return scanlines;
}

std::vector<Sprite> Processor::getSpriteData() const {
    std::vector<Sprite> sprites = {};
    for (int i = 0; i < NumberOfSpritesInOAM; i++) {
        uint16_t offset = i * 4;
        Sprite sprite = Sprite(spriteAttributeTable[offset], spriteAttributeTable[offset + 1], spriteAttributeTable[offset + 2], SpriteAttributes(spriteAttributeTable[offset + 3]), offset);
        sprites.push_back(sprite);
    }
    return sprites;
}

std::pair<std::vector<Sprite>, std::vector<Shinobu::Frontend::OpenGL::Vertex>> Processor::getSprites() const {
    const palette colors = paletteSelector->currentSelection();
    std::vector<Shinobu::Frontend::OpenGL::Vertex> vertices = {};
    std::vector<Sprite> sprites = getSpriteData();
    for (int i = 0; i < NumberOfSpritesInOAM; i++) {
        Sprite sprite = sprites[i];
        std::vector<Shinobu::Frontend::OpenGL::Vertex> tile = getTileByIndex(sprite.tileNumber, colors);
        tile = translateSpriteOwnCoordinatesToSpriteViewerCoordinates(tile, i);
        vertices.insert(vertices.end(), tile.begin(), tile.end());
    }
    return {sprites, vertices};
}

uint8_t Processor::VRAMBank() const {
    return _VBK.bank;
}
