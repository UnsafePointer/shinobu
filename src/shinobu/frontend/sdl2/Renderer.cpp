#include "shinobu/frontend/sdl2/Renderer.hpp"
#include <limits>
#include <imgui/sdl/imgui_impl_sdl.h>
#include <imgui/opengl3/imgui_impl_opengl3.h>
#include "common/System.hpp"
#include "core/device/PictureProcessingUnit.hpp"
#include "shinobu/Configuration.hpp"
#include "common/Performance.hpp"

using namespace Shinobu::Frontend::SDL2;

Renderer::Renderer(std::unique_ptr<Shinobu::Frontend::SDL2::Window> &window, std::unique_ptr<Core::Device::PictureProcessingUnit::Processor> &PPU) : Shinobu::Frontend::Renderer(window, PPU) {
    Configuration::Manager *configurationManager = Configuration::Manager::getInstance();
    renderer = std::make_unique<Shinobu::Frontend::OpenGL::TextureRenderer>(HorizontalResolution, VerticalResolution, configurationManager->shouldEmulateScreenDoorEffect());

    overlayScale = configurationManager->overlayScale();

    frames.assign(PerformancePlotPoints, { 16.0f, 1000.0f });

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = &ImGui::GetIO(); (void)io;
    io->ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
    ImGui_ImplSDL2_InitForOpenGL(window->windowRef(), window->GLContext());
    ImGui_ImplOpenGL3_Init();
}

Renderer::~Renderer() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

void Renderer::update() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(window->windowRef());
    ImGui::NewFrame();

    renderer->clear();

    auto textureData = PPU->getLCDData();
    renderer->addTextureData(textureData);
    renderer->render();

    if (overlayScale > 0 && frames.size() >= PerformancePlotPoints) {
        ImGui::SetNextWindowBgAlpha(0.35f);
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;
        ImGui::Begin("Performance", NULL, window_flags);
        ImGui::SetWindowPos(ImVec2(0, 0));
        ImGui::SetWindowFontScale(overlayScale);
        Common::Performance::Frame lastFrame = frames.back();
        ImGui::Text("Avg: %.2f ms\nElaps: %.2f ms", lastFrame.averageFrameTime, lastFrame.elapsedTime);
        static float values[PerformancePlotPoints] = {};
        int i = 0;
        for (Common::Performance::Frame frame : frames) {
            values[i] = frame.averageFrameTime;
            i++;
        }
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(121/255.0f, 97/255.0f, 177/255.0f, 0.35f));
        ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(121/255.0f, 97/255.0f, 177/255.0f, 1.0f));
        ImGui::PlotLines("", values, IM_ARRAYSIZE(values), 0, "", minValue, maxValue, ImVec2(0.0f, 0.0f));
        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(window->windowRef());
}

void Renderer::handleSDLEvent(SDL_Event event) {
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_LSHIFT) {
        renderer->toggleApplyScale();
        return;
    }
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_u) {
        if (overlayScale <= 0) {
            return;
        }
        overlayScale--;
        return;
    }
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_o) {
        if (overlayScale >= 5) {
            return;
        }
        overlayScale++;
        return;
    }
    ImGui_ImplSDL2_ProcessEvent(&event);
}

void Renderer::setLastPerformanceFrame(Common::Performance::Frame frame) {
    frames.push_back(frame);
    minValue = std::min(frame.averageFrameTime, minValue);
    maxValue = std::max(frame.averageFrameTime, maxValue);
    if (frames.size() > PerformancePlotPoints) {
        frames.pop_front();
    }
}
