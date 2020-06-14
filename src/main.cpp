#include <iostream>
#include <shinobu/Runner.hpp>
#include <shinobu/Emulator.hpp>
#include <shinobu/Configuration.hpp>
#include <SDL2/SDL.h>
#include <common/Timing.hpp>

using namespace Shinobu;

int main(int argc, char* argv[]) {
    Runner runner = Runner();
    runner.configure(argc, argv);
    Configuration::Manager *configurationManager = Configuration::Manager::getInstance();
    configurationManager->setupConfigurationFile();
    configurationManager->loadConfiguration();
    Emulator emulator = Emulator();
    runner.configureEmulator(emulator);
    emulator.powerUp();
    uint32_t initTicks = SDL_GetTicks();
    float interval = 1000;
    interval /= FrameRate;
    while (true) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            emulator.handleSDLEvent(event);
        }
        uint32_t currentTicks = SDL_GetTicks();
        if (initTicks + interval < currentTicks) {
            emulator.emulateFrame();
            initTicks = SDL_GetTicks();
        }
    }
    return 0;
}
