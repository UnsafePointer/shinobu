#pragma once
#include <cstdint>
#include <gb_apu/Gb_Apu.h>
#include <gb_apu/Multi_Buffer.h>
#include "common/Logger.hpp"
#include "core/Memory.hpp"

namespace Core {
    namespace Device {
        namespace Sound {
            const Core::Memory::Range AddressRange = Core::Memory::Range(0xFF10, 0x30);

            class Controller {
                Common::Logs::Logger logger;

                Gb_Apu apu;
                Stereo_Buffer buffer;
                blip_time_t time;

                blip_time_t clock();
                bool muted;
            public:
                Controller(Common::Logs::Level logLevel, bool mute);
                ~Controller();

                uint8_t load(uint16_t address);
                void store(uint16_t address, uint8_t value);
                long availableSamples() const;
                typedef blip_sample_t sample_t;
                long readSamples(sample_t* out, long count);
                blargg_err_t setSampleRate(long rate);
                void step(uint8_t cycles);
                void toggleMute();
            };
        };
    };
};
