#ifndef CAMERAMOVEMENTSYSTEM_H
#define CAMERAMOVEMENTSYSTEM_H

#include "../Components/CameraFollowComponent.h"
#include "../Components/TransformComponent.h"
#include "../ECS/ECS.h"
#include "../Game/Game.h"

#include <SDL2/SDL.h>

class CameraMovementSystem : public System {
public:
  CameraMovementSystem() {
    RequireComponent<CameraFollowComponent>();
    RequireComponent<TransformComponent>();
  }

  void Update(SDL_Rect& camera) {
    for (auto entity : GetSystemEntities()) {
      auto transform = entity.GetComponent<TransformComponent>();

      if (transform.position.x + (camera.w / 2.0) < Game::mapWidth) {
        camera.x = transform.position.x - (Game::windowWidth / 2.0);
      }
      if (transform.position.y + (camera.h / 2.0) < Game::mapHeight) {
        camera.y = transform.position.y - (Game::windowHeight / 2.0);
      }

      // Keep camera view inside the limits of the screen
      camera.x = camera.x < 0 ? 0 : camera.x;
      camera.y = camera.y < 0 ? 0 : camera.y;
      camera.x = camera.x > camera.w ? camera.w : camera.x;
      camera.y = camera.y > camera.h ? camera.h : camera.y;
    }
  }
};

#endif // !CAMERAMOVEMENTSYSTEM_H
