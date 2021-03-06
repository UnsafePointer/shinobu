#include "shinobu/Emulator.hpp"
#include <iostream>
#include "shinobu/Configuration.hpp"
#include "core/device/PictureProcessingUnit.hpp"
#include <glad/glad.h>
#include "common/System.hpp"
#include "shinobu/frontend/sdl2/Renderer.hpp"
#include "shinobu/frontend/sdl2/Error.hpp"
#include "common/Performance.hpp"
#include "shinobu/frontend/sdl2/Renderer.hpp"
#include <stdexcept>

using namespace Shinobu::Program;

Emulator::Emulator() : logger(Common::Logs::Level::Message, ""), currentFrameCycles(), frameCounter(), frameTime(SDL_GetTicks()), frameTimes(), soundQueue(), isMuted(), stopEmulation() {
    Shinobu::Configuration::Manager *configurationManager = Shinobu::Configuration::Manager::getInstance();
    paletteSelector = std::make_unique<Shinobu::Frontend::Palette::Selector>(configurationManager->paletteIndex());

    setupSDL(configurationManager->openGLLogLevel() != Common::Logs::Level::NoLog);

    Shinobu::Frontend::Kind frontend = configurationManager->frontendKind();
    isMuted = configurationManager->shouldMute();

    SDL_DisplayMode displayMode;
    Frontend::SDL2::handleSDL2Error(SDL_GetDesktopDisplayMode(0, &displayMode), logger);

    int width, heigth;
    if (configurationManager->shouldForceIntegerScale()) {
        int maxVerticalRatio = (displayMode.h / 2) / VerticalResolution;
        int maxHorizontalRatio = (displayMode.w / 2) / HorizontalResolution;
        int ratio = maxVerticalRatio > maxHorizontalRatio ? maxVerticalRatio : maxHorizontalRatio;
        heigth = VerticalResolution * ratio;
        width = HorizontalResolution * ratio;
    } else {
        heigth = displayMode.h / 2;
        width = (float)heigth * 1.11;
    }

    window = std::make_unique<Shinobu::Frontend::SDL2::Window>("しのぶ", width, heigth, configurationManager->shouldLaunchFullscreen());
    setupOpenGL();

    interrupt = std::make_unique<Core::Device::Interrupt::Controller>(configurationManager->interruptLogLevel());
    DMA = std::make_unique<Core::Device::DirectMemoryAccess::Controller>(configurationManager->DMALogLevel());
    PPU = std::make_unique<Core::Device::PictureProcessingUnit::Processor>(configurationManager->PPULogLevel(), configurationManager->shouldCorrectColors(), interrupt, paletteSelector, DMA);

    switch (frontend) {
    case Shinobu::Frontend::Kind::PPU:
        renderer = std::make_unique<Shinobu::Frontend::Imgui::Renderer>(window, PPU);
        break;
    case Shinobu::Frontend::Kind::SDL:
        renderer = std::make_unique<Shinobu::Frontend::SDL2::Renderer>(window, PPU);
        break;
    case Shinobu::Frontend::Kind::Unknown:
        logger.logError("Unknown frontend configuration");
        break;
    }
    PPU->setRenderer(renderer.get());

    sound = std::make_unique<Core::Device::Sound::Controller>(configurationManager->soundLogLevel(), isMuted);
    sound->setSampleRate(SampleRate);
    timer = std::make_unique<Core::Device::Timer::Controller>(configurationManager->timerLogLevel(), interrupt);
    joypad = std::make_unique<Core::Device::JoypadInput::Controller>(configurationManager->joypadLogLevel(), interrupt, configurationManager->gameControllerName());
    cartridge = std::make_unique<Core::ROM::Cartridge>(configurationManager->ROMLogLevel(), configurationManager->shouldOverrideCGBFlag());
    memoryController = std::make_unique<Core::Memory::Controller>(configurationManager->memoryLogLevel(), cartridge, PPU, sound, interrupt, timer, joypad, DMA);
    processor = std::make_unique<Core::CPU::Processor>(configurationManager->CPULogLevel(), memoryController, interrupt);
    disassembler = std::make_unique<Core::CPU::Disassembler::Disassembler>(configurationManager->disassemblerLogLevel(), processor);
    PPU->setMemoryController(memoryController);
    DMA->setMemoryController(memoryController);

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
        if (renderer->frontendKind() == Shinobu::Frontend::Kind::SDL) {
            dynamic_cast<Shinobu::Frontend::SDL2::Renderer*>(renderer.get())->setLastPerformanceFrame(frame);
        }
        frameTimes = 0;
    }
}

static void *invalid_mem = (void *)1;
void Emulator::crash() const {
    memset((char *)invalid_mem, 1, 100);
}

void Emulator::configure(Shinobu::Program::Configuration configuration) {
    cartridge->open(configuration.ROMFilePath);
    PPU->setCGBFlag(cartridge->cgbFlag());
    window->setROMFilename(configuration.ROMFilePath.filename().string());
    memoryController->initialize(configuration.skipBootROM);
    processor->initialize();
    if (configuration.disassemble) {
        disassembler->configure();
    }
}

void Emulator::emulate() {
    try {
        while (sound->availableSamples() <= AudioBufferSize) {
            Core::CPU::Instructions::Instruction instruction = processor->fetchInstruction();
            disassembler->disassembleWhileExecuting(instruction);
            Core::CPU::Instructions::InstructionHandler<void> handler = processor->decodeInstruction<void>(instruction);
            handler(processor, instruction);
            joypad->updateJoypad();
            processor->checkPendingInterrupts(instruction);
            updateCurrentFrameCycles(memoryController->elapsedCycles());
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
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_d) {
        disassembler->toggleEnabled();
        return;
    }
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_DELETE) {
        crash();
        return;
    }
    if (joypad->hasGameController()) {
        // TODO: Use Controller events API instead of Joypad
        if (event.type == SDL_JOYBUTTONDOWN && (event.button.which == 260 || event.button.which == 262)) {
            paletteSelector->backwardSelector();
            return;
        }
        if (event.type == SDL_JOYBUTTONDOWN && (event.button.which == 261 || event.button.which == 263)) {
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

void Emulator::saveExternalRAM() const {
    memoryController->saveExternalRAM();
}

void Emulator::flushLogs() const {
    logger.flush();
}

void Emulator::disassemble() {
    std::stringstream stream = std::stringstream();
    while (disassembler->canDisassemble()) {
        Core::CPU::Instructions::Instruction instruction = processor->fetchInstruction();
        stream << disassembler->disassemble(instruction);
        stream << std::endl;
    }
    std::filesystem::path disassemblyFilePath = cartridge->disassemblyFilePath();
    std::ofstream logfile = std::ofstream();
    logfile.open(disassemblyFilePath);
    logfile << stream.str();
    logfile.close();
    logger.logDebug("Disassembled ROM at file: %s", disassemblyFilePath.c_str());
}
