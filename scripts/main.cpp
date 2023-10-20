#include <SDL.h>
#include "glm/glm.hpp"
#include <iostream>
#include "color.h"

SDL_Renderer* renderer;



// Función para medir los FPS
class FPSCounter {
public:
    FPSCounter() : frameCount(0), lastTime(0) {}

    void StartFrame() {
        if (frameCount == 0) {
            lastTime = SDL_GetTicks();
        }
        frameCount++;
    }

    void EndFrame() {
        if (frameCount == 60) {
            Uint32 currentTime = SDL_GetTicks();
            float fps = 1000.0f / (currentTime - lastTime);
            std::cout << "FPS: " << fps << std::endl;
            frameCount = 0;
            lastTime = currentTime;
        }
    }

private:
    int frameCount;
    Uint32 lastTime;
};

// Función principal para inicializar SDL y mostrar "Hello, World!"
int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Error al inicializar SDL: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Hello, World!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Error al crear la ventana: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Error al crear el renderizador: " << SDL_GetError() << std::endl;
        return 1;
    }

    bool quit = false;
    SDL_Event e;
    FPSCounter fpsCounter;

    while (!quit) {
        fpsCounter.StartFrame();

        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Dibuja tu contenido aquí

        SDL_RenderPresent(renderer);

        fpsCounter.EndFrame();
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
