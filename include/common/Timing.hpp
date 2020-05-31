#pragma once
#include <cstdint>
#include <cmath>

const uint32_t CyclesPerSecond = 4194304;
const uint32_t TotalScanlines = 154;
const double FrameRate = 59.73;
const uint32_t CyclesPerScanline = ceil(((float)CyclesPerSecond / FrameRate) / (float)TotalScanlines);
