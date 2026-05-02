/*
   Création de cache qui permetre de gerer les images, son chargement 
   et son affichage
*/

#pragma once
#include <iostream>
#include<unordered_map>
#include <SDL.h>
#include <SDL_image.h>

// la classe de rendure
class Renderer {
    public:
    SDL_Texture* image   = nullptr;
    SDL_Renderer* render = nullptr;
// Controle les images avec leurs nom
    std::unordered_map<std::string, SDL_Texture*> textureCache;
    
    public:
    bool loadFromImage(const std::string& filePath);
    void setRenderer(SDL_Renderer* rend);
    void clearTexture();
};