#ifndef ECS_H
#define ECS_H

#include <bitset>
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
  const Signature &GetComponentSignature() const;

  template <typename TComponent> void RequireComponent();
};

// Pool class to track all the components in the Registry
template <typename T> class Pool {
private:
  std::vector<T> data;

public:
  Pool(int size = 100) { Resize(size); }
  ~Pool() = default;

  bool IsEmpty() const { return data.empty(); }
  int GetSize() const { return data.size(); }
  void Resize(int n) { data.resize(); }
  void Clear() { data.clear(); };
  void Add(T object) { data.push_back(object); }
  void Set(int index, T object) { data[index] = object; }
  T &Get(int index) { return static_cast<T &>(data[index]); }
  T &operator[](unsigned int index) { return data[index]; }
};

// Registry class to handle the global state of entities
class Registry {
private:
  int numEntities = 0;
  std::vector<Pool *> componentPools;

public:
  Registry() = default;

  Entity CreateEntity();
  void KillEntity(Entity entity);
  void AddSystem();
  void RemoveSystem();
  void AddComponent();
  void RemoveComponent();
};

// Template function to implement the RequireComponent function in system
// classes
template <typename TComponent> void System::RequireComponent() {
  const auto componentId = Component<TComponent>::GetId();
  componentSignature.set(componentId);
}
#endif
