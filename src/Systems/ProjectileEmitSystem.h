#ifndef PROJECTILEMITSYSTEM_H
#define PROJECTILEMITSYSTEM_H

#include "../Components/BoxColliderComponent.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/TransformComponent.h"
#include "../ECS/ECS.h"
#include <SDL2/SDL_timer.h>
#include <memory>

class ProjectileEmitSystem : public System {
public:
  ProjectileEmitSystem() {
    RequireComponent<ProjectileEmitterComponent>();
    RequireComponent<TransformComponent>();
  }

  void Update(std::unique_ptr<Registry>& registry) {
    for (auto entity : GetSystemEntities()) {
      auto& projectileEmitter =
          entity.GetComponent<ProjectileEmitterComponent>();
      auto transform = entity.GetComponent<TransformComponent>();

      if (SDL_GetTicks() - projectileEmitter.lastEmissionTime >
          projectileEmitter.repeatFrequency) {

        glm::vec2 projectilePosition = transform.position;
        if (entity.HasComponent<SpriteComponent>()) {
          auto sprite = entity.GetComponent<SpriteComponent>();
          projectilePosition.x += (transform.scale.x * sprite.width / 2);
          projectilePosition.y += (transform.scale.y * sprite.height / 2);
        }

        Entity projectile = registry->CreateEntity();
        projectile.AddComponent<TransformComponent>(projectilePosition,
                                                    glm::vec2(1.0, 1.0));
        projectile.AddComponent<RigidBodyComponent>(
            projectileEmitter.projectileVelocity);
        projectile.AddComponent<SpriteComponent>("bullet-image", 4, 4, 4);
        projectile.AddComponent<BoxColliderComponent>(4, 4);

        projectileEmitter.lastEmissionTime = SDL_GetTicks();
      }
    }
  }
};

#endif // !PROJECTILEMITSYSTEM_H
