#include "shinobu/frontend/imgui/Renderer.hpp"
#include <glad/glad.h>
#include <imgui/sdl/imgui_impl_sdl.h>
#include <imgui/opengl3/imgui_impl_opengl3.h>
#include <iostream>

using namespace Shinobu::Frontend::Imgui;

Renderer::Renderer(std::unique_ptr<Shinobu::Frontend::SDL2::Window> &window) : window(window), backgroundColor(ImVec4(121/255.0f, 97/255.0f, 177/255.0f, 1.00f)) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = &ImGui::GetIO(); (void)io;
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
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
    uint32_t width, height = 0;
    std::tie(width, height) = window->dimensions();
    ImVec2 syscallWindowSize = ImVec2(static_cast<float>(((width / 3) * 2) - 20), static_cast<float>(height - 20));
    ImGui::NewFrame();
    {
        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
        ImGui::SetNextWindowSize(syscallWindowSize, ImGuiCond_Always);
        ImGui::Begin("VRAM Tile Data", NULL, ImGuiWindowFlags_NoResize);
        {
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar;
            ImGui::BeginChild("Child1", ImVec2(ImGui::GetWindowContentRegionWidth(), static_cast<float>(height - 60)), false, window_flags);
            ImGui::SetScrollHereY();
            ImGui::EndChild();
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
