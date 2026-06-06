#pragma  once 

#include <iostream>
#include "renderer.hpp"
#include <vector>
#include <memory>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <ctime>

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

enum GameState
{
    MENU,
    PLAYING,
    GAME_OVER
};

enum EnemyState
{
    PATROL,
    CHASE,
    DEAD
};

const int TILE_SIZE = 64;

const int MAP_W = 30;
const int MAP_H = 20;

const int PLAYER_SPEED = 4;
static bool printed = false;

void init_Item();
void quit_Item();
class TileMap; // déclaration anticipée

class Item : public Renderer{
    public:
    Item();
    virtual ~Item();
    void setSize(int w, int h);
    void setPos(int x, int y);
    void move(int x, int y);
    void draw(const Camera& cam, SDL_RendererFlip flip = SDL_FLIP_NONE);
    void draw(const Camera& cam, double angle, SDL_RendererFlip flip = SDL_FLIP_NONE);
    virtual void update(int tick, float deltaTime);
    void updateCercle();
    void setCercle(int x, int y, double r);
    void setTarget(Item* t);
    SDL_Rect getPos();
    Cercle getCentre() const;
    int getX();
    int getY();
    int getHP() const { return hp; }
    void setHP(int v) { hp = v; }
    bool getCollision(const Item& other) const;
    bool getCollisionRect(const Item& other) const;
    bool isClicked(int x, int y) const;
    int z = 0;
    float damageCooldown = 0.0f;
    bool attacking = false;
    bool alreadyHit = false;
    float attackCooldown = 0.0f;
    bool dead = false;
    virtual void takeDamage(int dmg);
    bool isAttacking() const {return attacking;}
    bool isDead() const { return dead; }
    void setDead(bool v){dead = v;}
    // Variables knockback
    float knockbackX = 0.0f;
    float knockbackY = 0.0f;
    float knockbackTimer = 0.0f;
    bool hitFlash = false;
    float flashTimer = 0;
    // attack moment
    float attackTimer = 0;
    float attackDuration = 0.35f;
    float hitActiveTime  = 0.15f;
    void drawHealthBar(const Camera& cam);
    int maxHP = 100;
    int lives = 3;
    int maxLives = 3;

    protected:
    Item* target = nullptr;
    int hp = 100;
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
    virtual void update(int tick, float deltaTime) override;
    void setFPS(int FPS);

    private:
    int frameCount;
    int desiredDelta;
    std::vector<SDL_Texture*> images;
};
// ajout de la classe de ennemis
class Enemy : public Animation {
    private:
    float dirX;
    float dirY;
    float speed;
    // EnemyState
    EnemyState state = PATROL;
    // Vision range
    float visionRange = 200.0f;
    float knockX = 0;
    float knockY = 0;
    float knockTimer = 0;
    bool hitFlash = false;
    float flashTimer = 0;
    float stunTimer = 0;
    // on donne juste une référence (pointeur) vers celle de Board
    TileMap* map = nullptr; // référence vers la map

    public:
    Enemy();
    void setMap(TileMap* m);  //  injection de la map
    void update(int tick, float deltaTime) override;
    bool collideTile(SDL_Rect rect);
    //aycasting presque sans modifier le reste
    bool hasLineOfSight(Item* target);
    // Fonction applyKnockback
    void applyKnockback(float fromX, float fromY);
    void takeDamage(int dmg)override;
};

// classe de projectille

class Projectile : public Item
{
private:
    float vx;
    float vy;

    bool alive = true;

public:
    Projectile(float x, float y, float dirX, float dirY);

    void update(int tick, float deltaTime) override;

    bool isAlive() const { return alive; }
    void kill() { alive = false; }
};

// ////////////////////////////classe de particule//////////////////////

class Particle : public Item
{
private:
    float vx, vy;
    float life;
    float maxLife;

public:
    Particle(float x, float y, float vx, float vy);
    void update(int tick, float deltaTime) override;
    bool isAlive() const { return life > 0; }
    void draw(const Camera& cam);
};

// ajout d'une classe qui regouperait les items

class Group {
    public:
    void draw(const Camera& cam, SDL_RendererFlip flip = SDL_FLIP_NONE);
    void draw(const Camera& cam, double angle, SDL_RendererFlip flip = SDL_FLIP_NONE);
    void addRefe(Item* other);
    void remove(Item* other);
    void update(int tick, float deltaTime);
    void add(std::unique_ptr<Item> item);
    void spawnItems(int count, SDL_Renderer* rend);
    // fonctionspécialisées
    void handleClick(int x, int y);
    void checkCollision();
    // ajout du filtres 
    std::vector<Item*> getVisible();
    void move(int x, int y);
    // Collision Enemy / Player
    // permet une connection entre les pointeurs
    std::vector<Item*> getRaw()
    {
       return items;
    }

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
};

// ajout classe de board
class Board {
    public:
    Board(SDL_Renderer* rend);
    bool collideWithMap(const SDL_Rect& rect);
    void move(int x, int y);
    void update(int tick, float deltaTime);
    void clampPlayer();
    SDL_Rect getAttackBox();
    void handleEvent(const SDL_Event& ev);
    void resetGame();
    void draw();

    public:
    SDL_Renderer* render = nullptr;
    Group drawn; 
    Group enemies;
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
    GameState state = PLAYING;
    // Hit stop
    float hitStop = 0;
    // tir de projectille
    std::vector<std::unique_ptr<Projectile>> projectiles;
    std::vector<std::unique_ptr<Particle>> particles;
    //spawn automatique
    float enemySpawnTimer = 0.0f;
    float enemySpawnDelay = 3.0f;
};

