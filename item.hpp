#pragma  once 

#include <iostream>
#include "renderer.hpp"
#include <vector>
#include <memory>
#include <algorithm>
#include <fstream>
#include <sstream>

struct Cercle {
    int x, y;
    double r;
};

struct Camera {
    int x, y;
    int w, h;
};

// Ajouter un enum propre
enum TileType {
    TILE_EMPTY = 0,
    TILE_GRASS = 1,
    TILE_WALL  = 2,
    TILE_WATER = 3,
    TILE_SAND  = 4
};

const int TILE_SIZE = 64;

const int MAP_W = 30;
const int MAP_H = 20;

const int PLAYER_SPEED = 4;

void init_Item();
void quit_Item();

class Item : public Renderer{
    public:
    Item();
    virtual ~Item();
    void setSize(int w, int h);
    void setPos(int x, int y);
    void move(int x, int y);
    void draw(const Camera& cam, SDL_RendererFlip flip = SDL_FLIP_NONE);
    void draw(const Camera& cam, double angle, SDL_RendererFlip flip = SDL_FLIP_NONE);
    virtual void update(int tick);
    void updateCercle();
    void setCercle(int x, int y, double r);
    SDL_Rect getPos();
    Cercle getCentre() const;
    int getX();
    int getY();
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
    void draw(const Camera& cam, SDL_RendererFlip flip = SDL_FLIP_NONE);
    void draw(const Camera& cam, double angle, SDL_RendererFlip flip = SDL_FLIP_NONE);
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

//Ajout de classe de map

class TileMap {

private:

    SDL_Texture* tileset;
    SDL_Renderer* render;

public:

    int map[MAP_H][MAP_W];
    TileMap(SDL_Renderer* rend);
    ~TileMap();
    bool load(const std::string& file);
    bool loadFromFile(const std::string& path);
    int getTileIndex(int tileType);
    void draw(const Camera& cam);
    bool isSolid(int x, int y);
    // int getMask(int x, int y);
};

// ajout classe de board
class Board {
    public:
    Board(SDL_Renderer* rend);
    bool collideWithMap(const SDL_Rect& rect);
    void move(int x, int y);
    void update(int tick, float deltaTime);
    void clampPlayer();
    void handleEvent(const SDL_Event& ev);
    void draw();

    public:
    SDL_Renderer* render = nullptr;
    Group drawn; 
    Group gem;
    Group click;  
    Group collide;
    Animation player;
    float speedX = PLAYER_SPEED;
    float speedY = PLAYER_SPEED;
    int mapWidth = 1000;
    int mapHeight = 600;
    double a = 1.1;
    SDL_RendererFlip flip; 
    Camera cam;
    TileMap tileMap;
};

