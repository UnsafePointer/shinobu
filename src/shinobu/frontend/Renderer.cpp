#include "shinobu/frontend/Renderer.hpp"

using namespace Shinobu::Frontend;

Renderer::Renderer(std::unique_ptr<Shinobu::Frontend::SDL2::Window> &window, std::unique_ptr<Core::Device::PictureProcessingUnit::Processor> &PPU) : window(window), PPU(PPU) {

}

Renderer::~Renderer() {

}
