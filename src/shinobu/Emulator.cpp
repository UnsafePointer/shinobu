#include "shinobu/Emulator.hpp"
#include <iostream>
#include "shinobu/Configuration.hpp"
#include "core/device/PictureProcessingUnit.hpp"
#include <glad/glad.h>
#include "common/System.hpp"
#include "shinobu/frontend/sdl2/Renderer.hpp"
#include "shinobu/frontend/sdl2/Error.hpp"
#include "common/Performance.hpp"
#include "shinobu/frontend/performance/Renderer.hpp"
#include <stdexcept>

using namespace Shinobu;

Emulator::Emulator() : logger(Common::Logs::Level::Message, ""), shouldSkipBootROM(false), currentFrameCycles(), frameCounter(), frameTime(SDL_GetTicks()), frameTimes(), soundQueue(), isMuted(), stopEmulation() {
    Configuration::Manager *configurationManager = Configuration::Manager::getInstance();
    paletteSelector = std::make_unique<Shinobu::Frontend::Palette::Selector>(configurationManager->paletteIndex());

    setupSDL(configurationManager->openGLLogLevel() != Common::Logs::Level::NoLog);

    Shinobu::Frontend::Kind frontend = configurationManager->frontendKind();
    isMuted = configurationManager->shouldMute();

    SDL_DisplayMode displayMode;
    Frontend::SDL2::handleSDL2Error(SDL_GetDesktopDisplayMode(0, &displayMode), logger);

    int heigth = displayMode.h / 2;
    int width = (float)heigth * 1.11;
    window = std::make_unique<Shinobu::Frontend::SDL2::Window>("しのぶ", width, heigth, configurationManager->shouldLaunchFullscreen());
    setupOpenGL();

    interrupt = std::make_unique<Core::Device::Interrupt::Controller>(configurationManager->interruptLogLevel());
    PPU = std::make_unique<Core::Device::PictureProcessingUnit::Processor>(configurationManager->PPULogLevel(), interrupt, paletteSelector);

    switch (frontend) {
    case Shinobu::Frontend::Kind::PPU:
        renderer = std::make_unique<Shinobu::Frontend::Imgui::Renderer>(window, PPU);
        break;
    case Shinobu::Frontend::Kind::SDL:
        renderer = std::make_unique<Shinobu::Frontend::SDL2::Renderer>(window, PPU);
        break;
    case Shinobu::Frontend::Kind::Perf:
        renderer = std::make_unique<Shinobu::Frontend::Performance::Renderer>(window, PPU);
        break;
    case Shinobu::Frontend::Kind::Unknown:
        logger.logError("Unknown frontend configuration");
        break;
    }
    PPU->setRenderer(renderer.get());

    sound = std::make_unique<Core::Device::Sound::Controller>(configurationManager->soundLogLevel(), isMuted);
    sound->setSampleRate(SampleRate);
    timer = std::make_unique<Core::Device::Timer::Controller>(configurationManager->timerLogLevel(), interrupt);
    joypad = std::make_unique<Core::Device::JoypadInput::Controller>(configurationManager->joypadLogLevel(), interrupt);
    cartridge = std::make_unique<Core::ROM::Cartridge>(configurationManager->ROMLogLevel());
    memoryController = std::make_unique<Core::Memory::Controller>(configurationManager->memoryLogLevel(), cartridge, PPU, sound, interrupt, timer, joypad);
    processor = std::make_unique<Core::CPU::Processor>(configurationManager->CPULogLevel(), memoryController, interrupt);
    disassembler = std::make_unique<Core::CPU::Disassembler::Disassembler>(configurationManager->disassemblerLogLevel(), processor);
    interrupt->setProcessor(processor);
    PPU->setMemoryController(memoryController);

    soundQueue.start(SampleRate, 2);
}

Emulator::~Emulator() {
    SDL_Quit();
}

void Emulator::setupSDL(bool debug) const {
    Frontend::SDL2::handleSDL2Error(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO), logger);
    Frontend::SDL2::handleSDL2Error(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4), logger);
    Frontend::SDL2::handleSDL2Error(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5), logger);
    if (debug) {
        Frontend::SDL2::handleSDL2Error(SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG), logger);
    }
}

void Emulator::setupOpenGL() const {
    if (!gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress)) {
        logger.logError("Unable to load OpenGL");
    }
}

void Emulator::enqueueSound() {
    static blip_sample_t buffer[AudioBufferSize];
    long count = sound->readSamples(buffer, AudioBufferSize);
    soundQueue.write(buffer, count);
}

void Emulator::updateCurrentFrameCycles(uint8_t cycles) {
    currentFrameCycles += cycles;
    if (currentFrameCycles < CyclesPerFrame) {
        return;
    }
    currentFrameCycles %= CyclesPerFrame;
    frameCounter++;
    frameTimes += SDL_GetTicks() - frameTime;
    frameTime = SDL_GetTicks();
    if (frameCounter >= 60) {
        frameCounter = 0;
        float averageFrameTime = (float)frameTimes / 60.0f;
        Common::Performance::Frame frame = { averageFrameTime, (float)frameTimes };
        window->updateWindowTitleWithFramePerformance(frame);
        if (renderer->frontendKind() == Shinobu::Frontend::Kind::Perf) {
            dynamic_cast<Shinobu::Frontend::Performance::Renderer*>(renderer.get())->setLastPerformanceFrame(frame);
        }
        frameTimes = 0;
    }
}

void Emulator::setROMFilePath(std::filesystem::path &filePath) {
    cartridge->open(filePath);
    PPU->setCGBFlag(cartridge->cgbFlag());
}

void Emulator::setShouldSkipBootROM(bool skipBootROM) {
    shouldSkipBootROM = skipBootROM;
}

void Emulator::powerUp() {
    memoryController->initialize(shouldSkipBootROM);
    processor->initialize();
}

void Emulator::emulate() {
    try {
        while (sound->availableSamples() <= AudioBufferSize) {
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
            sound->step(cycles);
            PPU->step(cycles);
            timer->step(cycles);
            joypad->updateJoypad();
            processor->checkPendingInterrupts(instruction);
            updateCurrentFrameCycles(cycles);
        }
        enqueueSound();
    } catch(...) {
        stopEmulation = true;
    }
}

void Emulator::handleSDLEvent(SDL_Event event) {
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
        stopEmulation = true;
        return;
    }
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_m) {
        sound->toggleMute();
        return;
    }
    if (joypad->hasGameController()) {
        // TODO: Use Controller events API instead of Joypad
        if (event.type == SDL_JOYBUTTONDOWN && event.button.which == 260) {
            paletteSelector->backwardSelector();
            return;
        }
        if (event.type == SDL_JOYBUTTONDOWN && event.button.which == 261) {
            paletteSelector->forwardSelector();
            return;
        }
    } else {
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_q) {
            paletteSelector->backwardSelector();
            return;
        }
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_e) {
            paletteSelector->forwardSelector();
            return;
        }
    }
    if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE) {
        stopEmulation = true;
        return;
    }
    window->handleSDLEvent(event);
    renderer->handleSDLEvent(event);
}

bool Emulator::shouldExit() const {
    return stopEmulation;
}

void Emulator::saveExternalRAM() {
    memoryController->saveExternalRAM();
}
