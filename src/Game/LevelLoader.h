#ifndef LEVELLOADER_H
#define LEVELLOADER_H

#include "../AssetStore/AssetStore.h"
#include "../ECS/ECS.h"
#include <SDL2/SDL.h>
#include <memory>

class LevelLoader {
public:
  LevelLoader();
  ~LevelLoader();

  void LoadLevel(const std::unique_ptr<Registry>& registry,
                 const std::unique_ptr<AssetStore>& assetStore,
                 SDL_Renderer* renderer, int level);
};

#endif
