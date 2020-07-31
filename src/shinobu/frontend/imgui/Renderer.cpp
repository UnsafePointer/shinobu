#include "shinobu/frontend/imgui/Renderer.hpp"
#include <glad/glad.h>
#include <imgui/sdl/imgui_impl_sdl.h>
#include <imgui/opengl3/imgui_impl_opengl3.h>
#include <iostream>
#include "shinobu/frontend/opengl/Vertex.hpp"
#include "shinobu/frontend/opengl/Framebuffer.hpp"
#include "core/device/PictureProcessingUnit.hpp"
#include "common/System.hpp"
#include "shinobu/frontend/sdl2/Window.hpp"
#include "common/Performance.hpp"

using namespace Shinobu::Frontend::Imgui;

Renderer::Renderer(std::unique_ptr<Shinobu::Frontend::SDL2::Window> &window, std::unique_ptr<Core::Device::PictureProcessingUnit::Processor> &PPU) :
                        Shinobu::Frontend::Renderer(window, PPU),
                        backgroundColor(ImVec4(121/255.0f, 97/255.0f, 177/255.0f, 1.00f)),
                        backgroundMapTexture(Shinobu::Frontend::OpenGL::Texture(VRAMTileBackgroundMapSide * VRAMTileDataSide * PixelScale, VRAMTileBackgroundMapSide * VRAMTileDataSide * PixelScale)),
                        tileDataTexture(Shinobu::Frontend::OpenGL::Texture(VRAMTileDataViewerWidth * VRAMTileDataSide * PixelScale, VRAMTileDataViewerHeight * VRAMTileDataSide * PixelScale)),
                        LCDOutputTexture(Shinobu::Frontend::OpenGL::Texture(HorizontalResolution * PixelScale, VerticalResolution * PixelScale)),
                        spriteTextures(Shinobu::Frontend::OpenGL::TextureArray(NumberOfSpritesInOAM, VRAMTileDataSide * PixelScale, VRAMTileDataSide * 2 * PixelScale))
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = &ImGui::GetIO(); (void)io;
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
    ImGui_ImplSDL2_InitForOpenGL(window->windowRef(), window->GLContext());
    ImGui_ImplOpenGL3_Init();
    tileDataRenderer = std::make_unique<Shinobu::Frontend::OpenGL::Renderer>(VRAMTileDataViewerWidth * VRAMTileDataSide, VRAMTileDataViewerHeight * VRAMTileDataSide, PixelScale);
    backgroundMapRenderer = std::make_unique<Shinobu::Frontend::OpenGL::Renderer>(VRAMTileBackgroundMapSide * VRAMTileDataSide, VRAMTileBackgroundMapSide * VRAMTileDataSide, PixelScale);
    LCDOutputRenderer = std::make_unique<Shinobu::Frontend::OpenGL::Renderer>(HorizontalResolution, VerticalResolution, PixelScale);
    spriteRenderer = std::make_unique<Shinobu::Frontend::OpenGL::Renderer>(VRAMTileDataSide, VRAMTileDataSide * 2, PixelScale);
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
            static int selection = 0;
            ImGui::RadioButton("Bank 0", &selection, 0); ImGui::SameLine();
            ImGui::RadioButton("Bank 1", &selection, 1);
            {
                Shinobu::Frontend::OpenGL::Framebuffer framebuffer = Shinobu::Frontend::OpenGL::Framebuffer(tileDataTexture);
                tileDataRenderer->addPixels(PPU->getTileDataPixels(selection));
                tileDataRenderer->render();
                ImVec2 size = ImVec2(static_cast<float>(VRAMTileDataViewerWidth * VRAMTileDataSide * PixelScale), static_cast<float>(VRAMTileDataViewerHeight * VRAMTileDataSide * PixelScale));
                ImGui::Image(reinterpret_cast<ImTextureID>(tileDataTexture.getObject()), size, ImVec2(0, 1), ImVec2(1, 0));
            }
        }
        ImGui::End();
        if (ImGui::Begin("VRAM Background Map", NULL, ImGuiWindowFlags_NoResize)) {
            static int selection = Core::Device::PictureProcessingUnit::BackgroundType::Normal;
            ImGui::RadioButton("Background", &selection, Core::Device::PictureProcessingUnit::BackgroundType::Normal); ImGui::SameLine();
            ImGui::RadioButton("Window", &selection, Core::Device::PictureProcessingUnit::BackgroundType::Window);
            {
                Shinobu::Frontend::OpenGL::Framebuffer framebuffer = Shinobu::Frontend::OpenGL::Framebuffer(backgroundMapTexture);
                backgroundMapRenderer->addPixels(PPU->getBackgroundMap(Core::Device::PictureProcessingUnit::BackgroundType(selection)));
                backgroundMapRenderer->addViewPort(PPU->getScrollingViewPort());
                backgroundMapRenderer->render();
                ImVec2 size = ImVec2(static_cast<float>(VRAMTileBackgroundMapSide * VRAMTileDataSide * PixelScale), static_cast<float>(VRAMTileBackgroundMapSide * VRAMTileDataSide * PixelScale));
                ImGui::Image(reinterpret_cast<ImTextureID>(backgroundMapTexture.getObject()), size, ImVec2(0, 1), ImVec2(1, 0));
            }
        }
        ImGui::End();
        if (ImGui::Begin("LCD Output", NULL, ImGuiWindowFlags_NoResize)) {
            {
                Shinobu::Frontend::OpenGL::Framebuffer framebuffer = Shinobu::Frontend::OpenGL::Framebuffer(LCDOutputTexture);
                auto scanlines = PPU->getLCDOutput();
                for (const auto& scanline : scanlines) {
                    LCDOutputRenderer->addPixels(scanline);
                    LCDOutputRenderer->render();
                }
                ImVec2 size = ImVec2(static_cast<float>(HorizontalResolution * PixelScale), static_cast<float>(VerticalResolution * PixelScale));
                ImGui::Image(reinterpret_cast<ImTextureID>(LCDOutputTexture.getObject()), size, ImVec2(0, 1), ImVec2(1, 0));
            }
        }
        ImGui::End();
        if (ImGui::Begin("Sprites", NULL, ImGuiWindowFlags_NoResize)) {
            {
                for (int i = 0; i < NumberOfSpritesInOAM; i++) {
                    Core::Device::PictureProcessingUnit::Sprite sprite;
                    std::vector<Shinobu::Frontend::OpenGL::Vertex> vertices;
                    std::tie(sprite, vertices) = PPU->getSpriteAtIndex(i);
                    auto spriteTexture = spriteTextures.getTextureAtIndex(i);
                    GLsizei width, height;
                    std::tie(width, height) = spriteTextures.getDimensions();
                    Shinobu::Frontend::OpenGL::Framebuffer framebuffer = Shinobu::Frontend::OpenGL::Framebuffer(spriteTexture, width, height);
                    spriteRenderer->addPixels(vertices);
                    spriteRenderer->render();
                    ImGui::BeginGroup();
                    ImVec2 size = ImVec2(static_cast<float>(width + 6), static_cast<float>(height + 6));
                    ImGui::Image(reinterpret_cast<ImTextureID>(spriteTexture), size, ImVec2(0, 1), ImVec2(1, 0));
                    ImGui::SameLine(40);
                    ImGui::BeginGroup();
                    ImGui::Dummy(ImVec2(0.0f, 8.0f));
                    ImGui::Text("X = %04d", (int16_t)sprite.x - 8);
                    ImGui::Text("Y = %04d", (int16_t)sprite.y - 16);
                    ImGui::EndGroup();
                    ImGui::Dummy(ImVec2(0.0f, 5.0f));
                    ImGui::EndGroup();

                    if (i % 2 == 0) {
                        ImGui::SameLine(120);
                    }
                }
            }
        }
        ImGui::End();
        ImGui::Render();
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
