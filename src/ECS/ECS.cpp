#include "ECS.h"
#include "../Logger/Logger.h"

#include <algorithm>
#include <string>
#include <utility>

// ENTITY METHODS
int Entity::GetId() const { return id; }

// SYSTEM METHODS
void System::AddEntityToSystem(Entity entity) { entities.push_back(entity); }

void System::RemoveEntityFromSystem(Entity entity) {
  for (int i = 0; i < entities.size(); i++) {
    if (entities[i].GetId() == entity.GetId()) {
      entities.erase(entities.begin() + i);
    }
  }
}

std::vector<Entity> System::GetAllSystemEntities() const { return entities; }

const Signature& System::GetComponentSignature() const {
  return componentSignature;
}

// REGISTRY METHODS
Entity Registry::CreateEntity() {
  int entityId = numEntities++;
  if (entityId >= entityComponentSignature.size()) {
    entityComponentSignature.resize(entityId + 1);
  }

  Entity entityToCreate = Entity(entityId);
  entitiesToBeAdded.insert(entityToCreate);

  Logger::Log("Entity created with ID: " + std::to_string(entityId));

  return entityToCreate;
}
