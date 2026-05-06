#pragma  once 

#include <iostream>
#include "renderer.hpp"
#include <vector>
#include <memory>
#include <algorithm>

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
    int z = 0;

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
    void draw(double angle);
    void addRefe(Item* other);
    void remove(Item* other);
    void update(int itck);
    void add(std::unique_ptr<Item> item);
    void spawnItems(int count, SDL_Renderer* rend);
    // fonctionspécialisées
    void handleClick(int x, int y);
    void checkCollision(int x, int y);
    // ajout du filtres 
    std::vector<Item*> getVisible();
    void move(int x, int y);

    public:
    std::vector<Item*> items;
    std::vector<std::unique_ptr<Item>> gemItems;
};

// ajout classe de board
class Board {
    public:
    Board(SDL_Renderer* rend);
    void move(int x, int y);
    void update(int tick);
    void handleEvent(const SDL_Event& ev);
    void draw();

    public:
    SDL_Renderer* render = nullptr;
    Group drawn; 
    Group gem;
    Group click;  
    Group collide;
    Animation player;
    Item bkgr;
    int speedX = 0;
    int speedY = 0;
};