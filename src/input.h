#pragma once

#include <SDL.h>

#include "camera.h"

namespace Magpie {
    class Input {
        public:
            bool quit = false;
            Input(Magpie::Camera* camera) {
                this->camera = camera;
            }
            void HandleInput(float deltaTime);
        private:
            SDL_Event windowEvent;
            Magpie::Camera* camera;

            const float sensitivity = 0.1f;
            const float speedModifier = 2.0f;

            //input state
            int xOffset, yOffset;
            const Uint8 *keyState;
            Uint32 mouseState;
            int currentTime;
            int lastEscapeTime = 0;
    };
}