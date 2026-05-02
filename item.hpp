#pragma  once 

#include <iostream>
#include <SDL.h>

void init_Item();
void quit_Item();

class Item {
    public:
    Item();
    virtual ~Item();
    bool loadImage(const char* pathName);
    void setRenderer(SDL_Renderer* render);
    void setSize(int w, int h);
    void setPos(int x, int y);
    void draw();

    private:
    SDL_Rect pos;
    SDL_Texture* image;
    SDL_Renderer* rend;
};