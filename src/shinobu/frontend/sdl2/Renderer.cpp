#include "shinobu/frontend/sdl2/Renderer.hpp"
#include "common/System.hpp"
#include "core/device/PictureProcessingUnit.hpp"
#include <imgui/sdl/imgui_impl_sdl.h>
#include <imgui/opengl3/imgui_impl_opengl3.h>
#include "shinobu/Configuration.hpp"
#include <limits>

using namespace Shinobu::Frontend::SDL2;

Renderer::Renderer(std::unique_ptr<Shinobu::Frontend::SDL2::Window> &window, std::unique_ptr<Core::Device::PictureProcessingUnit::Processor> &PPU) : Shinobu::Frontend::Renderer(window, PPU), frames(), maxValue(std::numeric_limits<uint32_t>::min()), minValue(std::numeric_limits<uint32_t>::max()) {
    Configuration::Manager *configurationManager = Configuration::Manager::getInstance();
    shouldDisplayPerformanceOverlay = configurationManager->shouldLaunchFullscreen();
    overlayScale = configurationManager->overlayScale();

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

    ImGui::SetNextWindowBgAlpha(0.35f);
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;
    if (shouldDisplayPerformanceOverlay && frames.size() >= 10) {
        ImGui::Begin("Performance", NULL, window_flags);
        ImGui::SetWindowPos(ImVec2(0, 0));
        ImGui::SetWindowFontScale(overlayScale);
        Shinobu::Frontend::Performance::Frame lastFrame = frames.back();
        ImGui::Text("Avg: %d ms\nElaps: %d ms", lastFrame.averageFrameTime, lastFrame.elapsedTime);
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        static float values[10] = {};
        int i = 0;
        for (Shinobu::Frontend::Performance::Frame frame : frames) {
            values[i] = frame.averageFrameTime;
            i++;
        }
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(121/255.0f, 97/255.0f, 177/255.0f, 0.35f));
        ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(121/255.0f, 97/255.0f, 177/255.0f, 1.0f));
        ImGui::PlotLines("", values, IM_ARRAYSIZE(values), 0, "", minValue, maxValue, ImVec2(viewport[0], 0.0f));
        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(window->windowRef());
}

void Renderer::handleSDLEvent(SDL_Event event) {
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_f) {
        shouldDisplayPerformanceOverlay = SDL_GetWindowFlags(window->windowRef()) & SDL_WINDOW_FULLSCREEN_DESKTOP;
        return;
    }
    ImGui_ImplSDL2_ProcessEvent(&event);
 }

void Renderer::setLastPerformanceFrame(Shinobu::Frontend::Performance::Frame frame) {
    frames.push_back(frame);
    minValue = std::min(frame.averageFrameTime, minValue);
    maxValue = std::max(frame.averageFrameTime, maxValue);
    if (frames.size() > 10) {
        frames.pop_front();
    }
}
