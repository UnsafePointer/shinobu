#include "shinobu/frontend/Renderer.hpp"

using namespace Shinobu::Frontend;

Kind Shinobu::Frontend::kindWithValue(std::string value) {
    if (value.compare("PPU") == 0) {
        return Kind::PPU;
    } else if (value.compare("SDL") == 0) {
        return Kind::SDL;
    }
    return Kind::Unknown;
}

Renderer::Renderer(std::unique_ptr<Shinobu::Frontend::SDL2::Window> &window, std::unique_ptr<Core::Device::PictureProcessingUnit::Processor> &PPU) : window(window), PPU(PPU) {

}

Renderer::~Renderer() {

}
