#pragma  once 

#include <iostream>
#include "renderer.hpp"
#include <vector>
#include <memory>
#include <algorithm>

struct Cercle {
    int x, y;
    double r;
};

void init_Item();
void quit_Item();

class Item : public Renderer{
    public:
    Item();
    virtual ~Item();
    void setSize(int w, int h);
    void setPos(int x, int y);
    void move(int x, int y);
    void draw(SDL_RendererFlip flip = SDL_FLIP_NONE);
    void draw(double angle, SDL_RendererFlip flip = SDL_FLIP_NONE);
    virtual void update(int tick);
    void updateCercle();
    void setCercle(int x, int y, double r);
    SDL_Rect getPos();
    Cercle getCentre() const;
    int getX();
    bool getCollision(const Item& other) const;
    bool getCollisionRect(const Item& other) const;
    bool isClicked(int x, int y) const;
    int z = 0;

    protected:
    SDL_Rect pos;
    Cercle centre;
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
    void draw(SDL_RendererFlip flip = SDL_FLIP_NONE);
    void draw(double angle, SDL_RendererFlip flip = SDL_FLIP_NONE);
    void addRefe(Item* other);
    void remove(Item* other);
    void update(int itck);
    void add(std::unique_ptr<Item> item);
    void spawnItems(int count, SDL_Renderer* rend);
    // fonctionspécialisées
    void handleClick(int x, int y);
    void checkCollision();
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
    double a = 1.1;
    SDL_RendererFlip flip; 
};