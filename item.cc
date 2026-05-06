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
     std::sort(items.begin(), items.end(), [](Item*a, Item* b){
      return a->z <b->z;
     });

     for(auto* item : items) {
      item->draw();
     }     
}

void Group::draw(double angle) {
    
     for(auto& item : gemItems) {
      item->draw(angle);
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
        gem->setSize(32, 32);
        gem->setPos(rand() % 750, rand() % 450);

        add(std::move(gem));
    }
}
    
// fonctionspécialisées
void Group::handleClick(int x, int y) {
    for(auto* item : items) {
        std::cout << "tester si clic dans pos"<< std::endl;
    }
}
    
void Group::checkCollision(int x, int y) {
    for(auto* a : items) {
        for(auto* b : items) {
            if(a!=b) {
                std::cout << " test collision \n" << std::endl;
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

Board::Board(SDL_Renderer* rend) {
    render = rend;
    int w, h;
    SDL_GetRendererOutputSize(render, &w, &h);
    player.setRenderer(render);
    player.loadAnimation("person/JK_P_Sword__Run_", "000", ".png");
    player.setPos(w/2-32, h/2-32);
    player.setSize(64, 64);

    gem.spawnItems(10, render);
    
    bkgr.setRenderer(render);
    bkgr.loadFromImage("terre.png");
    bkgr.setSize(w, h);
    bkgr.setPos(0, 0);

    drawn.addRefe(&player);
}

void Board::move(int x, int y) {
    drawn.move(x, y);
    click.move(-x, -y);
    collide.move(-x, -y);
}

void Board::update(int tick) {
    // deplacement joueur
    move(speedX, speedY);
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
                    case SDLK_LEFT: speedX =-1; break;
                    case SDLK_RIGHT: speedX = 1; break;
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
    }
}
    
void Board::draw() {
    double a = 1.1;
    bkgr.draw();
    drawn.draw();
    gem.draw(a);
    collide.draw();
    click.draw();
    a ++;
}