#ifndef ECS_H
#define ECS_H

#include "../Logger/Logger.h"
#include <bitset>
#include <deque>
#include <memory>
#include <set>
#include <typeindex>
#include <unordered_map>
#include <vector>

const unsigned int MAX_COMPONENTS = 32;
// TODO: Use a profiler (Valgrind) to measure memory/cache performance to check
// for optimizations

////////////////////////////////////////////////////////////////////////////////
// Signature
////////////////////////////////////////////////////////////////////////////////
// We use a bitset (1s and 0s) to keep track of which components an entity has,
// and also helps keep track of which entities a system is interested in.
////////////////////////////////////////////////////////////////////////////////
typedef std::bitset<MAX_COMPONENTS> Signature;

struct IComponent {
protected:
  static int nextId;
};

// Used to assign a unique id to a component type
template <typename T> class Component : public IComponent {
public:
  // Returns the unique id of Component<T>
  static int GetId() {
    static auto id = nextId++;
    return id;
  }
};

class Entity {
private:
  int id;

public:
  Entity(int id) : id(id){};
  Entity(const Entity& entity) = default;
  void Kill();
  int GetId() const;

  // TODO: Remove string-based matching. Each tag/group is unique ID
  // Stored in order to allow for Binary Search

  // Manage entity tags and Groups
  void Tag(const std::string& tag);
  bool HasTag(const std::string& tag) const;
  void Group(const std::string& group);
  bool BelongsToGroup(const std::string& group) const;

  Entity& operator=(const Entity& other) = default;
  bool operator==(const Entity& other) const { return id == other.id; }
  bool operator!=(const Entity& other) const { return id != other.id; }
  bool operator>(const Entity& other) const { return id > other.id; }
  bool operator<(const Entity& other) const { return id < other.id; }

  template <typename TComponent, typename... TArgs>
  void AddComponent(TArgs&&... args);
  template <typename TComponent> void RemoveComponent();
  template <typename TComponent> bool HasComponent() const;
  template <typename TComponent> TComponent& GetComponent() const;

  // Hold a pointer to the entity's owner registry
  class Registry* registry;
};

////////////////////////////////////////////////////////////////////////////////
// System
////////////////////////////////////////////////////////////////////////////////
// The system processes entities that contain a specific signature
////////////////////////////////////////////////////////////////////////////////
class System {
private:
  Signature componentSignature;
  std::vector<Entity> entities;

public:
  System() = default;
  ~System() = default;

  void AddEntityToSystem(Entity entity);
  void RemoveEntityFromSystem(Entity entity);
  std::vector<Entity> GetSystemEntities() const;
  const Signature& GetComponentSignature() const;

  // Defines the component type that entities must have to be considered by the
  // system
  template <typename TComponent> void RequireComponent();
};

////////////////////////////////////////////////////////////////////////////////
// Pool
////////////////////////////////////////////////////////////////////////////////
// A pool is just a vector (contiguous data) of objects of type T
////////////////////////////////////////////////////////////////////////////////
class IPool {
public:
  virtual ~IPool() = default;
  virtual void RemoveEntityFromPool(int entityId) = 0;
};

template <typename T> class Pool : public IPool {
private:
  // Vector of objects and current number of elements
  std::vector<T> data;
  int size;

  // Maps to keep track of entity ids per index
  std::unordered_map<int, int> entityIdToIndex;
  std::unordered_map<int, int> indexToEntityId;

public:
  Pool(int capacity = 100) {
    size = 0;
    data.resize(capacity);
  }

  virtual ~Pool() = default;

  bool IsEmpty() const { return size == 0; }

  int GetSize() const { return size; }

  void Clear() {
    data.clear();
    entityIdToIndex.clear();
    indexToEntityId.clear();
    size = 0;
  }

  void Add(T object) { data.push_back(object); }

  void Set(int entityId, T object) {
    if (entityIdToIndex.find(entityId) != entityIdToIndex.end()) {
      // If the element exists, replace the component with current object
      int index = entityIdToIndex[entityId];
      data[index] = object;
    } else {
      int index = size;
      entityIdToIndex.emplace(entityId, index);
      indexToEntityId.emplace(index, entityId);

      if (index >= data.capacity()) {
        data.resize(size * 2);
      }
      data[index] = object;
      size++;
    }
  }

  void Remove(int entityId) {
    // Find the index of the removed entity and the index of the entity at the
    // end of the pool
    int indexOfRemoved = entityIdToIndex[entityId];
    int indexOfLast = size - 1;
    // Replace the removed entity with the last entity in the pool
    data[indexOfRemoved] = data[indexOfLast];

    // Find the entity id of the last element
    int entityIdOfLastElement = indexToEntityId[indexOfLast];
    // Swap removed element with last element
    entityIdToIndex[entityIdOfLastElement] = indexOfRemoved;
    indexToEntityId[indexOfRemoved] = entityIdOfLastElement;

    // Erase from the maps after swap
    entityIdToIndex.erase(entityId);
    indexToEntityId.erase(indexOfLast);

    size--;
  }

  void RemoveEntityFromPool(int entityId) override {
    if (entityIdToIndex.find(entityId) != entityIdToIndex.end()) {
      Remove(entityId);
    }
  }

  T& Get(int entityId) {
    int index = entityIdToIndex[entityId];
    return static_cast<T&>(data[index]);
  }

  T& operator[](unsigned int index) { return data[index]; }
};

////////////////////////////////////////////////////////////////////////////////
// Registry
////////////////////////////////////////////////////////////////////////////////
// The registry manages the creation and destruction of entities, add systems,
// and components.
////////////////////////////////////////////////////////////////////////////////
class Registry {
private:
  int numEntities = 0;

  // Vector of component pools, each pool contains all the data for a certain
  // compoenent type [Vector index = component type id] [Pool index = entity id]
  std::vector<std::shared_ptr<IPool>> componentPools;

  // Vector of component signatures per entity, saying which component is turned
  // "on" for a given entity [Vector index = entity id]
  std::vector<Signature> entityComponentSignatures;

  // Map of active systems
  // [Map key = system type id]
  std::unordered_map<std::type_index, std::shared_ptr<System>> systems;

  // Set of entities that are flagged to be added or removed in the next
  // registry Update()
  std::set<Entity> entitiesToBeAdded;
  std::set<Entity> entitiesToBeKilled;

  std::deque<int> freeIds;

  // Entity tags
  std::unordered_map<std::string, Entity> entityPerTag;
  std::unordered_map<int, std::string> tagPerEntity;

  // Entity Groups
  std::unordered_map<std::string, std::set<Entity>> entitiesPerGroup;
  std::unordered_map<int, std::string> groupPerEntity;

public:
  Registry() { Logger::Log("Registry constructor called"); }

  ~Registry() { Logger::Log("Registry destructor called"); }

  // The registry Update() finally processes the entities that are waiting to be
  // added/killed to the systems
  void Update();

  // Entity management
  Entity CreateEntity();
  void KillEntity(Entity entity);

  // Tag management
  void TagEntity(Entity entity, const std::string& tag);
  bool EntityHasTag(Entity entity, const std::string& tag) const;
  Entity GetEntityByTag(const std::string& tag) const;
  void RemoveEntityTag(Entity entity);

  // Group management
  void GroupEntity(Entity entity, const std::string& group);
  bool EntityBelongsToGroup(Entity entity, const std::string& group) const;
  std::vector<Entity> GetEntitiesByGroup(const std::string& group) const;
  void RemoveEntityGroup(Entity entity);

  // Component management
  template <typename TComponent, typename... TArgs>
  void AddComponent(Entity entity, TArgs&&... args);
  template <typename TComponent> void RemoveComponent(Entity entity);
  template <typename TComponent> bool HasComponent(Entity entity) const;
  template <typename TComponent> TComponent& GetComponent(Entity entity) const;

  // Add a system to the registry
  template <typename TSystem, typename... TArgs>
  void AddSystem(TArgs&&... args);
  // Remove a system from the registry
  template <typename TSystem> void RemoveSystem();
  // Check if the registry contains a system
  template <typename TSystem> bool HasSystem() const;
  // Get a system from the registry
  template <typename TSystem> TSystem& GetSystem() const;

  // Checks the component signature of an entity and add the entity to the
  // systems that are interested in it
  void AddEntityToSystems(Entity entity);
  void RemoveEntityFromSystems(Entity entity);
};

template <typename TComponent> void System::RequireComponent() {
  const auto componentId = Component<TComponent>::GetId();
  componentSignature.set(componentId);
}

template <typename TSystem, typename... TArgs>
void Registry::AddSystem(TArgs&&... args) {
  std::shared_ptr<TSystem> newSystem =
      std::make_shared<TSystem>(std::forward<TArgs>(args)...);
  systems.insert(std::make_pair(std::type_index(typeid(TSystem)), newSystem));
}

template <typename TSystem> void Registry::RemoveSystem() {
  auto system = systems.find(std::type_index(typeid(TSystem)));
  systems.erase(system);
}

template <typename TSystem> bool Registry::HasSystem() const {
  return systems.find(std::type_index(typeid(TSystem))) != systems.end();
}

template <typename TSystem> TSystem& Registry::GetSystem() const {
  auto system = systems.find(std::type_index(typeid(TSystem)));
  return *(std::static_pointer_cast<TSystem>(system->second));
}

template <typename TComponent, typename... TArgs>
void Registry::AddComponent(Entity entity, TArgs&&... args) {
  const auto componentId = Component<TComponent>::GetId();
  const auto entityId = entity.GetId();

  if (componentId >= static_cast<int>(componentPools.size())) {
    componentPools.resize(componentId + 1, nullptr);
  }

  if (!componentPools[componentId]) {
    std::shared_ptr<Pool<TComponent>> newComponentPool(new Pool<TComponent>());
    componentPools[componentId] = newComponentPool;
  }

  std::shared_ptr<Pool<TComponent>> componentPool =
      std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);

  TComponent newComponent(std::forward<TArgs>(args)...);

  componentPool->Set(entityId, newComponent);

  entityComponentSignatures[entityId].set(componentId);

  Logger::Log("Component id = " + std::to_string(componentId) +
              " was added to entity id " + std::to_string(entityId));
}

template <typename TComponent> void Registry::RemoveComponent(Entity entity) {
  const auto componentId = Component<TComponent>::GetId();
  const auto entityId = entity.GetId();

  // Remove component from the list for that entity
  std::shared_ptr<Pool<TComponent>> componentPool =
      std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);
  componentPool->Remove(entityId);

  // Set component signature to false
  entityComponentSignatures[entityId].set(componentId, false);

  Logger::Log("Component id = " + std::to_string(componentId) +
              " was removed from entity id " + std::to_string(entityId));
}

template <typename TComponent>
bool Registry::HasComponent(Entity entity) const {
  const auto componentId = Component<TComponent>::GetId();
  const auto entityId = entity.GetId();
  return entityComponentSignatures[entityId].test(componentId);
}

template <typename TComponent>
TComponent& Registry::GetComponent(Entity entity) const {
  const auto componentId = Component<TComponent>::GetId();
  const auto entityId = entity.GetId();
  auto componentPool =
      std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);
  return componentPool->Get(entityId);
}

template <typename TComponent, typename... TArgs>
void Entity::AddComponent(TArgs&&... args) {
  registry->AddComponent<TComponent>(*this, std::forward<TArgs>(args)...);
}

template <typename TComponent> void Entity::RemoveComponent() {
  registry->RemoveComponent<TComponent>(*this);
}

template <typename TComponent> bool Entity::HasComponent() const {
  return registry->HasComponent<TComponent>(*this);
}

template <typename TComponent> TComponent& Entity::GetComponent() const {
  return registry->GetComponent<TComponent>(*this);
}

#endif
