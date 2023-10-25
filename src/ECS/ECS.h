#ifndef ECS_H
#define ECS_H

#include "../Logger/Logger.h"
#include <bitset>
#include <cwchar>
#include <set>
#include <string>
#include <strings.h>
#include <typeindex>
#include <unordered_map>
#include <vector>

const unsigned int MAX_COMPONENTS = 32;
typedef std::bitset<MAX_COMPONENTS> Signature;

// BaseComponent Class to generate unique IDs for components
struct BaseComponent {
protected:
  static int nextId;
};

// Component class to expose the private ID variable
template <typename TComponent> class Component : public BaseComponent {
  static int GetId() {
    static auto id = nextId++;
    return id;
  }
};

// Entity class that will only store an ID
class Entity {
private:
  int id;

public:
  Entity(int id) : id(id){};
  int GetId() const;
};

// System class to perform logic on data in components
class System {
private:
  Signature componentSignature;
  std::vector<Entity> entities;

public:
  System() = default;
  ~System() = default;

  void AddEntityToSystem(Entity entity);
  void RemoveEntityFromSystem(Entity entity);

  std::vector<Entity> GetAllSystemEntities() const;
  const Signature& GetComponentSignature() const;

  template <typename TComponent> void RequireComponent();
};

class TPool {
public:
  virtual ~TPool() {}
};

// Pool class to track all the components in the Registry
template <typename T> class Pool : public TPool {
private:
  std::vector<T> data;

public:
  Pool(int size = 100) { Resize(size); }
  virtual ~Pool() = default;

  bool IsEmpty() const { return data.empty(); }
  int GetSize() const { return data.size(); }
  void Resize(int n) { data.resize(); }
  void Clear() { data.clear(); };
  void Add(T object) { data.push_back(object); }
  void Set(int index, T object) { data[index] = object; }
  void RemoveAt(int index) { data.erase(data.begin() + index); }
  T& Get(int index) { return static_cast<T&>(data[index]); }
  T& operator[](unsigned int index) { return data[index]; }
};

// Registry class to handle the global state of entities, add systems, and
// components
class Registry {
private:
  int numEntities = 0;

  // Each pool contains data for a certain component type
  // [vector index = componentId], [pool index = entityId]
  std::vector<TPool*> componentPools;

  // Signatures tell which components are active for an entity
  // [vector index = entity id]
  std::vector<Signature> entityComponentSignature;

  // Map of active systems
  // [index = system typeid]
  std::unordered_map<std::type_index, System*> systems;

  std::set<Entity> entitiesToBeAdded;
  std::set<Entity> entitiesToBeDeleted;

public:
  Registry() = default;

  void Update();

  // Entity Methods
  Entity CreateEntity();
  void KillEntity(Entity entity);

  // System Methods
  template <typename TSystem> void AddSystem(System* systemToAdd);
  template <typename TSystem> void RemoveSystem();
  bool HasSystem();
  void GetSystem();
  void AddEntityToSystem(Entity entity);

  // Component Methods
  template <typename T, typename... TArgs>
  void AddComponent(Entity entity, TArgs&&... args);
  template <typename T> void RemoveComponent(Entity entity);
  template <typename T> bool HasComponent(Entity entity) const;
};

// Template functions
template <typename TComponent> void System::RequireComponent() {
  const auto componentId = Component<TComponent>::GetId();
  componentSignature.set(componentId);
}

template <typename T, typename... TArgs>
void Registry::AddComponent(Entity entity, TArgs&&... args) {
  const auto componentId = Component<T>::GetId();
  const auto entityId = entity.GetId();

  if (componentId >= componentPools.size()) {
    componentPools.resize(componentId + 1, nullptr);
  }

  if (!componentPools[componentId]) {
    Pool<T>* newComponentPool = new Pool<T>();
    componentPools[componentId] = newComponentPool;
  }

  Pool<T>* currentPool = componentPools[componentId];

  if (entityId > currentPool->GetSize()) {
    currentPool->Resize(numEntities);
  }

  // Create component of type T and pass in the parameters
  T newComponent(std::forward<TArgs>(args)...);

  // Add the new component to the entity within the current pool
  currentPool->Set(entityId, newComponent);

  // Flip the bit for the newly added component to "on"
  entityComponentSignature[entityId].set(componentId);
}

template <typename T> void Registry::RemoveComponent(Entity entity) {
  const auto componentId = Component<T>::GetId();
  const auto entityId = entity.GetId();

  if (componentId >= componentPools.size() || !componentPools[componentId]) {
    Logger::Err("Attempting to remove component with ID: " +
                std::to_string(componentId) +
                "on the entity with ID: " + std::to_string(entityId));
    return;
  }

  Pool<T>* currentPool = componentPools[componentId];

  if (!currentPool[entityId]) {
    Logger::Err("Trying to remove a component from an entity that does not "
                "contain that component");
    return;
  }

  currentPool->RemoveAt(entityId);
  entityComponentSignature[entityId].set(componentId, false);
}

template <typename T> bool Registry::HasComponent(Entity entity) const {
  const auto componentId = Component<T>::GetId();
  const auto entityId = entity.GetId();

  return entityComponentSignature[entityId].test(componentId);
}

template <typename TSystem> void Registry::AddSystem(System* systemToAdd) {
  std::string typeNameStr = std::string(typeid(TSystem).name());
  std::type_index systemTypeKey = std::type_index(typeid(TSystem));

  if (!systems[systemTypeKey]) {
    systems.insert({systemTypeKey, systemToAdd});

    Logger::Log("Added a system of type: " + typeNameStr);

    return;
  }

  Logger::Err("Tried to add a system of type: " + typeNameStr +
              "that already exists!");
  return;
}

template <typename TSystem> void Registry::RemoveSystem() {
  std::string typeNameStr = std::string(typeid(TSystem).name());
  std::type_index systemTypeKey = std::type_index(typeid(TSystem));

  if (!systems[systemTypeKey]) {
    Logger::Err("Tried to remove a system of type: " + typeNameStr +
                "that doesnt't exist!");
    return;
  }

  systems.erase(systemTypeKey);

  Logger::Log("Successfully removed system of type: " + typeNameStr);

  return;
}

#endif
