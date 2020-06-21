#pragma once
#include <cstdint>
#include <cmath>

const uint32_t CyclesPerSecond = 4194304;
const uint32_t TotalScanlines = 154;
const double FrameRate = 59.73;
const uint32_t CyclesPerFrame = ceil((float)CyclesPerSecond / FrameRate);
const uint32_t CyclesPerScanline = ceil((float)CyclesPerFrame / (float)TotalScanlines);
const uint8_t clocks[] = { 9, 3, 5, 7 };
