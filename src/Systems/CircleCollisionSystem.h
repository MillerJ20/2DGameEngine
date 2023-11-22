#ifndef CIRCLECOLLISIONSYSTEM_H
#define CIRCLECOLLISIONSYSTEM_H

#include "../Components/CircleColliderComponent.h"
#include "../Components/TransformComponent.h"
#include "../ECS/ECS.h"
#include "../EventManager/EventManager.h"
#include "../Events/CollisionEvent.h"
#include <cmath>
#include <memory>
#include <string>

class CircleCollisionSystem : public System {
public:
  CircleCollisionSystem() {
    RequireComponent<CircleColliderComponent>();
    RequireComponent<TransformComponent>();
  };

  void Update(std::unique_ptr<EventManager>& eventManager) {
    auto entities = GetSystemEntities();
    for (auto i = entities.begin(); i != entities.end(); i++) {
      Entity a = *i;
      const auto aTransform = a.GetComponent<TransformComponent>();
      const auto aCollider = a.GetComponent<CircleColliderComponent>();

      for (auto j = i; j != entities.end(); j++) {
        Entity b = *j;

        if (a == b) {
          continue;
        }

        const auto bTransform = b.GetComponent<TransformComponent>();
        const auto bCollider = b.GetComponent<CircleColliderComponent>();

        double distance = calculateDistance(aTransform, bTransform);

        if (distance <= (aCollider.radius + bCollider.radius)) {
          Logger::Log(
              "Collision detected between: " + std::to_string(a.GetId()) +
              " and " + std::to_string(b.GetId()));

          eventManager->EmitEvent<CollisionEvent>(a, b);
        }
      }
    }
  }

  double calculateDistance(TransformComponent a, TransformComponent b) {
    double distX = a.position.x - b.position.x;
    double distY = a.position.y - b.position.y;

    return std::sqrt((distX * distX) + (distY * distY));
  }
};

#endif // !CIRCLECOLLISIONSYSTEM_H
