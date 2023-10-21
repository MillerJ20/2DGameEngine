#include "ECS.h"

int Entity::GetId() const { return id; }

void AddEntityToSystem(Entity entity) {
  // TODO: Add the passed in Entity to this system
}

void RemoveEntityFromSystem(Entity entity) {
  // TODO: Remove the passed in entity from this system
}

std::vector<Entity> GetAllSystemEntities() {
  // TODO: Return all of the entities that this system is applying to
}
Signature &GetComponentSignature() {
  // TODO: Return the Signature for a component
}
