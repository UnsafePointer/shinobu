#pragma once
#include <cstdint>
#include <gb_apu/Gb_Apu.h>
#include <gb_apu/Multi_Buffer.h>
#include "common/Logger.hpp"

namespace Core {
    namespace Device {
        namespace Sound {
            class Controller {
                Common::Logs::Logger logger;

                Gb_Apu apu;
                Stereo_Buffer buffer;
                blip_time_t time;

                blip_time_t clock();
            public:
                Controller(Common::Logs::Level logLevel);
                ~Controller();

                uint8_t load(uint16_t address);
                void store(uint16_t address, uint8_t value);
                void endFrame();
            };
        };
    };
};
