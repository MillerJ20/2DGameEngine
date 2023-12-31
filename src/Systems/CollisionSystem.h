#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H

#include "../Components/BoxColliderComponent.h"
#include "../Components/TransformComponent.h"
#include "../ECS/ECS.h"
#include "../EventManager/EventManager.h"
#include "../Events/CollisionEvent.h"
#include <memory>
#include <string>

class CollisionSystem : public System {
public:
  CollisionSystem() {
    RequireComponent<BoxColliderComponent>();
    RequireComponent<TransformComponent>();
  };

  // TODO: This class should handle all collision checks
  // Circle collision class should be removed for this base class
  // How to handle collisions between different shapes and rotations?
  void Update(std::unique_ptr<EventManager>& eventManager) {
    auto entities = GetSystemEntities();
    for (auto i = entities.begin(); i != entities.end(); i++) {
      Entity a = *i;
      const auto aTransform = a.GetComponent<TransformComponent>();
      const auto aCollider = a.GetComponent<BoxColliderComponent>();

      for (auto j = i; j != entities.end(); j++) {
        Entity b = *j;

        if (a == b) {
          continue;
        }

        const auto bTransform = b.GetComponent<TransformComponent>();
        const auto bCollider = b.GetComponent<BoxColliderComponent>();

        // AABB Collision Check
        if (aTransform.position.x + aCollider.offset.x <
                bTransform.position.x + bCollider.offset.x + bCollider.width &&
            aTransform.position.x + aCollider.offset.x + aCollider.width >
                bTransform.position.x + bCollider.offset.x &&
            aTransform.position.y + aCollider.offset.y <
                bTransform.position.y + bCollider.offset.y + bCollider.height &&
            aTransform.position.y + aCollider.offset.y + aCollider.height >
                bTransform.position.y + bCollider.offset.y) {
          Logger::Log(
              "Collision detected between: " + std::to_string(a.GetId()) +
              " and " + std::to_string(b.GetId()));

          eventManager->EmitEvent<CollisionEvent>(a, b);
        }
      }
    }
  }
};

#endif // !COLLISIONSYSTEM_H
