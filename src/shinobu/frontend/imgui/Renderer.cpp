#include "shinobu/frontend/imgui/Renderer.hpp"
#include <glad/glad.h>
#include <imgui/sdl/imgui_impl_sdl.h>
#include <imgui/opengl3/imgui_impl_opengl3.h>
#include <iostream>
#include "shinobu/frontend/opengl/Vertex.hpp"
#include "shinobu/frontend/opengl/Framebuffer.hpp"
#include "core/device/PictureProcessingUnit.hpp"
#include "common/System.hpp"

using namespace Shinobu::Frontend::Imgui;

Renderer::Renderer(std::unique_ptr<Shinobu::Frontend::SDL2::Window> &window, std::unique_ptr<Core::Device::PictureProcessingUnit::Processor> &PPU) : window(window), PPU(PPU), backgroundColor(ImVec4(121/255.0f, 97/255.0f, 177/255.0f, 1.00f)) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = &ImGui::GetIO(); (void)io;
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
    ImGui_ImplSDL2_InitForOpenGL(window->windowRef(), window->GLContext());
    ImGui_ImplOpenGL3_Init();
    tileDataRenderer = std::make_unique<Shinobu::Frontend::OpenGL::Renderer>(VRAMTileDataViewerWidth * VRAMTileDataSide, VRAMTileDataViewerHeight * VRAMTileDataSide, PixelScale);
    backgroundMapRenderer = std::make_unique<Shinobu::Frontend::OpenGL::Renderer>(VRAMTileBackgroundMapSide * VRAMTileDataSide, VRAMTileBackgroundMapSide * VRAMTileDataSide, PixelScale);
}

Renderer::~Renderer() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

void Renderer::update() {
    tileDataRenderer->addPixels(PPU->getTileDataPixels());
    tileDataRenderer->render();
    backgroundMapRenderer->addPixels(PPU->getBackgroundMap01Pixels());
    backgroundMapRenderer->render();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(window->windowRef());
    ImVec2 VRAMTileDataWindowSize = ImVec2(static_cast<float>(VRAMTileDataViewerWidth * VRAMTileDataSide * PixelScale), static_cast<float>(VRAMTileDataViewerHeight * VRAMTileDataSide * PixelScale));
    ImVec2 VRAMBackgroundMap = ImVec2(static_cast<float>(VRAMTileBackgroundMapSide * VRAMTileDataSide * PixelScale), static_cast<float>(VRAMTileBackgroundMapSide * VRAMTileDataSide * PixelScale));
    ImGui::NewFrame();
    {
        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(VRAMTileDataWindowSize.x, VRAMTileDataWindowSize.y + 5), ImGuiCond_Always);
        ImGui::Begin("VRAM Tile Data", NULL, ImGuiWindowFlags_NoResize);
        ImGui::GetWindowDrawList()->AddImage(
            reinterpret_cast<ImTextureID>(tileDataRenderer->framebufferTextureObject()),
            ImVec2(10, 10),
            ImVec2(10 + VRAMTileDataWindowSize.x, 10 + VRAMTileDataWindowSize.y),
            ImVec2(0, 1),
            ImVec2(1, 0));
        ImGui::End();
        ImGui::SetNextWindowPos(ImVec2(30 + VRAMTileDataWindowSize.x, 10), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(VRAMBackgroundMap.x, VRAMBackgroundMap.y + 5), ImGuiCond_Always);
        ImGui::Begin("VRAM Background Map", NULL, ImGuiWindowFlags_NoResize);
        ImGui::GetWindowDrawList()->AddImage(
            reinterpret_cast<ImTextureID>(backgroundMapRenderer->framebufferTextureObject()),
            ImVec2(30 + VRAMTileDataWindowSize.x, 10),
            ImVec2(30 + VRAMTileDataWindowSize.x + VRAMBackgroundMap.x, 10 + VRAMBackgroundMap.y),
            ImVec2(0, 1),
            ImVec2(1, 0));
        ImGui::End();
    }
    ImGui::Render();
    glViewport(0, 0, (int)io->DisplaySize.x, (int)io->DisplaySize.y);
    glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, backgroundColor.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(window->windowRef());
}

void Renderer::handleSDLEvent(SDL_Event event) {
    ImGui_ImplSDL2_ProcessEvent(&event);
}
