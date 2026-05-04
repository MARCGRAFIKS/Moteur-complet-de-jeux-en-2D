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
    void move(int x, int y);
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
   virtual ~Animation();
    bool addImage(const std::string& file);
    bool loadAnimation(const std::string& base, std::string count, const std::string& ext);
    void next();
    virtual void update(int tick) override;
    void setFPS(int FPS);

    private:
    int frameCount;
    int desiredDelta;
    std::vector<SDL_Texture*> images;
};

// ajout d'une classe qui regouperait les items

class Group {
    public:
    void draw();
    void addRefe(Item* other);
    void move(int x, int y);

    public:
    std::vector<Item*> items;
};

// ajout classe de board
class Board {
    public:
    Board(SDL_Renderer* rend);
    void move(int x, int y);
    void draw();

    public:
    SDL_Renderer* render = nullptr;
    Group drawn; 
    Group click;  
    Group collide;
    Item player;
    Item bkgr;
};