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

                bool isGameControllerButtonPressed(Button button);
                bool isKeyboardBindingForButtonPressed(Button button);
            public:
                GameController(Common::Logs::Level logLevel);
                ~GameController();

                bool isButtonPressed(Button button);
                bool hasGameController() const;
            };
        };
    };
};
