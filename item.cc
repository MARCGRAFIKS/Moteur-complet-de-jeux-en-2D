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
    
void Item::draw(SDL_RendererFlip flip) {
    if(image!=nullptr) {
        SDL_RenderCopyEx(render, image, nullptr, &pos, 0, nullptr, flip);
    }
}

void Item::draw(double angle, SDL_RendererFlip flip) {
    if(image!=nullptr) {
        SDL_RenderCopyEx(render, image, nullptr, &pos, angle, nullptr, flip);
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

void Group::draw(SDL_RendererFlip flip) {
     std::sort(items.begin(), items.end(), [](Item*a, Item* b){
      return a->z <b->z;
     });

     for(auto* item : items) {
      item->draw(flip);
     }     
}

void Group::draw(double angle, SDL_RendererFlip flip) {
    
     for(auto& item : gemItems) {
      item->draw(angle, flip);
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

Board::Board(SDL_Renderer* rend) {
    render = rend;
    flip = SDL_FLIP_NONE;
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
    bkgr.draw();
    drawn.draw(flip);
    gem.draw(a);
    collide.draw();
    click.draw();
    a ++;
}