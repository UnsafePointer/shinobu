#include "shinobu/ArgumentParser.hpp"
#include "shinobu/Emulator.hpp"
#include "shinobu/Configuration.hpp"
#include "shinobu/Sentry.hpp"

using namespace Shinobu;

int main(int argc, char* argv[]) {
    Configuration::Manager *configurationManager = Configuration::Manager::getInstance();
    configurationManager->setupConfigurationFile();
    configurationManager->loadConfiguration();
    Configuration::Sentry::Manager *sentryManager = Configuration::Sentry::Manager::getInstance();
    sentryManager->initialize(configurationManager->getSentryDSN());
    Program::Emulator emulator = Program::Emulator();
    Program::ArgumentParser argvParser = Program::ArgumentParser();
    Program::Configuration configuration = argvParser.parse(argc, argv);
    emulator.configure(configuration);
    if (configuration.disassemble) {
        emulator.disassemble();
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
    emulator.flushLogs();
    sentryManager->shutdown();
    return 0;
}
