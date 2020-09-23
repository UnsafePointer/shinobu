#pragma once
#include <SDL2/SDL.h>
#include "common/Logger.hpp"

namespace Shinobu {
    namespace Frontend {
        namespace SDL2 {
            enum Button {
                Up,
                Down,
                Left,
                Right,
                A,
                B,
                Start,
                Select
            };

            class GameController {
                Common::Logs::Logger logger;
                SDL_GameController *controller;
                SDL_Joystick *joystick;

                bool isGameControllerButtonPressed(Button button) const;
                bool isKeyboardBindingForButtonPressed(Button button) const;
                bool isJoystickButtonPressed(Button button) const;
            public:
                GameController(Common::Logs::Level logLevel, std::string controllerName);
                ~GameController();

                bool isButtonPressed(Button button) const;
                bool hasGameController() const;
            };
        };
    };
};
