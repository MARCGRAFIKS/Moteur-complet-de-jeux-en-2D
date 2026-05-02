#include "item.hpp"

int main(int argc, char**argv) {

    init_Item();
    SDL_Window* win = SDL_CreateWindow("NEW_WOLD_2D", 100, 100, 800, 500, SDL_WINDOW_SHOWN);
    SDL_Renderer* render = SDL_CreateRenderer(win, -1, 0);
    SDL_SetRenderDrawColor(render,30, 180, 30, 255);

    Item bob;
    bob.setRenderer(render);
    if(!bob.loadImage("terre.png"))
    std::cout << "Rien de bon a afficher" << std::endl;
    bob.setPos(50, 50);
    bob.setSize(100, 100);

    bool run = false;

    while(!run) {
        SDL_Event ev;
        while(SDL_PollEvent(&ev)) {
            switch(ev.type) {
                case SDL_QUIT:
                run = true;
                break;
            }
        }
        SDL_RenderClear(render);
        bob.draw();
        SDL_RenderPresent(render);
    }
    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(win);
    quit_Item();
    return 0;

}