#ifndef CIRCLECOLLIDERCOMPONENT_H
#define CIRCLECOLLIDERCOMPONENT_H

#include <glm/glm.hpp>

struct CircleColliderComponent {
  double radius;

  // TODO: Offset should be a vec2
  double offset;

  CircleColliderComponent(double radius = 0, double offset = 0) {
    this->radius = radius;
    this->offset = offset;
  }
};

#endif // !CIRCLECOLLIDERCOMPONENT_H
