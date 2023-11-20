#ifndef DAMAGESYSTEM_H
#define DAMAGESYSTEM_H

#include "../Components/BoxColliderComponent.h"
#include "../ECS/ECS.h"
#include "../EventManager/EventManager.h"
#include "../Events/CollisionEvent.h"

class DamageSystem : public System {
public:
  DamageSystem() { RequireComponent<BoxColliderComponent>(); };

  void SubscribeToEvents(std::unique_ptr<EventManager>& eventManager) {
    eventManager->SubscribeToEvent<CollisionEvent>(this,
                                                   &DamageSystem::onCollision);
  };

  void onCollision(CollisionEvent& event) {
    event.a.Kill();
    event.b.Kill();
  }
};

#endif // !DAMAGESYSTEM_H
