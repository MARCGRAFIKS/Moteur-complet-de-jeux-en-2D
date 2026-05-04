#include "item.hpp"
#include <ctime>
#include <cstdlib>
#include <memory>

// une fonction qui va multiplier les items
void multiItem(std::vector<std::unique_ptr<Item>>& gems, int maxGem, SDL_Renderer* rend) {
    for(int i=0; i<maxGem; i++) {
        auto gem = std::make_unique<Item>();
        gem->setRenderer(rend);
        gem->loadFromImage("terre.png");
        gem->setSize(32, 32);
        gem->setPos(rand()%750, rand()%450);
        gems.push_back(std::move(gem));
    }
}

int main(int argc, char**argv) {

    init_Item();
    SDL_Window* win = SDL_CreateWindow("NEW_WOLD_2D", 100, 100, 800, 500, SDL_WINDOW_SHOWN);
    SDL_Renderer* render = SDL_CreateRenderer(win, -1, 0);
    SDL_SetRenderDrawColor(render,30, 180, 2, 255);
    // introduction de rendure
    Renderer renderer;
    
    // instation du player
    Animation bob;
    bob.setRenderer(render);
    if(!bob.loadAnimation("person/JK_P_Sword__Run_", "000", ".png"))
    std::cout << "Rien de bon a afficher" << std::endl;
    bob.setPos(50, 50);
    bob.setSize(100, 100);
    
    // instantion du obstacle multiple
    std::vector<std::unique_ptr<Item>>  gems;
    int maxGem = 100;
    multiItem(gems, maxGem, render);

    bool run = false;
    double angle = 0.0;
    int speedX = 0;
    int speedY = 0;

    while(!run) {
        int startLoop = SDL_GetTicks();
        SDL_Event ev;
        while(SDL_PollEvent(&ev)) {
            switch(ev.type) {
                case SDL_QUIT:
                run = true;
                break;
                case SDL_KEYDOWN:
                switch(ev.key.keysym.sym) {
                    case SDLK_UP: speedY = -1; break;
                    case SDLK_DOWN: speedY = 1; break;
                    case SDLK_LEFT: speedX =-1; break;
                    case SDLK_RIGHT: speedX = 1; break;
                }
                break;
                case SDL_KEYUP:
                switch(ev.key.keysym.sym) {
                    case SDLK_UP:
                    case SDLK_DOWN: speedY = 0; break;
                    case SDLK_LEFT:
                    case SDLK_RIGHT: speedX = 0; break;
                }
                break;
            }
        }
        SDL_RenderClear(render);
        bob.move(speedX, speedY);
        bob.draw();
        for(auto it=gems.begin(); it!=gems.end();) {
            (*it)->draw(angle);
            ++it;
        }
        SDL_RenderPresent(render);
        bob.update(startLoop);
        ++angle;
    }

    renderer.clearTexture();
    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(win);
    quit_Item();
    return 0;

}