#include "shinobu/frontend/sdl2/GameController.hpp"

using namespace Shinobu::Frontend::SDL2;

GameController::GameController(Common::Logs::Level logLevel) : logger(logLevel, " [GameController]: "), controller(nullptr) {
    int numberOfJoysticks = SDL_NumJoysticks();
    if (numberOfJoysticks < 0) {
        logger.logError("Error getting number of joysticks: %s", SDL_GetError());
    }
    for (int i = 0; i < SDL_NumJoysticks(); ++i) {
        if (SDL_IsGameController(i)) {
            controller = SDL_GameControllerOpen(i);
            if (controller) {
                break;
            } else {
                logger.logError("Could not open gamecontroller %i: %s", i, SDL_GetError());
            }
        }
    }
}

GameController::~GameController() { }

bool GameController::isGameControllerButtonPressed(Button button) {
    (void)button;
    return false;
}

bool GameController::isKeyboardBindingForButtonPressed(Button button) {
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    switch (button) {
    case Button::Up:
        return state[SDL_SCANCODE_UP];
    case Button::Down:
        return state[SDL_SCANCODE_DOWN];
    case Button::Left:
        return state[SDL_SCANCODE_LEFT];
    case Button::Right:
        return state[SDL_SCANCODE_RIGHT];
    case Button::A:
        return state[SDL_SCANCODE_A];
    case Button::B:
        return state[SDL_SCANCODE_S];
    case Button::Start:
        return state[SDL_SCANCODE_RETURN];
    case Button::Select:
        return state[SDL_SCANCODE_SPACE];
    }
    return false;
}

bool GameController::isButtonPressed(Button button) {
    if (controller != nullptr) {
        return isGameControllerButtonPressed(button);
    } else {
        return isKeyboardBindingForButtonPressed(button);
    }
}
