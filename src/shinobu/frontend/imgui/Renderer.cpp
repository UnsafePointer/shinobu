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
    LCDOutputRenderer = std::make_unique<Shinobu::Frontend::OpenGL::Renderer>(160, 144, PixelScale);
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
    {
        if (ImGui::Begin("VRAM Tile Data", NULL, ImGuiWindowFlags_NoResize)) {
            tileDataRenderer->addPixels(PPU->getTileDataPixels());
            tileDataRenderer->render();
            ImVec2 VRAMTileDataWindowSize = ImVec2(static_cast<float>(VRAMTileDataViewerWidth * VRAMTileDataSide * PixelScale), static_cast<float>(VRAMTileDataViewerHeight * VRAMTileDataSide * PixelScale));
            ImGui::Image(reinterpret_cast<ImTextureID>(tileDataRenderer->framebufferTextureObject()), VRAMTileDataWindowSize, ImVec2(0, 1), ImVec2(1, 0));
        }
        ImGui::End();
        if (ImGui::Begin("VRAM Background Map", NULL, ImGuiWindowFlags_NoResize)) {
            backgroundMapRenderer->addPixels(PPU->getBackgroundMap01Pixels());
            backgroundMapRenderer->addViewPort(PPU->getScrollingViewPort());
            backgroundMapRenderer->render();
            ImVec2 VRAMBackgroundMapSize = ImVec2(static_cast<float>(VRAMTileBackgroundMapSide * VRAMTileDataSide * PixelScale), static_cast<float>(VRAMTileBackgroundMapSide * VRAMTileDataSide * PixelScale));
            ImGui::Image(reinterpret_cast<ImTextureID>(backgroundMapRenderer->framebufferTextureObject()), VRAMBackgroundMapSize, ImVec2(0, 1), ImVec2(1, 0));
        }
        ImGui::End();
        if (ImGui::Begin("LCD Output", NULL, ImGuiWindowFlags_NoResize)) {
            LCDOutputRenderer->addPixels(PPU->getLCDOutput());
            LCDOutputRenderer->render();
            ImVec2 LCDOutputSize = ImVec2(static_cast<float>(160 * PixelScale), static_cast<float>(144 * PixelScale));
            ImGui::Image(reinterpret_cast<ImTextureID>(LCDOutputRenderer->framebufferTextureObject()), LCDOutputSize, ImVec2(0, 1), ImVec2(1, 0));
        }
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
