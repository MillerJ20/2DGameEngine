#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "../AssetStore/AssetStore.h"
#include "../Components/SpriteComponent.h"
#include "../Components/TransformComponent.h"
#include "../ECS/ECS.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <memory>

class RenderSystem : public System {
public:
  RenderSystem() {
    RequireComponent<SpriteComponent>();
    RequireComponent<TransformComponent>();
  }

  void Update(SDL_Renderer* renderer,
              std::unique_ptr<AssetStore>& asssetStore) {
    for (auto entity : GetSystemEntities()) {
      const auto transform = entity.GetComponent<TransformComponent>();
      const auto sprite = entity.GetComponent<SpriteComponent>();

      SDL_Rect srcRect = sprite.srcRect;
      SDL_Rect dstRect = {static_cast<int>(transform.position.x),
                          static_cast<int>(transform.position.y),
                          static_cast<int>(sprite.width * transform.scale.x),
                          static_cast<int>(sprite.height * transform.scale.y)};

      SDL_RenderCopyEx(renderer, asssetStore->GetTexture(sprite.assetId),
                       &srcRect, &dstRect, transform.rotation, NULL,
                       SDL_FLIP_NONE);
    }
  }
};

#endif
