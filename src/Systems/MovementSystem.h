#ifndef MOVEMENTSYSTEM_H
#define MOVEMENTSYSTEM_H

#include "../src/ECS/ECS.h"

class MovementSystem : public System {
public:
  MovementSystem();

  void Update();
};

#endif
