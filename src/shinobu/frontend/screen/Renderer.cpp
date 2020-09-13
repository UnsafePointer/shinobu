#include "shinobu/frontend/screen/Renderer.hpp"
#include "common/System.hpp"
#include "core/device/PictureProcessingUnit.hpp"
#include "shinobu/frontend/opengl/Framebuffer.hpp"

using namespace Shinobu::Frontend::Screen;

Shinobu::Frontend::Screen::Renderer::Renderer(std::unique_ptr<Shinobu::Frontend::SDL2::Window> &window, std::unique_ptr<Core::Device::PictureProcessingUnit::Processor> &PPU) : Shinobu::Frontend::Renderer(window, PPU) {
    renderer = std::make_unique<Shinobu::Frontend::OpenGL::Renderer>(HorizontalResolution, VerticalResolution, 1);
    program = std::make_unique<Shinobu::Frontend::OpenGL::Program>("glsl/screen_vertex.glsl", "glsl/screen_fragment.glsl");
    texture = std::make_unique<Shinobu::Frontend::OpenGL::Texture>(HorizontalResolution, VerticalResolution);
    buffer = std::make_unique<Shinobu::Frontend::OpenGL::Buffer<Shinobu::Frontend::OpenGL::Texel>>(program, 4);
}

Shinobu::Frontend::Screen::Renderer::~Renderer() {
}

void Renderer::update() {
    GLsizei width, height;
    std::tie(width, height) = texture->dimensions();
    glViewport(0, 0, width, height);
    {
        Shinobu::Frontend::OpenGL::Framebuffer framebuffer = Shinobu::Frontend::OpenGL::Framebuffer(texture->getObject(), width, height);
        renderer->clear();
        auto scanlines = PPU->getLCDOutput();
        for (const auto& scanline : scanlines) {
            renderer->addPixels(scanline);
        }
        renderer->render();
    }

    std::tie(width, height) = window->dimensions();
    glViewport(0, 0, width, height);
    texture->bind(GL_TEXTURE0);
    program->useProgram();
    std::vector<Shinobu::Frontend::OpenGL::Texel> data = {
        Shinobu::Frontend::OpenGL::Texel({{-1.0f, -1.0f}, {0.0f, 1.0f}}),
        Shinobu::Frontend::OpenGL::Texel({{1.0f, -1.0f}, {1.0f, 1.0f}}),
        Shinobu::Frontend::OpenGL::Texel({{-1.0f, 1.0f}, {0.0f, 0.0f}}),
        Shinobu::Frontend::OpenGL::Texel({{1.0f, 1.0f}, {1.0f, 0.0f}}),
    };
    buffer->addData(data.data(), data.size());
    buffer->draw(GL_TRIANGLE_STRIP);

    SDL_GL_SwapWindow(window->windowRef());
}

void Renderer::handleSDLEvent(SDL_Event event) {
    (void)event;
}
