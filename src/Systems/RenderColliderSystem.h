#ifndef RENDERCOLLIDERSYSTEM_H
#define RENDERCOLLIDERSYSTEM_H

#include "../Components/BoxColliderComponent.h"
#include "../Components/TransformComponent.h"
#include "../ECS/ECS.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>

class RenderColliderSystem : public System {
public:
  RenderColliderSystem() {
    RequireComponent<TransformComponent>();
    RequireComponent<BoxColliderComponent>();
  }

  // TODO: This should be refactored into a "DebugUpdate" method in the base
  // RenderSystem This class should be removed after
  void Update(SDL_Renderer* renderer) {
    for (auto entity : GetSystemEntities()) {
      const auto transform = entity.GetComponent<TransformComponent>();
      const auto collider = entity.GetComponent<BoxColliderComponent>();

      SDL_Rect colliderRect = {
          static_cast<int>(transform.position.x + collider.offset.x),
          static_cast<int>(transform.position.y + collider.offset.y),
          static_cast<int>(collider.width), static_cast<int>(collider.height)};
      SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
      SDL_RenderDrawRect(renderer, &colliderRect);
    }
  }
};

#endif // !RENDERCOLLIDERSYSTEM_H
