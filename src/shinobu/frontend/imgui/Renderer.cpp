#include "shinobu/frontend/imgui/Renderer.hpp"
#include <glad/glad.h>
#include <imgui/sdl/imgui_impl_sdl.h>
#include <imgui/opengl3/imgui_impl_opengl3.h>
#include <iostream>
#include "shinobu/frontend/opengl/Vertex.hpp"

using namespace Shinobu::Frontend::Imgui;

Renderer::Renderer(std::unique_ptr<Shinobu::Frontend::SDL2::Window> &window) : window(window), backgroundColor(ImVec4(121/255.0f, 97/255.0f, 177/255.0f, 1.00f)) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = &ImGui::GetIO(); (void)io;
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
    ImGui_ImplSDL2_InitForOpenGL(window->windowRef(), window->GLContext());
    ImGui_ImplOpenGL3_Init();
    renderer = std::make_unique<Shinobu::Frontend::OpenGL::Renderer>(3, 3, 200);
}

Renderer::~Renderer() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

void Renderer::update() {
    // TODO: Remove test data
    std::vector<Shinobu::Frontend::OpenGL::Vertex> pixels = {
        Shinobu::Frontend::OpenGL::Vertex({0, 0,  255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f}),
        Shinobu::Frontend::OpenGL::Vertex({0, 1,  170.0f / 255.0f, 170.0f / 255.0f, 170.0f / 255.0f}),
        Shinobu::Frontend::OpenGL::Vertex({1, 0,  85.0f / 255.0f, 85.0f / 255.0f, 85.0f / 255.0f}),
        Shinobu::Frontend::OpenGL::Vertex({1, 1,  0.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f}),
    };
    renderer->addPixels(pixels);
    renderer->render();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(window->windowRef());
    uint32_t width, height = 0;
    std::tie(width, height) = window->dimensions();
    ImVec2 VRAMTileDataWindowSize = ImVec2(static_cast<float>(((width / 3) * 2) - 20), static_cast<float>(height - 20));
    ImGui::NewFrame();
    {
        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
        ImGui::SetNextWindowSize(VRAMTileDataWindowSize, ImGuiCond_Always);
        ImGui::Begin("VRAM Tile Data", NULL, ImGuiWindowFlags_NoResize);
        ImGui::GetWindowDrawList()->AddImage(
            reinterpret_cast<ImTextureID>(renderer->framebufferTextureObject()),
            ImVec2(ImGui::GetCursorScreenPos()),
            VRAMTileDataWindowSize,
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
