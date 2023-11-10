#include "./AssetStore.h"
#include "../Logger/Logger.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <string>

AssetStore::AssetStore() { Logger::Log("Asset Store constructor called!"); }

AssetStore::~AssetStore() {
  ClearAssets();
  Logger::Log("Asset Store destructor called!");
}

void AssetStore::ClearAssets() {
  for (auto texture : textures) {
    SDL_DestroyTexture(texture.second);
  }
  textures.clear();
}

void AssetStore::AddTexture(SDL_Renderer* renderer, const std::string& assetId,
                            const std::string& filePath) {
  SDL_Surface* surface = IMG_Load(filePath.c_str());
  SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_FreeSurface(surface);
  textures.emplace(assetId, texture);

  Logger::Log("New texture added into the asset store: " + assetId);
}

SDL_Texture* AssetStore::GetTexture(const std::string& assetId) {
  return textures[assetId];
}
