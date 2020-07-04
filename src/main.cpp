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
    while (!emulator.shouldExit()) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            emulator.handleSDLEvent(event);
        }
        emulator.emulateFrame();
    }
    emulator.saveExternalRAM();
    return 0;
}
