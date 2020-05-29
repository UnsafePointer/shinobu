#include <iostream>
#include <shinobu/Runner.hpp>
#include <shinobu/Emulator.hpp>
#include <shinobu/Configuration.hpp>

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
    emulator.start();
    return 0;
}
