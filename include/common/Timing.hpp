#pragma once
#include <cstdint>
#include <cmath>

const uint32_t CyclesPerSecond = 4194304;
const uint32_t TotalScanlines = 154;
const double FrameRate = 59.73;
const uint32_t CyclesPerScanline = ceil(((float)CyclesPerSecond / FrameRate) / (float)TotalScanlines);
const uint32_t DividerStepFrequency = 16384;
const uint32_t DividerCycleStep = CyclesPerSecond / DividerStepFrequency;
const uint32_t Clock0StepFrequency = 4096;
const uint32_t Clock0CycleStep = CyclesPerSecond / Clock0StepFrequency;
const uint32_t Clock1StepFrequency = 262144;
const uint32_t Clock1CycleStep = CyclesPerSecond / Clock1StepFrequency;
const uint32_t Clock2StepFrequency = 65536;
const uint32_t Clock2CycleStep = CyclesPerSecond / Clock2StepFrequency;
const uint32_t Clock3StepFrequency = 16384;
const uint32_t Clock3CycleStep = CyclesPerSecond / Clock3StepFrequency;
const uint32_t clocks[] = { Clock0CycleStep, Clock1CycleStep, Clock2CycleStep, Clock3CycleStep };
