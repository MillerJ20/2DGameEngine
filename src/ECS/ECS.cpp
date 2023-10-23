#include "ECS.h"

#include <algorithm>

int Entity::GetId() const { return id; }

void System::AddEntityToSystem(Entity entity) { entities.push_back(entity); }

void System::RemoveEntityFromSystem(Entity entity) {
  for (int i = 0; i < entities.size(); i++) {
    if (entities[i].GetId() == entity.GetId()) {
      entities.erase(entities.begin() + i);
    }
  }
}

std::vector<Entity> System::GetAllSystemEntities() const { return entities; }

const Signature &System::GetComponentSignature() const {
  return componentSignature;
}
