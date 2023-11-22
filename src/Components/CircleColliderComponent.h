#ifndef CIRCLECOLLIDERCOMPONENT_H
#define CIRCLECOLLIDERCOMPONENT_H

#include <glm/glm.hpp>

struct CircleColliderComponent {
  double radius;

  CircleColliderComponent(double radius = 0) { this->radius = radius; }
};

#endif // !CIRCLECOLLIDERCOMPONENT_H
