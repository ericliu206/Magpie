#include "input.h"

#include <iostream>
#include <sstream>

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
                std::cout << "set camera position(p) or direction(d): ";
                std::string option;
                std::cin >> option;
                std::string input;
                if (option == "p") {
                    std::cout << "x y z: ";
                    std::cin.ignore(1,'\n');
                    getline(std::cin, input);
                    std::stringstream ss(input);
                    float x, y, z;
                    ss >> x;
                    ss >> y;
                    ss >> z;
                    camera->pos = Magpie::Vec3(x, y, z);
                } else if (option == "d") {
                    std::cout << "yaw pitch: ";
                    std::cin.ignore(1,'\n');
                    getline(std::cin, input);
                    std::stringstream ss(input);
                    float y, p;
                    ss >> y;
                    ss >> p;
                    camera->yaw = y;
                    camera->pitch = p;
                }
            } else {
                SDL_SetRelativeMouseMode(SDL_TRUE);
            }
        }
        lastEscapeTime = currentTime;
    }
}