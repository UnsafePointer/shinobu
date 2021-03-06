#include "shinobu/frontend/sdl2/Window.hpp"
#include <glad/glad.h>
#include "common/Formatter.hpp"
#include "common/Performance.hpp"

using namespace Shinobu::Frontend::SDL2;

Window::Window(std::string title, uint32_t width, uint32_t height, bool fullscreen) : logger(Common::Logs::Level::NoLog, ""), title(title), ROMfilename(""), width(width), height(height), lastFullscreenViewport(std::nullopt) {
    Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
    if (fullscreen) {
        flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
        SDL_ShowCursor(SDL_DISABLE);
    }
    window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
    SDL_SetWindowMinimumSize(window, width, height);
    windowID = SDL_GetWindowID(window);
    SDLGLContext = SDL_GL_CreateContext(window);
}

Window::~Window() {
    SDL_GL_DeleteContext(SDLGLContext);
    SDL_DestroyWindow(window);
}

void Window::toggleFullscreen() const {
    bool fullscreen = SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN_DESKTOP;
    if (!fullscreen) {
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
        SDL_ShowCursor(SDL_DISABLE);
    } else {
        SDL_SetWindowFullscreen(window, 0);
        SDL_ShowCursor(SDL_ENABLE);
    }
}

void Window::handleWindowResize(uint32_t newWidth, uint32_t newHeight) {
    glMatrixMode(GL_PROJECTION_MATRIX);
    glLoadIdentity();
    glOrtho(0, width, height, 0, 1, -1);
    float horizontalRatio = newWidth / (float)width;
    float verticalRatio = newHeight / (float)height;
    float ratio = horizontalRatio < verticalRatio ? horizontalRatio : verticalRatio;
    float viewWidth = width * ratio;
    float viewHeight = height * ratio;
    uint32_t viewPositionX = (newWidth - width * ratio) / 2;
    uint32_t viewPositionY = (newHeight - height * ratio) / 2;
    lastFullscreenViewport = std::make_tuple(viewPositionX, viewPositionY, viewWidth, viewHeight);
    glViewport(viewPositionX, viewPositionY, viewWidth, viewHeight);
    glMatrixMode(GL_MODELVIEW_MATRIX);
    glLoadIdentity();
}

std::pair<uint32_t,uint32_t> Window::dimensions() const {
    return { width, height };
}

std::optional<std::tuple<uint32_t,uint32_t,float,float>> Window::fullscreenViewport() const {
    return lastFullscreenViewport;
}

SDL_Window* Window::windowRef() const {
    return window;
}

SDL_GLContext Window::GLContext() const {
    return SDLGLContext;
}

void Window::handleSDLEvent(SDL_Event event) {
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_f) {
        toggleFullscreen();
        return;
    }
    if (event.type != SDL_WINDOWEVENT || event.window.windowID != windowID) {
        return;
    }
    switch (event.window.event) {
        case SDL_WINDOWEVENT_SIZE_CHANGED: {
            handleWindowResize(event.window.data1, event.window.data2);
            break;
        }
    }
}

void Window::updateWindowTitleWithFramePerformance(Common::Performance::Frame frame) const {
    std::string updatedTitle = Common::Formatter::format("%s - %s - %.2f ms - %.2f ms", title.c_str(), ROMfilename.c_str(), frame.averageFrameTime, frame.elapsedTime);
    SDL_SetWindowTitle(window, updatedTitle.c_str());
}

void Window::setROMFilename(std::string filename) {
    ROMfilename = filename;
}
