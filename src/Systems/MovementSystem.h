#ifndef MOVEMENTSYSTEM_H
#define MOVEMENTSYSTEM_H

#include "../Components/RigidBodyComponent.h"
#include "../Components/TransformComponent.h"
#include "../ECS/ECS.h"
#include "../Game/Game.h"

class MovementSystem : public System {
public:
  MovementSystem() {
    RequireComponent<TransformComponent>();
    RequireComponent<RigidBodyComponent>();
  }

  void Update(double deltaTime) {
    for (auto entity : GetSystemEntities()) {
      auto& transform = entity.GetComponent<TransformComponent>();
      auto& rigidbody = entity.GetComponent<RigidBodyComponent>();

      transform.position.x += rigidbody.velocity.x * deltaTime;
      transform.position.y += rigidbody.velocity.y * deltaTime;

      bool isEntityOutsideMap = (transform.position.x < 0 ||
                                 transform.position.x > (Game::mapWidth - 32) ||
                                 transform.position.y < 0 ||
                                 transform.position.y > (Game::mapHeight - 32));

      if (isEntityOutsideMap && !entity.HasTag("player")) {
        entity.Kill();
      } else if (isEntityOutsideMap && entity.HasTag("player")) {
        rigidbody.velocity = glm::vec2(0, 0);
      }
    }
  }
};

#endif
