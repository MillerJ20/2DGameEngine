#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "../AssetStore/AssetStore.h"
#include "../Components/SpriteComponent.h"
#include "../Components/TransformComponent.h"
#include "../ECS/ECS.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <algorithm>
#include <memory>
#include <vector>

class RenderSystem : public System {
public:
  RenderSystem() {
    RequireComponent<SpriteComponent>();
    RequireComponent<TransformComponent>();
  }

  // TODO: Rendering of colliders in debug mode should also be handled here
  // Add a "DebugUpdate()" method for drawing of ebug information
  // Remove BoxColliderRenderSystem and CircleColliderRenderSystem
  void Update(SDL_Renderer* renderer, std::unique_ptr<AssetStore>& assetStore,
              SDL_Rect& camera) {

    // UGLY ASS SORTING BELOW:
    // Create a vector to hold both sprite and transform components of all
    // entites
    struct RenderableEntity {
      TransformComponent transformComponent;
      SpriteComponent spriteComponent;
    };

    std::vector<RenderableEntity> renderableEntities;

    for (auto entity : GetSystemEntities()) {
      RenderableEntity renderableEntity;
      renderableEntity.spriteComponent = entity.GetComponent<SpriteComponent>();
      renderableEntity.transformComponent =
          entity.GetComponent<TransformComponent>();

      bool isEntityOutsideCameraView =
          (renderableEntity.transformComponent.position.x +
                   (renderableEntity.transformComponent.scale.x *
                    renderableEntity.spriteComponent.width) <
               camera.x ||
           renderableEntity.transformComponent.position.x +
                   renderableEntity.spriteComponent.width >
               camera.x + camera.w ||
           renderableEntity.transformComponent.position.y +
                   (renderableEntity.transformComponent.scale.y *
                    renderableEntity.spriteComponent.height) <
               camera.y ||
           renderableEntity.transformComponent.position.y >
               camera.y + camera.h);
      // Only render entities in camera view
      if (isEntityOutsideCameraView &&
          !renderableEntity.spriteComponent.isFixed) {
        continue;
      }

      renderableEntities.emplace_back(renderableEntity);
    }

    // Sort entities by z_index (Could be refactored to sort on entity creation)
    std::sort(renderableEntities.begin(), renderableEntities.end(),
              [](const RenderableEntity a, const RenderableEntity b) {
                return a.spriteComponent.zIndex < b.spriteComponent.zIndex;
              });

    for (auto entity : renderableEntities) {
      const auto transform = entity.transformComponent;
      const auto sprite = entity.spriteComponent;

      SDL_Rect srcRect = sprite.srcRect;
      SDL_Rect dstRect = {static_cast<int>(transform.position.x -
                                           (sprite.isFixed ? 0 : camera.x)),
                          static_cast<int>(transform.position.y -
                                           (sprite.isFixed ? 0 : camera.y)),
                          static_cast<int>(sprite.width * transform.scale.x),
                          static_cast<int>(sprite.height * transform.scale.y)};

      SDL_RenderCopyEx(renderer, assetStore->GetTexture(sprite.assetId),
                       &srcRect, &dstRect, transform.rotation, NULL,
                       sprite.flip);
    }
  }
};

#endif
