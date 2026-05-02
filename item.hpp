#pragma  once 

#include <iostream>
#include "renderer.hpp"
#include <vector>

void init_Item();
void quit_Item();

class Item : public Renderer{
    public:
    Item();
    virtual ~Item();
    void setSize(int w, int h);
    void setPos(int x, int y);
    void draw();
    void draw(double angle);
    virtual void update(int tick);

    protected:
    SDL_Rect pos;
    int oldTick;
};

// On ajoute une classe qui va pérmtre l'animation

class Animation : public Item {
    public:
    Animation();
    ~Animation();
    bool addImage(const std::string& file);
    bool loadAnimation(std::string base, std::string count, std::string ext);
    void next();
    virtual void update(int tick);
    void setFPS(int FPS);

    private:
    int frameCount;
    int desiredDelta;
    std::vector<SDL_Texture*> images;
};