#include "core/device/PictureProcessingUnit.hpp"
#include <iostream>
#include "common/Timing.hpp"
#include <bitset>
#include "common/System.hpp"

using namespace Core::Device::PictureProcessingUnit;

Processor::Processor(Common::Logs::Level logLevel) : logger(logLevel, "  [PPU]: "), memory(), control(), status(), scrollY(), scrollX(), LY(), LYC(), steps() {
    memory.resize(0x2000);
}

Processor::~Processor() {

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
        logger.logError("Unhandled Picture Processing Unit load at offset: %04x", offset);
        return 0;
    }
}

void Processor::store(uint16_t offset, uint8_t value) {
    switch (offset) {
    case 0x0:
        control._value = value;
        return;
    case 0x1: {
        status._value = value;
        // TODO: update status mode and coincidence
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
        logger.logError("Unhandled Picture Processing Unit at offset: %04x with value: %04x", offset, value);
        return;
    }
}

void Processor::step(uint8_t cycles) {
    if (!control.LCDDisplayEnable) {
        return;
    }
    steps += cycles;
    if (steps >= CyclesPerScanline) {
        LY++;
        if (LY >= TotalScanlines) {
            LY = 0;
        }
        steps %= CyclesPerScanline;
    }
    return;
}

void Processor::VRAMStore(uint16_t offset, uint8_t value) {
    memory[offset] = value;
}

std::vector<Shinobu::Frontend::OpenGL::Color> Processor::getTileRowPixelsWithData(uint8_t low, uint8_t high) const {
    std::vector<Shinobu::Frontend::OpenGL::Color> tileRowPixels = {};
    std::bitset<8> lowBits = std::bitset<8>(low);
    std::bitset<8> highBits = std::bitset<8>(high);
    for (int i = 7; i >= 0; i--) {
        uint8_t highMask = (uint8_t)highBits.test(i) << 0x1;
        uint8_t low = lowBits.test(i);
        uint8_t index = highMask | low;
        Shinobu::Frontend::OpenGL::Color color = backgroundPalette.colorWithIndex(index);
        tileRowPixels.push_back(color);
    }
    return tileRowPixels;
}

std::vector<Shinobu::Frontend::OpenGL::Vertex> Processor::getTileByIndex(uint16_t index) const {
    std::vector<Shinobu::Frontend::OpenGL::Vertex> tile = {};
    for (int i = 0; i < VRAMTileDataSide; i++) {
        uint16_t offset = (0x10 * index);
        uint16_t lowAddress = i * 2 + offset;
        uint16_t highAddress = (i * 2 + 1) + offset;
        uint8_t low = memory[lowAddress];
        uint8_t high = memory[highAddress];
        std::vector<Shinobu::Frontend::OpenGL::Color> colorData = getTileRowPixelsWithData(low, high);
        for (int j = 0; j < VRAMTileDataSide; j++) {
            Shinobu::Frontend::OpenGL::Vertex vertex = { { (GLfloat)j, (GLfloat)(7 - i) }, colorData[j] };
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

std::vector<Shinobu::Frontend::OpenGL::Vertex> Processor::getTileDataPixels() const {
    std::vector<Shinobu::Frontend::OpenGL::Vertex> pixels = {};
    uint16_t index = 0;
    for (int y = (VRAMTileDataViewerHeight - 1); y >= 0; y--) {
        for (int x = 0; x < VRAMTileDataViewerWidth; x++) {
            std::vector<Shinobu::Frontend::OpenGL::Vertex> tile = getTileByIndex(index);
            tile = translateTileOwnCoordinatesToTileDataViewerCoordinates(tile, x, y);
            pixels.insert(pixels.end(), tile.begin(), tile.end());
            index++;
        }
    }
    return pixels;
}
