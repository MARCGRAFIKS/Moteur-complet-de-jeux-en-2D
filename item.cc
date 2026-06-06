#include "item.hpp"
#include <SDL_Image.h>
#include <cmath>

void init_Item(){
    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_PNG);
}

void quit_Item() {
    IMG_Quit();
    SDL_Quit();
}

Item::Item() : pos{0, 0, 64, 32}, oldTick(0) {
     maxHP = hp;
}

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
            pos.x - cam.x + (attacking ? 10 : 0), // décalage si attaque
            pos.y - cam.y,
            pos.w, 
            pos.h
        };
        
        if(flashTimer > 0){
          if((int)(flashTimer * 100) % 2 == 0)
             SDL_SetTextureColorMod(image, 255, 255, 255);
          else
             SDL_SetTextureColorMod(image, 255, 80, 80);
        }else{
          SDL_SetTextureColorMod(image, 255, 255, 255);
        }

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
    
         if(flashTimer > 0){
          if((int)(flashTimer * 100) % 2 == 0)
             SDL_SetTextureColorMod(image, 255, 255, 255);
          else
             SDL_SetTextureColorMod(image, 255, 80, 80);
           }else{
          SDL_SetTextureColorMod(image, 255, 255, 255);
         }

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

void Item::setTarget(Item* t)
{
    target = t;
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

void Item::update(int tick, float deltaTime) {
    oldTick = tick;
    if(knockbackTimer > 0)
    {
       move(knockbackX * deltaTime, knockbackY * deltaTime);

       knockbackTimer -= deltaTime;
    }
}


void Item::takeDamage(int dmg)
{
    if (dead) return;

    hp -= dmg;

    if (hp <= 0)
    {
        hp = 0;
        dead = true;
    }
}

void Item::drawHealthBar(const Camera& cam) {
    if (dead) return;

    SDL_Rect barBg;
    barBg.x = pos.x - cam.x;
    barBg.y = pos.y - cam.y - 10;
    barBg.w = pos.w;
    barBg.h = 6;

    float ratio = (maxHP > 0) ? (float)hp / (float)maxHP : 0;

    SDL_Rect barFill = barBg;
    barFill.w = (int)(barBg.w * ratio);

    // fond rouge
    SDL_SetRenderDrawColor(render, 80, 0, 0, 255);
    SDL_RenderFillRect(render, &barBg);

    // vie verte
    if(ratio > 0.6f) {
        SDL_SetRenderDrawColor(render, 0, 200, 0, 255);
    }else if(ratio > 0.3f){
        SDL_SetRenderDrawColor(render, 255, 180, 0, 255);
    }else{
        SDL_SetRenderDrawColor(render, 220, 0, 0, 255);
    }

    SDL_RenderFillRect(render, &barFill);
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

void Animation::update(int tick, float deltaTime) {
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

////////////////////////:classe d'ennemis//////////////////////////////////////

Enemy::Enemy()
{
    dirX = 1.0;
    dirY = 0.0;

    speed = 100.0f;

    setSize(48, 48);
}

void Enemy::setMap(TileMap* m)
{
    map = m;
}

void Enemy::update(int tick, float deltaTime)
{
    Animation::update(tick, deltaTime);

    if(dead)
    {
        state = DEAD;
        return;
    }
    // hitStop
    if(stunTimer > 0){
    stunTimer -= deltaTime;
    return;
    }

    // FLASH
    if(flashTimer > 0)
    {
        flashTimer -= deltaTime;

        if(flashTimer <= 0)
        {
            hitFlash = false;
        }
    }

    // KNOCKBACK
    if(knockTimer > 0){
       float vx = knockX * deltaTime;
       float vy = knockY * deltaTime;
       move(vx, 0);
       if(collideTile(getPos())) {
        move(-vx, 0);
        knockX = 0;
       }

       move(0, vy);
       if(collideTile(getPos())){
       move(0, -vy);
       knockY = 0;
       }
       knockTimer -= deltaTime;
       return;
   }

    if(!target || !map)
        return;

    float ex = getX();
    float ey = getY();

    float px = target->getX();
    float py = target->getY();

    float dx = px - ex;
    float dy = py - ey;

    float dist = sqrt(dx*dx + dy*dy);

    // FSM
    bool canSee = false;

    if(dist < visionRange)
    {
       canSee = hasLineOfSight(target);
    }

    if(canSee)
     {
       state = CHASE;
     }
     else
     {
       state = PATROL;
     }

    // CHASE
    if(state == CHASE)
    {
        float len = dist;

        if(len > 0)
        {
            dx /= len;
            dy /= len;
        }

        float vx = dx * speed * deltaTime;
        float vy = dy * speed * deltaTime;

        move(vx, vy);

        if(collideTile(getPos()))
        {
            move(-vx, -vy);
        }
    }

    // PATROL
    else if(state == PATROL)
    {
        move(dirX * speed * 0.5f * deltaTime,
             dirY * speed * 0.5f * deltaTime);

        if(collideTile(getPos()))
        {
            dirX = -dirX;
            dirY = -dirY;
        }

        if(rand() % 1000 < 5)
        {
            // pour éviter ennemi immobile lorsqu'on 0,0
            do{
               dirX = rand() % 3 - 1;
               dirY = rand() % 3 - 1;
              }while(dirX == 0 && dirY == 0);
        }
    }
}

bool Enemy::collideTile(SDL_Rect rect)
{
    int left   = rect.x + 4;
    int right  = rect.x + rect.w - 4;

    int top    = rect.y + 4;
    int bottom = rect.y + rect.h - 4;

    int points[4][2] = {
        {left, top},
        {right, top},
        {left, bottom},
        {right, bottom}
    };

    for(auto& p : points)
    {
        int tx = p[0] / TILE_SIZE;
        int ty = p[1] / TILE_SIZE;

        if(map->isSolid(tx, ty))
            return true;
    }

    return false;
}

bool Enemy::hasLineOfSight(Item* target)
{
    if(!target || !map)
        return false;

    float x1 = getX() + getPos().w / 2;
    float y1 = getY() + getPos().h / 2;

    float x2 = target->getX() + target->getPos().w / 2;
    float y2 = target->getY() + target->getPos().h / 2;

    float dx = x2 - x1;
    float dy = y2 - y1;

    float distance = sqrt(dx * dx + dy * dy);

    // nombre d'échantillons
    int steps = distance / 8.0f;

    if(steps <= 0)
        return true;

    for(int i = 0; i <= steps; i++)
    {
        float t = (float)i / steps;

        float x = x1 + dx * t;
        float y = y1 + dy * t;

        int tx = (int)x / TILE_SIZE;
        int ty = (int)y / TILE_SIZE;

        if(map->isSolid(tx, ty))
        {
            return false;
        }
    }

    return true;
}

void Enemy::applyKnockback(float fromX, float fromY){
    float dx = getX() - fromX;
    float dy = getY() - fromY;

    float len = sqrt(dx*dx + dy*dy);

    if(len == 0)
        return;

    dx /= len;
    dy /= len;

    knockX = dx * 120.0f;
    knockY = dy * 120.0f;

    knockTimer = 0.10f;
}

void Enemy::takeDamage(int dmg)
{
    if(dead)
        return;

    hp -= dmg;

    hitFlash = true;
    flashTimer = 0.1f;
    stunTimer = 0.2f;
    if(hp <= 0)
    {
        hp = 0;
        dead = true;
        state = DEAD;
    }
}

/////////////////////////////////////classe de projectille///////////////////////

Projectile::Projectile(float x, float y, float dirX, float dirY) {
    setPos(x, y);
    setSize(16, 16);

    float speed = 400.0f;

    float len = sqrt(dirX * dirX + dirY * dirY);

    if(len == 0)
        len = 1;

    vx = dirX / len * speed;
    vy = dirY / len * speed;
}

void Projectile::update(int tick, float deltaTime)
{
    // déplacer le projectile
    move(vx * deltaTime, vy * deltaTime);

    // retirer si hors de l'écran
    if(getX() < 0 || getX() > 5000 || getY() < 0 || getY() > 5000)
        alive = false;
}

///////////////////////////////////classe de particule//////////////////////////
Particle::Particle(float x, float y, float vx, float vy) {
    setPos(x, y);
    setSize(4, 4);

    this->vx = vx;
    this->vy = vy;

    life = 0.3f;
    maxLife = life;
}

void Particle::update(int tick, float deltaTime) {
    move(vx * deltaTime, vy * deltaTime);
    life -= deltaTime;
}

void Particle::draw(const Camera& cam) {
    SDL_Rect r = {getX() - cam.x, getY() - cam.y, 32, 32};
    
    SDL_SetRenderDrawColor(render,255,0,0,255);
    SDL_RenderFillRect(render,&r);
}

///////////////////////////////////classe groupe ///////////////////////////////

void Group::draw(const Camera& cam, SDL_RendererFlip flip) {
     std::sort(items.begin(), items.end(), [](Item*a, Item* b){ return a->z <b->z;});

     for(auto* item : items) {
        // Ne plus dessiner ennemi mort
         if(item->isDead())
            continue;
      item->draw(cam, flip);
      item->drawHealthBar(cam);
     }     
}

void Group::draw(const Camera& cam, double angle, SDL_RendererFlip flip) {
    
     for(auto& item : gemItems) {
      item->draw(cam, angle, flip);
      item->drawHealthBar(cam);
     }
        
}
    
void Group::addRefe(Item* other) {
    if(std::find(items.begin(), items.end(), other) == items.end())
    items.push_back(other);
}

void Group::remove(Item* other) {
    items.erase(std::remove(items.begin(), items.end(), other), items.end());
}
    
void Group::update(int tick, float deltaTime) {
    for(auto* item : items) {
        item->update(tick, deltaTime);
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
    
// fonction spécialisées
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
    srand(time(nullptr));
    render = rend;
    SDL_GetRendererOutputSize(rend, &cam.w, &cam.h);
    cam.x = 0;
    cam.y = 0;
    a = 1.1;
    flip = SDL_FLIP_NONE;
    int w, h;

    SDL_GetRendererOutputSize(render, &w, &h);
    player.setRenderer(render);
    player.loadAnimation("person/JK_P_Sword__Run_", "000", ".png");
    player.setPos(128, 128);
    player.setSize(48, 48);
    player.setFPS(10);

    for(int i = 0; i < 10; i++) { 
    auto enemy = std::make_unique<Enemy>();
    enemy->setRenderer(render);
    enemy->loadFromImage("terre.png");
    int x, y;
    do{
        x = rand() % (MAP_W * TILE_SIZE);
        y = rand() % (MAP_H * TILE_SIZE);
        enemy->setPos(x, y);
    } while(collideWithMap(enemy->getPos()));
    enemy->setMap(&tileMap);
    enemy->setTarget(&player);
    enemies.addRefe(enemy.get());
    enemies.add(std::move(enemy));
    }

    gem.spawnItems(10, render);

    drawn.addRefe(&player);
}

bool Board::collideWithMap(const SDL_Rect& rect)
{
    int left   = rect.x + 4;
    int right  = rect.x + rect.w -4;
    int top    = rect.y + 4;
    int bottom = rect.y + rect.h -4;

    int tiles[4][2] = {
        {left,  top},
        {right, top},
        {left,  bottom},
        {right, bottom}
    };

    for (auto& t : tiles)
    {
        int tx = t[0] / TILE_SIZE;
        int ty = t[1] / TILE_SIZE;

        if (tileMap.isSolid(tx, ty))
            return true;
    }

    return false;
}

void Board::move(int x, int y) {
    drawn.move(x, y);
    clampPlayer();
}

void Board::update(int tick, float deltaTime) {
     //  Freeze gameplay 
    float realDeltaTime = deltaTime;
    float timeScale = 1.0f; 
     if(hitStop > 0){
       hitStop -= realDeltaTime;
       timeScale = 0.0f;;
     }
    deltaTime *= timeScale;
    // Faire suive camera au joueur
    cam.x = player.getX() - cam.w/2;
    cam.y = player.getY() - cam.h/2;
    // contr$ole de camera 
    if(cam.x < 0) cam.x = 0;
    if(cam.y < 0) cam.y = 0;
    if(cam.x > MAP_W * TILE_SIZE - cam.w) cam.x = MAP_W * TILE_SIZE - cam.w;
    if(cam.y > MAP_H * TILE_SIZE - cam.h) cam.y = MAP_H * TILE_SIZE - cam.h;
    // deplacement joueur
    // move(speedX, speedY);
    // collision
    // réduire vitesse diagonale
     float moveSpeed = 200.0f;
     float sx = speedX * moveSpeed * deltaTime;
     float sy = speedY * moveSpeed * deltaTime;

     if (sx != 0 && sy != 0)
     {
        sx *= 0.7f;
        sy *= 0.7f;
     }
    // mouvement X
     player.move(sx, 0);
     if (collideWithMap(player.getPos()))
     {
        player.move(-sx, 0);
     }
    // mouvement Y
    player.move(0, sy);
    if (collideWithMap(player.getPos()))
     {
       player.move(0, -sy);
     }
    // lancer projectille
    // for(auto& p : projectiles)
    // {
    // p->update(tick, deltaTime);
    // }
    // Collision projectile ↔ ennemi
    for(auto& p : projectiles)
    {
    if(!p->isAlive())
        continue;

    SDL_Rect pr = p->getPos();

    for(auto* e : enemies.getRaw())
    {
        if(e->isDead())
            continue;

        SDL_Rect er = e->getPos();

        if(SDL_HasIntersection(&pr, &er))
        {
            e->takeDamage(20);

            Enemy* enemy =
                static_cast<Enemy*>(e);

            enemy->applyKnockback(
                p->getX(),
                p->getY());

            p->kill();

            break;
        }
    }
    }
    // Collision projectile ↔ mur
   for(auto& p : projectiles)
   {
    p->update(tick, deltaTime);

    // collision avec la map
    if(collideWithMap(p->getPos()))
    {
        p->kill();  // ou p->setAlive(false)
    }
   }
//    supprétion d'ennims
    projectiles.erase(
    std::remove_if(projectiles.begin(), projectiles.end(), [](const auto& p) {
            return !p->isAlive();
    }),
    projectiles.end()
);
    // attacke ennemis et player
    player.damageCooldown -= deltaTime;
    for (auto* e : enemies.getRaw())
     {
        // sotie de ennemi est mort
        if(e->isDead())
         continue;
       if (player.getCollisionRect(*e))
        {
          if (player.damageCooldown <= 0)
          {
              player.takeDamage(10);
              player.damageCooldown = 1.0f;
          }
        }
     } 
    //  Update cooldown attaque
     if(player.attackCooldown > 0)
     {
       player.attackCooldown -= deltaTime;
     }

     if(player.attackCooldown <= 0)
     {
       player.attacking = false;
       player.alreadyHit = false;
     }
    //  Collision attaque vs ennemis
    if(player.attacking){
       player.attackTimer -= deltaTime;
       float elapsed = player.attackDuration - player.attackTimer;

       if(elapsed >= player.hitActiveTime && !player.alreadyHit) { 
       SDL_Rect atk = getAttackBox();

       for(auto* e : enemies.getRaw())
       {
           if(e->isDead())
              continue;
              SDL_Rect er = e->getPos();

           if(SDL_HasIntersection(&atk, &er)){
              if(!player.alreadyHit) {
                  e->takeDamage(25);
                //   comme ennemi n'a pas de applyKnokback
                // on va Cast e vers Enemy*
                 Enemy* enemy = static_cast<Enemy*>(e);
                  enemy->applyKnockback(player.getX(), player.getY());
                   hitStop = 0.05f;

                  for(int i = 0; i < 6; i++){
                  float vx = (rand() % 200 - 100);
                  float vy = (rand() % 200 - 100);

                  particles.push_back(std::make_unique<Particle>(e->getX() + 24, e->getY() + 24, vx, vy));
                 }
                   player.alreadyHit = true;
                }
            }
        }
        }
        if(player.attackTimer <= 0) {
        player.attacking = false;
        player.attackTimer = 0;
        player.alreadyHit = false;
        }
    }
    // ajout ennemis spontaner
    if(enemies.getRaw().size() < 30) {
        enemySpawnTimer += deltaTime;

    if(enemySpawnTimer >= enemySpawnDelay){
    if(enemySpawnDelay > 0.5f){
    enemySpawnDelay -= 0.02f;
    }
    auto enemy = std::make_unique<Enemy>();
    enemy->setRenderer(render);
    enemy->loadFromImage("terre.png");
    int x;
    int y;
    do{
       x = rand() % (MAP_W * TILE_SIZE);
       y = rand() % (MAP_H * TILE_SIZE);
       enemy->setPos(x, y);
    }while(collideWithMap(enemy->getPos()));
    enemy->setMap(&tileMap);
    enemy->setTarget(&player);

    enemies.addRefe(enemy.get());
    enemies.add(std::move(enemy));
   }
    }
    // update des groupes
    enemies.update(tick, deltaTime);
    drawn.update(tick, deltaTime);
    click.update(tick, deltaTime);
    collide.update(tick, deltaTime);
    // simple version (freeze jeu)
    if(player.isDead() && state == PLAYING)
     {
        state = GAME_OVER;
     } 
    // Bloquer gameplay si GAME OVER
     if(state != PLAYING)
     {
       return;
     }
    //  paticule
    for(auto& p : particles){
    p->update(tick, deltaTime);
    }
//     //Suppression de particule
    particles.erase(std::remove_if(particles.begin(), particles.end(), [](const std::unique_ptr<Particle>& p)
    {
         return !p->isAlive();
    }),
    particles.end()
   );
}

void Board::handleEvent(const SDL_Event& ev) {
    
    switch(ev.type) {
        case SDL_KEYDOWN:
                switch(ev.key.keysym.sym) {
                    case SDLK_UP: speedY = -1; break;
                    case SDLK_DOWN: speedY = 1; break;
                    case SDLK_LEFT: speedX =-1; flip = SDL_FLIP_HORIZONTAL; break;
                    case SDLK_RIGHT: speedX = 1; flip = SDL_FLIP_NONE; break;
                    case SDLK_SPACE:
                    if(player.attackCooldown <= 0){
                        player.attacking = true; 
                        player.attackCooldown = 0.35f;
                    } break;
                    case SDLK_f:{
                       float dirX = (flip == SDL_FLIP_NONE) ? 1.0f : -1.0f;
                       float dirY = 0.0f;

                       auto p = std::make_unique<Projectile>(
                              player.getX() + 20,
                              player.getY() + 20,
                              dirX,
                              dirY
                            );

                      p->setRenderer(render);
                      p->loadFromImage("yellow.png");

                      projectiles.push_back(std::move(p));

                      break;
                    }
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
    if(state == GAME_OVER) {
       if(ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_r)
       {
          resetGame();
       }
         return;
     }
}

void Board::resetGame(){
    state = PLAYING;
    printed = false;
    player.setPos(128, 128);
    player.setHP(100);
    player.setDead(false);

    speedX = 0;
    speedY = 0;
}
    
void Board::draw() {
    SDL_SetRenderDrawColor(render, 0, 0, 0, 255);
    SDL_RenderClear(render);
    tileMap.draw(cam);
    drawn.draw(cam, flip);
    for(auto& p : projectiles){
        p->draw(cam);
    }
    enemies.draw(cam);
    // gem.draw(cam, a);
    collide.draw(cam);
    click.draw(cam);
    a ++;
    if(state == GAME_OVER && !printed){
       std::cout << "GAME OVER - press R to restart\n";
       printed = true;
    }
    // draw particule
    for(auto& p : particles) {
      p->draw(cam);
    }
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

SDL_Rect Board::getAttackBox()
{
    SDL_Rect p = player.getPos();

    SDL_Rect box;

    box.w = 40;
    box.h = 40;

    box.y = p.y + 4;

    if(flip == SDL_FLIP_NONE)
    {
        box.x = p.x + p.w;
    }
    else
    {
        box.x = p.x - box.w;
    }

    return box;
}


//////////////////:classe de tilemap///////////////////////////////

TileMap::TileMap(SDL_Renderer* rend) : render(rend), tileset(nullptr) {
    load("maptile.png");
    loadFromFile("map.txt");
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

bool TileMap::loadFromFile(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open())
        return false;

    std::string line;
    int y = 0;

    while (std::getline(file, line) && y < MAP_H)
    {
        std::stringstream ss(line);
        int x = 0;
        int value;

        while (ss >> value && x < MAP_W)
        {
            map[y][x] = value;
            x++;
        }

        y++;
    }

    return true;
}

// fonction de décalage
int TileMap::getTileIndex(int tileType)
{
    switch(tileType)
    {
        case TILE_EMPTY: return -1;
        case TILE_GRASS: return 0;
        case TILE_WALL:  return 1;
        case TILE_WATER: return 2;
        case TILE_SAND:  return 3;
        default: return 0;
    }
}

void TileMap::draw(const Camera& cam) {

    SDL_Rect src;
    SDL_Rect dst;

    src.w = TILE_SIZE;
    src.h = TILE_SIZE;

    dst.w = TILE_SIZE;
    dst.h = TILE_SIZE;
    
    // permet de déssiner la partie positionné dans le camera
    // debut
    int startX = cam.x / TILE_SIZE;
    int startY = cam.y / TILE_SIZE;
    //  fin
    int endX = (cam.x + cam.w) / TILE_SIZE + 1;
    int endY = (cam.y + cam.h) / TILE_SIZE + 1;

    for(int y = startY; y < endY; y++) {

        for(int x = startX; x < endX; x++) {

            // Sécurité importante (anti-crash)
            if (y < 0 || y >= MAP_H || x < 0 || x >= MAP_W)
                continue;
            
            int index = getTileIndex(map[y][x]); //décalage maintenir proprement
            if(index <= 0)
             continue;
            src.x = index* TILE_SIZE;
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