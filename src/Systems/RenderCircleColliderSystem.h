#ifndef RENDERCIRCLECOLLIDERSYSTEM_H
#define RENDERCIRCLECOLLIDERSYSTEM_H

#include "../Components/CircleColliderComponent.h"
#include "../Components/TransformComponent.h"
#include "../ECS/ECS.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>

class RenderCircleColliderSystem : public System {
public:
  RenderCircleColliderSystem() {
    RequireComponent<TransformComponent>();
    RequireComponent<CircleColliderComponent>();
  }

  // TODO: This should be refactored into a "DebugUpdate" method in the base
  // RenderSystem This class should be removed after
  void Update(SDL_Renderer* renderer, SDL_Rect& camera) {
    for (auto entity : GetSystemEntities()) {
      const auto transform = entity.GetComponent<TransformComponent>();
      const auto collider = entity.GetComponent<CircleColliderComponent>();

      // TODO: Investigate scaling circles to ensure this transform.scale.x is
      // the best to use here
      DrawCircle(
          renderer,
          transform.position.x + collider.radius + collider.offset - camera.x,
          transform.position.y + collider.radius + collider.offset - camera.y,
          collider.radius * transform.scale.x);
    }
  }

  // TODO: Verify that this is the best way to draw a 2D circle with SDL
  void DrawCircle(SDL_Renderer* renderer, int32_t centerX, int32_t centerY,
                  int32_t radius) {
    const int32_t diameter = (radius * 2);

    int32_t x = (radius - 1);
    int32_t y = 0;
    int32_t tx = 1;
    int32_t ty = 1;
    int32_t error = (tx - diameter);

    while (x >= y) {
      // Each of the following renders an octant of the circle
      SDL_RenderDrawPoint(renderer, centerX + x, centerY - y);
      SDL_RenderDrawPoint(renderer, centerX + x, centerY + y);
      SDL_RenderDrawPoint(renderer, centerX - x, centerY - y);
      SDL_RenderDrawPoint(renderer, centerX - x, centerY + y);
      SDL_RenderDrawPoint(renderer, centerX + y, centerY - x);
      SDL_RenderDrawPoint(renderer, centerX + y, centerY + x);
      SDL_RenderDrawPoint(renderer, centerX - y, centerY - x);
      SDL_RenderDrawPoint(renderer, centerX - y, centerY + x);

      if (error <= 0) {
        ++y;
        error += ty;
        ty += 2;
      }

      if (error > 0) {
        --x;
        tx += 2;
        error += (tx - diameter);
      }
    }
  }
};

#endif // !RENDERCIRCLECOLLIDERSYSTEM_H
