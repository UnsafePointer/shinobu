#include "shinobu/frontend/sdl2/Renderer.hpp"
#include "common/System.hpp"
#include "core/device/PictureProcessingUnit.hpp"
#include <imgui/sdl/imgui_impl_sdl.h>
#include <imgui/opengl3/imgui_impl_opengl3.h>

using namespace Shinobu::Frontend::SDL2;

Renderer::Renderer(std::unique_ptr<Shinobu::Frontend::SDL2::Window> &window, std::unique_ptr<Core::Device::PictureProcessingUnit::Processor> &PPU) : Shinobu::Frontend::Renderer(window, PPU) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = &ImGui::GetIO(); (void)io;
    io->ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
    ImGui_ImplSDL2_InitForOpenGL(window->windowRef(), window->GLContext());
    ImGui_ImplOpenGL3_Init();
    renderer = std::make_unique<Shinobu::Frontend::OpenGL::Renderer>(HorizontalResolution, VerticalResolution, PixelScale, false);
}

Renderer::~Renderer() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

void Renderer::update() {
    renderer->clear();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(window->windowRef());
    ImGui::NewFrame();

    auto scanlines = PPU->getLCDOutput();
    for (const auto& scanline : scanlines) {
        renderer->addPixels(scanline);
        renderer->render();
    }
    renderer->render();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(window->windowRef());
}

void Renderer::handleSDLEvent(SDL_Event event) {
    ImGui_ImplSDL2_ProcessEvent(&event);
 }
