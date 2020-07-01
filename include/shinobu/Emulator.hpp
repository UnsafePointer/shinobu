#pragma once
#include <filesystem>
#include <memory>
#include "core/cpu/CPU.hpp"
#include "core/ROM.hpp"
#include "core/Memory.hpp"
#include "core/device/PictureProcessingUnit.hpp"
#include "core/device/Interrupt.hpp"
#include "core/device/Timer.hpp"
#include "core/cpu/Disassembler.hpp"
#include "shinobu/frontend/sdl2/Window.hpp"
#include "shinobu/frontend/imgui/Renderer.hpp"
#include "core/device/JoypadInput.hpp"
#include "core/device/Sound.hpp"
#include <gb_apu/Sound_Queue.h>
#include "common/Logger.hpp"

namespace Shinobu {
    class Emulator {
        Common::Logs::Logger logger;

        std::unique_ptr<Shinobu::Frontend::SDL2::Window> window;
        std::unique_ptr<Shinobu::Frontend::Renderer> renderer;

        std::unique_ptr<Core::CPU::Processor> processor;
        std::unique_ptr<Core::ROM::Cartridge> cartridge;
        std::unique_ptr<Core::Memory::Controller> memoryController;
        std::unique_ptr<Core::Device::Sound::Controller> sound;
        std::unique_ptr<Core::Device::PictureProcessingUnit::Processor> PPU;
        std::unique_ptr<Core::Device::Interrupt::Controller> interrupt;
        std::unique_ptr<Core::Device::Timer::Controller> timer;
        std::unique_ptr<Core::Device::JoypadInput::Controller> joypad;
        std::unique_ptr<Core::CPU::Disassembler::Disassembler> disassembler;

        bool shouldSkipBootROM;
        uint32_t frameCounter;
        uint32_t frameTimes;

        Sound_Queue soundQueue;
        bool isMuted;

        void setupSDL() const;
        void setupOpenGL() const;
        void enqueueSound();
    public:
        Emulator();
        ~Emulator();

        void setROMFilePath(std::filesystem::path &filePath);
        void setShouldSkipBootROM(bool skipBootROM);
        void powerUp();
        void emulateFrame();
        void handleSDLEvent(SDL_Event event);
    };
};
