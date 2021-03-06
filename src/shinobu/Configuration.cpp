#include "shinobu/Configuration.hpp"
#include <fstream>

using namespace Shinobu;

Configuration::Manager::Manager() : logger(Common::Logs::Logger(Common::Logs::Level::Message, "")),
    CPU(Common::Logs::Level::NoLog),
    memory(Common::Logs::Level::NoLog),
    ROM(Common::Logs::Level::NoLog),
    PPU(Common::Logs::Level::NoLog),
    serial(Common::Logs::Level::NoLog),
    disassembler(Common::Logs::Level::NoLog),
    interrupt(Common::Logs::Level::NoLog),
    timer(Common::Logs::Level::NoLog),
    openGL(Common::Logs::Level::NoLog),
    joypad(Common::Logs::Level::NoLog),
    sound(Common::Logs::Level::NoLog),
    DMA(Common::Logs::Level::NoLog),
    frontend(Shinobu::Frontend::Kind::Unknown),
    mute(),
    launchFullscreen(),
    scale(),
    palette(),
    overrideCGBFlag(),
    dmgBootstrapROM(),
    cgbBootstrapROM(),
    colorCorrection(true),
    screenDoorEffect(false),
    forceIntegerScale(false),
    sentryDSN(""),
    controllerName("")
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

Common::Logs::Level Configuration::Manager::DMALogLevel() const {
    return DMA;
}

Shinobu::Frontend::Kind Configuration::Manager::frontendKind() const {
    return frontend;
}

bool Configuration::Manager::shouldMute() const {
    return mute;
}

bool Configuration::Manager::shouldLaunchFullscreen() const {
    return launchFullscreen;
}

int Configuration::Manager::overlayScale() const {
    return scale;
}

int Configuration::Manager::paletteIndex() const {
    return palette;
}

bool Configuration::Manager::shouldOverrideCGBFlag() const {
    return overrideCGBFlag;
}

std::string Configuration::Manager::DMGBootstrapROM() const {
    return dmgBootstrapROM;
}

std::string Configuration::Manager::CGBBootstrapROM() const {
    return cgbBootstrapROM;
}

bool Configuration::Manager::shouldCorrectColors() const {
    return colorCorrection;
}

bool Configuration::Manager::shouldEmulateScreenDoorEffect() const {
    return screenDoorEffect;
}

bool Configuration::Manager::shouldForceIntegerScale() const {
    return forceIntegerScale;
}

std::string Configuration::Manager::getSentryDSN() const {
    return sentryDSN;
}

std::string Configuration::Manager::gameControllerName() const {
    return controllerName;
}

void Configuration::Manager::setupConfigurationFile() const {
    std::ifstream file = std::ifstream(filePath);
    if (file.good()) {
        return;
    }
    logger.logWarning("Local configuration file not found");
    Yaml::Node videoConfiguration = Yaml::Node();
    Yaml::Node &videoConfigurationRef = videoConfiguration;
    videoConfigurationRef["fullscreen"] = "false";
    videoConfigurationRef["overlayScale"] = "0";
    videoConfigurationRef["palette"] = "0";
    videoConfigurationRef["screenDoorEffect"] = "false";
    videoConfigurationRef["integerScale"] = "false";
    Yaml::Node audioConfiguration = Yaml::Node();
    Yaml::Node &audioConfigurationRef = audioConfiguration;
    audioConfigurationRef["mute"] = "false";
    Yaml::Node inputConfiguration = Yaml::Node();
    Yaml::Node &inputConfigurationRef = inputConfiguration;
    inputConfigurationRef["controllerName"] = "";
    Yaml::Node frontendConfiguration = Yaml::Node();
    Yaml::Node &frontendConfigurationRef = frontendConfiguration;
    frontendConfigurationRef["kind"] = "SDL";
    Yaml::Node emulationConfiguration = Yaml::Node();
    Yaml::Node &emulationConfigurationRef = emulationConfiguration;
    emulationConfiguration["overrideCGB"] = "false";
    emulationConfiguration["CGBBootstrapROM"] = "CGB_ROM.BIN";
    emulationConfiguration["DMGBootstrapROM"] = "DMG_ROM.BIN";
    emulationConfiguration["colorCorrection"] = "true";
    Yaml::Node logConfiguration = Yaml::Node();
    Yaml::Node &logConfigurationRef = logConfiguration;
    logConfigurationRef["CPU"] = "NOLOG";
    logConfigurationRef["memory"] = "NOLOG";
    logConfigurationRef["ROM"] = "NOLOG";
    logConfigurationRef["PPU"] = "NOLOG";
    logConfigurationRef["serial"] = "NOLOG";
    logConfigurationRef["disassembler"] = "NOLOG";
    logConfigurationRef["interrupt"] = "NOLOG";
    logConfigurationRef["timer"] = "NOLOG";
    logConfigurationRef["openGL"] = "NOLOG";
    logConfigurationRef["joypad"] = "NOLOG";
    logConfigurationRef["sound"] = "NOLOG";
    logConfigurationRef["DMA"] = "NOLOG";
    Yaml::Node sentryConfiguration = Yaml::Node();
    Yaml::Node &sentryConfigurationRef = sentryConfiguration;
    sentryConfigurationRef["dsn"] = "";
    Yaml::Node configuration = Yaml::Node();
    Yaml::Node &configurationRef = configuration;
    configurationRef["log"] = logConfiguration;
    configurationRef["frontend"] = frontendConfiguration;
    configurationRef["audio"] = audioConfiguration;
    configurationRef["video"] = videoConfiguration;
    configurationRef["emulation"] = emulationConfigurationRef;
    configurationRef["sentry"] = sentryConfigurationRef;
    configurationRef["input"] = inputConfigurationRef;
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
    DMA = Common::Logs::levelWithValue(configuration["log"]["DMA"].As<std::string>());
    frontend = Shinobu::Frontend::kindWithValue(configuration["frontend"]["kind"].As<std::string>());
    mute = configuration["audio"]["mute"].As<bool>();
    launchFullscreen = configuration["video"]["fullscreen"].As<bool>();
    scale = configuration["video"]["overlayScale"].As<int>();
    palette = configuration["video"]["palette"].As<int>();
    screenDoorEffect = configuration["video"]["screenDoorEffect"].As<bool>();
    forceIntegerScale = configuration["video"]["integerScale"].As<bool>();
    overrideCGBFlag = configuration["emulation"]["overrideCGB"].As<bool>();
    dmgBootstrapROM = configuration["emulation"]["DMGBootstrapROM"].As<std::string>();
    cgbBootstrapROM = configuration["emulation"]["CGBBootstrapROM"].As<std::string>();
    colorCorrection = configuration["emulation"]["colorCorrection"].As<bool>();
    sentryDSN = configuration["sentry"]["dsn"].As<std::string>();
    controllerName = configuration["input"]["controllerName"].As<std::string>();
    std::filesystem::remove(Common::Logs::filePath);
}
