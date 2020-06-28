#include "shinobu/Configuration.hpp"
#include <fstream>

using namespace Shinobu;

Configuration::Manager::Manager() : logger(Common::Logs::Logger(Common::Logs::Level::Message, "", false)),
    CPU(Common::Logs::Level::NoLog),
    memory(Common::Logs::Level::NoLog),
    ROM(Common::Logs::Level::NoLog),
    PPU(Common::Logs::Level::NoLog),
    serial(Common::Logs::Level::NoLog),
    disassembler(Common::Logs::Level::NoLog),
    interrupt(Common::Logs::Level::NoLog)
{

}

Configuration::Manager* Configuration::Manager::instance = nullptr;

Configuration::Manager* Configuration::Manager::getInstance() {
    if (instance == nullptr) {
        instance = new Configuration::Manager();
    }
    return instance;
}

Common::Logs::Level Configuration::Manager::CPULogLevel() const {
    return CPU;
}

Common::Logs::Level Configuration::Manager::memoryLogLevel() const {
    return memory;
}

Common::Logs::Level Configuration::Manager::ROMLogLevel() const {
    return ROM;
}

Common::Logs::Level Configuration::Manager::PPULogLevel() const {
    return PPU;
}

Common::Logs::Level Configuration::Manager::serialLogLevel() const {
    return serial;
}

Common::Logs::Level Configuration::Manager::disassemblerLogLevel() const {
    return disassembler;
}

Common::Logs::Level Configuration::Manager::interruptLogLevel() const {
    return interrupt;
}

Common::Logs::Level Configuration::Manager::timerLogLevel() const {
    return timer;
}

Common::Logs::Level Configuration::Manager::openGLLogLevel() const {
    return openGL;
}

Common::Logs::Level Configuration::Manager::joypadLogLevel() const {
    return joypad;
}

Common::Logs::Level Configuration::Manager::soundLogLevel() const {
    return sound;
}

bool Configuration::Manager::shouldTraceLogs() const {
    return trace;
}

bool Configuration::Manager::shouldUseImGuiFrontend() const {
    return useImGuiFrontend;
}

void Configuration::Manager::setupConfigurationFile() const {
    std::ifstream file = std::ifstream(filePath);
    if (file.good()) {
        return;
    }
    logger.logWarning("Local configuration file not found");
    Yaml::Node PPUConfiguration = Yaml::Node();
    Yaml::Node &PPUConfigurationRef = PPUConfiguration;
    PPUConfigurationRef["debugger"] = "false";
    Yaml::Node logConfiguration = Yaml::Node();
    Yaml::Node &logConfigurationRef = logConfiguration;
    logConfigurationRef["CPU"] = "NOLOG";
    logConfigurationRef["memory"] = "NOLOG";
    logConfigurationRef["ROM"] = "NOLOG";
    logConfigurationRef["PPU"] = "NOLOG";
    logConfigurationRef["serial"] = "NOLOG";
    logConfigurationRef["disassembler"] = "NOLOG";
    logConfigurationRef["trace"] = "false";
    logConfigurationRef["interrupt"] = "NOLOG";
    logConfigurationRef["timer"] = "NOLOG";
    logConfigurationRef["openGL"] = "NOLOG";
    logConfigurationRef["joypad"] = "NOLOG";
    logConfigurationRef["sound"] = "NOLOG";
    Yaml::Node configuration = Yaml::Node();
    Yaml::Node &configurationRef = configuration;
    configurationRef["log"] = logConfiguration;
    configurationRef["PPU"] = PPUConfiguration;
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
    disassembler = Common::Logs::levelWithValue(configuration["log"]["disassembler"].As<std::string>());
    interrupt = Common::Logs::levelWithValue(configuration["log"]["interrupt"].As<std::string>());
    timer = Common::Logs::levelWithValue(configuration["log"]["timer"].As<std::string>());
    openGL = Common::Logs::levelWithValue(configuration["log"]["openGL"].As<std::string>());
    joypad = Common::Logs::levelWithValue(configuration["log"]["joypad"].As<std::string>());
    sound = Common::Logs::levelWithValue(configuration["log"]["sound"].As<std::string>());
    trace = configuration["log"]["trace"].As<bool>();
    useImGuiFrontend = configuration["PPU"]["debugger"].As<bool>();
    if (trace) {
        std::filesystem::remove(Common::Logs::filePath);
    }
}
