#ifndef PROJECTILEMITSYSTEM_H
#define PROJECTILEMITSYSTEM_H

#include "../Components/BoxColliderComponent.h"
#include "../Components/CameraFollowComponent.h"
#include "../Components/ProjectileComponent.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/TransformComponent.h"
#include "../ECS/ECS.h"
#include "../EventManager/EventManager.h"
#include "../Events/KeyDownEvent.h"
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_timer.h>
#include <memory>

class ProjectileEmitSystem : public System {
public:
  ProjectileEmitSystem() {
    RequireComponent<ProjectileEmitterComponent>();
    RequireComponent<TransformComponent>();
  }

  void SubscribeToEvent(std::unique_ptr<EventManager>& eventBus) {
    eventBus->SubscribeToEvent<KeyDownEvent>(
        this, &ProjectileEmitSystem::OnKeyPressed);
  }

  void OnKeyPressed(KeyDownEvent& event) {
    if (event.keyCode == SDLK_SPACE) {
      for (auto entity : GetSystemEntities()) {
        if (entity.HasComponent<CameraFollowComponent>()) {
          const auto projectileEmitter =
              entity.GetComponent<ProjectileEmitterComponent>();
          const auto transform = entity.GetComponent<TransformComponent>();
          const auto rigidbody = entity.GetComponent<RigidBodyComponent>();

          // TODO: Refactor below code into a "Fire" function to avoid repition

          // If parent entity has sprite, start the projectile position in the
          // middle of the entity
          glm::vec2 projectilePosition = transform.position;
          if (entity.HasComponent<SpriteComponent>()) {
            auto sprite = entity.GetComponent<SpriteComponent>();
            projectilePosition.x += (transform.scale.x * sprite.width / 2);
            projectilePosition.y += (transform.scale.y * sprite.height / 2);
          }

          // If parent entity direction is controlled by the keyboard keys,
          // modify the direction of the projectile accordingly
          glm::vec2 projectileVelocity = projectileEmitter.projectileVelocity;
          int directionX = 0;
          int directionY = 0;
          if (rigidbody.velocity.x > 0)
            directionX = +1;
          if (rigidbody.velocity.x < 0)
            directionX = -1;
          if (rigidbody.velocity.y > 0)
            directionY = +1;
          if (rigidbody.velocity.y < 0)
            directionY = -1;
          projectileVelocity.x =
              projectileEmitter.projectileVelocity.x * directionX;
          projectileVelocity.y =
              projectileEmitter.projectileVelocity.y * directionY;

          // Create new projectile entity and add it to the world
          Entity projectile = entity.registry->CreateEntity();
          projectile.AddComponent<TransformComponent>(projectilePosition,
                                                      glm::vec2(1.0, 1.0), 0.0);
          projectile.AddComponent<RigidBodyComponent>(projectileVelocity);
          projectile.AddComponent<SpriteComponent>("bullet-image", 4, 4, 4);
          projectile.AddComponent<BoxColliderComponent>(4, 4);
          projectile.AddComponent<ProjectileComponent>(
              projectileEmitter.isFriendly, projectileEmitter.hitPercentDamage,
              projectileEmitter.projectileDuration);
        }
      }
    }
  }

  void Update(std::unique_ptr<Registry>& registry) {
    for (auto entity : GetSystemEntities()) {
      auto& projectileEmitter =
          entity.GetComponent<ProjectileEmitterComponent>();
      const auto transform = entity.GetComponent<TransformComponent>();

      // If emission frequency is zero, bypass re-emission logic
      if (projectileEmitter.repeatFrequency == 0) {
        continue;
      }

      // Check if its time to re-emit a new projectile
      if (SDL_GetTicks() - projectileEmitter.lastEmissionTime >
          projectileEmitter.repeatFrequency) {
        glm::vec2 projectilePosition = transform.position;
        if (entity.HasComponent<SpriteComponent>()) {
          const auto sprite = entity.GetComponent<SpriteComponent>();
          projectilePosition.x += (transform.scale.x * sprite.width / 2);
          projectilePosition.y += (transform.scale.y * sprite.height / 2);
        }

        // Add a new projectile entity to the registry
        Entity projectile = registry->CreateEntity();
        projectile.AddComponent<TransformComponent>(projectilePosition,
                                                    glm::vec2(1.0, 1.0), 0.0);
        projectile.AddComponent<RigidBodyComponent>(
            projectileEmitter.projectileVelocity);
        projectile.AddComponent<SpriteComponent>("bullet-image", 4, 4, 4);
        projectile.AddComponent<BoxColliderComponent>(4, 4);
        projectile.AddComponent<ProjectileComponent>(
            projectileEmitter.isFriendly, projectileEmitter.hitPercentDamage,
            projectileEmitter.projectileDuration);

        // Update the projectile emitter component last emission to the current
        // milliseconds
        projectileEmitter.lastEmissionTime = SDL_GetTicks();
      }
    }
  }
};

#endif // !PROJECTILEMITSYSTEM_H
