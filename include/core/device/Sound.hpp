#pragma once
#include <cstdint>
#include "core/Memory.hpp"
#include "common/Logger.hpp"

namespace Core {
    namespace Device {
        namespace Sound {
            namespace Square {
                union Shift_Negate_SweepPeriod {
                    uint8_t _value;
                    struct {
                        uint8_t shift : 3;
                        uint8_t negate : 1;
                        uint8_t sweepPerioid : 3;
                        uint8_t unused : 1;
                    };
                    Shift_Negate_SweepPeriod() : _value() {}
                };
                union LengthLoad_Duty {
                    uint8_t _value;
                    struct {
                        uint8_t lengthLoad : 6;
                        uint8_t duty : 2;
                    };
                    LengthLoad_Duty() : _value() {}
                };
                union Period_Envelope_StartingVolume {
                    uint8_t _value;
                    struct {
                        uint8_t period : 3;
                        uint8_t envelope : 1;
                        uint8_t startingVolume : 4;
                    };
                    Period_Envelope_StartingVolume() : _value() {}
                };
                union FrequencyMSB_LengthEnable_Trigger {
                    uint8_t _value;
                    struct {
                        uint8_t frequencyMSB : 3;
                        uint8_t unused : 3;
                        uint8_t lengthEnable : 1;
                        uint8_t trigger;
                    };
                    FrequencyMSB_LengthEnable_Trigger() : _value() {}
                };

                struct One {
                    Shift_Negate_SweepPeriod _NR10;
                    LengthLoad_Duty _NR11;
                    Period_Envelope_StartingVolume _NR12;
                    uint8_t _NR13;
                    FrequencyMSB_LengthEnable_Trigger _NR14;
                };

                struct Two {
                    LengthLoad_Duty _NR21;
                    Period_Envelope_StartingVolume _NR22;
                    uint8_t _NR23;
                    FrequencyMSB_LengthEnable_Trigger _NR24;
                };
            };

            const Core::Memory::Range AddressRange = Core::Memory::Range(0xFF10, 0x16);

            class Controller {
                Common::Logs::Logger logger;

                Square::One squareOne;
                Square::Two squareTwo;
            public:
                Controller(Common::Logs::Level logLevel);
                ~Controller();
            };
        };
    };
};