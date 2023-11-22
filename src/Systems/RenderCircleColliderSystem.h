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

  void Update(SDL_Renderer* renderer) {
    for (auto entity : GetSystemEntities()) {
      const auto transform = entity.GetComponent<TransformComponent>();
      const auto collider = entity.GetComponent<CircleColliderComponent>();

      DrawCircle(renderer, transform.position.x, transform.position.y,
                 collider.radius);
    }
  }

  void DrawCircle(SDL_Renderer* renderer, int32_t centreX, int32_t centreY,
                  int32_t radius) {
    const int32_t diameter = (radius * 2);

    int32_t x = (radius - 1);
    int32_t y = 0;
    int32_t tx = 1;
    int32_t ty = 1;
    int32_t error = (tx - diameter);

    while (x >= y) {
      // Each of the following renders an octant of the circle
      SDL_RenderDrawPoint(renderer, centreX + x, centreY - y);
      SDL_RenderDrawPoint(renderer, centreX + x, centreY + y);
      SDL_RenderDrawPoint(renderer, centreX - x, centreY - y);
      SDL_RenderDrawPoint(renderer, centreX - x, centreY + y);
      SDL_RenderDrawPoint(renderer, centreX + y, centreY - x);
      SDL_RenderDrawPoint(renderer, centreX + y, centreY + x);
      SDL_RenderDrawPoint(renderer, centreX - y, centreY - x);
      SDL_RenderDrawPoint(renderer, centreX - y, centreY + x);

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
