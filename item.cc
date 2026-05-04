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

Item::Item() : pos{0, 0, 32, 32}, oldTick(0) {}

Item::~Item() {
    image = nullptr;
}
    
void Item::setSize(int w, int h) {
    pos.w = w;
    pos.h = h;
}
    
void Item::setPos(int x, int y) {
    pos.x = x;
    pos.y = y;
}

void Item::move(int x, int y) {
    pos.x += x;
    pos.y += y;
}
    
void Item::draw() {
    if(image!=nullptr) {
        SDL_RenderCopy(render, image, nullptr, &pos);
    }
}

void Item::draw(double angle) {
    if(image!=nullptr) {
        SDL_RenderCopyEx(render, image, nullptr, &pos, angle, nullptr, SDL_FLIP_NONE);
    }
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

void Group::draw() {
   for(auto* item : items) {
        item->draw();
    }
}
    
void Group::addRefe(Item* other) {
    items.push_back(other);
}
    
void Group::move(int x, int y) {
    for(auto* item : items) {
        item->move(x, y);
    }
}

///////////////////////Board////////////////////////////

Board::Board(SDL_Renderer* rend) {
    render = rend;
    int w, h;
    SDL_GetRendererOutputSize(render, &w, &h);
    player.setPos(w/2-32, h/2-32);
    bkgr.setSize(w, h);
    bkgr.setPos(0, 0);
}

void Board::move(int x, int y) {
    drawn.move(-x, -y);
    click.move(-x, -y);
    collide.move(-x, -y);
}
    
void Board::draw() {
    bkgr.draw();
    drawn.draw();
    player.draw();
}