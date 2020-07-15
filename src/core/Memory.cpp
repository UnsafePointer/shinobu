#include "core/Memory.hpp"
#include <iostream>
#include "core/device/SerialDataTransfer.hpp"
#include "core/device/PictureProcessingUnit.hpp"
#include "core/ROM.hpp"
#include "shinobu/Configuration.hpp"
#include "core/device/Interrupt.hpp"
#include "core/device/Timer.hpp"
#include "core/device/JoypadInput.hpp"
#include "core/device/Sound.hpp"
#include <cstring>
#include "common/System.hpp"

using namespace Core::Memory;

Range::Range(uint32_t start, uint32_t length) : start(start), length(length) {

}

Range::~Range() {

}

std::optional<uint32_t> Range::contains(uint32_t address) const {
    if (address >= start && address < (start + length)) {
        uint32_t offset = address - start;
        return { offset };
    } else {
        return std::nullopt;
    }
 }

BankController::BankController(Common::Logs::Level logLevel,
                               std::unique_ptr<Core::ROM::Cartridge> &cartridge,
                               std::unique_ptr<Core::ROM::BOOT::ROM> &bootROM,
                               std::unique_ptr<Core::Device::PictureProcessingUnit::Processor> &PPU,
                               std::unique_ptr<Core::Device::Sound::Controller> &sound,
                               std::unique_ptr<Core::Device::Interrupt::Controller> &interrupt,
                               std::unique_ptr<Core::Device::Timer::Controller> &timer,
                               std::unique_ptr<Core::Device::JoypadInput::Controller> &joypad) : logger(logLevel, "  [Memory]: "),
                                                                                                 cartridge(cartridge),
                                                                                                 bootROM(bootROM),
                                                                                                 WRAMBank00(),
                                                                                                 WRAMBank01_N(),
                                                                                                 PPU(PPU),
                                                                                                 sound(sound),
                                                                                                 HRAM(),
                                                                                                 externalRAM(),
                                                                                                 interrupt(interrupt),
                                                                                                 timer(timer),
                                                                                                 joypad(joypad) {
    externalRAM.resize(cartridge->RAMSize());
    Shinobu::Configuration::Manager *configurationManager = Shinobu::Configuration::Manager::getInstance();
    serialCommController = std::make_unique<Device::SerialDataTransfer::Controller>(configurationManager->serialLogLevel());
}

BankController::~BankController() {

}

void BankController::loadExternalRAMFromSaveFile() {
    std::filesystem::path filePath = cartridge->saveFilePath();
    if (!std::filesystem::exists(filePath)) {
        logger.logWarning("No save file found.");
        return;
    }

    std::ifstream file = std::ifstream();
    file.open(filePath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        logger.logError("Unable to load save file at path: %s", filePath.string().c_str());
    }

    std::streampos fileSize = file.tellg();
    logger.logMessage("Opened save file path of size: %x", fileSize);

    file.seekg(0, file.beg);
    file.read(reinterpret_cast<char *>(&externalRAM[0]), cartridge->RAMSize());

    if (fileSize > cartridge->RAMSize() && cartridge->header.cartridgeType == Core::ROM::Type::MBC3_TIMER_RAM_BATTERY) {
        uint32_t remainingData = (uint32_t)fileSize - cartridge->RAMSize();
        if (remainingData > ClockDataSize) {
            logger.logError("Can't load incompatible clock data of size: %d", remainingData);
            return;
        }
        std::vector<uint8_t> clockData = std::vector<uint8_t>(ClockDataSize);
        file.read(reinterpret_cast<char *>(&clockData[0]), ClockDataSize);
        dynamic_cast<Core::Memory::MBC3::Controller*>(this)->loadClockData(clockData);
    }

    file.close();
}

void BankController::saveExternalRAM() {
    if (!cartridge->isOpen()) {
        return;
    }
    if (cartridge->header.cartridgeType == Core::ROM::Type::MBC1_RAM_BATTERY ||
        cartridge->header.cartridgeType == Core::ROM::Type::MBC3_RAM_BATTERY ||
        cartridge->header.cartridgeType == Core::ROM::Type::MBC3_TIMER_RAM_BATTERY) {
        std::ofstream saveFile = std::ofstream();
        saveFile.open(cartridge->saveFilePath(), std::ios::out | std::ios::trunc | std::ios::binary);
        saveFile.write(reinterpret_cast<char *>(&externalRAM[0]), externalRAM.size());
        if (cartridge->header.cartridgeType == Core::ROM::Type::MBC3_TIMER_RAM_BATTERY) {
            std::vector<uint8_t> clockData = dynamic_cast<Core::Memory::MBC3::Controller*>(this)->clockData();
            saveFile.write(reinterpret_cast<char *>(&clockData[0]), clockData.size());
        }
        saveFile.close();
    }
}

void BankController::executeDMA(uint8_t value) {
    uint16_t source = value;
    source <<= 8;
    uint16_t sourceEnd = source + 0x9F;
    uint16_t destination = 0xFE00;
    while (source <= sourceEnd) {
        uint8_t value = load(source);
        store(destination, value);
        source++;
        destination++;
    }
}

uint8_t BankController::loadInternal(uint16_t address) const {
    std::optional<uint32_t> offset = VideoRAM.contains(address);
    if (offset) {
        return PPU->VRAMload(*offset);
    }
    offset = WorkRAMBank00.contains(address);
    if (offset) {
        return WRAMBank00[*offset];
    }
    offset = WorkRAMBank01_N.contains(address);
    if (offset) {
        return WRAMBank01_N[*offset];
    }
    offset = EchoRAM.contains(address);
    if (offset) {
        return WRAMBank00[*offset];
    }
    offset = SpriteAttributeTable.contains(address);
    if (offset) {
        logger.logWarning("Unhandled Sprite attribute table load at address: %04x", address);
        return 0;
    }
    offset = NotUsable.contains(address);
    if (offset) {
        logger.logWarning("Unhandled Not Usable load at address: %04x", address);
        return 0;
    }
    offset = I_ORegisters.contains(address);
    if (offset) {
        offset = Device::JoypadInput::AddressRange.contains(address);
        if (offset) {
            return joypad->load();
        }
        offset = Device::SerialDataTransfer::AddressRange.contains(address);
        if (offset) {
            return serialCommController->load(*offset);
        }
        offset = Device::PictureProcessingUnit::AddressRange.contains(address);
        if (offset) {
            return PPU->load(*offset);;
        }
        offset = Core::ROM::BOOT::BootROMRegisterRange.contains(address);
        if (offset) {
            return bootROM->loadLockRegister();
        }
        offset = Core::Device::Interrupt::FlagAddressRange.contains(address);
        if (offset) {
            return interrupt->loadFlag();
        }
        offset = Core::Device::Timer::AddressRange.contains(address);
        if (offset) {
            return timer->load(*offset);
        }
        offset = Core::Device::Sound::AddressRange.contains(address);
        if (offset) {
            return sound->load(address);
        }
        logger.logWarning("Unhandled I/O Register load at address: %04x", address);
        return 0;
    }
    offset = HighRAM.contains(address);
    if (offset) {
        return HRAM[*offset];
    }
    offset = Core::Device::Interrupt::EnableAddressRange.contains(address);
    if (offset) {
        return interrupt->loadEnable();
    }
    logger.logError("Unhandled load at address: %04x", address);
    return 0;
}

void BankController::storeInternal(uint16_t address, uint8_t value) {
    std::optional<uint32_t> offset = VideoRAM.contains(address);
    if (offset) {
        PPU->VRAMStore(*offset, value);
        return;
    }
    offset = WorkRAMBank00.contains(address);
    if (offset) {
        WRAMBank00[*offset] = value;
        return;
    }
    offset = WorkRAMBank01_N.contains(address);
    if (offset) {
        WRAMBank01_N[*offset] = value;
        return;
    }
    offset = EchoRAM.contains(address);
    if (offset) {
        WRAMBank00[*offset] = value;
        return;
    }
    offset = SpriteAttributeTable.contains(address);
    if (offset) {
        PPU->OAMStore(*offset, value);
        return;
    }
    offset = NotUsable.contains(address);
    if (offset) {
        logger.logWarning("Unhandled Not Usable store at address: %04x with value %02x", address, value);
        return;
    }
    offset = I_ORegisters.contains(address);
    if (offset) {
        offset = Device::JoypadInput::AddressRange.contains(address);
        if (offset) {
            joypad->store(value);
            return;
        }
        offset = Device::SerialDataTransfer::AddressRange.contains(address);
        if (offset) {
            serialCommController->store(*offset, value);
            return;
        }
        offset = Device::PictureProcessingUnit::AddressRange.contains(address);
        if (offset) {
            if (Device::PictureProcessingUnit::DMATransferRange.contains(address)) {
                executeDMA(value);
                return;
            } else {
                PPU->store(*offset, value);
                return;
            }
        }
        offset = Core::ROM::BOOT::BootROMRegisterRange.contains(address);
        if (offset) {
            bootROM->storeLockRegister(value);
            return;
        }
        offset = Core::Device::Interrupt::FlagAddressRange.contains(address);
        if (offset) {
            interrupt->storeFlag(value);
            return;
        }
        offset = Device::Timer::AddressRange.contains(address);
        if (offset) {
            timer->store(*offset, value);
            return;
        }
        offset = Device::Sound::AddressRange.contains(address);
        if (offset) {
            sound->store(address, value);
            return;
        }
        logger.logWarning("Unhandled I/O Register write at address: %04x with value: %02x", address, value);
        return;
    }
    offset = HighRAM.contains(address);
    if (offset) {
        HRAM[*offset] = value;
        return;
    }
    offset = Core::Device::Interrupt::EnableAddressRange.contains(address);
    if (offset) {
        interrupt->storeEnable(value);
        return;
    }
    logger.logError("Unhandled store at address: %04x with value: %02x", address, value);
    exit(1);
}

uint8_t ROM::Controller::load(uint16_t address) const {
    std::optional<uint32_t> offset = ROMRange.contains(address);
    if (offset) {
        return cartridge->load(*offset);
    }
    return loadInternal(address);
}

void ROM::Controller::store(uint16_t address, uint8_t value) {
    std::optional<uint32_t> offset = Core::Memory::MBC1::BANK1Range.contains(address);
    if (offset) {
        return;
    }
    storeInternal(address, value);
    return;
}

uint8_t MBC1::Controller::load(uint16_t address) const {
    std::optional<uint32_t> offset = ROMBank00.contains(address);
    if (offset) {
        uint32_t upperMask = mode.mode ? _BANK2.bank2 << 5 : 0x0;
        uint32_t physicalAddress = ((upperMask << 14) % cartridge->ROMSize()) | (address & 0x3FFF);
        return cartridge->load(physicalAddress);
    }
    offset = ROMBank01_N.contains(address);
    if (offset) {
        uint32_t upperMask = _BANK2.bank2 << 5 | _BANK1.bank1;
        uint32_t physicalAddress = ((upperMask << 14) % cartridge->ROMSize()) | (address & 0x3FFF);
        return cartridge->load(physicalAddress);
    }
    offset = ExternalRAM.contains(address);
    if (offset) {
        if (_RAMG.enableAccess == 0b1010) {
            uint32_t upperMask = mode.mode ? _BANK2.bank2 : 0x0;
            uint32_t physicalAddress = (upperMask << 13) | (address & 0x1FFF);
            return externalRAM[physicalAddress];
        } else {
            return 0xFF;
        }
    }
    return loadInternal(address);
}

void MBC1::Controller::store(uint16_t address, uint8_t value) {
    std::optional<uint32_t> offset = RAMGRange.contains(address);
    if (offset) {
        _RAMG._value = value;
        return;
    }
    offset = BANK1Range.contains(address);
    if (offset) {
        _BANK1._value = value;
        if (_BANK1.bank1 == 0) {
            _BANK1.bank1 = 1;
        }
        return;
    }
    offset = BANK2Range.contains(address);
    if (offset) {
        _BANK2._value = value;
        return;
    }
    offset = ModeRange.contains(address);
    if (offset) {
        mode._value = value;
        return;
    }
    offset = ExternalRAM.contains(address);
    if (offset) {
        if (_RAMG.enableAccess == 0b1010) {
            uint32_t upperMask = mode.mode ? _BANK2.bank2 : 0x0;
            uint32_t physicalAddress = (upperMask << 13) | (address & 0x1FFF);
            externalRAM[physicalAddress] = value;
        }
        return;
    }
    storeInternal(address, value);
    return;
}

uint8_t MBC3::Controller::load(uint16_t address) const {
    std::optional<uint32_t> offset = ROMBank00.contains(address);
    if (offset) {
        uint32_t physicalAddress = address & 0x3FFF;
        return cartridge->load(physicalAddress);
    }
    offset = ROMBank01_N.contains(address);
    if (offset) {
        uint32_t upperMask = _ROMBANK._value;
        uint32_t physicalAddress = ((upperMask << 14) % cartridge->ROMSize()) | (address & 0x3FFF);
        return cartridge->load(physicalAddress);
    }
    offset = ExternalRAM.contains(address);
    if (offset) {
        if (_RAMG.enableAccess == 0b1010) {
            if (_RAMBANK_RTCRegister._value <= 0x3) {
                uint32_t upperMask = _RAMBANK_RTCRegister.bank2;
                uint32_t physicalAddress = (upperMask << 13) | (address & 0x1FFF);
                return externalRAM[physicalAddress];
            } else if (_RAMBANK_RTCRegister._value >= 0x08 && _RAMBANK_RTCRegister._value <= 0x0C) {
                if (!hasRTC) {
                    logger.logWarning("Using RTC registers when RTC isn't available");
                }
                switch (_RAMBANK_RTCRegister._value) {
                case 0x08:
                    return _RTCS;
                case 0x09:
                    return _RTCM;
                case 0x0A:
                    return _RTCH;
                case 0x0B:
                    return _RTCDL;
                case 0x0C:
                    return _RTCDH._value;
                }
            } else {
                logger.logMessage("Unhandled load at External RAM range with address: %04x", address);
            }
        } else {
            return 0xFF;
        }
    }
    return loadInternal(address);
}

void MBC3::Controller::store(uint16_t address, uint8_t value) {
    std::optional<uint32_t> offset = RAMG_TimerEnableRange.contains(address);
    if (offset) {
        _RAMG._value = value;
        return;
    }
    offset = ROMBANKRange.contains(address);
    if (offset) {
        _ROMBANK._value = value;
        if (_ROMBANK.bank1 == 0) {
            _ROMBANK.bank1 = 1;
        }
        return;
    }
    offset = RAMBANK_RTCRegisterRange.contains(address);
    if (offset) {
        _RAMBANK_RTCRegister._value = value;
        return;
    }
    offset = LatchClockDataRange.contains(address);
    if (offset) {
        if (!hasRTC) {
            logger.logWarning("Using RTC registers when RTC isn't available");
        }
        if (latchClockData == 0x0 && value == 0x1) {
            calculateTime();
        }
        latchClockData = value;
        return;
    }
    offset = ExternalRAM.contains(address);
    if (offset) {
        if (_RAMG.enableAccess == 0b1010) {
            if (_RAMBANK_RTCRegister._value <= 0x3) {
                uint32_t upperMask = _RAMBANK_RTCRegister.bank2;
                uint32_t physicalAddress = (upperMask << 13) | (address & 0x1FFF);
                externalRAM[physicalAddress] = value;
            } else if (_RAMBANK_RTCRegister._value >= 0x08 && _RAMBANK_RTCRegister._value <= 0x0C) {
                if (!hasRTC) {
                    logger.logWarning("Using RTC registers when RTC isn't available");
                }
                switch (_RAMBANK_RTCRegister._value) {
                case 0x08:
                    _RTCS = value;
                    break;
                case 0x09:
                    _RTCM = value;
                    break;
                case 0x0A:
                    _RTCH = value;
                    break;
                case 0x0B:
                    _RTCDL = value;
                    break;
                case 0x0C:
                    _RTCDH._value = value;
                    break;
                }
            } else {
                logger.logMessage("Unhandled store at External RAM range with address: %04x and value: %02x", address, value);
            }
        }
        return;
    }
    storeInternal(address, value);
    return;
}

void MBC3::Controller::calculateTime(bool overrideHalt) {
    if (_RTCDH.halt && !overrideHalt) {
        return;
    }
    if (!hasRTC) {
        return;
    }
    auto now = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsedTime = now - lastTimePoint;
    auto elapsedTimeMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(elapsedTime);
    if (elapsedTimeMilliseconds.count() < 1000) {
        return;
    }
    elapsedTimeMilliseconds += calculationRemainder;
    elapsedTimeMilliseconds += std::chrono::seconds(_RTCS);
    elapsedTimeMilliseconds += std::chrono::minutes(_RTCM);
    elapsedTimeMilliseconds += std::chrono::hours(_RTCH);
    uint16_t currentDays = _RTCDH.dayCounterMSB << 8 | _RTCDL;
    elapsedTimeMilliseconds += std::chrono::hours(currentDays) * 24;
    auto elapsedHours = std::chrono::duration_cast<std::chrono::hours>(elapsedTimeMilliseconds);
    auto days = elapsedHours / std::chrono::hours(24);
    _RTCDL = days & 0xFF;
    _RTCDH.dayCounterCarry = (days & 0x100) > 0 ? 0x1 : 0x0;
    _RTCH = std::chrono::duration_cast<std::chrono::hours>(elapsedTimeMilliseconds % std::chrono::hours(24)).count();
    _RTCM = std::chrono::duration_cast<std::chrono::minutes>(elapsedTimeMilliseconds % std::chrono::hours(1)).count();
    _RTCS = std::chrono::duration_cast<std::chrono::seconds>(elapsedTimeMilliseconds % std::chrono::minutes(1)).count();
    calculationRemainder = elapsedTimeMilliseconds % std::chrono::seconds(1);
    lastTimePoint = now;
}

std::vector<uint8_t> MBC3::Controller::clockData() {
    uint8_t latchedRTCS = _RTCS;
    uint8_t latchedRTCM = _RTCM;
    uint8_t latchedRTCH = _RTCH;
    uint8_t latchedRTCDL = _RTCDL;
    uint8_t latchedRTCDH = _RTCDH._value;
    calculateTime(true);

    std::vector<uint8_t> clockData = {
        _RTCS, 0x0, 0x0, 0x0,
        _RTCM, 0x0, 0x0, 0x0,
        _RTCH, 0x0, 0x0, 0x0,
        _RTCDL, 0x0, 0x0, 0x0,
        _RTCDH._value, 0x0, 0x0, 0x0,
        latchedRTCS, 0x0, 0x0, 0x0,
        latchedRTCM, 0x0, 0x0, 0x0,
        latchedRTCH, 0x0, 0x0, 0x0,
        latchedRTCDL, 0x0, 0x0, 0x0,
        latchedRTCDH, 0x0, 0x0, 0x0,
    };

    auto epochNow = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    uint8_t serializableEpochNow[sizeof(epochNow)];
    std::copy(reinterpret_cast<char *>(reinterpret_cast<void*>(&epochNow)), reinterpret_cast<char *>(static_cast<void*>(&epochNow)) + sizeof(epochNow), serializableEpochNow);
    clockData.insert(clockData.end(), &serializableEpochNow[0], &serializableEpochNow[sizeof(epochNow)]);
    return clockData;
}

void MBC3::Controller::loadClockData(std::vector<uint8_t> clockData) {
    long int epoch;
    std::memcpy(&epoch, &clockData[40], sizeof(epoch));

    auto value = std::chrono::duration<long int>(epoch);
    lastTimePoint = std::chrono::time_point<std::chrono::system_clock>(value);
    _RTCS = clockData[0];
    _RTCM = clockData[4];
    _RTCH = clockData[8];
    _RTCDL = clockData[12];
    _RTCDH._value = clockData[16];
}

Controller::Controller(Common::Logs::Level logLevel,
                       std::unique_ptr<Core::ROM::Cartridge> &cartridge,
                       std::unique_ptr<Core::Device::PictureProcessingUnit::Processor> &PPU,
                       std::unique_ptr<Core::Device::Sound::Controller> &sound,
                       std::unique_ptr<Core::Device::Interrupt::Controller> &interrupt,
                       std::unique_ptr<Core::Device::Timer::Controller> &timer,
                       std::unique_ptr<Core::Device::JoypadInput::Controller> &joypad) : logger(logLevel, "  [Memory]: "),
                                                                                         cartridge(cartridge),
                                                                                         PPU(PPU),
                                                                                         sound(sound),
                                                                                         interrupt(interrupt),
                                                                                         timer(timer),
                                                                                         joypad(joypad) {
    Shinobu::Configuration::Manager *configurationManager = Shinobu::Configuration::Manager::getInstance();
    bootROM = std::make_unique<Core::ROM::BOOT::ROM>(configurationManager->ROMLogLevel());
}

Controller::~Controller() {

}

void Controller::initialize(bool skipBootROM) {
    bootROM->initialize(skipBootROM);
    if (!cartridge->isOpen()) {
        if (!bootROM->hasBootROM()) {
            logger.logError("No cartridge or BOOT ROM detected, nothing to execute.");
        }
        bankController = std::make_unique<ROM::Controller>(logger.logLevel(), cartridge, bootROM, PPU, sound, interrupt, timer, joypad);
        logger.logWarning("ROM file not open, unable to initialize memory.");
        return;
    }
    Core::ROM::Type cartridgeType = cartridge->header.cartridgeType;
    switch (cartridgeType) {
    case Core::ROM::ROM:
        bankController = std::make_unique<ROM::Controller>(logger.logLevel(), cartridge, bootROM, PPU, sound, interrupt, timer, joypad);
        break;
    case Core::ROM::MBC1:
    case Core::ROM::MBC1_RAM:
    case Core::ROM::MBC1_RAM_BATTERY:
        bankController = std::make_unique<MBC1::Controller>(logger.logLevel(), cartridge, bootROM, PPU, sound, interrupt, timer, joypad);
        break;
    case Core::ROM::MBC3:
    case Core::ROM::MBC3_RAM:
    case Core::ROM::MBC3_RAM_BATTERY:
        bankController = std::make_unique<MBC3::Controller>(logger.logLevel(), cartridge, bootROM, PPU, sound, interrupt, timer, joypad, false);
        break;
    case Core::ROM::MBC3_TIMER_RAM_BATTERY:
        bankController = std::make_unique<MBC3::Controller>(logger.logLevel(), cartridge, bootROM, PPU, sound, interrupt, timer, joypad, true);
        break;
    default:
        logger.logError("Unhandled cartridge type: %02x", cartridgeType);
        break;
    }
    bankController->loadExternalRAMFromSaveFile();
}

bool Controller::hasBootROM() const {
    return bootROM->hasBootROM();
}

void Controller::saveExternalRAM() const {
    bankController->saveExternalRAM();
}

uint8_t Controller::load(uint16_t address) const {
    if (bootROM->isLocked()) {
        std::optional<uint32_t> offset = Core::ROM::BOOT::AddressRange.contains(address);
        if (offset) {
            return bootROM->load(*offset);
        }
    }
    return bankController->load(address);
}

void Controller::store(uint16_t address, uint8_t value) {
    if (bootROM->isLocked()) {
        std::optional<uint32_t> offset = Core::ROM::BOOT::AddressRange.contains(address);
        if (offset) {
            return;
        }
    }
    bankController->store(address, value);
}

uint16_t Controller::loadDoubleWord(uint16_t address) const {
    uint16_t lsb = load(address);
    uint16_t msb = load(address + 1);
    return (msb << 8) | lsb;
}

void Controller::storeDoubleWord(uint16_t address, uint16_t value) {
    uint8_t lsb = value & 0xFF;
    store(address, lsb);
    uint8_t msb = value >> 8;
    store(address + 1, msb);
}
