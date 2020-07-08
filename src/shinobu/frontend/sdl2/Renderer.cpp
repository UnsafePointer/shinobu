#include "shinobu/frontend/sdl2/Renderer.hpp"
#include "common/System.hpp"
#include "core/device/PictureProcessingUnit.hpp"
#include "shinobu/Configuration.hpp"
#include <limits>
#include "common/Performance.hpp"

using namespace Shinobu::Frontend::SDL2;

Renderer::Renderer(std::unique_ptr<Shinobu::Frontend::SDL2::Window> &window, std::unique_ptr<Core::Device::PictureProcessingUnit::Processor> &PPU) : Shinobu::Frontend::Renderer(window, PPU) {
    renderer = std::make_unique<Shinobu::Frontend::OpenGL::Renderer>(HorizontalResolution, VerticalResolution, PixelScale, false);
}

Renderer::~Renderer() {
}

void Renderer::update() {
    renderer->clear();

    auto scanlines = PPU->getLCDOutput();
    for (const auto& scanline : scanlines) {
        renderer->addPixels(scanline);
        renderer->render();
    }
    renderer->render();

    SDL_GL_SwapWindow(window->windowRef());
}

void Renderer::handleSDLEvent(SDL_Event event) {
    (void)event;
}
