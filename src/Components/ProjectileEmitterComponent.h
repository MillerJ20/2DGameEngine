#ifndef PROJECTILEEMITTERCOMPONENT_H
#define PROJECTILEEMITTERCOMPONENT_H

#include <SDL2/SDL_timer.h>
#include <glm/glm.hpp>

struct ProjectileEmitterComponent {
  glm::vec2 projectileVelocity;
  int repeatFrequency;
  int projectileDuration;
  int hitPercentDamage;
  bool isFriendly;
  int lastEmissionTime;

  ProjectileEmitterComponent(glm::vec2 projectileVelocity = glm::vec2(0),
                             int repeatFrequency = 0,
                             int projectileDuration = 10000,
                             int hitPrecentDamage = 10,
                             bool isFriendly = false) {
    this->projectileVelocity = projectileVelocity;
    this->repeatFrequency = repeatFrequency;
    this->projectileDuration = projectileDuration;
    this->hitPercentDamage = hitPrecentDamage;
    this->isFriendly = isFriendly;
    this->lastEmissionTime = SDL_GetTicks();
  }
};

#endif // !PROJECTILEEMITTERCOMPONENT_H
