#include "shinobu/frontend/sdl2/Window.hpp"

using namespace Shinobu::Frontend::SDL2;

Window::Window(std::string title, uint32_t width, uint32_t height) : logger(Common::Logs::Level::NoLog, ""), title(title), width(width), height(height) {
    Uint32 flags = SDL_WINDOW_OPENGL;
    window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
    windowID = SDL_GetWindowID(window);
    SDLGLContext = SDL_GL_CreateContext(window);
}

Window::~Window() {
    SDL_GL_DeleteContext(SDLGLContext);
    SDL_DestroyWindow(window);
}

std::pair<uint32_t,uint32_t> Window::dimensions() const {
    return { width, height };
}

SDL_Window* Window::windowRef() const {
    return window;
}

SDL_GLContext Window::GLContext() const {
    return SDLGLContext;
}

void Window::handleSDLEvent(SDL_Event event) const {
    if(event.type != SDL_WINDOWEVENT || event.window.windowID != windowID) {
        return;
    }
    switch (event.window.event) {
        case SDL_WINDOWEVENT_CLOSE: {
            logger.logError("Exiting...");
            break;
        }
    }
}

void Window::updateWindowTitleWithSuffix(std::string suffix) const {
    std::string titleCopy = title;
    SDL_SetWindowTitle(window, titleCopy.append(suffix).c_str());
}
