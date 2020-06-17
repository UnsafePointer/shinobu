#pragma once
#include <cstdint>

const int WindowWidth = 1200;
const int WindowHeight = 1200;
const int PixelScale = 3;
const int NumberOfVRAMTileDataTiles = 384;
const int VRAMTileDataSide = 8;
const int VRAMTileDataViewerWidth = 16;
const int VRAMTileDataViewerHeight = NumberOfVRAMTileDataTiles / VRAMTileDataViewerWidth;
const int VRAMTileBackgroundMapSide = 32;
const int TileMapResolution = VRAMTileDataSide * VRAMTileBackgroundMapSide;
const int HorizontalResolution = 160;
const int VerticalResolution = 144;
