#include "input.h"

void Magpie::Input::HandleInput(float deltaTime) {
    currentTime = SDL_GetTicks();

    mouseState = SDL_GetRelativeMouseState(&xOffset, &yOffset);
    xOffset *= sensitivity;
    yOffset *= -sensitivity;

    camera->Rotate(xOffset, yOffset);

    float cameraSpeed = speedModifier * deltaTime;
    keyState = SDL_GetKeyboardState(NULL);
    if (keyState[SDL_SCANCODE_W]) {
        camera->pos += cameraSpeed * camera->front;
    }
    if (keyState[SDL_SCANCODE_A]) {
        camera->pos -= Magpie::Vector::Normalize(Magpie::Vector::Cross(camera->front, camera->up)) * cameraSpeed;
    }
    if (keyState[SDL_SCANCODE_S]) {
        camera->pos -= cameraSpeed * camera->front;
    }
    if (keyState[SDL_SCANCODE_D]) {
        camera->pos += Magpie::Vector::Normalize(Magpie::Vector::Cross(camera->front, camera->up)) * cameraSpeed;
    }
    if (keyState[SDL_SCANCODE_ESCAPE]) {
        if (currentTime-lastEscapeTime >= 100) {
            if (SDL_GetRelativeMouseMode()) {
                SDL_SetRelativeMouseMode(SDL_FALSE);
            } else {
                SDL_SetRelativeMouseMode(SDL_TRUE);
            }
        }
        lastEscapeTime = currentTime;
    }
}