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
    Program::Emulator emulator = Program::Emulator();
    emulator.configure(runner.configuration());
    emulator.powerUp();
    if (emulator.willDisassemble()) {
        return 0;
    }
    while (!emulator.shouldExit()) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            emulator.handleSDLEvent(event);
        }
        emulator.emulate();
    }
    emulator.saveExternalRAM();
    return 0;
}
