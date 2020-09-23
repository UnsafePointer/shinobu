#include "shinobu/frontend/sdl2/GameController.hpp"

using namespace Shinobu::Frontend::SDL2;

GameController::GameController(Common::Logs::Level logLevel, std::string controllerName) : logger(logLevel, " [GameController]: "), controller(nullptr), joystick(nullptr) {
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
        } else {
            SDL_Joystick *currentJoystick = SDL_JoystickOpen(i);
            if (currentJoystick != NULL) {
                const char* joystickName = SDL_JoystickName(currentJoystick);
                if (joystickName != NULL && std::string(joystickName).find(controllerName) != std::string::npos) {
                    logger.logMessage("Using joystick with name: %s", joystickName);
                    joystick = currentJoystick;
                }
            }
        }
    }
}

GameController::~GameController() { }

bool GameController::isGameControllerButtonPressed(Button button) const {
    switch (button) {
    case Button::Up:
        return SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_UP);
    case Button::Down:
        return SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_DOWN);
    case Button::Left:
        return SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
    case Button::Right:
        return SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
    case Button::A:
        return SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_B);
    case Button::B:
        return SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_A);
    case Button::Start:
        return SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_START);
    case Button::Select:
        return SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_BACK);
    }
    return false;
}

bool GameController::isJoystickButtonPressed(Button button) const {
    switch (button) {
    case Button::Up:
        return SDL_JoystickGetAxis(joystick, 1) == std::numeric_limits<int16_t>::min();
    case Button::Down:
        return SDL_JoystickGetAxis(joystick, 1) == std::numeric_limits<int16_t>::max();
    case Button::Left:
        return SDL_JoystickGetAxis(joystick, 0) == std::numeric_limits<int16_t>::min();
    case Button::Right:
        return SDL_JoystickGetAxis(joystick, 0) == std::numeric_limits<int16_t>::max();
    case Button::A:
        return SDL_JoystickGetButton(joystick, 0);
    case Button::B:
        return SDL_JoystickGetButton(joystick, 1);
    case Button::Start:
        return SDL_JoystickGetButton(joystick, 11);
    case Button::Select:
        return SDL_JoystickGetButton(joystick, 10);
    }
    return false;
}

bool GameController::isKeyboardBindingForButtonPressed(Button button) const {
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

bool GameController::isButtonPressed(Button button) const {
    if (controller != nullptr) {
        return isGameControllerButtonPressed(button);
    } else if (joystick != nullptr) {
        return isJoystickButtonPressed(button);
    } else {
        return isKeyboardBindingForButtonPressed(button);
    }
}

bool GameController::hasGameController() const {
    return controller != nullptr || joystick != nullptr;
}
