#include "shinobu/Sentry.hpp"
#ifdef SENTRY
    #include <sentry.h>
#endif
#include <stdexcept>
#include <iostream>

using namespace Shinobu::Configuration::Sentry;

Manager::Manager() : initialized(false) {}

Manager* Manager::instance = nullptr;

Manager* Manager::getInstance() {
    if (instance == nullptr) {
        instance = new Manager();
    }
    return instance;
}

void Manager::initialize(std::string dsn) {
    if (dsn.empty()) {
        return;
    }
    #ifdef SENTRY
        sentry_options_t *options = sentry_options_new();
        sentry_options_set_dsn(options, dsn.c_str());
        sentry_options_set_database_path(options, ".sentry-native");
        sentry_options_set_auto_session_tracking(options, false);
        sentry_options_set_symbolize_stacktraces(options, true);
        sentry_options_set_environment(options, "development");
        if (sentry_init(options) == 0) {
            initialized = true;
        } else {
            std::string message = "Can't initialize Sentry with non-empty DSN.";
            std::cout << message << std::endl;
            throw std::runtime_error(message);
        }
    #endif
}

void Manager::shutdown() {
    if (!initialized) {
        return;
    }
    #ifdef SENTRY
        sentry_shutdown();
    #endif
}
