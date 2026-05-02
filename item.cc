#include "item.hpp"
#include <SDL_Image.h>

void init_Item(){
    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_PNG);
}

void quit_Item() {
    IMG_Quit();
    SDL_Quit();
}

Item::Item() : pos{0, 0, 32, 32}, image(nullptr), rend() {}

Item::~Item() {

    if(image != nullptr){
        SDL_DestroyTexture(image);
        image = nullptr;
    }
}
    
bool Item::loadImage(const char* pathName) {
    image = IMG_LoadTexture(rend, pathName);
    if(!image) {
        std::cout << "Error image in loadImage!" << IMG_GetError() << std::endl;
    }
    return image!=nullptr;
}
    
void Item::setRenderer(SDL_Renderer* render) {
    rend = render;
}
    
void Item::setSize(int w, int h) {
    pos.w = w;
    pos.h = h;
}
    
void Item::setPos(int x, int y) {
    pos.x = x;
    pos.y = y;
}
    
void Item::draw() {
    if(image!=nullptr) {
        SDL_RenderCopy(rend, image, nullptr, &pos);
    }
}