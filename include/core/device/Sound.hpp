#pragma once
#include <cstdint>
#include "core/Memory.hpp"
#include "common/Logger.hpp"

namespace Core {
    namespace Device {
        namespace Sound {
            namespace Shared {
                union FrequencyMSB_LengthEnable_Trigger {
                    uint8_t _value;
                    struct {
                        uint8_t frequencyMSB : 3;
                        uint8_t unused : 3;
                        uint8_t lengthEnable : 1;
                        uint8_t trigger : 1;
                    };
                    FrequencyMSB_LengthEnable_Trigger() : _value() {}
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
            };
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

                struct One {
                    Shift_Negate_SweepPeriod _NR10;
                    LengthLoad_Duty _NR11;
                    Shared::Period_Envelope_StartingVolume _NR12;
                    uint8_t _NR13;
                    Shared::FrequencyMSB_LengthEnable_Trigger _NR14;
                };

                struct Two {
                    LengthLoad_Duty _NR21;
                    Shared::Period_Envelope_StartingVolume _NR22;
                    uint8_t _NR23;
                    Shared::FrequencyMSB_LengthEnable_Trigger _NR24;
                };
            };

            namespace Wave {
                union DAC {
                    uint8_t _value;
                    struct {
                        uint8_t unused : 7;
                        uint8_t power : 1;
                    };

                    DAC() : _value() {}
                };
                union Volume {
                    uint8_t _value;
                    struct {
                        uint8_t unused : 5;
                        uint8_t code : 2;
                        uint8_t unused2 : 1;
                    };

                    Volume() : _value() {}
                };

                struct Wave {
                    DAC _NR30;
                    uint8_t _NR31;
                    Volume _NR32;
                    uint8_t _NR33;
                    Shared::FrequencyMSB_LengthEnable_Trigger _NR34;
                };
            };

            namespace Noise {
                union LengthLoad {
                    uint8_t _value;
                    struct {
                        uint8_t lengthLoad : 6;
                        uint8_t unused : 2;
                    };
                    LengthLoad() : _value() {}
                };
                union Divisor_LFSRWidthMode_ClockShift {
                    uint8_t _value;
                    struct {
                        uint8_t divisorCode : 3;
                        uint8_t width : 1;
                        uint8_t clockShift : 4;
                    };
                };
                union LengthEnable_Trigger {
                    uint8_t _value;
                    struct {
                        uint8_t frequencyMSB : 6;
                        uint8_t lengthEnable : 1;
                        uint8_t trigger : 1;
                    };
                    LengthEnable_Trigger() : _value() {}
                };

                struct Noise {
                    LengthLoad _NR41;
                    Shared::Period_Envelope_StartingVolume _NR42;
                    Divisor_LFSRWidthMode_ClockShift _NR43;
                    LengthEnable_Trigger _NR44;
                };
            };

            namespace Control {
                union VinLEnable_LeftVolume_VinREnable_RightVolume {
                    uint8_t _value;
                    struct {
                        uint8_t rightVolume : 3;
                        uint8_t vinRightEnable : 1;
                        uint8_t leftVolume : 3;
                        uint8_t vinLeftEnable : 1;
                    };

                    VinLEnable_LeftVolume_VinREnable_RightVolume() : _value() {}
                };
                union LeftEnables_RightEnables {
                    uint8_t _value;
                    struct {
                        uint8_t squareOneRightEnable : 1;
                        uint8_t squareTwoRightEnable : 1;
                        uint8_t waveRightEnable : 1;
                        uint8_t noiseRightEnable : 1;
                        uint8_t squareOneLeftEnable : 1;
                        uint8_t squareTwoLeftEnable : 1;
                        uint8_t waveLeftEnable : 1;
                        uint8_t noiseLeftEnable : 1;
                    };

                    LeftEnables_RightEnables() : _value() {}
                };
                union Power_Statuses {
                    uint8_t _value;
                    struct {
                        uint8_t squareOneLengthStatus : 1;
                        uint8_t squareTwoLengthStatus : 1;
                        uint8_t waveLengthStatus : 1;
                        uint8_t noiseLengthStatus : 1;
                        uint8_t unused : 3;
                        uint8_t power : 1;
                    };
                    Power_Statuses() : _value() {}
                };

                struct Control {
                    VinLEnable_LeftVolume_VinREnable_RightVolume _NR50;
                    LeftEnables_RightEnables _NR51;
                    Power_Statuses _NR52;
                };
            };

            const Core::Memory::Range AddressRange = Core::Memory::Range(0xFF10, 0x17);

            class Controller {
                Common::Logs::Logger logger;

                Square::One squareOne;
                Square::Two squareTwo;
                Wave::Wave wave;
                Noise::Noise noise;
                Control::Control control;
            public:
                Controller(Common::Logs::Level logLevel);
                ~Controller();

                uint8_t load(uint16_t offset) const;
                void store(uint16_t offset, uint8_t value);
            };
        };
    };
};
