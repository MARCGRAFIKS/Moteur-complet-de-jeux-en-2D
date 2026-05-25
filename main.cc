#include "item.hpp"
#include <ctime>
#include <cstdlib>
#include <memory>

int main(int argc, char**argv) {

    init_Item();
    SDL_Window* win = SDL_CreateWindow("NEW_WOLD_2D", 100, 100, 800, 500, SDL_WINDOW_SHOWN);
    SDL_Renderer* render = SDL_CreateRenderer(win, -1, 0);
    SDL_SetRenderDrawColor(render,30, 180, 2, 255);
    // introduction de rendure
    Renderer renderer;
    
    Board board(render);

    bool run = true;
    SDL_Event ev;

    Uint32 lastTick = SDL_GetTicks();
    
    while(run) {
        Uint32 currentTick = SDL_GetTicks();
        float deltaTime = (currentTick - lastTick) / 1000.0f;
        lastTick = currentTick;

        while(SDL_PollEvent(&ev)) {
            switch(ev.type) {
                case SDL_QUIT:
                run = false;
                break;
            }
            board.handleEvent(ev); // input centralisé
        }
    
        board.update(currentTick, deltaTime); //logique centralisé
        SDL_RenderClear(render);
        board.draw();
        SDL_RenderPresent(render);
        SDL_Delay(16);
    }

    renderer.clearTexture();
    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(win);
    quit_Item();
    return 0;
}