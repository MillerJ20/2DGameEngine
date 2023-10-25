#include "MovementSystem.h"
#include "../src/Components/TransformComponent.h"
#include "../src/ECS/ECS.h"

MovementSystem::MovementSystem() {
  System::RequireComponent<TransformComponent>();
  // TODO:: Once the Registry is done, can use the entities and
  // componentSignature fields to implement update();
}

void MovementSystem::Update() {
  // TODO: Loop all entities the system is interested in
  // for (auto entity : GetEntities()) {
  // Update entity position based on velocity
  //}
}
