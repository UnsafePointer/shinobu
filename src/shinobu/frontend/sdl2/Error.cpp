#include "shinobu/frontend/sdl2/Error.hpp"
#include "common/Logger.hpp"
#include <SDL2/SDL.h>

void Shinobu::Frontend::SDL2::handleSDL2Error(int errorCode, Common::Logs::Logger logger) {
    if (errorCode == 0) {
        return;
    }
    logger.logError("SDL error: %s", SDL_GetError());
}
