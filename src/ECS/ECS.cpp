#include "ECS.h"
#include "../Logger/Logger.h"

#include <algorithm>
#include <string>
#include <utility>

// ENTITY METHODS
int Entity::GetId() const { return id; }

int BaseComponent::nextId = 0;

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
  int entityId;

  entityId = numEntities++;

  Entity entityToCreate(entityId);
  entitiesToBeAdded.insert(entityToCreate);

  if (entityId >= entityComponentSignature.size()) {
    entityComponentSignature.resize(entityId + 1);
  }

  Logger::Log("Entity created with ID: " + std::to_string(entityId));

  return entityToCreate;
}

void Registry::AddEntityToSystem(Entity entity) {
  const auto entityId = entity.GetId();
  const auto entitySignature = entityComponentSignature[entityId];

  for (auto& system : systems) {
    const auto& systemSignature = system.second->GetComponentSignature();

    bool signaturesMatch =
        (entitySignature & systemSignature) == systemSignature;

    if (signaturesMatch) {
      system.second->AddEntityToSystem(entity);
    }
  }
}

void Registry::Update() {
  for (auto entity : entitiesToBeAdded) {
    AddEntityToSystem(entity);
  }
  entitiesToBeAdded.clear();
}
