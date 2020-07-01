#include "shinobu/Emulator.hpp"
#include <iostream>
#include "common/Formatter.hpp"
#include "shinobu/Configuration.hpp"
#include "core/device/PictureProcessingUnit.hpp"
#include <glad/glad.h>
#include "common/System.hpp"
#include "shinobu/frontend/sdl2/Renderer.hpp"
#include "common/Formatter.hpp"
#include "shinobu/frontend/sdl2/Error.hpp"

using namespace Shinobu;

Emulator::Emulator() : logger(Common::Logs::Level::Message, ""), shouldSkipBootROM(false), frameCounter(), frameTimes(), soundQueue(), isMuted() {
    setupSDL();

    Configuration::Manager *configurationManager = Configuration::Manager::getInstance();
    bool shouldUseImGuiFrontend = configurationManager->shouldUseImGuiFrontend();
    isMuted = configurationManager->shouldMute();

    window = std::make_unique<Shinobu::Frontend::SDL2::Window>("しのぶ", WindowWidth, shouldUseImGuiFrontend ? DebugWindowHeight : WindowHeight);
    setupOpenGL();

    interrupt = std::make_unique<Core::Device::Interrupt::Controller>(configurationManager->interruptLogLevel());
    PPU = std::make_unique<Core::Device::PictureProcessingUnit::Processor>(configurationManager->PPULogLevel(), interrupt);

    if (shouldUseImGuiFrontend) {
        renderer = std::make_unique<Shinobu::Frontend::Imgui::Renderer>(window, PPU);
    } else {
        renderer = std::make_unique<Shinobu::Frontend::SDL2::Renderer>(window, PPU);
    }
    PPU->setRenderer(renderer.get());

    sound = std::make_unique<Core::Device::Sound::Controller>(configurationManager->soundLogLevel());
    sound->setSampleRate(SampleRate);
    timer = std::make_unique<Core::Device::Timer::Controller>(configurationManager->timerLogLevel(), interrupt);
    joypad = std::make_unique<Core::Device::JoypadInput::Controller>(configurationManager->joypadLogLevel(), interrupt);
    cartridge = std::make_unique<Core::ROM::Cartridge>(configurationManager->ROMLogLevel());
    memoryController = std::make_unique<Core::Memory::Controller>(configurationManager->memoryLogLevel(), cartridge, PPU, sound, interrupt, timer, joypad);
    processor = std::make_unique<Core::CPU::Processor>(configurationManager->CPULogLevel(), memoryController, interrupt);
    disassembler = std::make_unique<Core::CPU::Disassembler::Disassembler>(configurationManager->disassemblerLogLevel(), processor);
    interrupt->setProcessor(processor);

    if (!isMuted) {
        soundQueue.start(SampleRate, 2);
    }
}

Emulator::~Emulator() {
    SDL_Quit();
}

void Emulator::setupSDL() const {
    Frontend::SDL2::handleSDL2Error(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO), logger);
    Frontend::SDL2::handleSDL2Error(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4), logger);
    Frontend::SDL2::handleSDL2Error(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5), logger);
    Frontend::SDL2::handleSDL2Error(SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG), logger);
}

void Emulator::setupOpenGL() const {
    if (!gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress)) {
        logger.logError("Unable to load OpenGL");
    }
}

void Emulator::enqueueSound() {
    if (isMuted) {
        return;
    }
    sound->endFrame();
    static blip_sample_t buffer[AudioBufferSize];
    long count = sound->readSamples(buffer, AudioBufferSize);
    soundQueue.write(buffer, count);
}

void Emulator::setROMFilePath(std::filesystem::path &filePath) {
    cartridge->open(filePath);
}

void Emulator::setShouldSkipBootROM(bool skipBootROM) {
    shouldSkipBootROM = skipBootROM;
}

void Emulator::powerUp() {
    memoryController->initialize(shouldSkipBootROM);
    processor->initialize();
}

void Emulator::emulateFrame() {
    uint32_t currentCycles = 0;
    uint32_t frameTime = SDL_GetTicks();
    while (currentCycles < CyclesPerFrame) {
        uint8_t cycles;
        Core::CPU::Instructions::Instruction instruction;
        if (!processor->isHalted()) {
            instruction = processor->fetchInstruction();
            disassembler->disassemble(instruction);
            Core::CPU::Instructions::InstructionHandler<uint8_t> handler = processor->decodeInstruction<uint8_t>(instruction);
            cycles = handler(processor, instruction);
        } else {
            instruction = Core::CPU::Instructions::Instruction(0x76, false);
            cycles = 4;
        }
        PPU->step(cycles);
        timer->step(cycles);
        joypad->updateJoypad();
        processor->checkPendingInterrupts(instruction);
        currentCycles += cycles;
    }
    enqueueSound();
    frameTimes += SDL_GetTicks() - frameTime;
    frameCounter++;
    if (frameCounter >= 60) {
        frameCounter = 0;
        uint32_t averageFrameTime = frameTimes / 60;
        window->updateWindowTitleWithSuffix(Common::Formatter::format(" - %d ms - %d ms", averageFrameTime, frameTimes));
        frameTimes = 0;
    }
    memoryController->saveExternalRAM();
}

void Emulator::handleSDLEvent(SDL_Event event) {
    window->handleSDLEvent(event);
    renderer->handleSDLEvent(event);
}
