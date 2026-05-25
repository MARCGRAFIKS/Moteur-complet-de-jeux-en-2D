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

Item::Item() : pos{0, 0, 64, 32}, oldTick(0) {}

Item::~Item() {
    image = nullptr;
}
    
void Item::setSize(int w, int h) {
    pos.w = w;
    pos.h = h;

    updateCercle();
}
    
void Item::setPos(int x, int y) {
    pos.x = x;
    pos.y = y;

    updateCercle();
}

void Item::move(int x, int y) {
    pos.x += x;
    pos.y += y;

    updateCercle();
}
    
void Item::draw(const Camera& cam, SDL_RendererFlip flip) {
    if(image!=nullptr) {
        SDL_Rect screenPos = {
            pos.x - cam.x,
            pos.y - cam.y,
            pos.w, 
            pos.h
        };
    

        SDL_RenderCopyEx(render, image, nullptr, &screenPos, 0, nullptr, flip);
    }
}

void Item::draw(const Camera& cam, double angle, SDL_RendererFlip flip) {
    if(image!=nullptr) {
        SDL_Rect screenPos = {
            pos.x - cam.x,
            pos.y - cam.y,
            pos.w, 
            pos.h
        };

        SDL_RenderCopyEx(render, image, nullptr, &screenPos, angle, nullptr, flip);
    }
}

void Item::setCercle(int x, int y, double r) {
    centre.x = x;
    centre.y = y;
    centre.r = r;
}
    
SDL_Rect Item::getPos() {
    return pos;
}
    
Cercle Item::getCentre() const{
    return centre;
}

int Item::getX() {
    return pos.x;
}

int Item::getY() {
    return pos.y;
}

void Item::updateCercle() {
    int centerX = pos.x + pos.w / 2;
    int centerY = pos.y + pos.h / 2;
    double r = std::min(pos.w, pos.h) / 2;
    setCercle(centerX, centerY, r);
}
    
    
bool Item::getCollision(const Item& other) const{
    int dx = centre.x - other.getCentre().x;
    int dy = centre.y - other.getCentre().y;

    double rs = centre.r + other.getCentre().r;

    return (dx * dx + dy * dy) < (rs * rs);
}

bool Item::getCollisionRect(const Item& other) const {
    return (
        pos.x < other.pos.x + other.pos.w &&
        pos.x + pos.w > other.pos.x &&
        pos.y < other.pos.y + other.pos.h &&
        pos.y + pos.h > other.pos.y
    );
}

    
bool Item::isClicked(int x, int y) const{
    int cx = pos.x + pos.w / 2;
    int cy = pos.y + pos.h / 2;
    int r = pos.w / 2;

    int dx = cx - x;
    int dy = cy - y;

    return(dx*dx+dy*dy) <= (r*r); 
}

void Item::update(int tick) {
    oldTick = tick;
}

//////////////////////// La clsse de l'animation ///////////////

 Animation::Animation() : frameCount(0), desiredDelta(0) {}
    
 Animation::~Animation() {}

bool Animation::addImage(const std::string& file) {
    if(loadFromImage(file)) {
        images.push_back(image);
        return true;
    }
    return false;
}

bool Animation::loadAnimation(const std::string& base, std::string count, const std::string& ext) {
    int lenght = count.length();
    while(true) {
        
        if(!addImage(base+count+ext))
        break;
        for(int i=lenght-1; i>=0; i--) {
            if(count[i]<'9'){
                count[i]++;
                break;
            }else {
                count[i] = '0';
            }
        }
    }

    return !images.empty();
}

void Animation::update(int tick) {
    if(tick - oldTick > desiredDelta) {
        next();
        oldTick = tick;
    }
}

    
void Animation::setFPS(int FPS) {
    if(FPS>0)
    desiredDelta = 1000/FPS;
}

void Animation::next() {
    if(images.empty()) {
        return;
    }
    frameCount = (frameCount +1) % images.size();
    image = images[frameCount];
}

///////////////////////////////////classe groupe ///////////////////////////////:

void Group::draw(const Camera& cam, SDL_RendererFlip flip) {
     std::sort(items.begin(), items.end(), [](Item*a, Item* b){
      return a->z <b->z;
     });

     for(auto* item : items) {
      item->draw(cam, flip);
     }     
}

void Group::draw(const Camera& cam, double angle, SDL_RendererFlip flip) {
    
     for(auto& item : gemItems) {
      item->draw(cam, angle, flip);
     }
        
}
    
void Group::addRefe(Item* other) {
    if(std::find(items.begin(), items.end(), other) == items.end())
    items.push_back(other);
}

void Group::remove(Item* other) {
    items.erase(std::remove(items.begin(), items.end(), other), items.end());
}
    
void Group::update(int tick) {
    for(auto* item : items) {
        item->update(tick);
    }
}

void Group::add(std::unique_ptr<Item> item) {
    gemItems.push_back(std::move(item));
}

void Group::spawnItems(int count, SDL_Renderer* rend) {
    for (int i = 0; i < count; i++) {
        auto gem = std::make_unique<Item>();
        gem->setRenderer(rend);
        gem->loadFromImage("terre.png");
        gem->setSize(64, 64);
        gem->setPos(rand() % 750, rand() % 450);

        add(std::move(gem));
    }
}
    
// fonctionspécialisées
void Group::handleClick(int x, int y) {
    for(auto* item : items) {
        if (item->isClicked(x, y))
        std::cout << "tester si clic dans pos"<< std::endl;
    }
}
    
void Group::checkCollision() {
    for(size_t i = 0; i < items.size(); i++) {
        for(size_t j = i + 1; j < items.size(); j++) {

            items[i]->updateCercle();
            items[j]->updateCercle();

            if(items[i]->getCollision(*items[j])) {
                std::cout << "collision\n";
            }
        }
    }
}
    
// ajout du filtres 
std::vector<Item*> Group::getVisible() {
    std::vector<Item*> result;
    for(auto* item : items) {
        // if(item->isVisible())
        result.push_back(item);
    }
    return result;
}
    
void Group::move(int x, int y) {
    for(auto* item : items) {
        item->move(x, y);
    }
}

///////////////////////Board////////////////////////////

Board::Board(SDL_Renderer* rend) : tileMap(rend){
    render = rend;
    SDL_GetRendererOutputSize(rend, &cam.w, &cam.h);
    cam.x = 0;
    cam.y = 0;
    a = 1.1;
    flip = SDL_FLIP_NONE;
    int w, h;
    tileMap.load("maptile.png");

    SDL_GetRendererOutputSize(render, &w, &h);
    player.setRenderer(render);
    player.loadAnimation("person/JK_P_Sword__Run_", "000", ".png");
    player.setPos(128, 128);
    player.setSize(64, 64);
    player.setFPS(10);

    gem.spawnItems(10, render);

    drawn.addRefe(&player);
}

void Board::move(int x, int y) {
    drawn.move(x, y);
    clampPlayer();
}

void Board::update(int tick) {
    // Faire suive camera au joueur
    cam.x = player.getX() - cam.w/2;
    cam.y = player.getY() - cam.h/2;
    // contr$ole de camera 
    if(cam.x < 0) cam.x = 0;
    if(cam.y < 0) cam.y = 0;
    if(cam.x > MAP_W * TILE_SIZE - cam.w) cam.x = MAP_W * TILE_SIZE - cam.w;
    if(cam.y > MAP_H * TILE_SIZE - cam.h) cam.y = MAP_H * TILE_SIZE - cam.h;
    // deplacement joueur
    move(speedX, speedY);
    // collision
     SDL_Rect p = player.getPos();

    int centerX = p.x + p.w / 2;
    int centerY = p.y + p.h / 2;

    int tileX = centerX / TILE_SIZE;
    int tileY = centerY / TILE_SIZE;

    if(tileMap.isSolid(tileX, tileY)) {

        player.move(-speedX, -speedY);
    }
    // update des groupes
    drawn.update(tick);
    click.update(tick);
    collide.update(tick);
    
}

void Board::handleEvent(const SDL_Event& ev) {
    switch(ev.type) {
        case SDL_KEYDOWN:
                switch(ev.key.keysym.sym) {
                    case SDLK_UP: speedY = -1; break;
                    case SDLK_DOWN: speedY = 1; break;
                    case SDLK_LEFT: speedX =-1; flip = SDL_FLIP_HORIZONTAL; break;
                    case SDLK_RIGHT: speedX = 1; flip = SDL_FLIP_NONE; break;
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
        case SDL_MOUSEBUTTONUP:
        click.handleClick(ev.button.x, ev.button.y);
        break;
    }
}
    
void Board::draw() {
    tileMap.draw(cam);
    drawn.draw(cam, flip);
    gem.draw(cam, a);
    collide.draw(cam);
    click.draw(cam);
    a ++;
}

void Board::clampPlayer() {

    SDL_Rect p = player.getPos();

    if(p.x < 0)
        p.x = 0;

    if(p.y < 0)
        p.y = 0;

    if(p.x + p.w > MAP_W * TILE_SIZE)
        p.x = MAP_W * TILE_SIZE - p.w;

    if(p.y + p.h >MAP_H * TILE_SIZE)
        p.y = MAP_H * TILE_SIZE - p.h;

    player.setPos(p.x, p.y);
}


//////////////////:classe de tilemap///////////////////////////////

TileMap::TileMap(SDL_Renderer* rend) : render(rend), tileset(nullptr) {
   
    int temp[MAP_H][MAP_W] = {
                 {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
                 {2, 4, 4, 4, 4, 4, 4, 4, 2, 2, 4, 4, 4, 4, 4, 4, 4, 4, 1, 1, 4, 4, 4, 4, 1, 4, 1, 4, 4, 2}, 
                 {2, 4, 4, 4, 2, 2, 2, 4, 4, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 1, 1, 1, 4, 1, 1, 1, 2, 1, 1, 2}, 
                 {2, 2, 2, 4, 3, 3, 3, 4, 4, 2, 2, 1, 1, 3, 3, 4, 4, 4, 4, 4, 1, 1, 1, 1, 4, 1, 2, 3, 3, 2}, 
                 {2, 2, 1, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 4, 4, 4, 4, 2, 2, 4, 4, 4, 4, 2, 1, 3, 2}, 
                 {2, 1, 1, 4, 4, 4, 4, 1, 4, 2, 4, 4, 1, 1, 1, 3, 4, 4, 4, 4, 2, 4, 4, 4, 4, 2, 2, 1, 3, 2}, 
                 {2, 1, 1, 4, 2, 2, 2, 1, 2, 2, 4, 4, 2, 2, 3, 3, 4, 4, 4, 4, 2, 4, 4, 1, 1, 2, 4, 1, 4, 2}, 
                 {2, 4, 1, 1, 3, 3, 3, 1, 2, 4, 4, 4, 2, 3, 3, 4, 4, 1, 1, 4, 4, 1, 1, 2, 1, 2, 4, 1, 4, 2}, 
                 {2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 3, 4, 4, 4, 1, 1, 1, 1, 4, 2, 2, 4, 1, 1, 1, 1, 2}, 
                 {2, 4, 2, 1, 1, 1, 1, 4, 4, 4, 2, 3, 3, 3, 2, 4, 1, 1, 4, 4, 4, 4, 4, 4, 4, 4, 1, 1, 4, 2}, 
                 {2, 4, 2, 1, 2, 2, 2, 4, 4, 4, 2, 3, 4, 2, 2, 1, 1, 4, 4, 3, 3, 3, 3, 3, 3, 4, 4, 1, 4, 2}, 
                 {2, 4, 1, 2, 2, 1, 2, 4, 4, 2, 2, 3, 2, 2, 1, 4, 4, 4, 4, 4, 4, 4, 3, 4, 4, 4, 4, 1, 4, 2}, 
                 {2, 4, 1, 1, 1, 1, 1, 4, 4, 2, 3, 3, 4, 1, 1, 4, 4, 1, 1, 1, 4, 4, 3, 4, 1, 1, 4, 1, 4, 2}, 
                 {2, 4, 1, 1, 3, 3, 1, 4, 4, 2, 3, 3, 4, 1, 1, 4, 4, 4, 1, 1, 4, 4, 3, 4, 1, 1, 4, 4, 4, 2}, 
                 {2, 4, 1, 4, 3, 1, 4, 4, 4, 2, 3, 3, 4, 1, 1, 4, 4, 4, 4, 1, 1, 4, 3, 4, 4, 1, 1, 1, 4, 2}, 
                 {2, 4, 1, 1, 4, 1, 1, 4, 4, 2, 3, 4, 4, 1, 4, 4, 4, 4, 1, 1, 4, 4, 3, 4, 4, 4, 4, 4, 1, 2}, 
                 {2, 4, 4, 1, 1, 4, 1, 4, 4, 3, 3, 4, 1, 1, 4, 4, 4, 4, 2, 4, 4, 3, 2, 2, 1, 4, 4, 1, 1, 2}, 
                 {2, 4, 4, 4, 1, 1, 4, 4, 3, 3, 4, 4, 1, 4, 4, 4, 4, 4, 2, 2, 3, 3, 2, 1, 4, 1, 1, 4, 4, 2}, 
                 {2, 4, 4, 4, 4, 1, 4, 4, 4, 4, 4, 4, 1, 1, 1, 1, 4, 4, 4, 3, 3, 1, 1, 1, 4, 1, 4, 4, 4, 2}, 
                 {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2}
            };

   for(int y=0; y<MAP_H; y++) {
    for(int x=0; x<MAP_W; x++) {
                map[y][x] = temp[y][x];
     }
   }
}

TileMap::~TileMap() {
    if(nullptr!=tileset) {
        SDL_DestroyTexture(tileset);
    }
}

bool TileMap::load(const std::string& file) {

    SDL_Surface* surf = IMG_Load(file.c_str());

    if(!surf)
        return false;

    tileset = SDL_CreateTextureFromSurface(render, surf);

    SDL_FreeSurface(surf);

    return tileset != nullptr;
}

void TileMap::draw(const Camera& cam) {

    SDL_Rect src;
    SDL_Rect dst;

    src.w = TILE_SIZE;
    src.h = TILE_SIZE;

    dst.w = TILE_SIZE;
    dst.h = TILE_SIZE;
    
    // permet de déssiner la partie posiner dans le camera
    // debut
    int startX = cam.x / TILE_SIZE;
    int startY = cam.y / TILE_SIZE;
    //  fin
    int endX = (cam.x + cam.w) / TILE_SIZE + 1;
    int endY = (cam.y + cam.h) / TILE_SIZE + 1;

    for(int y = startY; y < endY; y++) {

        for(int x = startX; x < endX; x++) {

            int tile = map[y][x];
            if(tile <= 0)
             continue;
            src.x = (tile-1)* TILE_SIZE;
            src.y = 0;

            dst.x = x * TILE_SIZE - cam.x;
            dst.y = y * TILE_SIZE - cam.y;

            SDL_RenderCopy(render, tileset, &src, &dst);
      }
    }
}

bool TileMap::isSolid(int x, int y) {
// Vérification des limites
// Donc tout ce qui est hors de la map est considéré comme un mur/solide
    if(x < 0 || x >= MAP_W || y < 0 || y >= MAP_H)
        return true;

// Test de collision 
    return (map[y][x] == TILE_WALL || // mur
            map[y][x] == TILE_WATER    // eau
            );
}

// int TileMap::getMask(int x, int y) {
//      int mask = 0;

//     // haut
//     if(y > 0 && map[y - 1][x] == 1)
//         mask += 1;

//     // droite
//     if(x < MAP_W - 1 && map[y][x + 1] == 1)
//         mask += 2;

//     // bas
//     if(y < MAP_H - 1 && map[y + 1][x] == 1)
//         mask += 4;

//     // gauche
//     if(x > 0 && map[y][x - 1] == 1)
//         mask += 8;

//     return mask;
// }