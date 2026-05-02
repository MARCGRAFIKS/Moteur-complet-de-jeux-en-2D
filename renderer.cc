#include "renderer.hpp"

    bool Renderer::loadFromImage(const std::string& filePath) {
    auto it = textureCache.find(filePath);
    if(it!=textureCache.end())
    return (image = it->second)!=nullptr;
    image = IMG_LoadTexture(render, filePath.c_str());
    if(!image)
    return false;
    textureCache[filePath] = image;
    return image!=nullptr;
 }

void Renderer::setRenderer(SDL_Renderer* rend) {
    render = rend;
}

void Renderer::clearTexture() {
    for(auto& pair : textureCache) {
        SDL_DestroyTexture(pair.second);
    }
    textureCache.clear();
}