#include <SDL.h>
#include <glad/glad.h>

#include <Magpie.h>
#include "camera.h"
#include "input.h"
#include "display.h"

#include <iostream>

float currentTime = 0.0f;
float lastFrame = 0.0f;
float deltaTime = 0.0f;

int main(int argc, char ** argv) {
    Magpie::Display* display = new Magpie::Display();
    Magpie::Camera* camera = new Magpie::Camera();
    Magpie::Input* input = new Magpie::Input(camera);
    Magpie::PathTracer* renderer = new Magpie::OpenCLPathTracer();

    // create a window
    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_SetRelativeMouseMode(SDL_TRUE);

    SDL_Window* window = SDL_CreateWindow("Magpie", 100, 100, 800, 600, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(window);
    SDL_WarpMouseInWindow(NULL, 0, 0);

    if (!gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress)) {
        std::cerr << "Failed to initialize the OpenGL context." << std::endl;
        exit(1);
    }

    display->Initialize();
    renderer->Initialize();

    if (!argv[1]) {
        std::cerr << "no scene file specified.\n";
        return EXIT_FAILURE;
    }

    Magpie::Scene scene = Magpie::LoadSceneFromFile(argv[1]);
    renderer->LoadScene(scene);

    while (!SDL_QuitRequested()) {
        input->HandleInput(deltaTime);
        renderer->SetViewMatrix(Magpie::Matrix::LookAt(camera->pos, camera->pos + camera->front, camera->up));
        display->SwitchToColorTexture();
        renderer->Render();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 800, 600, 0, GL_RGBA, GL_FLOAT, renderer->GetPixels());
        display->Render();
        currentTime = (float)SDL_GetTicks()/1000;
        deltaTime = currentTime - lastFrame;
        lastFrame = currentTime;
        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DeleteContext(context);
    SDL_Quit();

    delete display;
    delete camera;
    delete input;
    delete renderer;

    return 0;
}