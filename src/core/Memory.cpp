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
                               std::unique_ptr<Core::Device::JoypadInput::Controller> &joypad,
                               std::unique_ptr<Core::Device::DirectMemoryAccess::Controller> &DMA) : logger(logLevel, "  [Memory]: "),
                                                                                                     cartridge(cartridge),
                                                                                                     bootROM(bootROM),
                                                                                                     WRAMBank(),
                                                                                                     PPU(PPU),
                                                                                                     sound(sound),
                                                                                                     HRAM(),
                                                                                                     externalRAM(),
                                                                                                     interrupt(interrupt),
                                                                                                     timer(timer),
                                                                                                     joypad(joypad),
                                                                                                     DMA(DMA),
                                                                                                     _SVBK(),
                                                                                                     _KEY1() {
    externalRAM.resize(cartridge->RAMSize());
    if (cartridge->cgbFlag() == Core::ROM::CGBFlag::DMG) {
        WRAMBank.resize(WRAMBankSize * 2);
    } else {
        WRAMBank.resize(WRAMBankSize * 8);
    }
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

    if (fileSize > cartridge->RAMSize() && cartridge->type() == Core::ROM::Type::MBC3_TIMER_RAM_BATTERY) {
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
    Core::ROM::Type cartridgeType = cartridge->type();
    if (cartridgeType == Core::ROM::Type::MBC1_RAM_BATTERY ||
        cartridgeType == Core::ROM::Type::MBC3_RAM_BATTERY ||
        cartridgeType == Core::ROM::Type::MBC3_TIMER_RAM_BATTERY) {
        std::ofstream saveFile = std::ofstream();
        saveFile.open(cartridge->saveFilePath(), std::ios::out | std::ios::trunc | std::ios::binary);
        saveFile.write(reinterpret_cast<char *>(&externalRAM[0]), externalRAM.size());
        if (cartridgeType == Core::ROM::Type::MBC3_TIMER_RAM_BATTERY) {
            std::vector<uint8_t> clockData = dynamic_cast<Core::Memory::MBC3::Controller*>(this)->clockData();
            saveFile.write(reinterpret_cast<char *>(&clockData[0]), clockData.size());
        }
        saveFile.close();
    }
}

void BankController::executeHDMA(uint16_t source, uint16_t destination, uint16_t length) {
    uint16_t sourceEnd = source + length;
    while (source <= sourceEnd) {
        uint8_t value = load(source);
        uint16_t mask = (PPU->VRAMBank() << 13);
        uint16_t physicalAddress = mask | (destination & 0x1FFF);
        PPU->VRAMStore(physicalAddress, value);
        source++;
        destination++;
    }
}

void BankController::handleSpeedSwitch() {
    if (_KEY1.prepareSwitch() == SpeedSwitch::PrepareSwitch::No) {
        return;
    }
    _KEY1.toggleSpeed();
}

SpeedSwitch::Speed BankController::currentSpeed() const {
    return _KEY1.currentSpeed();
}

uint8_t BankController::loadInternal(uint16_t address) const {
    std::optional<uint32_t> offset = VideoRAM.contains(address);
    if (offset) {
        uint16_t mask = (PPU->VRAMBank() << 13);
        uint16_t physicalAddress = mask | (*offset & 0x1FFF);
        return PPU->VRAMLoad(physicalAddress);
    }
    offset = WorkRAMBank00.contains(address);
    if (offset) {
        return WRAMBank[*offset];
    }
    offset = WorkRAMBank01_N.contains(address);
    if (offset) {
        uint32_t upperMask = _SVBK.WRAMBank;
        uint32_t physicalAddress = (upperMask << 12) | (*offset & 0xFFF);
        return WRAMBank[physicalAddress];
    }
    offset = EchoRAM.contains(address);
    if (offset) {
        return WRAMBank[*offset];
    }
    offset = SpriteAttributeTable.contains(address);
    if (offset) {
        return PPU->OAMLoad(*offset);
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
        offset = Device::PictureProcessingUnit::VBKAddressRange.contains(address);
        if (offset) {
            return PPU->VBKLoad(*offset);
        }
        offset = Device::PictureProcessingUnit::HDMARange.contains(address);
        if (offset) {
            return PPU->HDMALoad(*offset);
        }
        offset = Device::PictureProcessingUnit::ColorPaletteRange.contains(address);
        if (offset) {
            return PPU->colorPaletteLoad(*offset);
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
        offset = Core::Memory::SVBKRegisterRange.contains(address);
        if (offset) {
            return _SVBK._value;
        }
        offset = Core::Memory::KEY1AddressRange.contains(address);
        if (offset) {
            return _KEY1._value;
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
        uint16_t mask = (PPU->VRAMBank() << 13);
        uint16_t physicalAddress = mask | (*offset & 0x1FFF);
        PPU->VRAMStore(physicalAddress, value);
        return;
    }
    offset = WorkRAMBank00.contains(address);
    if (offset) {
        WRAMBank[*offset] = value;
        return;
    }
    offset = WorkRAMBank01_N.contains(address);
    if (offset) {
        uint32_t upperMask = _SVBK.WRAMBank;
        uint32_t physicalAddress = (upperMask << 12) | (*offset & 0xFFF);
        WRAMBank[physicalAddress] = value;
        return;
    }
    offset = EchoRAM.contains(address);
    if (offset) {
        WRAMBank[*offset] = value;
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
            PPU->store(*offset, value);
            return;
        }
        offset = Device::PictureProcessingUnit::VBKAddressRange.contains(address);
        if (offset) {
            PPU->VBKStore(*offset, value);
            return;
        }
        offset = Device::PictureProcessingUnit::HDMARange.contains(address);
        if (offset) {
            PPU->HDMAStore(*offset, value);
            return;
        }
        offset = Device::PictureProcessingUnit::ColorPaletteRange.contains(address);
        if (offset) {
            PPU->colorPaletteStore(*offset, value);
            return;
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
        offset = Core::Memory::SVBKRegisterRange.contains(address);
        if (offset) {
            _SVBK._value = value;
            if (_SVBK.WRAMBank == 0x0) {
                _SVBK.WRAMBank = 0x1;
            }
            return;
        }
        offset = Core::Memory::KEY1AddressRange.contains(address);
        if (offset) {
            _KEY1._value = value;
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
    return;
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
                logger.logWarning("Unhandled load at External RAM range with address: %04x", address);
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
                logger.logWarning("Unhandled store at External RAM range with address: %04x and value: %02x", address, value);
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

uint8_t MBC5::Controller::load(uint16_t address) const {
    std::optional<uint32_t> offset = ROMBank00.contains(address);
    if (offset) {
        uint32_t physicalAddress = address & 0x3FFF;
        return cartridge->load(physicalAddress);
    }
    offset = ROMBank01_N.contains(address);
    if (offset) {
        uint32_t upperMask = _ROMB1.ROMBankNumberMSB << 8;
        upperMask |= ROMB0;
        uint32_t physicalAddress = ((upperMask << 14) % cartridge->ROMSize()) | (address & 0x3FFF);
        return cartridge->load(physicalAddress);
    }
    offset = ExternalRAM.contains(address);
    if (offset) {
        if ((RAMG & 0xF) == 0b1010) {
            uint32_t upperMask = _RAMB.RAMBankNumber;
            uint32_t physicalAddress = (upperMask << 13) | (address & 0x1FFF);
            return externalRAM[physicalAddress];
        } else {
            return 0xFF;
        }
    }
    return loadInternal(address);
}

void MBC5::Controller::store(uint16_t address, uint8_t value) {
    std::optional<uint32_t> offset = MBC1::RAMGRange.contains(address);
    if (offset) {
        RAMG = value;
        return;
    }
    offset = ROMB0Range.contains(address);
    if (offset) {
        ROMB0 = value;
        return;
    }
    offset = ROMB1Range.contains(address);
    if (offset) {
        _ROMB1._value = value;
        return;
    }
    offset = RAMBRange.contains(address);
    if (offset) {
        _RAMB._value = value;
        return;
    }
    offset = ExternalRAM.contains(address);
    if (offset) {
        if (RAMG == 0b00001010) {
            uint32_t upperMask = _RAMB.RAMBankNumber;
            uint32_t physicalAddress = (upperMask << 13) | (address & 0x1FFF);
            externalRAM[physicalAddress] = value;
        }
        return;
    }
    storeInternal(address, value);
    return;
}

Controller::Controller(Common::Logs::Level logLevel,
                       std::unique_ptr<Core::ROM::Cartridge> &cartridge,
                       std::unique_ptr<Core::Device::PictureProcessingUnit::Processor> &PPU,
                       std::unique_ptr<Core::Device::Sound::Controller> &sound,
                       std::unique_ptr<Core::Device::Interrupt::Controller> &interrupt,
                       std::unique_ptr<Core::Device::Timer::Controller> &timer,
                       std::unique_ptr<Core::Device::JoypadInput::Controller> &joypad,
                       std::unique_ptr<Core::Device::DirectMemoryAccess::Controller> &DMA) : logger(logLevel, "  [Memory]: "),
                                                                                             cartridge(cartridge),
                                                                                             PPU(PPU),
                                                                                             sound(sound),
                                                                                             interrupt(interrupt),
                                                                                             timer(timer),
                                                                                             joypad(joypad),
                                                                                             DMA(DMA),
                                                                                             cyclesCurrentInstruction(0) {
    Shinobu::Configuration::Manager *configurationManager = Shinobu::Configuration::Manager::getInstance();
    bootROM = std::make_unique<Core::ROM::BOOT::ROM>(configurationManager->ROMLogLevel());
}

Controller::~Controller() {

}

void Controller::beginCurrentInstruction() {
    cyclesCurrentInstruction = 0;
}

void Controller::step(uint8_t cycles) {
    if (cycles == 0) {
        return;
    }
    cyclesCurrentInstruction += cycles;
    timer->step(cycles);
    DMA->step(cycles);
    if (bankController->currentSpeed() == SpeedSwitch::Double) {
        sound->step(cycles / 2);
        PPU->step(cycles / 2);
    } else {
        sound->step(cycles);
        PPU->step(cycles);
    }
}

uint8_t Controller::elapsedCycles() const {
    return cyclesCurrentInstruction;
}

void Controller::handleSpeedSwitch() {
    bankController->handleSpeedSwitch();
}

void Controller::initialize(bool skipBootROM) {
    bootROM->initialize(skipBootROM, cartridge->cgbFlag());
    if (!cartridge->isOpen()) {
        if (!bootROM->hasBootROM()) {
            logger.logError("No cartridge or BOOT ROM detected, nothing to execute.");
        }
        bankController = std::make_unique<ROM::Controller>(logger.logLevel(), cartridge, bootROM, PPU, sound, interrupt, timer, joypad, DMA);
        logger.logWarning("ROM file not open, unable to initialize memory.");
        return;
    }
    Core::ROM::Type cartridgeType = cartridge->type();
    switch (cartridgeType) {
    case Core::ROM::ROM:
        bankController = std::make_unique<ROM::Controller>(logger.logLevel(), cartridge, bootROM, PPU, sound, interrupt, timer, joypad, DMA);
        break;
    case Core::ROM::MBC1:
    case Core::ROM::MBC1_RAM:
    case Core::ROM::MBC1_RAM_BATTERY:
        bankController = std::make_unique<MBC1::Controller>(logger.logLevel(), cartridge, bootROM, PPU, sound, interrupt, timer, joypad, DMA);
        break;
    case Core::ROM::MBC3:
    case Core::ROM::MBC3_RAM:
    case Core::ROM::MBC3_RAM_BATTERY:
        bankController = std::make_unique<MBC3::Controller>(logger.logLevel(), cartridge, bootROM, PPU, sound, interrupt, timer, joypad, DMA, false);
        break;
    case Core::ROM::MBC3_TIMER_BATTERY:
    case Core::ROM::MBC3_TIMER_RAM_BATTERY:
        bankController = std::make_unique<MBC3::Controller>(logger.logLevel(), cartridge, bootROM, PPU, sound, interrupt, timer, joypad, DMA, true);
        break;
    case Core::ROM::MBC5:
    case Core::ROM::MBC5_RAM:
    case Core::ROM::MBC5_RAM_BATTERY:
        bankController = std::make_unique<MBC5::Controller>(logger.logLevel(), cartridge, bootROM, PPU, sound, interrupt, timer, joypad, DMA);
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

uint8_t Controller::load(uint16_t address, bool shouldStep, bool hasPriority) {
    if (shouldStep) {
        step(4);
    }
    if (bootROM->shouldHandleAddress(address, cartridge->cgbFlag())) {
        return bootROM->load(address);
    }
    if (DMA->isActive() && !hasPriority) {
        auto offset = I_ORegisters.contains(address);
        if (offset) {
            return bankController->load(address);
        }
        offset = HighRAM.contains(address);
        if (offset) {
            return bankController->load(address);
        }
        return 0xFF;
    }
    return bankController->load(address);
}

void Controller::store(uint16_t address, uint8_t value, bool shouldStep, bool hasPriority) {
    if (shouldStep) {
        step(4);
    }
    if (bootROM->shouldHandleAddress(address, cartridge->cgbFlag())) {
        return;
    }
    if (DMA->isActive() && !hasPriority) {
        auto offset = I_ORegisters.contains(address);
        if (offset) {
            bankController->store(address, value);
        }
        offset = HighRAM.contains(address);
        if (offset) {
            bankController->store(address, value);
        }
        return;
    }
    bankController->store(address, value);
}

uint16_t Controller::loadDoubleWord(uint16_t address, bool shouldStep) {
    uint16_t lsb = load(address, shouldStep);
    uint16_t msb = load(address + 1, shouldStep);
    return (msb << 8) | lsb;
}

void Controller::storeDoubleWord(uint16_t address, uint16_t value, bool shouldStep) {
    uint8_t lsb = value & 0xFF;
    store(address, lsb, shouldStep);
    uint8_t msb = value >> 8;
    store(address + 1, msb, shouldStep);
}

void Controller::executeHDMA(uint16_t source, uint16_t destination, uint16_t length) {
    bankController->executeHDMA(source, destination, length);
}
