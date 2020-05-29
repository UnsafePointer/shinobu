#include "shinobu/Configuration.hpp"
#include <fstream>

using namespace Shinobu;

Configuration::Manager::Manager() : logger(Common::Logs::Logger(Common::Logs::Level::Message, "")),
    CPU(Common::Logs::Level::NoLog),
    memory(Common::Logs::Level::NoLog),
    ROM(Common::Logs::Level::NoLog),
    PPU(Common::Logs::Level::NoLog),
    serial(Common::Logs::Level::NoLog)
{

}

Configuration::Manager* Configuration::Manager::instance = nullptr;

Configuration::Manager* Configuration::Manager::getInstance() {
    if (instance == nullptr) {
        instance = new Configuration::Manager();
    }
    return instance;
}

Common::Logs::Level Configuration::Manager::CPULogLevel() {
    return CPU;
}

Common::Logs::Level Configuration::Manager::memoryLogLevel() {
    return memory;
}

Common::Logs::Level Configuration::Manager::ROMLogLevel() {
    return ROM;
}

Common::Logs::Level Configuration::Manager::PPULogLevel() {
    return PPU;
}

Common::Logs::Level Configuration::Manager::serialLogLevel() {
    return serial;
}

void Configuration::Manager::setupConfigurationFile() const {
    std::ifstream file = std::ifstream(filePath);
    if (file.good()) {
        return;
    }
    logger.logWarning("Local configuration file not found");
    Yaml::Node logConfiguration = Yaml::Node();
    Yaml::Node &logConfigurationRef = logConfiguration;
    logConfigurationRef["CPU"] = "NOLOG";
    logConfigurationRef["memory"] = "NOLOG";
    logConfigurationRef["ROM"] = "NOLOG";
    logConfigurationRef["PPU"] = "NOLOG";
    logConfigurationRef["serial"] = "NOLOG";
    Yaml::Node configuration = Yaml::Node();
    Yaml::Node &configurationRef = configuration;
    configurationRef["log"] = logConfiguration;
    Yaml::Serialize(configuration, filePath.string().c_str());
}

void Configuration::Manager::loadConfiguration() {
    Yaml::Node configuration = Yaml::Node();
    Yaml::Parse(configuration, filePath.string().c_str());
    CPU = Common::Logs::levelWithValue(configuration["log"]["CPU"].As<std::string>());
    memory = Common::Logs::levelWithValue(configuration["log"]["memory"].As<std::string>());
    ROM = Common::Logs::levelWithValue(configuration["log"]["ROM"].As<std::string>());
    PPU = Common::Logs::levelWithValue(configuration["log"]["PPU"].As<std::string>());
    serial = Common::Logs::levelWithValue(configuration["log"]["serial"].As<std::string>());
}
