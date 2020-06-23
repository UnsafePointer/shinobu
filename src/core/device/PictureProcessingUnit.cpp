#include "core/device/PictureProcessingUnit.hpp"
#include <iostream>
#include "common/Timing.hpp"
#include <bitset>
#include "common/System.hpp"
#include "shinobu/frontend/Renderer.hpp"

using namespace Core::Device::PictureProcessingUnit;

Processor::Processor(Common::Logs::Level logLevel, std::unique_ptr<Core::Device::Interrupt::Controller> &interrupt) : logger(logLevel, "  [PPU]: "), interrupt(interrupt), memory(), spriteAttributeTable(), control(), status(), scrollY(), scrollX(), LY(), LYC(), steps(), interruptConditions(), renderer(nullptr), scanlines() {
    interruptConditions[Mode2] = false;
    interruptConditions[Mode1] = false;
    interruptConditions[Mode0] = false;
    interruptConditions[Coincidence] = false;
}

Processor::~Processor() {

}

void Processor::setRenderer(Shinobu::Frontend::Renderer *renderer) {
    this->renderer = renderer;
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
    case 0x7:
        return backgroundPalette._value;
    case 0x8:
        return object0Palette._value;
    case 0x9:
        return object1Palette._value;
    default:
        logger.logWarning("Unhandled Picture Processing Unit load at offset: %04x", offset);
        return 0;
    }
}

void Processor::store(uint16_t offset, uint8_t value) {
    switch (offset) {
    case 0x0:
        control._value = value;
        return;
    case 0x1: {
        status._value &= ~0xF8;
        status._value |= value & 0xF8;
        return;
    }
    case 0x2:
        scrollY = value;
        return;
    case 0x3:
        scrollX = value;
        return;
    case 0x4:
        logger.logWarning("Attempting to write read-only LY register with value: %02x", value);
        return;
    case 0x5:
        LYC = value;
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
    default:
        logger.logWarning("Unhandled Picture Processing Unit at offset: %04x with value: %04x", offset, value);
        return;
    }
}

bool Processor::isAnyConditionMet() {
    return interruptConditions[Mode2] | interruptConditions[Mode1] | interruptConditions[Mode0] | interruptConditions[Coincidence];
}

void Processor::step(uint8_t cycles) {
    if (!control.LCDDisplayEnable) {
        return;
    }
    steps += cycles;
    bool areAnyConditionsMet = isAnyConditionMet();

    if (LY < 144) {
        if (steps <= 80) {
            status.setMode(SearchingOAM);
            interruptConditions[Mode2] = status.mode2InterruptEnable;
        } else if (steps <= 289) {
            status.setMode(TransferingData);
        } else {
            status.setMode(HBlank);
            interruptConditions[Mode0] = status.mode0InterruptEnable;
        }
    } else {
        status.setMode(VBlank);
        interruptConditions[Mode1] = status.mode1InterruptEnable;
    }

    if (steps >= CyclesPerScanline) {
        if (LY <= 143) {
            renderScanline();
        }
        LY++;
        if (LY == 144) {
            interrupt->requestInterrupt(Interrupt::VBLANK);
            renderer->update();
            scanlines.clear();
        }
        status.coincidence = LY == LYC;
        interruptConditions[Coincidence] = status.coincidence;
        if (LY >= TotalScanlines) {
            LY = 0;
        }
        steps %= CyclesPerScanline;
    }

    if (!areAnyConditionsMet && isAnyConditionMet()) {
        interrupt->requestInterrupt(Interrupt::LCDSTAT);
    }
    return;
}

uint8_t Processor::VRAMload(uint16_t offset) const {
    return memory[offset];
}

void Processor::VRAMStore(uint16_t offset, uint8_t value) {
    memory[offset] = value;
}

void Processor::OAMStore(uint16_t offset, uint8_t value) {
    spriteAttributeTable[offset] = value;
}

void Processor::renderScanline() {
    std::vector<Shinobu::Frontend::OpenGL::Vertex> scanline = {};
    uint16_t y = (LY + scrollY) % TileMapResolution;
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
    Background_WindowTileDataLocation tileDataLocation = control.background_WindowTileDataSelect();
    std::vector<Sprite> visibleSprites = {};
    std::vector<Sprite> sprites = getSpriteData();
    for (auto const& sprite : sprites) {
        int16_t spriteY = sprite.y;
        spriteY -= 16;
        int16_t spriteX = sprite.x;
        spriteX -= 8;
        if ((y >= spriteY && y < (spriteY + 8)) && spriteX >= 0 && spriteX < 160) {
            visibleSprites.push_back(sprite);
        }
    }
    const std::array<Shinobu::Frontend::OpenGL::Color, 4> backgroundPaletteColors = { colors[backgroundPalette.color0], colors[backgroundPalette.color1], colors[backgroundPalette.color2], colors[backgroundPalette.color3] };
    const std::array<Shinobu::Frontend::OpenGL::Color, 4> object0PaletteColors = { colors[object0Palette.color0], colors[object0Palette.color1], colors[object0Palette.color2], colors[object0Palette.color3] };
    const std::array<Shinobu::Frontend::OpenGL::Color, 4> object1PaletteColors = { colors[object1Palette.color0], colors[object1Palette.color1], colors[object1Palette.color2], colors[object1Palette.color3] };
    for (int i = 0; i < HorizontalResolution; i++) {
        uint16_t x = (scrollX + i) % TileMapResolution;
        Sprite spriteToDraw;
        bool drawSprite = false;
        for (auto const& sprite : visibleSprites) {
            int16_t spriteX = sprite.x;
            spriteX -= 8;
            spriteX = (scrollX + spriteX) % TileMapResolution;
            if (x >= spriteX && x < spriteX + 8) {
                spriteToDraw = sprite;
                drawSprite = true;
                break;
            }
        }
        uint16_t tileIndex;
        if (drawSprite) {
            tileIndex = spriteToDraw.tileNumber;
        } else {
TILE_LOOKUP:
            uint16_t tileIndexInMap = (x / VRAMTileDataSide) + (y / VRAMTileDataSide) * VRAMTileBackgroundMapSide;
            if (tileDataLocation == _8000_8FFF) {
                uint8_t indexOffset = memory[backgroundMapAddressStart + tileIndexInMap];
                tileIndex = indexOffset;
            } else {
                int8_t indexOffset = memory[backgroundMapAddressStart + tileIndexInMap];
                tileIndex = 256 + indexOffset;
            }
        }
        uint16_t offset = (0x10 * tileIndex);
        uint16_t yInTile;
        if (drawSprite) {
            yInTile = y - (spriteToDraw.y - 16);
        } else {
            yInTile = y % VRAMTileDataSide;
        }
        uint16_t lowAddress = yInTile * 2 + offset;
        uint16_t highAddress = (yInTile * 2 + 1) + offset;
        uint8_t low = memory[lowAddress];
        uint8_t high = memory[highAddress];
        auto colorData = getTileRowPixelsColorIndicesWithData(low, high);
        Shinobu::Frontend::OpenGL::Color color;
        if (drawSprite) {
            uint8_t colorDataIndex = i - (spriteToDraw.x - 8);
            if (spriteToDraw.attributes.xFlip) {
                colorDataIndex = 7 - colorDataIndex;
            }
            uint8_t colorIndex = colorData[colorDataIndex];
            if (colorIndex == 0) {
                drawSprite = false;
                goto TILE_LOOKUP;
            } else {
                if (spriteToDraw.attributes.DMGPalette == 0) {
                    color = object0PaletteColors[colorIndex];
                } else {
                    color = object1PaletteColors[colorIndex];
                }
            }
        } else {
            color = backgroundPaletteColors[colorData[x % 8]];
        }
        Shinobu::Frontend::OpenGL::Vertex vertex = { { (GLfloat)i, (GLfloat)(VerticalResolution - 1 - LY) }, color};
        scanline.push_back(vertex);
    }
    scanlines.insert(scanlines.end(), scanline.begin(), scanline.end());
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

std::vector<Shinobu::Frontend::OpenGL::Vertex> Processor::getTileByIndex(uint16_t index, std::array<Shinobu::Frontend::OpenGL::Color, 4> paletteColors) const {
    std::vector<Shinobu::Frontend::OpenGL::Vertex> tile = {};
    for (int i = 0; i < VRAMTileDataSide; i++) {
        uint16_t offset = (0x10 * index);
        uint16_t lowAddress = i * 2 + offset;
        uint16_t highAddress = (i * 2 + 1) + offset;
        uint8_t low = memory[lowAddress];
        uint8_t high = memory[highAddress];
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
    const std::array<Shinobu::Frontend::OpenGL::Color, 4> paletteColors = { colors[backgroundPalette.color0], colors[backgroundPalette.color1], colors[backgroundPalette.color2], colors[backgroundPalette.color3] };
    Background_WindowTileDataLocation tileDataLocation = control.background_WindowTileDataSelect();
    std::vector<Shinobu::Frontend::OpenGL::Vertex> pixels = {};
    uint16_t index = 0;
    for (int y = (VRAMTileBackgroundMapSide - 1); y >= 0; y--) {
        for (int x = 0; x < VRAMTileBackgroundMapSide; x++) {
            std::vector<Shinobu::Frontend::OpenGL::Vertex> tile;
            if (tileDataLocation == _8000_8FFF) {
                uint8_t tileIndex = memory[backgroundMapAddressStart + index];
                tile = getTileByIndex(tileIndex, paletteColors);
            } else {
                int8_t tileIndex = memory[backgroundMapAddressStart + index];
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
    Shinobu::Frontend::OpenGL::Vertex v1 = { upperLeftTranslated, color };
    Shinobu::Frontend::OpenGL::Vertex v2 = { { upperLeftTranslated.x + HorizontalResolution, upperLeftTranslated.y }, color };
    Shinobu::Frontend::OpenGL::Vertex v3 = { { upperLeftTranslated.x + HorizontalResolution, upperLeftTranslated.y - VerticalResolution }, color };
    Shinobu::Frontend::OpenGL::Vertex v4 = { { upperLeftTranslated.x, upperLeftTranslated.y - VerticalResolution }, color };
    return { v1, v2, v3, v4 };
}

std::vector<Shinobu::Frontend::OpenGL::Vertex> Processor::getLCDOutput() const {
    return scanlines;
}

std::vector<Sprite> Processor::getSpriteData() const {
    std::vector<Sprite> sprites = {};
    for (int i = 0; i < NumberOfSpritesInOAM; i++) {
        uint16_t offset = i * 4;
        Sprite sprite = Sprite(spriteAttributeTable[offset], spriteAttributeTable[offset + 1], spriteAttributeTable[offset + 2], SpriteAttributes(spriteAttributeTable[offset + 3]));
        sprites.push_back(sprite);
    }
    return sprites;
}

std::pair<std::vector<Sprite>, std::vector<Shinobu::Frontend::OpenGL::Vertex>> Processor::getSprites() const {
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
