#pragma once
#include <cstdint>

const int PixelScale = 3;
const int VRAMTileDataSide = 8;
const int VRAMTileDataViewerWidth = 16;
const int VRAMTileDataViewerHeight = 32;
const int VRAMTileBackgroundMapSide = 32;
const int TileMapResolution = VRAMTileDataSide * VRAMTileBackgroundMapSide;
const int HorizontalResolution = 160;
const int VerticalResolution = 144;
const int NumberOfSpritesInOAM = 40;
const int SampleRate = 44100;
const int AudioBufferSize = 2048;
const int PerformancePlotPoints = 20;
const int ClockDataSize = 48;
const int WRAMBankSize = 0x1000;
