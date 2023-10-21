#ifndef ECS_H
#define ECS_H
#include <bitset>
#include <vector>

const unsigned int MAX_COMPONENTS = 32;
typedef std::bitset<MAX_COMPONENTS> Signature;

class BaseComponent {
protected:
  static int nextId;
};

template <typename TComponent> class Component : public BaseComponent {
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
  int GetId() const;
};

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
  Signature &GetComponentSignature() const;

  template <typename TComponent> void RequireComponent();
};

class Registry {};

template <typename TComponent> void System::RequireComponent() {
  const auto componentId = Component<TComponent>::GetId();
  componentSignature.set(componentId);
}
#endif
